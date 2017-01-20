#pragma once


#include "BBInterface.h"
#include "RxPacket.h"
#include <array>
#include <cassert>
#include <vector>



struct PhysicalInterface
{
    void pop(RxPacket packet)
    {
        mBBInterfaces[packet.mBBInterfaceId].pop(packet);
    }

    BBInterface& getBBInterface(uint32_t i)
    {
        return mBBInterfaces[i];
    }

    std::array<BBInterface, 48> mBBInterfaces;
};
