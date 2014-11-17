#include <iostream>
#include "socket_for_win.h"

using namespace std;

#pragma comment(lib, "ws2_32.lib")

// Initialize Winsock
void init_winsock()
{
	WSADATA wsaData;
	int ret = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (ret == SOCKET_ERROR)
	{
		fprintf(stderr, "WSAStartup failed with error: %d\n", ret);
		exit(1);
	}
}

// Resolve the server address and port
struct addrinfo* get_server_addr_info(const char* server_name, const char* port)
{
	struct addrinfo hints;
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	struct addrinfo* result = NULL;
	int iResult = getaddrinfo(server_name, port, &hints, &result);
	if (iResult != 0) {
		fprintf(stderr, "getaddrinfo failed with error: %d\n", iResult);
		WSACleanup();
		exit(1);
	}

	return result;
}

// Create a SOCKET for connecting to server
SOCKET create_socket(addrinfo* addr)
{
	SOCKET sock = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
	if (sock == INVALID_SOCKET)
	{
		print_sys_error("socket failed");
		freeaddrinfo(addr);
		WSACleanup();
		exit(1);
	}

	return sock;
}

// Setup the TCP listening socket
void bind_socket(SOCKET socket_listen, addrinfo* addr_server)
{
	int ret = bind(socket_listen, addr_server->ai_addr, (int)addr_server->ai_addrlen);
	if (ret == SOCKET_ERROR) {
		print_sys_error("bind failed");
		freeaddrinfo(addr_server);
		closesocket(socket_listen);
		WSACleanup();
		exit(1);
	}
}

void listen_socket(SOCKET socket_listen)
{
	int ret = listen(socket_listen, SOMAXCONN);
	if (ret == SOCKET_ERROR) {
		print_sys_error("listen failed");
		closesocket(socket_listen);
		WSACleanup();
		exit(1);
	}
}

// Accept a client socket
SOCKET accept_socket(SOCKET socket_listen)
{
	SOCKET socket = accept(socket_listen, NULL, NULL);
	if (socket == INVALID_SOCKET) {
		print_sys_error("accept failed");
		closesocket(socket_listen);
		WSACleanup();
		exit(1);
	}

	return socket;
}

// shutdown the connection since we're done
void shutdown_connection(SOCKET socket)
{
	int ret = shutdown(socket, SD_SEND);
	if (ret == SOCKET_ERROR) {
		print_sys_error("shutdown failed");
		closesocket(socket);
		WSACleanup();
		exit(1);
	}
}

// Connect to server.
void connect_socket(SOCKET socket, addrinfo* addr)
{
	int ret = connect(socket, addr->ai_addr, (int)addr->ai_addrlen);
	if (ret == SOCKET_ERROR)
	{
		print_sys_error("connect failed");
		closesocket(socket);
		freeaddrinfo(addr);
		WSACleanup();
		exit(1);
	}
}

void print_sys_error(const char* desc)
{
	LPWSTR errString = NULL;
	int err_no = WSAGetLastError();
	FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, err_no,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPWSTR)&errString, 0, NULL);
	fprintf(stderr, "**Error : %s - %d %s\n", desc, err_no, errString);
	LocalFree(errString);
}
