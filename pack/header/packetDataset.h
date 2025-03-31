#ifndef _GROUNDNUT_PACKET_DATASET_H_
#define _GROUNDNUT_PACKET_DATASET_H_

#include <mutex>
#include <iostream>
#include <vector>
#include <sstream>
#include <set>
#include <filesystem>
#include "kpacket.h"
#include "pcpp-common.h"
#include "serialize-utils.h"
#include "kdevice.h"
#include <map>
#include <shared_mutex>
#include <future>

namespace groundnut{

struct DeviceStat
{
	std::string instanceName;
	std::atomic<uint64_t> totalSend{0};
    std::atomic<uint64_t> totalRecv{0};

	DeviceStat(std::string name): 
		instanceName(name){ }

	// 显式定义移动构造函数
	DeviceStat(DeviceStat&& other) noexcept 
		: instanceName(other.instanceName),
		  totalSend(other.totalSend.load(std::memory_order_relaxed)),
		  totalRecv(other.totalRecv.load(std::memory_order_relaxed)) {
	      // 其他成员的移动操作（如果有）
	}

	DeviceStat(const DeviceStat&) = delete;
    DeviceStat& operator=(const DeviceStat&) = delete;

	std::string ToString()
	{
		std::stringstream ss;
		ss << instanceName << ","
			<< totalSend << ","
			<< totalRecv;
		return ss.str();
	}
};

struct DatasetStat
{
	long totalPktNum = 0L;
	long usefulPktNum = 0L;
	std::vector<DeviceStat> devStat;

	std::string ToString()
	{
		std::stringstream ss;
		ss << "name,totalSend,totalRecv" << std::endl;

		for (size_t i = 0; i < devStat.size(); i++)
		{
			ss << devStat[i].ToString() << std::endl;
		}

		ss << "totalPktNum, usefulPktNum" << std::endl;
		ss << totalPktNum << "," << usefulPktNum << std::endl;
		return ss.str();
	}
};

class PacketDataset
{
public:
	inline PacketDataset(const std::string& datasetName):name(datasetName)
	{
	}

	// 选择性载入
	void AutoLoad(const std::string& datasetRootFolder, const std::string& pktDatasetOutName);

	// load
    void LoadPcap(const std::filesystem::path& inputFolder);

    // getter and setter
	inline const std::string GetName(){return name;}
	inline std::set<KPacket>& GetDataset(){return dataset;}
	inline const std::unordered_map<std::string, groundnut::KDevice> & GetDevicesMap(){return this->devicesMap;}
    inline const DatasetStat& GetDatasetStat(){return stat;}
    inline void SetDatasetName(const std::string& datasetName){this->name = datasetName;}
    inline void AddTragetDevice(pcpp::MacAddress mac, std::string label)
    {
		stat.devStat.emplace_back(label);
        devicesMap.try_emplace(mac.toString(), label, mac);
    }

    void AddTragetDevices(std::filesystem::path mapping_mac_path);


    // serialize
    inline void Serialize(const std::filesystem::path& outPath)
    {
        groundnut::Serialize(dataset, outPath);
    } 
	
	inline void LoadBin(const std::filesystem::path& inPath)
	{
		groundnut::LoadBin(dataset, inPath);
	}
	
private:   
    void GarbageCollection(int taskNum);	
    void StoreKPackets(int index, std::vector<pcpp::RawPacket*> rawVec, const std::string& datasetName);
    void AddPacket(short deviceId, short signedLen, uint32_t hash5tuple, timespec timestamp);
    bool AddPacket(const std::string& addr, bool isSource, pcpp::RawPacket* pRawPacket);
    int  safeGetDevId(const std::string& addr);
    void OpenReader(pcpp::IFileReaderDevice* reader);
    void CloseReader(pcpp::IFileReaderDevice* reader);

// catch2 test only getter
public:
	std::vector<pcpp::RawPacketVector*> GetRawPacketVec(){return rawPacketVec;}

private:
	std::string name;
	std::set<KPacket> dataset;
	DatasetStat stat;
    std::unordered_map<std::string, groundnut::KDevice> devicesMap; // mac string, kdevice

	std::mutex pktDatasetMutex;
    mutable std::shared_mutex addressMapMutex;
    std::vector<std::future<void>> storeFutures;   
    std::vector<pcpp::RawPacketVector*> rawPacketVec;
	std::condition_variable gcNotice;
	std::mutex gcMutex;
};

}
#endif