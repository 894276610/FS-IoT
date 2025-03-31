#include "kburst.h"
#include "kdevice.h"
#include "kpacket.h"
#include "time-utils.h"
#include "distance-utils.h"
#include <tuple>
#include <cmath>

namespace groundnut{

KBurst::KBurst(const KDevice& device): KBurst(device.GetLabel()){pfUmapMutex = new std::shared_mutex;}

void KBurst::convert_to_frequency()
{   
    if (this->pktNum == 0) {return; }

	std::unique_lock lock(*pfUmapMutex);

	fUmap.reserve(countMap.size());

    for (const auto& [sig, cnt] : countUmap) {

		fUmap.try_emplace(sig, static_cast<float>(cnt) / pktNum);
    }

	isfUmapUpdate = true;
}

float KBurst::Distance(std::shared_ptr<KBurst> burst)
{
	float distance = 0;

	if (pktNum == 0 || burst->pktNum == 0) { return 1.0f; }
	if (std::abs(burst->pktNum - pktNum) > 120)	{return 1.0f;}

	if(!isfUmapUpdate)
	{
		convert_to_frequency();
	}

	if(!burst->isfUmapUpdate)
	{
		burst->convert_to_frequency();
	}

	std::shared_lock lock1(*pfUmapMutex, std::defer_lock);
	std::shared_lock lock2(*burst->pfUmapMutex, std::defer_lock);
	std::lock(lock1, lock2); // 避免死锁
	
	//auto tmpMap = burst->fUmap; // 受锁保护的拷贝

	return hellinger(fUmap, burst->fUmap);
}

float KBurst::Distance(KBurst& burst)
{
	float distance = 0;

	if (pktNum == 0 || burst.pktNum == 0) { return 1.0f; }
	if (std::abs(burst.pktNum - pktNum) > 120)	{return 1;}

	if(!isfUmapUpdate)
	{
		convert_to_frequency();
	}

	if(!burst.isfUmapUpdate)
	{
		burst.convert_to_frequency();
	}

	std::shared_lock lock1(*pfUmapMutex, std::defer_lock);
	std::shared_lock lock2(*burst.pfUmapMutex, std::defer_lock);
	std::lock(lock1, lock2); // 避免死锁

	return hellinger(fUmap, burst.fUmap);
}

void KBurst::ExtendBurst(KBurst& burst)
{
	for (auto& [packetSig, num] : burst.countMap)
	{
		// packet count and distribution
		pktNum+=num;
		
		auto [it, isSuccess] = countMap.try_emplace(packetSig, num);
		if(!isSuccess){ 
			it->second += num;
		}
		else
		{
			uniPktNum++;
		}

		// update first and last packet
		if(burst.firstPktStamp < firstPktStamp || firstPktStamp == timespec{0,0})
		{
			firstPktStamp = burst.firstPktStamp;
		}
	
		if(lastPktStamp < burst.lastPktStamp  || lastPktStamp == timespec{0,0})
		{
			lastPktStamp = burst.lastPktStamp;
		}
	}

	isfUmapUpdate = false;
}

KBurst::KBurst(std::vector<std::shared_ptr<KBurst>> burstVec)
{
	pfUmapMutex = new std::shared_mutex;

	if (!burstVec.size()){return;}
	
	label = burstVec[0]->label;

	for (auto& burst : burstVec)
	{
		ExtendBurst(*burst);
	}
	
}

void KBurst::AddPacket(const KPacket& packet)
{	
	// update distribution and packet count
	auto [it, inserted] = countMap.try_emplace(packet.signedLen, 0);
    it->second++;  
    
	auto [uit, uinserted] = countUmap.try_emplace(packet.signedLen, 0);
	uit->second++;

	pktNum++;
	if(inserted){uniPktNum++;}

	// update first/last timestamp
	if(packet.timestamp < firstPktStamp || firstPktStamp == timespec{0,0})
	{
		firstPktStamp = packet.timestamp;
	}

	if(lastPktStamp < packet.timestamp  || lastPktStamp == timespec{0,0})
	{
		lastPktStamp = packet.timestamp;
	}

	isfUmapUpdate = false;
}

bool KBurst::operator<(const KBurst& other) const
{
	return firstPktStamp < other.firstPktStamp;
}

bool KBurst::operator==(const KBurst& other) const
{
	return isEqualDistribution(other);
}

bool KBurst::isEqualDistribution(const KBurst& other) const {
		
	return 	(uniPktNum == other.uniPktNum) &&
			(pktNum == other.pktNum) &&
			(countMap == other.countMap);
}

std::string KBurst::ToString() const {
    std::stringstream ss;
    
    ss << "Label=" << this->label
	   << ",Uni="  << this->uniPktNum  
       << ",PktNum=" << this->pktNum
       << ",Details=";

    for (const auto [signedLen, times]: countMap) {
        ss << "(" << signedLen << "X" << times << ")";
    }
    
    return ss.str();
}

}