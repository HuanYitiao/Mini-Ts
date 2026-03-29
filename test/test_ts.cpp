#include "ts_packet.hpp"
#include <gtest/gtest.h>

using namespace ts;

TEST(TSPacket, MakeAndReadHeader)
{
    std::vector<uint8_t> payload(100, 0xAB);

    auto pkt = TSPacket::make(0x0100, 5, payload);

    EXPECT_EQ(pkt.getSyncByte(), kSyncByte);
    EXPECT_EQ(pkt.getPid(), 0x0100);
    EXPECT_EQ(pkt.getContinuityCounter(), 5);
    EXPECT_FALSE(pkt.getTransportError());
    EXPECT_FALSE(pkt.getPayloadUnitStart());
    EXPECT_TRUE(pkt.isValid());
}

TEST(TSPacket, TestFromBytes)
{
    std::vector<uint8_t> payload(100, 0xAB);

    auto pkt    = TSPacket::make(0x0100, 5, payload);
    auto result = TSPacket::fromBytes(pkt.raw());

    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(result->getPid(), 0x0100);
    EXPECT_EQ(result->getContinuityCounter(), 5);
}

TEST(TSPacket, SetFlags)
{
    std::vector<uint8_t> payload(100, 0xAB);

    auto pkt = TSPacket::make(0x0100, 0, payload);

    pkt.setTransportError(true);
    EXPECT_TRUE(pkt.getTransportError());

    pkt.setPayloadUnitStart(true);
    EXPECT_TRUE(pkt.getPayloadUnitStart());
}

TEST(TSPacket, FromBytesInvalidSync)
{
    std::array<uint8_t, 188> badData {};
    badData[0]  = 0x00;
    auto result = TSPacket::fromBytes(badData);
    EXPECT_FALSE(result.has_value());
}