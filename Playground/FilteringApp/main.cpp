#include "Range.h"
#include <vector>
#include <forward_list>
#include <iostream>


struct RxPacket
{
    const uint8_t* data() const;
    uint32_t size() const;

    uint32_t get_interface_id() const;

private:
    const uint8_t* data_;
    uint16_t size_;
    uint8_t bb_interface_id_;
    uint8_t physical_port_id_;

    uint8_t ofload_flags_tcp_checksum_ok:1;
    uint8_t ofload_flags_udp_checksum_ok:1;
};


using RxPackets = Range<RxPacket>;
struct MACAddress
{
    bool isMulticast() const;
    bool isBroadcast() const;
};



struct Processor_Flow_v4
{
    void receive(RxPackets /*packet*/)
    {
//        auto l3 = packet.getLayer3Offset();
//        (void)l3;
        //etc...
    }

    void receive(RxPacket packet)
    {
        is_ipv4()

        // bpf_filter(...)
    }

    uint64_t mFields[2];
    uint64_t mMasks[2];
};


struct Processor_Flow_v6
{
    void receive(RxPackets /*packet*/)
    {

    }

    bool match(RxPacket rxPacket) const;

    void receive(RxPacket packet)
    {

        // bpf_filter(...)
    }

    uint64_t mFields[5];
    uint64_t mMasks[5];
};


struct Processor_BPF
{
    void receive(RxPackets /*packet*/)
    {
        // bpf_filter(...)
    }

    void receive(RxPacket /*packet*/)
    {

        // bpf_filter(...)
    }
};


struct Processor_BroadcastUnicast
{
    void receive(RxPacket packet)
    {
        auto mac = get_mac(packet);
        mBroadcast += mac.isBroadcast();
        mMulticast += mac.isMulticast();
    }

    MACAddress get_mac(RxPacket);

    uint64_t mBroadcast = 0;
    uint64_t mMulticast = 0;
};


struct ProtocolStack
{
    void receive(RxPackets packets)
    {
        // bpf_filter(...)
        (void)packets;
    }

};



struct BBPort
{
    void receive(RxPackets packets)
    {
        for (Processor_Flow_v4& flow : mProcessors_Flow_v4)
        {
            flow.receive(packets);
        }

        for (Processor_Flow_v6& flow : mProcessors_Flow_v6)
        {
            flow.receive(packets);
        }

        for (Processor_BPF& flow : mProcessors_BPF)
        {
            flow.receive(packets);
        }

        mStack.receive(packets);
    }

    Processor_BroadcastUnicast mPortCounters;
    std::vector<Processor_Flow_v4> mProcessors_Flow_v4;
    std::vector<Processor_Flow_v6> mProcessors_Flow_v6;
    std::vector<Processor_BPF> mProcessors_BPF;
    ProtocolStack mStack;
};


struct BBInterface
{
    void receive(RxPackets /*packets*/)
    {

        //BBPort& bbPort = get_bb_port(packet);
        //bbPort.receive(packet);
    }

    void add_to_batch(std::vector<BBInterface*>& active_list, RxPacket packet)
    {
        if (mBatch.empty())
        {
            active_list.push_back(this);
        }
        mBatch.push_back(packet);
    }

    static bool is_for_me(RxPacket p);

    RxPackets split_batch(RxPackets packets)
    {
        auto middle = std::stable_partition(packets.begin(), packets.end(), [&](RxPacket p) {
            return p.get_interface_id() == mInterfaceId;
        });

        receive(MakeRange(packets.begin(), middle));

        return MakeRange(middle, packets.end());
    }

    void process_batch()
    {
        receive(MakeRange(mBatch.data(), mBatch.data() + mBatch.size()));
    }

    BBPort& get_bb_port(RxPacket);

    uint16_t mInterfaceId;
    std::vector<RxPacket> mBatch;
};


struct PhysicalInterface
{
    // Receive packet from socket
    // - Dispatch to flow-based counters (5-tuple based)
    // - Dispatch to special counters (multicast and broadcast counters)
    // - Dispatch to bpf-based processors (user provided a BPF string)
    // - Dispatch to stack
    void run()
    {
        std::vector<BBInterface*> bb_interfaces;

        for (;;)
        {
            RxPackets packets = read_socket();

            for (RxPacket p : packets)
            {
                BBInterface& bbInterface = get_bb_interface(p);
                bbInterface.add_to_batch(bb_interfaces, p);
            }

            for (BBInterface* bbInterface : bb_interfaces)
            {
                bbInterface->process_batch();
            }

            bb_interfaces.clear();
        }
    }

    void run2()
    {
        for (;;)
        {
            RxPackets packets = read_socket();

            while (!packets.empty())
            {
                BBInterface& bbi = get_bb_interface(packets.front());
                packets = bbi.split_batch(packets);
            }
        }
    }

    void run3()
    {
        for (;;)
        {
            RxPackets packets = read_socket();

            std::stable_sort(packets.begin(), packets.end(), [&](RxPacket lhs, RxPacket rhs) {
                return lhs.get_interface_id() < rhs.get_interface_id();
            });

            while (!packets.empty())
            {
                auto p = packets.front();
                auto interface_id = p.get_interface_id();

                if (BBInterface* bb_interface = find_bb_interface(interface_id))
                {
                    packets = bb_interface->split_batch(packets);
                }
                else
                {
                    auto e = std::remove_if(packets.begin(), packets.end(), [&](RxPacket rxPacket) {
                        return rxPacket.get_interface_id() == interface_id;
                    });
                    packets = MakeRange(packets.begin(), e);
                }
            }
        }
    }


    RxPackets read_socket();

    BBInterface* find_bb_interface(uint16_t id);

    BBInterface& get_bb_interface(RxPacket);

private:
    std::vector<BBInterface> mBBInterfaces;
};


struct BBServer
{
    void run()
    {
    }

};


int main()
{
    std::cout << "Hello!" << std::endl;

    BBServer bbServer;
    bbServer.run();

    std::cout << "Goodbye!" << std::endl;
}



