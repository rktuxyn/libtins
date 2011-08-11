#include <cstring>
#include <cassert>
#ifndef WIN32
    #include <netinet/in.h>
#endif
#include "ip.h"
#include "utils.h"

#include <iostream>

using namespace std;


Tins::IP::IP(const string &ip_dst, const string &ip_src) : PDU(IPPROTO_IP) {
    init_ip_fields();
    if(ip_dst.size())
        _ip.daddr = Utils::ip_to_int(ip_dst);
    if(ip_src.size())
        _ip.saddr = Utils::ip_to_int(ip_src);

}

Tins::IP::IP(uint32_t ip_dst, uint32_t ip_src) : PDU(IPPROTO_IP) {
    init_ip_fields();
    _ip.daddr = ip_dst;
    _ip.saddr = ip_src;
}

void Tins::IP::init_ip_fields() {
    memset(&_ip, 0, sizeof(iphdr));
    _ip.version = 4;
    _ip.ihl = sizeof(iphdr) / sizeof(uint32_t);
    _ip.ttl = 128;
}

/* Setters */

void Tins::IP::tos(uint8_t new_tos) {
    _ip.tos = new_tos;
}

void Tins::IP::tot_len(uint16_t new_tot_len) {
    _ip.tot_len = new_tot_len;
}

void Tins::IP::id(uint16_t new_id) {
    _ip.id = new_id;
}

void Tins::IP::frag_off(uint16_t new_frag_off) {
    _ip.frag_off = new_frag_off;
}

void Tins::IP::ttl(uint8_t new_ttl) {
    _ip.ttl = new_ttl;
}

void Tins::IP::protocol(uint8_t new_protocol) {
    _ip.protocol = new_protocol;
}

void Tins::IP::check(uint16_t new_check) {
    _ip.check = new_check;
}

void Tins::IP::source_address(const string &ip) {
    _ip.saddr = Utils::ip_to_int(ip);
}

void Tins::IP::source_address(uint32_t ip) {
    _ip.saddr = ip;
}

void Tins::IP::dest_address(const string &ip) {
    _ip.daddr = Utils::ip_to_int(ip);
}

void Tins::IP::dest_address(uint32_t ip) {
    _ip.daddr = ip;
}

/* Virtual method overriding. */

uint32_t Tins::IP::header_size() const {
    return sizeof(iphdr);
}

bool Tins::IP::send(PacketSender* sender) {

    struct sockaddr_in link_addr;

    link_addr.sin_family = AF_INET;
    link_addr.sin_port = 0;
    link_addr.sin_addr.s_addr = _ip.daddr;

    return sender->send_l3(this, (const struct sockaddr*)&link_addr, sizeof(link_addr));

}

void Tins::IP::write_serialization(uint8_t *buffer, uint32_t total_sz) {
    uint32_t my_sz = header_size() + trailer_size();
    uint32_t new_flag = inner_pdu()? inner_pdu()->flag() : 255;
    assert(total_sz >= my_sz);
    if(new_flag == IPPROTO_IP)
        new_flag = IPPROTO_IPIP;
    flag(new_flag);
    _ip.protocol = new_flag;
    _ip.tot_len = total_sz;
    memcpy(buffer, &_ip, sizeof(iphdr));

    /* IP Options here... */
}