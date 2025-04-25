#include "burstDataset.h"
#include <cmath>
#include "kburst.h"
#include "timer.h"

namespace groundnut{

DivMetric BurstDataset::GenDivMetric(std::string name, BurstGroups& burstGroups)
{
    BPCountMap uniBPCountMap = MergeByHash(burstGroups);
    float repeatRate = RepetitionRate(uniBPCountMap);
    float burstRate = AvgBurstRate(uniBPCountMap);
    float entropy = ShannonEntropy(uniBPCountMap);
    float diversity = Diversity(uniBPCountMap);

    int totalUniBurstNum = uniBPCountMap.size();	  // totalBurstNumber
	int totalBurstNum = GetBurstSize(burstGroups);	  // totalBurstNumber
    float normEntropy = entropy / (log10(totalUniBurstNum));

    DivMetric metric;
    metric.burstRate = burstRate;
    metric.diversity = diversity;
    metric.entropy = entropy;
    metric.normEntropy = normEntropy;
    metric.repeatRate = repeatRate;
    metric.burstNum = totalBurstNum;
    metric.uniBurstNum = totalUniBurstNum;
    metric.name = name;

    return metric;
}

int BurstDataset::GetBurstSize(const BurstGroups& burstGroup)const
{
    int size = 0;
    for(auto& burstVec : burstGroup)
    {
        size += burstVec.size();
    }

    return size;
}


BPCountMap BurstDataset::MergeByHash(BurstGroups& burstGroups)
{
    BPCountMap trainMap;
    std::unordered_map<std::size_t, std::shared_ptr<KBurst>> hashMap;

    for (const auto& burstGroup : burstGroups) {
        for (const auto& burst : burstGroup) {
            const auto hash = std::hash<KBurst>{}(*burst);    
            auto [it, inserted] = hashMap.try_emplace(hash, burst);

            if (inserted) {
                trainMap.emplace(burst, 1);
            } else {
                trainMap[it->second]++;
            }
        }
    }

    return trainMap;
}

int BurstDataset::Diversity(const BPCountMap& uniBPCountMap) const
{
    std::set<short> uniPkts;
    for(auto& bpCount : uniBPCountMap)
    {
        for(auto& pktCount: bpCount.first->GetCountMap())
        {
            uniPkts.insert(pktCount.first);
        }
    }

    return uniPkts.size();
}

float BurstDataset::AvgBurstRate(const BPCountMap& uniBPCountMap) const
{
    timespec totalDuration{0,0};
    int nonSinglePkt = 0;

    for(auto& bpCount : uniBPCountMap)
    {
        totalDuration = totalDuration + bpCount.first->GetLastPktStamp() - bpCount.first->GetFirstPktStamp();
        totalDuration > timespec{0,0} ? nonSinglePkt += bpCount.first->GetPktNum() : nonSinglePkt;
    }
    std::cout << "totalDuration:" << totalDuration.tv_sec;
    std::cout << " nonSinglePkt:" << nonSinglePkt << std::endl;
    std::cout <<"nonSinpkt/totalDuration:" << nonSinglePkt/totalDuration << std::endl;
    return nonSinglePkt/totalDuration;
}

float BurstDataset::RepetitionRate(const BPCountMap& uniBPCountMap) const
{
    float repetitionRate = 0;
    float repeatCounter = 0;
    int sum = 0;

    for(auto&[bp, count]: uniBPCountMap)
    {
        count > 1 ? repeatCounter += count : repeatCounter;
        sum += count;
    }
    
    return repeatCounter / sum;
}

float BurstDataset::ShannonEntropy(const BPCountMap& uniBPCountMap) const
{
    float entropy = 0;
    int sum = 0;

    for(auto&[bp, count]: uniBPCountMap)
    {
        sum += count;
    }

    for(auto&[bp, count]: uniBPCountMap)
    {
        float p = count/(float)sum;
        entropy += -p * log10(p);
    }

    return entropy;
}

void BurstDataset::Load(PacketDataset& dataset)
{  
    PROFILE_SCOPE("Load");
    // name 
    this->name = dataset.GetName();

    // device migration
    devicesVec.clear();
    devicesVec.resize(dataset.GetDevicesMap().size());
    
    for(auto [mac_string, kdevice] : dataset.GetDevicesMap())
    {
        devicesVec[kdevice.GetDevId()] = kdevice;
    }
    
    // packet migration
    for (const groundnut::KPacket& packet : dataset.GetDataset())
    {
        time_t slotId = packet.timestamp.tv_sec / configBurstDataset.slotDuration;

        AddPacket(packet.deviceId, slotId, &packet);
    }

    MakeBursts();
}

void BurstDataset::AddPacket(uint16_t deviceId, time_t slotId, const KPacket* packet)
{
    auto& mutex = mapDevMutexes[deviceId % kDeviceMutexCount];
    std::unique_lock<std::mutex> lock(mutex);
    mapByDevTime[deviceId][slotId].push_back(packet);
    timeSlots.insert(slotId);
}

void BurstDataset::MakeBursts()
{
    for (time_t slotId : timeSlots) {
        for (auto& [deviceId, slotMap] : mapByDevTime) {

            std::vector<const KPacket*> pktVec;

            { // 临界区：快速提取数据并清理条目
                auto& mapMutex = this->mapDevMutexes[deviceId % kDeviceMutexCount];
                std::unique_lock<std::mutex> lock(mapMutex);
                auto it = slotMap.find(slotId);
                if (it == slotMap.end() || it->second.empty()) {
                    continue;
                }
                pktVec = std::move(it->second);
                slotMap.erase(it);
            } // 释放锁
            
            BurstMaker maker(&pktVec, devicesVec[deviceId], configBurstDataset.burstTrh);
            BurstVec burstBlock = maker.MakeBursts();

            { // 临界区：线程安全地添加结果
                auto& blockMutex = blockMutexes[deviceId % kDeviceMutexCount];
                std::unique_lock<std::mutex> lock(blockMutex);
                rawMap[deviceId].emplace_back(std::move(burstBlock));
            }
        }
    }

    //mapByDevTime.clear();
}

float BurstDataset::TrainTestSplitByTime(int min)
{
    PROFILE_SCOPE("Split");
    float avgInstance = 0;

    for (auto& [deviceId, burstGroup] : rawMap)
    {
        std::sort(burstGroup.begin(), burstGroup.end(), [](const BurstVec& a, const BurstVec& b) {
            return (*a.front()) < (*b.front());
        });   
        
        size_t totalSize = burstGroup.size();
        
        int minBudgetTrainNum = min * 60 / configBurstDataset.slotDuration;
        int maxTrainNum = std::ceil(totalSize * configBurstDataset.trainRate);
        size_t trainNum = std::min(maxTrainNum, minBudgetTrainNum);

        //std::cout << "device: " << GetDevicesVec()[deviceId].GetLabel() << "totalSize:" << totalSize << "trainNum:" << trainNum << std::endl;

        int valiNum = std::floor(totalSize * configBurstDataset.valiRate);
        size_t testNum = std::ceil(totalSize * configBurstDataset.testRate);

        size_t testStart = totalSize - testNum;
        size_t valiStart = totalSize - testNum - valiNum;

        if(trainNum + testNum + valiNum > burstGroup.size())
        {
            valiNum = burstGroup.size() - trainNum - testNum;
            std::cout << "overflow ! totalNum:" + std::to_string(burstGroup.size())\
             + "device: " + GetDevicesVec()[deviceId].GetLabel() + "\n";
        }

        if( valiNum <=0 )
        {
            std::cout << "valiNum < 0, device:" + GetDevicesVec()[deviceId].GetLabel() + "\n";
        }

        BurstGroups trainGroups(burstGroup.begin(), burstGroup.begin() + trainNum);
        BurstGroups valiGroups(burstGroup.begin() + valiStart, burstGroup.begin() + valiStart + valiNum);
        BurstGroups testGroups(burstGroup.begin() + testStart, burstGroup.end());

        trainset.insert({ deviceId, trainGroups });
        valiset.insert({ deviceId, valiGroups });
        testset.insert({ deviceId, testGroups });
        
        avgInstance += trainNum;
    }

    avgInstance /= trainset.size();
    return avgInstance;
}

void BurstDataset::TrainTestSplit()
{
    for (auto& [deviceId, burstGroup] : rawMap)
    {
        {
            std::stable_sort(burstGroup.begin(), burstGroup.end(), [](const BurstVec& a, const BurstVec& b) {
                return (*a.front()) < (*b.front());
            });
        }
        
        size_t trainNum = std::ceil(burstGroup.size() * configBurstDataset.trainRate);
        size_t testNum = std::ceil(burstGroup.size() * configBurstDataset.testRate);
        int valiNum = std::floor(burstGroup.size() * configBurstDataset.valiRate);
        
        if(trainNum + testNum + valiNum > burstGroup.size())
        {
            valiNum = burstGroup.size() - trainNum - testNum;
            throw "overflow ! totalNum:" + std::to_string(burstGroup.size())\
             + "device: " + GetDevicesVec()[deviceId].GetLabel() + "\n";
        }

        if( valiNum <=0 )
        {
            throw "valiNum < 0, device:" + GetDevicesVec()[deviceId].GetLabel() + "\n";
        }
        
        size_t testStart = burstGroup.size() - testNum;
        size_t valiStart = burstGroup.size() - testNum - valiNum;

        BurstGroups trainGroups(burstGroup.begin(), burstGroup.begin() + trainNum);
        BurstGroups valiGroups(burstGroup.begin() + valiStart, burstGroup.begin() + valiStart + valiNum);
        BurstGroups testGroups(burstGroup.begin() + testStart, burstGroup.end());

        trainset.insert({ deviceId, trainGroups });
        valiset.insert({ deviceId, valiGroups });
        testset.insert({ deviceId, testGroups });
    }
}

}