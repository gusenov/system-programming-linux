#include "paraminput.h"
#include <wchar.h>
#include <stdio.h>
#include <stdlib.h>

bool check_port(int port) {
	return !(port < 0 || port > 65535);
}

bool check_ip(char* ip_buffer, struct in_addr* s_addr) {
	int result = inet_aton(ip_buffer, s_addr);
	return !(result == 0);

}

uint16_t input_port(void) {
	int port;

	do {
		wprintf(L"Введите номер порта серверной части>");
		if (wscanf(L"%d", &port) > 0 && check_port(port)) {
			break;
		} else {

			int c;
			while ((c = getwchar()) != L'\n' && c != EOF) { }

			fwprintf(stderr, L"Неверный номер порта серверной части!\n");
			continue;
		}
	} while (true);

	return port;
}

char* input_ip(char* ip_buffer, struct in_addr* s_addr) {
	do {
		wprintf(L"Введите IP-адрес серверной части>");
		wscanf(L"%s", ip_buffer);

		if (check_ip(ip_buffer, s_addr)) {
			break;
		} else {
			fwprintf(stderr, L"Неверный IP-адрес серверной части!\n");
			continue;
		}
	} while (true);

	return ip_buffer;
}

uint16_t parse_port(char* port_str) {
	int port = atoi(port_str);
	return check_port(port) ? port : input_port();
}

char* parse_ip(char* ip_str, char* ip_buffer, struct in_addr* s_addr) {
	return check_ip(ip_str, s_addr) ? ip_str : input_ip(ip_buffer, s_addr);
}
