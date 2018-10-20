#include "UDPFlow.h"


static_assert(sizeof(UDPFlow) == 48, "");


UDPFlow::UDPFlow(IPv4Address src_ip, IPv4Address dst_ip, uint16_t src_port, uint16_t dst_port) :
    mFilter(ProtocolId::UDP, src_ip, dst_ip, src_port, dst_port)
{
    //std::cout << "UDP Flow:" << src_ip << ":" << src_port << ">" << dst_ip << ":" << dst_port << std::endl;
}
