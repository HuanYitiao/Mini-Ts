#pragma once

#include <array>
#include <cstdint>
#include <optional>
#include <span>
#include <string>

namespace ts
{
    inline constexpr std::size_t kPacketSize  = 188;
    inline constexpr std::size_t kHeaderSize  = 4;
    inline constexpr std::size_t kPayloadSize = 184;
    inline constexpr uint8_t     kSyncByte    = 0x47;

    inline constexpr uint16_t kPidPat  = 0x0000;
    inline constexpr uint16_t kPidNull = 0x1FFF;

    class TSPacket
    {
      public:
        TSPacket() = default;

        using RawData = std::array<uint8_t, kPacketSize>;

        static std::optional<TSPacket> fromBytes(std::span<const uint8_t> data);
        static TSPacket                make(uint16_t pid, uint8_t continuityCounter, std::span<const uint8_t> payload);

        [[nodiscard]] uint8_t  getSyncByte() const;
        [[nodiscard]] bool     getTransportError() const;
        [[nodiscard]] bool     getPayloadUnitStart() const;
        [[nodiscard]] bool     getTransportPriority() const;
        [[nodiscard]] uint16_t getPid() const;
        [[nodiscard]] uint8_t  getScramblingControl() const;
        [[nodiscard]] uint8_t  getAdaptationFieldControl() const;
        [[nodiscard]] uint8_t  getContinuityCounter() const;

        void setPid(uint16_t pid);
        void setContinuityCounter(uint8_t cc);
        void setTransportError(bool flag);
        void setPayloadUnitStart(bool flag);

        [[nodiscard]] const RawData& raw() const { return data_; }
        [[nodiscard]] RawData&       rawMut() { return data_; }

        [[nodiscard]] std::span<const uint8_t> payload() const;

        [[nodiscard]] bool isValid() const { return getSyncByte() == kSyncByte; }

        [[nodiscard]] std::string headerString() const;

      private:
        RawData data_ {};
    };
}  // namespace ts
