#pragma once
#include "ts_packet.hpp"
#include <cstdint>
#include <span>
#include <vector>

namespace ts
{
    struct ParseResult
    {
        std::vector<uint8_t> restoredData;
        std::size_t          packetsParsed    = 0;
        std::size_t          syncLosses       = 0;
        std::size_t          continuityErrors = 0;
    };

    class Parser
    {
      public:
        [[nodiscard]] ParseResult parse(std::span<const uint8_t> data, uint16_t extractPid = 0x0100) const;

      private:
        static std::size_t findSync(std::span<const uint8_t> data, std::size_t offset);
    };

}  // namespace ts