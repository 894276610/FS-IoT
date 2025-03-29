#ifndef _GROUNDNUT_BURST_MAKER_H_
#define _GROUNDNUT_BURST_MAKER_H_

#include <vector>
#include <memory>
#include <unordered_map>
#include <tuple>
#include "kpacket.h"
#include "kburst.h"
#include "kdevice.h"

namespace groundnut{

class BurstMaker {
public:
    using PacketVector = std::vector<const KPacket*>;
    using BurstContainer = std::vector<PacketVector>;
    using BurstVec = std::vector<std::shared_ptr<KBurst>>;

    BurstMaker(const PacketVector* pktVec, KDevice& device, const BurstTrh& trh)
        : m_device(device), m_pktVec(pktVec), m_trh(trh)
    {
    }

    BurstVec MakeBursts()
    {
        LongShortSplit();
        LongBurstSplit();
        ConstructBursts();
        SortInOrder();
        return m_burstVec;
    }

    BurstVec GetBurstVec() const & { return m_burstVec; }
    BurstVec&& GetBurstVec() && { return std::move(m_burstVec); }

private:
    // Core processing functions
    void LongShortSplit();
    void LongBurstSplit();
    void ConstructBursts();
    void SortInOrder();

    BurstVec CutoBursts(PacketVector packetVec) const;
    void HandleRawBurst(PacketVector& rawBurst, std::set<int>& uniPktSet, bool isLongBurst);

    inline static bool BurstTimeComparator(const std::shared_ptr<KBurst>& a,
        const std::shared_ptr<KBurst>& b)
    {
        return (*a) < (*b);
    }

private:

    // Member variables
    KDevice& m_device;
    const PacketVector* m_pktVec;
    const BurstTrh m_trh;
    
    BurstContainer m_longBurstVec;
    BurstContainer m_shortBurstVec;
    std::unordered_map<size_t, PacketVector> m_flowMap;
    BurstVec m_burstVec;
};

}

#endif