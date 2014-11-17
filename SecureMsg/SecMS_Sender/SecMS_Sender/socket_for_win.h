#ifndef _SOCKET_FOR_WIN_H_
#define _SOCKET_FOR_WIN_H_

#include <winsock2.h>
#include <ws2tcpip.h>

void init_winsock();
struct addrinfo* get_server_addr_info(const char* server_name, const char* port);
SOCKET create_socket(addrinfo* addr);
void bind_socket(SOCKET socket_listen, addrinfo* addr_server);
void listen_socket(SOCKET socket_listen);
SOCKET accept_socket(SOCKET socket_listen);
void shutdown_connection(SOCKET socket);
void connect_socket(SOCKET socket, addrinfo* addr);
void print_sys_error(const char* desc);

#endif