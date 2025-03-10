#ifndef _GROUNDNUT_BURSTDATASET_H_
#define _GROUNDNUT_BURSTDATASET_H_

#include <unordered_map>
#include <iostream>
#include <filesystem>
#include "kburst.h"
#include "serialize-utils.h"
#include "packetDataset.h"
#include "burstMaker.h"

namespace groundnut{

class BurstDataset
{
public:

	BurstDataset() {}
	BurstDataset(const std::string& datasetName) : seed(0), name(datasetName){}

	void Load(PacketDataset&, const BurstTrh& trh);
	
	// split operation
	void TrainTestSplit(float trainRate = 0.5);

	// getter and setter
	std::string & GetName(){return name;}
	int GetSlotDuration(){return slotDuration;}
	BurstTrh& GetBurstTrh(){return trh;}
	std::vector<KDevice>& GetDevicesVec(){return devicesVec;}
	std::unordered_map<uint16_t, BurstGroups>& GetRawMap(){return rawMap;}
	std::unordered_map<uint16_t, BurstGroups>& GetTrainset(){return trainset;}
	std::unordered_map<uint16_t, BurstGroups>& GetTestset(){return testset;}
	std::set<size_t>& GetTimeSlots(){return timeSlots;}

	void SetSlotDuration(int duration){slotDuration = duration;}
	void SetBurstTrh(const BurstTrh& trh){this->trh = trh;}
	void SetName(const std::string name){this->name = name;}
	
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version) {
		ar& name;
		ar& slotDuration;
		ar& trh;
		ar& devicesVec;
		ar& timeSlots;
		ar& rawMap;
	}

private:
	void AddPacket(uint16_t deviceId, time_t slotId, const KPacket* packet);
    void MakeBursts(const BurstTrh& trh);	

private:
	unsigned seed;
	std::string name;
	int slotDuration = 1800;
	BurstTrh trh;
    std::vector<groundnut::KDevice> devicesVec; // should Add constructor !

	std::unordered_map<uint16_t, BurstGroups> rawMap;
	std::unordered_map<uint16_t, BurstGroups> trainset;
	std::unordered_map<uint16_t, BurstGroups> testset;

	std::set<size_t> timeSlots;
	std::unordered_map<short, std::map<time_t, std::vector<const KPacket*>>> mapByDevTime;

	static constexpr int kDeviceMutexCount = 16;
    std::array<std::mutex, kDeviceMutexCount> mapDevMutexes;
    std::array<std::mutex, kDeviceMutexCount> blockMutexes;
};

}
#endif