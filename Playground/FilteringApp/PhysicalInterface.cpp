#include "PhysicalInterface.h"


PhysicalInterface::PhysicalInterface()
{
    mBBInterfaces.resize(64);
}


void PhysicalInterface::pop(const std::vector<RxPacket>& packets)
{
    for (const RxPacket& packet : packets)
    {
        BBInterface& bbInterface = mBBInterfaces[packet.mVlanId];
        bbInterface.pop_later(packet, mActiveInterfaces);
    }

    for (BBInterface* bb_interface : mActiveInterfaces)
    {
        bb_interface->pop_now();
    }

    mActiveInterfaces.clear();
}
