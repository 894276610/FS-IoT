#include "byteIoTDataset.h"
#include <cmath>
#include "timer.h"

namespace groundnut{

DivMetric ByteIoTDataset::GenDivMetric(std::string name, BurstVec& burstVec)
{
    BPCountMap uniBPCountMap = MergeByHash(burstVec);
    float repeatRate = RepetitionRate(uniBPCountMap);
    float burstRate = AvgBurstRate(uniBPCountMap);
    float entropy = ShannonEntropy(uniBPCountMap);
    float diversity = Diversity(uniBPCountMap);

	int totalUniBurstNum = uniBPCountMap.size();	  // totalBurstNumber
	int totalBurstNum = burstVec.size();	  // totalBurstNumber
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

int ByteIoTDataset::Diversity(const BPCountMap& uniBPCountMap) const
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

int ByteIoTDataset::Diversity(const BurstVec& burstVec) const
{
    std::set<short> uniPkts;
    for(const auto& burst : burstVec)
    {
        for(auto& pktCount: burst->GetCountMap())
        {
            uniPkts.insert(pktCount.first);
        }
    }

    return uniPkts.size();
}

float ByteIoTDataset::AvgBurstRate(const BPCountMap& uniBPCountMap) const
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

float ByteIoTDataset::RepetitionRate(const BPCountMap& uniBPCountMap) const
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

float ByteIoTDataset::ShannonEntropy(const BPCountMap& uniBPCountMap) const
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

BPCountMap ByteIoTDataset::MergeByHash(BurstVec& burstVec)
{
	BPCountMap trainMap;
    std::unordered_map<std::size_t, std::shared_ptr<KBurst>> hashMap;

    for (const auto& burst : burstVec) {
        const auto hash = std::hash<KBurst>{}(*burst);    
        auto [it, inserted] = hashMap.try_emplace(hash, burst);

        if (inserted) {
            trainMap.emplace(burst, 1);
        } else {
            trainMap[it->second]++;
        }
    }

    return trainMap;
}

void ByteIoTDataset::Load(PacketDataset& dataset)
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
        time_t slotId = packet.timestamp.tv_sec / config.slotDuration;

        AddPacket(packet.deviceId, slotId, &packet);
    }

    MakeInstances();
}

void ByteIoTDataset::AddPacket(uint16_t deviceId, time_t slotId, const KPacket* packet)
{
    auto& mutex = mapDevMutexes[deviceId % kDeviceMutexCount];
    std::unique_lock<std::mutex> lock(mutex);
    mapByDevTime[deviceId][slotId].push_back(packet);
    timeSlots.insert(slotId);
}

void ByteIoTDataset::MakeInstances()
{
    for (time_t slotId : timeSlots) {
        for (auto& [deviceId, slotMap] : mapByDevTime) {

            std::vector<const KPacket*> pktVec;

            auto it = slotMap.find(slotId);
            if (it == slotMap.end() || it->second.empty()) {
                continue;
            }

            pktVec = std::move(it->second);
            slotMap.erase(it);
            
            std::shared_ptr<KBurst> pInstance = std::make_shared<KBurst>(devicesVec[deviceId]);
            for(auto* packet : pktVec)
            {
                pInstance->AddPacket(*packet);
            }
            
            rawMap[deviceId].emplace_back(pInstance);
        }
    }
}

// should not exceed the bounded training rate(e.g. 15%)
// should not exceed the bounded instances min * 60/  1800
float ByteIoTDataset::TrainTestSplitByTime(int min)
{
    PROFILE_SCOPE("Split");
    float averageTrainInstanceSize = 0;
    for (auto& [deviceId, instances] : rawMap)
    {
        {
            std::sort(instances.begin(), instances.end(), [](const auto& a, const auto& b) {
                return (*a) < (*b);
            });
        }
        
        size_t totalSize = instances.size();

        std::cout << "device: " + GetDevicesVec()[deviceId].GetLabel() << std::endl;

        int maxTrainNum = std::ceil(totalSize * config.trainRate);
        std::cout << "totalSize:" << totalSize << "trainRate" << config.trainRate;
        std::cout << "maxTrainNum:" << maxTrainNum << std::endl;

        int min15xPreferTrainNum = min * 60 / config.slotDuration;
        size_t trainNum = std::min(maxTrainNum, min15xPreferTrainNum);

        size_t testNum = std::ceil(totalSize * config.testRate);
        size_t testStart = instances.size() - testNum;

        BurstVec trainInstances(instances.begin(), instances.begin() + trainNum);
        BurstVec testInstances(instances.begin() + testStart, instances.end());

        trainset.insert({ deviceId, trainInstances });
        testset.insert({  deviceId, testInstances });

        averageTrainInstanceSize += trainNum;
    }
    averageTrainInstanceSize /= rawMap.size();
    return averageTrainInstanceSize;
}

// return average train instance size (per device)
float ByteIoTDataset::TrainTestSplit()
{
    float averageTrainInstanceSize = 0;
    for (auto& [deviceId, instances] : rawMap)
    {
        {
            std::sort(instances.begin(), instances.end(), [](const auto& a, const auto& b) {
                return (*a) < (*b);
            });
        }
        
        size_t totalSize = instances.size();

        // std::cout << "device: " + GetDevicesVec()[deviceId].GetLabel() << std::endl;
        // std::cout << "totalSize:" << totalSize<< std::endl;
        size_t trainNum = std::ceil(totalSize * config.trainRate);
        size_t testNum = std::ceil(totalSize * config.testRate);

        if(trainNum  + testNum > instances.size())
        {
            std::cout << "warning: overflow of "<< GetDevicesVec()[deviceId].GetLabel()<<std::endl;
            std::cout << "trainNum:" << trainNum  << ",testNum:" << testNum << std::endl;
            std::cout << "totalSize:" + std::to_string(totalSize) << std::endl;
            
            //throw "overflow ! totalSize:" + std::to_string(totalSize) + "," \
            + std::to_string(trainNum + valiNum + testNum)\
            + "device: " + GetDevicesVec()[deviceId].GetLabel() + "\n";
        }

        size_t testStart = instances.size() - testNum;

        BurstVec trainInstances(instances.begin(), instances.begin() + trainNum);
        BurstVec testInstances(instances.begin() + testStart, instances.end());

        trainset.insert({ deviceId, trainInstances });
        testset.insert({ deviceId, testInstances });

        averageTrainInstanceSize += trainNum;
        // std::cout << "trainNum:" << trainNum << std::endl;
    }

    averageTrainInstanceSize /= rawMap.size();

    return averageTrainInstanceSize;
}

}