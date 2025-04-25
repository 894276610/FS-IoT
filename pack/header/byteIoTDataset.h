#include "burstDataset.h"

namespace groundnut
{

class ByteIoTDataset
{

public:
    ByteIoTDataset() = default;
	ByteIoTDataset(const DatasetEnum datasetName, ConfigDataset configDataset):
	name(datasetName), config(configDataset){}

    void Load(PacketDataset&);
	float TrainTestSplit();
	float TrainTestSplitByTime(int min);

    DivMetric GenDivMetric(std::string name, BurstVec& burstVec);

	std::vector<KDevice>& GetDevicesVec(){return devicesVec;}
    DatasetEnum GetName(){return name;}

    std::unordered_map<uint16_t, BurstVec>& GetRawMap(){return rawMap;}
	std::unordered_map<uint16_t, BurstVec>& GetTrainset(){return trainset;}
	std::unordered_map<uint16_t, BurstVec>& GetTestset(){return testset;}

private:
	void AddPacket(uint16_t deviceId, time_t slotId, const KPacket* packet);
    void MakeInstances();
    float RepetitionRate(const BPCountMap& uniBPCountMap) const;
    float ShannonEntropy(const BPCountMap& uniBPCountMap) const;
    int Diversity(const BPCountMap& uniBPCountMap) const;
    int Diversity(const BurstVec& burstVec) const;
    float AvgBurstRate(const BPCountMap& uniBPCountMap) const;
    std::unordered_map<std::shared_ptr<KBurst>, int> MergeByHash(BurstVec& burstVec);

private:
    DatasetEnum name;
    ConfigDataset config;

    std::vector<groundnut::KDevice> devicesVec;

    std::set<size_t> timeSlots;
	std::unordered_map<short, std::map<time_t, std::vector<const KPacket*>>> mapByDevTime;

	static constexpr int kDeviceMutexCount = 16;
    std::array<std::mutex, kDeviceMutexCount> mapDevMutexes;
    std::array<std::mutex, kDeviceMutexCount> blockMutexes;

    std::unordered_map<uint16_t, BurstVec> rawMap;
    std::unordered_map<uint16_t, BurstVec> trainset;
	std::unordered_map<uint16_t, BurstVec> testset;
};

}
