#ifndef PARAMINPUT_H
#define PARAMINPUT_H

#include <arpa/inet.h>
#include <stdbool.h>

bool check_port(int port);
bool check_ip(char* ip_buffer, struct in_addr* s_addr);

uint16_t input_port(void);
char* input_ip(char* ip_buffer, struct in_addr* s_addr);

uint16_t parse_port(char* port_str);
char* parse_ip(char* ip_str, char* ip_buffer, struct in_addr* s_addr);

#endif
