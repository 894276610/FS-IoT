#ifndef KPACKET_H
#define KPACKET_H

#include "types.h"
#include "time-utils.h"

#include <ctime>

namespace boost::serialization {
	template<class Archive>
	void serialize(Archive& ar, timespec& t, const unsigned int version) {
		ar& t.tv_sec;
		ar& t.tv_nsec;
		if(version >0)
		{}
	} 
} // namespace boost 

namespace groundnut{
	
struct KPacket
{
	uint16_t deviceId = 0;
	short signedLen = 0;
	uint32_t hash5tuple = 0;
	timespec timestamp = {0,0};

	KPacket() = default;
	KPacket(short deviceId, short signedLen, uint32_t chash5tuple, timespec timestamp);
	bool operator<(const KPacket& k2) const;

	template <class Archive>
    void serialize(Archive &ar, const unsigned int version){

		ar& deviceId;
		ar& signedLen;
		ar& hash5tuple;
		ar& timestamp;
		if(version >0)
		{}
	}
};
}
#endif