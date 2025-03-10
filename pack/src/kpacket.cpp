#include "kpacket.h"
#include "time-utils.h"

namespace groundnut{

// Full packet
KPacket::KPacket(short deviceId, short signedLen, uint32_t chash5tuple, timespec timestamp):
   deviceId(deviceId), signedLen(signedLen), hash5tuple(chash5tuple), timestamp(timestamp) {}

// Time Comparison
bool KPacket::operator<(const KPacket& other) const {
	return this->timestamp < other.timestamp;
}
}