#pragma once


#include "Features.h"
#include "MaskFilter.h"
#include "RxPacket.h"
#include <cstdint>


struct UDPFlow
{
    UDPFlow(IPv4Address src_ip, IPv4Address dst_ip, uint16_t src_port, uint16_t dst_port);

    bool match(RxPacket packet, uint32_t l3_offset)
    {
        return mFilter.match(packet.data() + l3_offset);
    }

    void accept(RxPacket packet)
    {
        mPacketsReceived++;
        mBytesReceived += packet.mSize;
    }

    MaskFilter mFilter;
    Counter mPacketsReceived = 0;
    Counter mBytesReceived = 0;
};
