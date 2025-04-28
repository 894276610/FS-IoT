#include "burstClassifier.h"
#include "timer.h"
#include <future>
#include <thread>

namespace groundnut
{

void BurstClassifier::Train(std::unordered_map<uint16_t, BurstGroups>* trainset)
{   
    std::vector<std::future<void>> tasks;
    oriTrainset = trainset;

	for (const auto& [deviceId, burstGroups] : *trainset)
	{   
        tasks.emplace_back(std::async(std::launch::async,[&]{
            TrainDevice(deviceId, burstGroups);
        }));                    
	}


    for(auto& task: tasks)
    {
        task.wait();
    }

}

void BurstClassifier::TrainDevice(uint16_t deviceId, BurstGroups burstGroups)
{
	std::unordered_map<std::shared_ptr<KBurst>, int> trainMap;

	trainMap = MergeByHash(burstGroups);
    MergeByKey(&trainMap);
	
	int pktIndex, uniPktIndex = 0;
    // std::cout << "deviceName:" << trainMap.begin()->first->GetLabel();
    // std::cout << ", size:" << trainMap.size() << std::endl;

	for (auto& [burst, num] : trainMap)
	{       
		pktIndex = std::min(burst->GetPktNum(), config.maxPktIndex) -1;
        uniPktIndex = burst->GetUniPktNum() -1;

        assert(uniPktIndex < uniPktMutex.size());
             
        {
            std::unique_lock<std::mutex> lock(*uniPktMutex[uniPktIndex]);         
            train[uniPktIndex][pktIndex].push_back(burst);
        }
        
	}

}

std::unordered_map<std::shared_ptr<KBurst>, int> BurstClassifier::MergeByHash(BurstGroups& burstGroups)
{
	std::unordered_map<std::shared_ptr<KBurst>, int> trainMap;
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

void BurstClassifier::MergeByKey(std::unordered_map<std::shared_ptr<KBurst>, int>* map) {
    // 将 map 内容转为 vector 并排序
    std::vector<std::pair<std::shared_ptr<KBurst>, int>> sorted_vec;
    sorted_vec.reserve(map->size());
    for (auto& pair : *map) sorted_vec.emplace_back(std::move(pair));
    map->clear();

    // 降序排序（按计数值）
    std::sort(sorted_vec.begin(), sorted_vec.end(), 
        [](const auto& a, const auto& b) { return a.second > b.second; });

    for (const auto& [burst, num] : sorted_vec) {
        if (map->size() >= config.maxTrainItem) break;

        // 提前过滤无需合并的情况
        if (burst->GetPktNum() <=6) {
            map->emplace(burst, num);
            continue;
        }

        // 合并查找逻辑
        const auto try_merge = [&](const auto& target) {
            if (target && target->Distance(*burst) < 0.7) {
                (*map)[target] += num;
                return true;
            }
            return false;
        };

        // 优先查找 UltraSimilar，再查找 Identical
        bool merged = false;
        std::shared_ptr<KBurst> target;

        if (target = FindDistanceLT(map, burst, 0.2); target) {
            merged = try_merge(target);
        }
        if (!merged && (target = FindKeysEQ(map, burst))) {
            merged = try_merge(target);
        }
        if (!merged) {
            map->emplace(burst, num);
        }
    }
}

std::shared_ptr<KBurst> BurstClassifier::FindDistanceLT(std::unordered_map<std::shared_ptr<KBurst>, int>* map, std::shared_ptr<KBurst> burst, float threshold)
{
    auto it = std::find_if(map->begin(), map->end(), [&](const auto& pair) { // pair = [burst, num]
        return pair.first->Distance(burst) < threshold;
    });
    return (it != map->end()) ? it->first : nullptr;
}

// return a burst that has eq keys
std::shared_ptr<KBurst> BurstClassifier::FindKeysEQ(
    const std::unordered_map<std::shared_ptr<KBurst>, int>* map,
    const std::shared_ptr<KBurst>& burst) 
{
    const auto& countMap = burst->GetCountMap();
    
    // 使用 std::find_if 替代显式循环
    const auto it = std::find_if(map->begin(), map->end(),
        [&countMap](const auto& entry) {
            const auto& [b, num] = entry;
            
            // 合并长度检查和元素比较
            return b->GetUniPktNum() == countMap.size() &&
                   std::equal(
                       b->GetCountMap().begin(), b->GetCountMap().end(),
                       countMap.begin(),
                       [](const auto& p1, const auto& p2) {
                           return p1.first == p2.first;
                       });
        });

    return it != map->end() ? it->first : nullptr;
}

SearchResult BurstClassifier::ReviewSearch(const std::shared_ptr<KBurst> burst)
{
    SearchResult result;
    float min_distance = 100, distance;
    BurstGroups targetGroups;

    // find same label target device in the trainset
    for(auto& [deviceId, burstGroups] : *oriTrainset)
    {
        if(burstGroups.size() == 0 && burstGroups[0].size() == 0)
        {
            continue;
        }

        if(burstGroups[0][0]->GetLabel() == burst->GetLabel())
        {
            targetGroups.insert(targetGroups.begin(), burstGroups.begin(), burstGroups.end());
        }
    }
    
    for(auto& targetGroup : targetGroups)
    {
        for(auto& targetBurst : targetGroup)
        {
            distance = burst->Distance(targetBurst);

            if (distance < min_distance)
            {
                result.nearTrainBursts.clear();
                result.nearTrainBursts.push_back(targetBurst);
                result.minDistance = distance;
                min_distance = distance;
            }
            else if (distance == min_distance)
            {
                result.nearTrainBursts.push_back(targetBurst);
            }
        }
    }

    return result;
}

SearchResult BurstClassifier::Predict(const std::shared_ptr<KBurst> burst)
{
    SearchResult result;    
    float min_distance = 100, distance;
    std::optional<SearchResult> cachedResult = cache.Read(*burst);
	
  	if (cachedResult.has_value())
	{
		return cachedResult.value();
	}

    int uniPktMin = std::max(0, burst->GetUniPktNum() - config.uniPktTolr);
    int uniPktMax = std::min(config.maxUniPkt, burst->GetUniPktNum() + config.uniPktTolr);
    // int pktMin = std::max(0, std::min(burst->GetPktNum() - config.pktTolr, config.maxPktIndex));
    // int pktMax = std::min(burst->GetPktNum() + config.pktTolr, config.maxPktIndex);

    int pktMin = std::max(0, burst->GetPktNum()/2 - 10);
    int pktMax = std::min(burst->GetPktNum()*2 + 10, config.maxPktIndex);

	for (int i = uniPktMin; i < uniPktMax; i++)
	{
		for (int j = pktMin; j < pktMax; j++)
		{
			for (const auto trainBurst : train[i][j])
			{
 				distance = burst->Distance(trainBurst);

				if (distance < min_distance)
				{
					result.nearTrainBursts.clear();
					result.nearTrainBursts.push_back(trainBurst);
					result.minDistance = distance;
                    min_distance = distance;
				}
				else if (distance == min_distance)
				{
					result.nearTrainBursts.push_back(trainBurst);
				}
			}	
		}
	}
    cache.Write(*burst, result);
	return result;
}
}