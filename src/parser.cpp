#include "parser.hpp"

namespace ts
{
    std::size_t Parser::findSync(std::span<const uint8_t> data, std::size_t offset)
    {
        while (offset < data.size())
        {
            if (data[offset] == kSyncByte)
            {
                auto next = offset + kPacketSize;
                if (next >= data.size() || data[next] == kSyncByte)
                {
                    return offset;
                }
            }
            ++offset;
        }
        return data.size();
    }

    ParseResult Parser::parse(std::span<const uint8_t> data, uint16_t extractPid) const
    {
        ParseResult result;
        std::size_t offset = 0;
        int8_t      lastCC = -1;

        while (offset + kPacketSize <= data.size())
        {
            if (data[offset] != kSyncByte)
            {
                auto newOffset = findSync(data, offset);
                if (newOffset >= data.size())
                {
                    break;
                }
                result.syncLosses++;
                offset = newOffset;
            }

            auto pktOpt = TSPacket::fromBytes(data.subspan(offset, kPacketSize));
            if (!pktOpt)
            {
                offset++;
                continue;
            }
            const auto& pkt = *pktOpt;
            result.packetsParsed++;

            if (pkt.getPid() == extractPid)
            {
                auto payload = pkt.payload();
                result.restoredData.insert(result.restoredData.end(), payload.begin(), payload.end());
            }

            if (lastCC >= 0)
            {
                uint8_t expected = (static_cast<uint8_t>(lastCC) + 1) & 0x0F;
                if (pkt.getContinuityCounter() != expected)
                {
                    result.continuityErrors++;
                }
            }
            lastCC = static_cast<int8_t>(pkt.getContinuityCounter());

            offset += kPacketSize;
        }

        return result;
    }

};  // namespace ts