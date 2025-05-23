
#ifndef _KBURST_H_
#define _KBURST_H_

#include "types.h"
#include "boost-serialize.h"
#include "time-utils.h"

#include <fstream>
#include <iostream>
#include <vector>
#include <memory>
#include <filesystem>
#include <shared_mutex>
#include <iomanip>
#include <mutex>

#include <optional>

namespace groundnut{

class KDevice;
class KPacket;

struct BurstTrh{
	int uniTrh = 1000;
	double inTrhF = 2.0;
	double ouTrhF = 15.0;
	bool longShortEnable = true;

	template<class Archive>
	void serialize(Archive& ar, const unsigned int version) {
		ar& uniTrh;
		ar& inTrhF;
		ar& ouTrhF;
		ar& longShortEnable;
	}

	std::string ToString() const
	{
		std::stringstream ss;
		ss << "(uniTrh=" << uniTrh << ")";
		ss << "(inTrh=" <<  std::fixed << std::setprecision(1)  << inTrhF << "s)";
		ss << "(ouTrh=" <<  std::fixed << std::setprecision(1)  << ouTrhF << "s)";
		ss << "(lsenable=" << longShortEnable << ")";
		return ss.str();
	}
};

class KBurst {

public:

	// constructor
	KBurst():label(""){pfUmapMutex = new std::shared_mutex;}
	KBurst(const KDevice& device);
	explicit KBurst(const std::string& label):label(label){ pfUmapMutex = new std::shared_mutex;}
	KBurst(std::vector<std::shared_ptr<KBurst>> burstVec);
	
	void AddPacket(const KPacket& packet);
	void ExtendBurst(KBurst& burst);

	// output view
	std::string ToString() const;

	// flag
	volatile bool isfUmapUpdate = false;
	
	// distance
	float Distance(KBurst& burst);
	float Distance(std::shared_ptr<KBurst> burst);

    // operator
	bool operator <(const KBurst &other) const;
	bool operator ==(const KBurst& other) const;
	bool isEqualDistribution(const KBurst&) const;

	// serialize
    template<class Archive>
	void serialize(Archive& ar, const unsigned int version) {
		ar& label;
		ar& pktNum;
		ar& uniPktNum;
		ar& firstPktStamp;
		ar& lastPktStamp;
		ar& countMap;
	}

	// getter
	std::string GetLabel() const {return this->label;}
	int GetUniPktNum() const {return this->uniPktNum;}
	int GetPktNum() const {return this->pktNum;}
	const timespec& GetFirstPktStamp() const {return firstPktStamp;}
	const timespec& GetLastPktStamp() const{return lastPktStamp;}
	const std::map<short, int>& GetCountMap() const{return countMap;}
	const std::unordered_map<short, int>& GetCountUmap(){return countUmap;}

	// conversion
	void convert_to_frequency();

private:
	int uniPktNum = 0;
	int pktNum = 0;
	std::string label = "";
	timespec firstPktStamp = {0,0};
	timespec lastPktStamp  = {0,0};
	std::map<short, int> countMap;
	std::unordered_map<short, int> countUmap;
	std::unordered_map<short, float> fUmap;

	std::shared_mutex* pfUmapMutex = nullptr;
};

typedef std::vector<std::shared_ptr<KBurst>> BurstVec;
typedef std::vector<BurstVec> BurstGroups;
typedef std::unordered_map<std::shared_ptr<KBurst>, int> BPCountMap;
}

template<>
struct std::hash<groundnut::KBurst>
{
	std::size_t operator()(const groundnut::KBurst& p) const {

		std::size_t seed = std::hash<int>{}(p.GetUniPktNum());
        seed ^= std::hash<uint32_t>{}(p.GetPktNum()) + 0x9e3779b9 + (seed << 6) + (seed >> 2);

		for ( auto& [signedLen, times] : p.GetCountMap())
		{
			std::size_t hash_val = std::hash<int>{}(signedLen);
            hash_val = hash_val * 0x27d4eb2d + times; // 质数乘法
            seed ^= hash_val + 0x9e3779b9 + (seed << 6) + (seed >> 2);
		}

		return seed;
	}
};

namespace groundnut{

struct DivMetric
{
	std::string name = "";
	
	// result of burst division rules
	float repeatRate = 0;  
	float entropy = 0;
	float normEntropy = 0;

	// intrinstic attributes
	float burstRate = 0; // pkt/s for 2+ bursts
	float diversity = 0; // |D|

	int uniBurstNum = 0;	  // totalBurstNumber
	int burstNum = 0;	 	  // totalBurstNumber

	std::string ToCsvString()
	{
		std::stringstream ss;
		ss << name << "," << repeatRate << "," << entropy << "," << burstRate << "," << diversity << "," << normEntropy << "," << uniBurstNum << "," << burstNum;
		return ss.str();
	}
	std::string ToString()
	{
		std::stringstream ss;
		ss << name << " traffic division metric:" << std::endl;
		ss << "repeat-rate:" << repeatRate << std::endl;
		ss << "entropy:" << entropy << std::endl;
		ss << "burstRate:" << burstRate << std::endl;
		ss << "diversity:" << diversity << std::endl;
		ss << "normEntropy:" << normEntropy << std::endl;
		ss << "uniBurstNum:" << uniBurstNum << std::endl;
		ss << "burstNum:" << burstNum << std::endl;
		return ss.str();
	}
};

struct DivResult
{
	std::vector<DivMetric> results;

	void AddDivMetric(const DivMetric& metric)
	{
		results.push_back(metric);
	}

	void SaveCsv(const std::filesystem::path& outPath)
	{
		if(!std::filesystem::exists(outPath.parent_path()))
        {
            std::filesystem::create_directories(outPath.parent_path());
        }

		std::ofstream ofs(outPath);

		ofs << ToCsvHeader() << std::endl;

		for(auto& result: results)
		{
			ofs << result.ToCsvString() << std::endl;
		}

		ofs.close();

        std::cout << "Output to " << outPath << std::endl;
	}

	std::string ToCsvHeader()
	{
		std::stringstream ss;
		ss << "name,repeatRate,entropy,burstRate,diversity,normEntropy,uniBurstNum,burstNum";
		return ss.str();
	}
};


struct SearchResult
{
	float minDistance = 100;
	BurstVec nearTrainBursts;

	std::string ToString()
	{
		std::stringstream ss;
		ss << "search result: ";
		ss << "min distance:" << minDistance << "\n";
		ss << "nearest bursts:" << "\n";
		for(auto& burst: nearTrainBursts)
		{
			ss << burst->ToString() << "\n";
		}
		return ss.str();
	}
};
	
// burst cache
struct BurstCache {

	BurstCache() = default; 

	std::shared_mutex mutex;
    std::unordered_map<KBurst, SearchResult> cache;

    std::optional<SearchResult> Read(const KBurst& key) {
        std::shared_lock lock(mutex);
        auto it = cache.find(key);
        return it != cache.end() ? std::make_optional(it->second) : std::nullopt;
    }

    void Write(const KBurst& key, SearchResult value) {
        std::unique_lock lock(mutex);
        cache.insert_or_assign(key, std::move(value));
    }
};

}

#endif
