#include "Packet.h"


Packet::Packet(ProtocolId protocol, IPv4Address src_ip, IPv4Address dst_ip, uint16_t src_port, uint16_t dst_port) :
    mStorage()
{

    assert(src_port >= 100 && src_port <= 10000);
    assert(dst_port >= 100 && dst_port <= 10000);


    auto eth_header = EthernetHeader::Create(EtherType::IPv4);
    memcpy(mStorage.data(), &eth_header, sizeof(eth_header));

    auto ip4_ptr = mStorage.data() + sizeof(EthernetHeader);
    auto ip4_header = IPv4Header::Create(protocol, src_ip, dst_ip);
    memcpy(ip4_ptr, &ip4_header, sizeof(ip4_header));

    auto tcp_ptr = mStorage.data() + sizeof(EthernetHeader) + sizeof(IPv4Header);
    auto tcp_header = TCPHeader::Create(src_port, dst_port);
    memcpy(tcp_ptr, &tcp_header, sizeof(tcp_header));

    #if 0
    {            
        // Print packet to pcap file to inspect and debug
        static uint32_t c = 0;
        PCAPWriter pcap("test" + std::to_string(c) + ".pcap");
        pcap.push_back(mStorage.data(), mStorage.size());
        if (++c >= 10) throw 1;
    }
    #endif
}
