#include <iostream>
#include <string>

#include "client.h"

SOCKET hSock = INVALID_SOCKET;
DWORD ip_addr;
char recv_buf[MAX_BUFFER];
unsigned short remain_packet_size;
char recv_packet_buf[MAX_PACKET_SIZE];
int g_myid;

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
	// 좌표와 움직임을 넣음 클라에서 서버로 좌표를 넘겨주는 방식

	//packet.x = x;
	//packet.y = y;
	//packet.z = z; etc
	
	send(socket, reinterpret_cast<char*>(&packet), packet.size, 0);
}

void send_colid_packet(SOCKET socket)
{
	c2s_packet_colid packet;
	packet.size = sizeof(packet);
	packet.type = C2S_PACKET_MOVE;

	send(socket, reinterpret_cast<char*>(&packet), packet.size, 0);
}

void ProcessPacket()
{
	static bool first_time = true;
	byte msg_type = recv_packet_buf[sizeof(byte)];
	switch (msg_type)
	{
	case S2C_PACKET_LOGIN_INFO:
	{
		s2c_packet_login_info* packet = reinterpret_cast<s2c_packet_login_info*>(recv_packet_buf);
		g_myid = packet->id;
		//avatar.m_x = packet->x;
		//avatar.m_y = packet->y;

		//avatar.show();
	}
	break;
	case S2C_PACKET_PC_LOGIN:
	{
		s2c_packet_pc_login* my_packet = reinterpret_cast<s2c_packet_pc_login*>(recv_packet_buf);
		int id = my_packet->id;

		if (id < MAX_USER) {
			players[id].move(my_packet->x, my_packet->y);
			players[id].show();
		}
		else {
			//npc[id - NPC_START].x = my_packet->x;
			//npc[id - NPC_START].y = my_packet->y;
			//npc[id - NPC_START].attr |= BOB_ATTR_VISIBLE;
		}
		break;
	}
	case S2C_PACKET_PC_MOVE:
	{
		s2c_packet_pc_move* my_packet = reinterpret_cast<s2c_packet_pc_move*>(recv_packet_buf);
		int other_id = my_packet->id;
		if (other_id == g_myid) {
			avatar.move(my_packet->x, my_packet->y);
			g_left_x = my_packet->x - (SCREEN_WIDTH / 2);
			g_top_y = my_packet->y - (SCREEN_HEIGHT / 2);
		}
		else if (other_id < MAX_USER) {
			players[other_id].move(my_packet->x, my_packet->y);
		}
		else {
			//npc[other_id - NPC_START].x = my_packet->x;
			//npc[other_id - NPC_START].y = my_packet->y;
		}
		break;
	}
	case S2C_PACKET_PC_LOGOUT:
	{
		s2c_packet_pc_logout* my_packet = reinterpret_cast<s2c_packet_pc_logout*>(recv_packet_buf);
		int other_id = my_packet->id;
		if (other_id == g_myid) {
			avatar.hide();
		}
		else if (other_id < MAX_USER) {
			players[other_id].hide();
		}
		else {
			//		npc[other_id - NPC_START].attr &= ~BOB_ATTR_VISIBLE;
		}
		break;
	}
	default:
		printf("Unknown PACKET type [%d]\n", recv_packet_buf);
	}
}

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

void RecvThread()
{
	while (true)
	{
		int len = recv(hSock, recv_buf, MAX_BUFFER, 0);

		// 패킷 조립 + 패킷처리
		if (len == 0) {
			//PostQuitMessage(0);
			return;
		}
		if (len < 0)
		{
			continue;
		}

		// packet 조립
		// ****이때 이 2byte도 끊겨서 올 수 있다.
		int rest_packet_size = len;
		int cur_recv_buf_idx = 0;

		while (rest_packet_size) {
			int required;
			if (remain_packet_size == 0) required = recv_buf[cur_recv_buf_idx];
			else required = recv_packet_buf[0] - remain_packet_size;
			if (required > rest_packet_size) {
				memcpy(recv_packet_buf + remain_packet_size, recv_buf + cur_recv_buf_idx, rest_packet_size);
				remain_packet_size += rest_packet_size;
				break;
			}


			memcpy(recv_packet_buf + remain_packet_size, recv_buf + cur_recv_buf_idx, required);
			cur_recv_buf_idx += required;
			rest_packet_size -= required;
			remain_packet_size = 0;

			ProcessPacket();
		}

		//Draw(gHwnd);
		//InvalidateRect(gHwnd, NULL, FALSE);
	}
}

int main()
{
	wcout.imbue(locale("korean"));

	if (Init(hSock, ip_addr) != true) exit(1);


	send_login_packet(hSock);

	remain_packet_size = 0;

	RecvThread();

	closesocket(hSock);
	WSACleanup();
	cout << "Client has been terminated..." << endl;
}