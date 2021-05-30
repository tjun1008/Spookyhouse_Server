#include "ClientSocket.h"

bool Init(SOCKET& hSock, DWORD ip_addr)
{
	WSADATA WSAData;
	int nRet = WSAStartup(MAKEWORD(2, 2), &WSAData);
	if (nRet != 0)
		return false;

	hSock = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (hSock == INVALID_SOCKET)
		return false;

	SOCKADDR_IN serverAddr;
	memset(&serverAddr, 0, sizeof(SOCKADDR_IN));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(SERVER_PORT);
	serverAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY); //htonl(ip_addr);
	bind(hSock, (struct sockaddr*)&serverAddr, sizeof(SOCKADDR_IN));
	nRet = WSAConnect(hSock, (SOCKADDR*)&serverAddr, sizeof(serverAddr), NULL, NULL, NULL, NULL);

	if (nRet == SOCKET_ERROR)
		return false;

	cout << "Connection success..." << endl;

	return true;

}