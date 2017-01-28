#pragma once


#include "RxPacket.h"
#include <vector>


struct PacketBuffer
{
    uint8_t mBuffer[1536];
};


struct Stack
{
    Stack();

    void add_to_queue(RxPacket packet);

    void flush();

    std::vector<PacketBuffer*> mPackets;
    std::vector<PacketBuffer*> mFreeBuffers;
};
