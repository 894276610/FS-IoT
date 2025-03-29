#include "burstMaker.h"
#include <algorithm>
#include <utility>
#include "kdevice.h"

namespace groundnut{

void BurstMaker::LongShortSplit() 
{
    PacketVector rawBurst;
    std::set<int> uniPktSet;

    for (auto* packet : *m_pktVec) {
        if (!packet) continue;

        if (!rawBurst.empty()) {
            const auto& prevPacket = rawBurst.back();
            const timespec innerDelta = packet->timestamp - prevPacket->timestamp;
            const timespec outerDelta = packet->timestamp - rawBurst.front()->timestamp;
             
            // Handle time interval condition
            if (m_trh.inTrh < innerDelta) {
                HandleRawBurst(rawBurst, uniPktSet, false);
            }
            else if (m_trh.ouTrh < outerDelta) {
                HandleRawBurst(rawBurst, uniPktSet, (true && m_trh.longShortEnable));
            }
            else if((uniPktSet.size() == m_trh.uniTrh) && uniPktSet.find(packet->signedLen) == uniPktSet.end()){
                HandleRawBurst(rawBurst, uniPktSet, (true && m_trh.longShortEnable));
            }
        }

        rawBurst.emplace_back(packet);  
        uniPktSet.insert(packet->signedLen);      
    }

    HandleRawBurst(rawBurst, uniPktSet, false);
}

// Helper function implementations
void BurstMaker::HandleRawBurst(PacketVector& rawBurst, std::set<int>& uniPktSet, bool isLongBurst)
{
    if (rawBurst.empty()) return;
    
    if (isLongBurst) {
        m_longBurstVec.emplace_back(std::move(rawBurst));
    } else {
        m_shortBurstVec.emplace_back(std::move(rawBurst));
    }
    rawBurst.clear();
    uniPktSet.clear();
}

void BurstMaker::LongBurstSplit()
{  
    // separate
    for (auto& burst : m_longBurstVec) {
        for (auto* packet : burst) {
            if (packet) {
                m_flowMap[packet->hash5tuple].emplace_back(packet);
            }
        }
    }
}

void BurstMaker::ConstructBursts()
{
    // Process short bursts
    for (const auto& burst : m_shortBurstVec) {
        auto bursts = CutoBursts(burst);
        m_burstVec.insert(m_burstVec.end(), 
                        std::make_move_iterator(bursts.begin()),
                        std::make_move_iterator(bursts.end()));
    }

    // Process long bursts
    for (auto&& [hash, packets] : m_flowMap) {
        auto bursts = CutoBursts(std::move(packets));
        m_burstVec.insert(m_burstVec.end(),
                        std::make_move_iterator(bursts.begin()),
                        std::make_move_iterator(bursts.end()));
    }
}

BurstVec BurstMaker::CutoBursts(PacketVector packetVec) const
{
    BurstVec burstVec;
    auto burst = std::make_shared<KBurst>(m_device);

    for (const auto* packet : packetVec) {
        if (!packet) continue;

        // Lambda for burst finalization conditions
        const bool shouldFinalize = [&]() {
            if (burst->GetPktNum() == 0) return false;
            
            const timespec innerDelta = packet->timestamp - burst->GetLastPktStamp();
            const timespec outerDelta = packet->timestamp - burst->GetFirstPktStamp();
            
            return (burst->GetUniPktNum() > m_trh.uniTrh ) ||
                   ((burst->GetUniPktNum() == m_trh.uniTrh) && 
                     burst->GetCountMap().find(packet->signedLen) == burst->GetCountMap().end()) ||
                   (m_trh.inTrh < innerDelta) || 
                   (m_trh.ouTrh < outerDelta);
        }();

        if (shouldFinalize) {
            burstVec.emplace_back(std::move(burst));
            burst = std::make_shared<KBurst>(m_device);
        }

        burst->AddPacket(*packet);
    }

    if (burst->GetPktNum() > 0) {
        burstVec.emplace_back(std::move(burst));
    }

    return burstVec;
}

void BurstMaker::SortInOrder()
{
    std::sort(m_burstVec.begin(), m_burstVec.end(), BurstTimeComparator);
}

}