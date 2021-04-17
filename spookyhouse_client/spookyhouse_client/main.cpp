#include <iostream>
#include <string>
#include "protocol.h"
using namespace std;

void send_login_packet(SOCKET socket)
{
	c2s_packet_login packet;
	packet.size = sizeof(packet);
	packet.type = C2S_PACKET_LOGIN;
	strcpy_s(packet.name, to_string(rand() % 100).c_str());


	
	send(socket, reinterpret_cast<char*>(&packet), packet.size, 0);
}

void send_move_packet(SOCKET socket)
{
	c2s_packet_move packet;
	packet.size = sizeof(packet);
	packet.type = C2S_PACKET_MOVE;
	//packet.x = x;
	//packet.y = y;

	send(socket, reinterpret_cast<char*>(&packet), packet.size, 0);
}

void send_colid_packet(SOCKET socket)
{
	c2s_packet_colid packet;
	packet.size = sizeof(packet);
	packet.type = C2S_PACKET_MOVE;

	send(socket, reinterpret_cast<char*>(&packet), packet.size, 0);
}

int main()
{
	wcout.imbue(locale("korean"));

	WSADATA WSAData;
	int nRet = WSAStartup(MAKEWORD(2, 2), &WSAData);
	if (nRet != 0) {
		cout << "Error : " << WSAGetLastError() << endl;
		return false;
	}

	SOCKET clientSocket  = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	SOCKADDR_IN serverAddr;
	memset(&serverAddr, 0, sizeof(SOCKADDR_IN));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(SERVER_PORT);
	serverAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	bind(clientSocket, (struct sockaddr*)&serverAddr, sizeof(SOCKADDR_IN)); //:: ¾ÈÇØµµ µÊ

	nRet = WSAConnect(clientSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr), NULL, NULL, NULL, NULL);
	if (nRet == SOCKET_ERROR) {
		cout << "Error : " << WSAGetLastError() << endl;
		return false;
	}

	cout << "Connection success..." << endl;

	send_login_packet(clientSocket);


	closesocket(clientSocket);
	WSACleanup();
	cout << "Client has been terminated..." << endl;
}