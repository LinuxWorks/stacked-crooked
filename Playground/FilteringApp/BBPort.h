#pragma once


#include "UDPFlow.h"
#include "RxPacket.h"
#include "MACAddress.h"
#include "Decode.h"
#include "Likely.h"
#include <vector>



struct BBPort
{
    BBPort(MACAddress local_mac);

    void addUDPFlow(uint16_t dst_port);

    UDPFlow& getUDPFlow(uint32_t i)
    {
        return mUDPFlows[i];
    }


	__attribute__((always_inline))
    void pop_many(const RxPacket* packet, uint32_t length)
    {
        while (length >= 4)
        {
            bool all_ok = true;
            all_ok &= check_mac(packet[0]);
            all_ok &= check_mac(packet[1]);
            all_ok &= check_mac(packet[2]);
            all_ok &= check_mac(packet[3]);

            if (all_ok)
            {
                mUnicastCounter += 4;
                udp_pop_4(packet);
            }
            else
            {
                pop(packet[0]);
                pop(packet[1]);
                pop(packet[2]);
                pop(packet[3]);
            }

            length -= 4;
            packet += 4;
        }

        for (auto i = 0u; i != length; ++i)
        {
            pop(packet[i]);
        }
    }

    void pop(RxPacket packet)
    {
        if (check_mac(packet))
        {
            mUnicastCounter++;
        }
        else if (is_broadcast(packet))
        {
            mBroadcastCounter++;
        }

        for (UDPFlow& flow : mUDPFlows)
        {
            flow.process(packet);
        }
    }

    __attribute__((always_inline))
    bool check_mac(const RxPacket& packet)
    {
        return (*reinterpret_cast<const uint32_t*>(mLocalMAC.data() + 2) == *reinterpret_cast<const uint32_t*>(packet.data() + 2))
            && (*reinterpret_cast<const uint16_t*>(mLocalMAC.data() + 0) == *reinterpret_cast<const uint16_t*>(packet.data() + 0))
        ;
    }

    __attribute__((always_inline))
    bool is_broadcast(const RxPacket& packet)
    {
        return (0xFFFFFFFF == *reinterpret_cast<const uint32_t*>(packet.data() + 2))
            && (0x0000FFFF == *reinterpret_cast<const uint16_t*>(packet.data() + 0))
        ;
    }

    __attribute__((always_inline))
	void udp_pop_4(const RxPacket* packet)
    {
        for (UDPFlow& flow : mUDPFlows)
        {
			bool all_ok = true;
			all_ok &= flow.match(packet[0]);
			all_ok &= flow.match(packet[1]);
			all_ok &= flow.match(packet[2]);
			all_ok &= flow.match(packet[3]);

            if (all_ok)
            {
                flow.accept_4(packet);
            }
            else
            {
                flow.process(packet[0]);
                flow.process(packet[1]);
                flow.process(packet[2]);
                flow.process(packet[3]);
            }
        }
    }

    MACAddress mLocalMAC = MACAddress();
    uint64_t mUnicastCounter = 0;
    uint64_t mBroadcastCounter = 0;
    std::vector<UDPFlow> mUDPFlows;

};


// Validate size
// Define one or more UDP flows.
// Use MaskFilter
