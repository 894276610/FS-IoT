#ifndef KDEVICE_H
#define KDEVICE_H

#include "string-utils.h"
#include <string>
#include <unordered_set>
#include "pcpp-common.h"
#include "boost-serialize.h"

// The implementation is not thread-safe
// Please use single thread to create kdevice

// Constructor:
// label: like AmazonEcho, a specific device model label
// The label is not unique, the devId is unique


namespace boost {
	namespace serialization {
	template <class Archive>
	void serialize(Archive& ar, pcpp::MacAddress& obj, const unsigned int version) {

		ar & boost::serialization::make_array(obj.getRaw(), 6);
	}
	}
}

namespace groundnut{

class KDevice
{
public:

	KDevice();
	KDevice(std::string label, pcpp::MacAddress macAddress);	
	const std::string& GetLabel() const { return m_label; }
    const pcpp::MacAddress& GetAddr() const { return m_mac; }
    uint16_t GetDevId() const { return devId; }

	template<class Archive>
	void serialize(Archive& ar, const unsigned int version) {
		ar& devId;
		ar& m_label;
		ar& m_mac;
	}
	
private:
	uint16_t devId = 0;
	std::string m_label = "";
	pcpp::MacAddress m_mac;
	inline static int devIdCounter = 0;
};

}
#endif