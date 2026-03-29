#include "packetizer.hpp"
#include "parser.hpp"
#include <cstring>
#include <fstream>
#include <iostream>
#include <vector>

int main(int argc, char* argv[])
{
    if (argc < 3)
    {
        std::cerr << "Usage: ts-mini round <input_file>\n";
        return 1;
    }

    const auto* cmd   = argv[1];
    const char* input = argv[2];

    if (std::strcmp(cmd, "round") == 0)
    {
        std::ifstream        ifs(input, std::ios::binary);
        auto                 fileSize = std::filesystem::file_size(input);
        std::vector<uint8_t> buffer(fileSize);
        ifs.read(reinterpret_cast<char*>(buffer.data()), fileSize);

        ts::Packetizer packer;
        auto           packets = packer.packetize(buffer);

        std::vector<uint8_t> stream;
        for (const auto& pkt : packets)
        {
            stream.insert(stream.end(), pkt.raw().begin(), pkt.raw().end());
        }
        ts::Parser parser;
        auto       result = parser.parse(stream);

        bool match = (result.restoredData.size() >= buffer.size())
                     && std::equal(buffer.begin(), buffer.end(), result.restoredData.begin());
        std::cout << "Original file: " << input << " (" << fileSize << " bytes)\n\n";
        std::cout << "[Pack]  " << packets.size() << " packets generated\n";
        std::cout << "[Pack]  TS stream: " << stream.size() << " bytes\n\n";
        std::cout << "[Parse] Packets parsed: " << result.packetsParsed << "\n";
        std::cout << "[Parse] Sync losses: " << result.syncLosses << "\n";
        std::cout << "[Parse] CC errors: " << result.continuityErrors << "\n\n";
        std::cout << "[Verify] Original size:  " << buffer.size() << " bytes\n";
        std::cout << "[Verify] Restored size:  " << result.restoredData.size() << " bytes\n";
        std::cout << "[Verify] Data integrity: " << (match ? "PASS" : "FAIL") << "\n";
    }
    else
    {
        std::cerr << "Unknown command: " << cmd << "\n";
        return 1;
    }
    return 0;
}