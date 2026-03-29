#include "ts_packet.hpp"

#include <algorithm>
#include <cstdio>

namespace ts
{
    uint8_t TSPacket::getSyncByte() const
    {
        return data_[0];
    }

    bool TSPacket::getTransportError() const
    {
        return (data_[1] & 0x80) != 0;
    }

    bool TSPacket::getPayloadUnitStart() const
    {
        return (data_[1] & 0x40) != 0;
    }

    bool TSPacket::getTransportPriority() const
    {
        return (data_[1] & 0x20) != 0;
    }

    uint16_t TSPacket::getPid() const
    {
        return ((data_[1] & 0x1F) << 8) | data_[2];
    }

    uint8_t TSPacket::getScramblingControl() const
    {
        return (data_[3] & 0xC0) >> 6;
    }

    uint8_t TSPacket::getAdaptationFieldControl() const
    {
        return (data_[3] & 0x30) >> 4;
    }

    uint8_t TSPacket::getContinuityCounter() const
    {
        return data_[3] & 0x0F;
    }

    void TSPacket::setPid(uint16_t pid)
    {
        data_[1] = (data_[1] & 0xE0) | ((pid >> 8) & 0x1F);
        data_[2] = pid & 0xFF;
    }

    void TSPacket::setContinuityCounter(uint8_t cc)
    {
        data_[3] = (data_[3] & 0xF0) | (cc & 0x0F);
    }

    void TSPacket::setTransportError(bool flag)
    {
        if (flag)
        {
            data_[1] |= 0x80;
        }
        else
        {
            data_[1] &= ~0x80;
        }
    }

    void TSPacket::setPayloadUnitStart(bool flag)
    {
        if (flag)
        {
            data_[1] |= 0x40;
        }
        else
        {
            data_[1] &= ~0x40;
        }
    }

    TSPacket TSPacket::make(uint16_t pid, uint8_t cc, std::span<const uint8_t> payload)
    {
        TSPacket pkt;
        pkt.data_.fill(0xFF);
        pkt.data_[0] = kSyncByte;
        pkt.data_[1] = 0x00;
        pkt.data_[2] = 0x00;
        pkt.data_[3] = 0x00;

        pkt.setPid(pid);
        pkt.setContinuityCounter(cc);
        pkt.data_[3] = (pkt.data_[3] & 0xCF) | 0x10;

        auto len = std::min(payload.size(), kPayloadSize);
        std::copy_n(payload.begin(), len, pkt.data_.begin() + kHeaderSize);

        return pkt;
    }

    std::optional<TSPacket> TSPacket::fromBytes(std::span<const uint8_t> data)
    {
        if (data.size() < kPacketSize)
        {
            return std::nullopt;
        }
        TSPacket pkt;
        std::copy_n(data.begin(), kPacketSize, pkt.data_.begin());
        if (!pkt.isValid())
        {
            return std::nullopt;
        }
        return pkt;
    }

    std::span<const uint8_t> TSPacket::payload() const
    {
        return std::span<const uint8_t>(data_.data() + kHeaderSize, kPayloadSize);
    }

    std::string TSPacket::headerString() const
    {
        char buf[128];
        std::snprintf(
                buf,
                sizeof(buf),
                "sync=0x%02X PID=0x%04X CC=%u TEI=%d PUSI=%d AFC=%u",
                getSyncByte(),
                getPid(),
                getContinuityCounter(),
                getTransportError(),
                getPayloadUnitStart(),
                getAdaptationFieldControl());
        return std::string(buf);
    }

}  // namespace ts
