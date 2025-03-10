#include "kdevice.h"

namespace groundnut{

KDevice::KDevice():
devId(devIdCounter++), m_label(""),m_mac()
{}

KDevice::KDevice(std::string label, pcpp::MacAddress macAddress):
devId(devIdCounter++), m_label(std::move(label)), m_mac(std::move(macAddress))
{}

}