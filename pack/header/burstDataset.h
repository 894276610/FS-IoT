#ifndef _GROUNDNUT_BURSTDATASET_H_
#define _GROUNDNUT_BURSTDATASET_H_

#include <unordered_map>
#include <iostream>
#include <filesystem>
#include "kburst.h"
#include "serialize-utils.h"
#include "packetDataset.h"
#include "burstMaker.h"
#include <sstream>

namespace groundnut{

struct ConfigBurstDataset
{
	int slotDuration = 1800;
	float trainRate = 0.15;
	float valiRate = 0.3;
	float testRate = 0.5;
	BurstTrh burstTrh;

	template<class Archive>
	void serialize(Archive& ar, const unsigned int version) {
		ar& slotDuration;
		ar& trainRate;
		ar& burstTrh;
	}

	inline std::string ToString() const
	{
		std::stringstream ss;
		ss << "(slotDur=" << slotDuration << ")";
		ss << "(trainRate=" << trainRate << ")";
		ss << "(valiRate=" << valiRate << ")";
		ss << "(testRate="<< testRate << ")";
		ss << burstTrh.ToString();
		return ss.str();
	}
};

class BurstDataset
{
public:

	BurstDataset() = default;
	BurstDataset(const std::string& datasetName, const ConfigBurstDataset configBurstDataset):
	name(datasetName), configBurstDataset(configBurstDataset) {}
	BurstDataset(const std::string& datasetName) : name(datasetName){}

	void Load(PacketDataset&);
	
	// split operation
	void TrainTestSplit();
	float TrainTestSplitByTime(int min15x);

	// getter and setter
	std::string & GetName(){return name;}
	int GetSlotDuration(){return configBurstDataset.slotDuration;}
	BurstTrh& GetBurstTrh(){return configBurstDataset.burstTrh;}
	std::vector<KDevice>& GetDevicesVec(){return devicesVec;}
	std::unordered_map<uint16_t, BurstGroups>& GetRawMap(){return rawMap;}
	std::unordered_map<uint16_t, BurstGroups>& GetTrainset(){return trainset;}
	std::unordered_map<uint16_t, BurstGroups>& GetValiset(){return valiset;}
	std::unordered_map<uint16_t, BurstGroups>& GetTestset(){return testset;}
	std::set<size_t>& GetTimeSlots(){return timeSlots;}

	void SetSlotDuration(int duration){configBurstDataset.slotDuration = duration;}
	void SetBurstTrh(const BurstTrh& trh){this->configBurstDataset.burstTrh = trh;}
	void SetName(const std::string name){this->name = name;}
	
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version) {
		ar& name;
		ar& configBurstDataset;
		ar& devicesVec;
		ar& timeSlots;
		ar& rawMap;
	}

private:
	void AddPacket(uint16_t deviceId, time_t slotId, const KPacket* packet);
    void MakeBursts();	

private:
	std::string name;
	ConfigBurstDataset configBurstDataset;
    std::vector<groundnut::KDevice> devicesVec; // should Add constructor !

	std::unordered_map<uint16_t, BurstGroups> rawMap;
	std::unordered_map<uint16_t, BurstGroups> trainset;
	std::unordered_map<uint16_t, BurstGroups> valiset;
	std::unordered_map<uint16_t, BurstGroups> testset;

	std::set<size_t> timeSlots;
	std::unordered_map<short, std::map<time_t, std::vector<const KPacket*>>> mapByDevTime;

	static constexpr int kDeviceMutexCount = 16;
    std::array<std::mutex, kDeviceMutexCount> mapDevMutexes;
    std::array<std::mutex, kDeviceMutexCount> blockMutexes;
};

}
#endif