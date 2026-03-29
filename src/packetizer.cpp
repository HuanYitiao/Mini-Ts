#include "packetizer.hpp"

namespace ts
{
    Packetizer::Packetizer(PacketizerConfig config) : config_(config)
    {
    }

    std::vector<TSPacket> Packetizer::packetize(std::span<const uint8_t> data) const
    {
        std::vector<TSPacket> packets;
        if (data.empty())
            return packets;

        uint8_t     cc     = 0;
        std::size_t offset = 0;

        while (offset < data.size())
        {
            auto chunkSize = std::min(kPayloadSize, data.size() - offset);
            auto chunk     = data.subspan(offset, chunkSize);

            auto pkt = TSPacket::make(config_.pid, cc, chunk);

            if (offset == 0 && config_.markFirstPacket)
            {
                pkt.setPayloadUnitStart(true);
            }

            packets.push_back(pkt);

            cc = (cc + 1) & 0x0F;
            offset += chunkSize;
        }

        return packets;
    }
}  // namespace ts