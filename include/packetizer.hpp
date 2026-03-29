#pragma once

#include "ts_packet.hpp"

#include <cstdint>
#include <filesystem>
#include <string>
#include <vector>

namespace ts
{
    struct PacketizerConfig
    {
        uint16_t pid             = 0x0100;
        bool     markFirstPacket = true;
    };

    class Packetizer
    {
      public:
        explicit Packetizer(PacketizerConfig config = {});

        [[nodiscard]] std::vector<TSPacket> packetize(std::span<const uint8_t> data) const;

        std::size_t packetizeFile(const std::filesystem::path& input, const std::filesystem::path& output) const;

      private:
        PacketizerConfig config_;
    };

}  // namespace ts