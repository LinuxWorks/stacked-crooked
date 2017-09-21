#include "PhysicalInterface.h"


PhysicalInterface::PhysicalInterface()
{
    mBBInterfaces.resize(64);
}


void PhysicalInterface::pop(const std::vector<RxPacket>& packets)
{
    for (const RxPacket& packet : packets)
    {
        if (packet.mVlanId < mBBInterfaces.size())
        {
            BBInterface& bbInterface = mBBInterfaces[packet.mVlanId];
            bbInterface.pop(packet);
        }
    }

    for (BBInterface* bb_interface : mBatchedBBInterfaces)
    {
        bb_interface->pop_now();
    }

    mBatchedBBInterfaces.clear();
}
