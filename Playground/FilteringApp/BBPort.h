#pragma once


#include "Decode.h"
#include "Likely.h"
#include "MACAddress.h"
#include "RxPacket.h"
#include "RxTrigger.h"
#include "Stack.h"
#include "UDPFlow.h"
#include <vector>


struct BBPort
{
    BBPort(MACAddress local_mac);

    UDPFlow& getUDPFlow(uint32_t index) { return *mUDPFlows[index]; }

    void addUDPFlow(uint16_t dst_port);

    void pop(const RxPacket& packet)
    {
        if (is_local_mac(packet))
        {
            mStats.mUnicastCounter++;
        }
        else if (is_broadcast(packet))
        {
            mStats.mBroadcastCounter++;
        }
        else if (is_multicast(packet))
        {
            mStats.mMulticastCounter++;
        }
        else
        {
            return;
        }

        if (is_ipv4(packet))
        {
            for (auto& udp_flow_ptr : mUDPFlows)
            {
                UDPFlow& udp_flow = *udp_flow_ptr;
                if (udp_flow.match(packet, mLayer3Offset))
                {
                    udp_flow.accept(packet);
                    mStats.mUDPAccepted++;
                    return;
                }
            }

            // If we didn't match any UDP flows then the IP may wrong. So we still need to check it.
            auto dst_ip = Decode<IPv4Header>(packet.data() + mLayer3Offset).mDestinationIP;

            if (dst_ip != mLocalIP && !dst_ip.isBroadcast() && !dst_ip.isMulticast())
            {
                // Invalid destination IP.
                return;
            }
        }

        // Send it to the stack.
        handle_other(packet);
    }

    struct Stats
    {
        uint64_t mUnicastCounter = 0;
        uint64_t mBroadcastCounter = 0;
        uint64_t mMulticastCounter = 0;
        uint64_t mUDPAccepted = 0;
    };

    const Stats& stats() const { return mStats; }

private:
    bool is_ipv4(const RxPacket& packet) const { return Decode<EthernetHeader>(packet.data()).mEtherType == Net16(0x0800); }
    bool is_local_mac(const RxPacket& packet) { return mLocalMAC.equals(packet.data()); }
    bool is_broadcast(const RxPacket& packet) { return 0x0000FFFFFFFFFFFF == (Decode<uint64_t>(packet.data()) & 0x0000FFFFFFFFFFFF); }
    bool is_multicast(const RxPacket& packet) { return packet[0] & 0x01; }

    void handle_other(const RxPacket& packet)
    {
        mStack.add_to_queue(packet);
    }

    LocalMAC mLocalMAC;
    IPv4Address mLocalIP;
    uint16_t mLayer3Offset = sizeof(EthernetHeader); // default
    Stats mStats;
    std::vector<std::unique_ptr<UDPFlow>> mUDPFlows;
    Stack mStack;
};
