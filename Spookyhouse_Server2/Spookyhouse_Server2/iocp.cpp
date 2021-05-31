#include "main_iocp.h"
#include "Ghost.h"

map <int, SESSION> players;
map<int, Ghost> Ghosts;

void MonsterManagementThread(int p_id);

void send_packet(int p_id, void* buf)
{
	EX_OVER* s_over = new EX_OVER;

	unsigned char packet_size = reinterpret_cast<unsigned char*>(buf)[0];
	s_over->m_op = OP_SEND;
	memset(&s_over->m_over, 0, sizeof(s_over->m_over));
	memcpy(s_over->m_netbuf, buf, packet_size);
	s_over->m_wsabuf[0].buf = reinterpret_cast<char*>(s_over->m_netbuf);
	s_over->m_wsabuf[0].len = packet_size;

	WSASend(players[p_id].m_s, s_over->m_wsabuf, 1, 0, 0, &s_over->m_over, 0);
}

void send_move_packet(int c_id, int p_id)
{
	s2c_packet_pc_move packet;
	packet.id = p_id;
	packet.size = sizeof(packet);
	packet.type = S2C_PACKET_PC_MOVE;

	packet.x = players[p_id].character.x;
	packet.y = players[p_id].character.y;
	packet.z = players[p_id].character.z;

	packet.pitch = players[p_id].character.pitch;
	packet.roll = players[p_id].character.roll;
	packet.yaw = players[p_id].character.yaw;


	send_packet(c_id, &packet); //오버라이트가 커서 주소만 보내서 전송
}

/*
void player_move(int p_id, char dir)
{
	short x = players[p_id].character.x;
	short y = players[p_id].character.y;
	short z = players[p_id].character.z;

	//움직일때 클라에서 처리 or 여기서 처리

	for (auto& cl : players) {
		if (false == cl.second.m_ingame) continue;
		send_move_packet(cl.second.m_id, p_id);
	}
}
*/


void send_login_info(int p_id)
{
	s2c_packet_login_info packet;
	packet.hp = 100;
	packet.id = p_id;
	packet.size = sizeof(packet);
	packet.type = S2C_PACKET_LOGIN_INFO;

	packet.x = players[p_id].character.x;
	packet.y = players[p_id].character.y;
	packet.z = players[p_id].character.z;

	packet.yaw = players[p_id].character.yaw;
	packet.pitch = players[p_id].character.pitch;
	packet.roll = players[p_id].character.roll;

	packet.vx = players[p_id].character.vx;
	packet.vy = players[p_id].character.vy;
	packet.vz = players[p_id].character.vz;
	
	packet.skill_gage = 0;
	packet.isalive = true;
	
	send_packet(p_id, &packet); //오버라이트가 커서 주소만 보내서 전송
}


void send_pc_login(int c_id, int p_id)
{
	s2c_packet_pc_login packet;
	packet.id = p_id;
	packet.size = sizeof(packet);

	packet.type = S2C_PACKET_PC_LOGIN;
	packet.x = players[p_id].character.x;
	packet.y = players[p_id].character.y;
	packet.z = players[p_id].character.z;

	packet.yaw = players[p_id].character.yaw;
	packet.pitch = players[p_id].character.pitch;
	packet.roll = players[p_id].character.roll;

	packet.vx = players[p_id].character.vx;
	packet.vy = players[p_id].character.vy;
	packet.vz = players[p_id].character.vz;

	strcpy_s(packet.name, players[p_id].m_name);

	packet.o_type = 0;

	send_packet(c_id, &packet); //오버라이트가 커서 주소만 보내서 전송
}

void send_pc_logout(int c_id, int p_id)
{
	s2c_packet_pc_logout packet;
	packet.id = p_id;
	packet.size = sizeof(packet);
	packet.type = S2C_PACKET_PC_LOGOUT;

	cout << "[" << p_id << "] logout \n";
	send_packet(c_id, &packet); //오버라이트가 커서 주소만 보내서 전송
}


void process_packet(int p_id, unsigned char* packet)
{
	c2s_packet_login* p = reinterpret_cast<c2s_packet_login *>(packet);
	switch (p->type) {
	case C2S_PACKET_LOGIN:
	{
	strcpy_s(players[p_id].m_name, p->name);

	players[p_id].character.x = -1620;
	players[p_id].character.y = -29;
	players[p_id].character.z = 117;

	players[p_id].character.vx = 0;
	players[p_id].character.vy = 0;
	players[p_id].character.vz = 0;

	players[p_id].character.yaw = 0;
	players[p_id].character.pitch = 0;
	players[p_id].character.roll = 0;

	//플레이어 정보 초기화

	send_login_info(p_id);
	players[p_id].m_ingame = true;

	for (auto& p : players) {
		if (p.second.m_id == p_id) continue;
		if (p.second.m_ingame == false) continue;
		send_pc_login(p_id, p.second.m_id);  //상대방에게 내 아이디
		send_pc_login(p.second.m_id, p_id);
	}
	}
		break;
	case C2S_PACKET_MOVE:
	{
	c2s_packet_move* move_packet = reinterpret_cast<c2s_packet_move*>(packet);
    //cout << move_packet->x <<" " << move_packet->y<< " " << move_packet->z<<endl;

	players[p_id].character.x = move_packet->x;
	players[p_id].character.y = move_packet->y;
	players[p_id].character.z = move_packet->z;

	players[p_id].character.yaw = move_packet->yaw;
	players[p_id].character.pitch = move_packet->pitch;
	players[p_id].character.roll = move_packet->roll;



	for (auto& cl : players) {
		if (false == cl.second.m_ingame) continue;
		send_move_packet(cl.second.m_id, p_id);
		
	}

	
	}
		break;

	default:
		cout << "UNknown packet type[" << p->type << "] Error\n";
		exit(1);
	}
}

void do_recv(int p_id)
{
	SESSION& pl = players[p_id];
	EX_OVER& r_over = pl.m_recv_over;
	//r_over.m_op = OP_RECV;
	memset(&r_over.m_over, 0, sizeof(r_over.m_over));
	r_over.m_wsabuf[0].buf = reinterpret_cast<CHAR *>(r_over.m_netbuf) + pl.m_prev_recv;
	r_over.m_wsabuf[0].len = MAX_BUFFER - pl.m_prev_recv;
	DWORD r_flag = 0;
	WSARecv(pl.m_s, r_over.m_wsabuf, 1, 0, &r_flag, &r_over.m_over, 0);
}

int get_new_player_id()
{
	for (int i = 0; i < MAX_USER; ++i) {
		if (0 == players.count(i)) return i;
	}
	return -1;
}

void do_accept(SOCKET s_socket ,SOCKET *c_socket, EX_OVER *a_over)
{

	*c_socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	memset(&a_over->m_over, 0, sizeof(a_over->m_over));
	DWORD num_byte;
	int addr_size = sizeof(SOCKADDR_IN) + 16;
	BOOL ret = AcceptEx(s_socket, *c_socket, a_over->m_netbuf, 0, addr_size, addr_size, &num_byte, &a_over->m_over); //주소구조체보다 16바이트를 더 줘야한다
	if (FALSE == ret) {
		int err = WSAGetLastError();
		if (WSA_IO_PENDING != err) {
			error_display("ACCPTEX: ", err);
			exit(-1);
		}
	}
}

void disconnect(int p_id)
{
	players[p_id].m_ingame = false;
	closesocket(players[p_id].m_s);
	players.erase(p_id);

	for (auto& cl : players) {
		if (false == cl.second.m_ingame) continue;
		send_pc_logout(cl.second.m_id, p_id);
	}
}

int main()
{
	wcout.imbue(locale("korean"));

	WSADATA WSAData;
	WSAStartup(MAKEWORD(2, 2), &WSAData);
	SOCKET listenSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	SOCKADDR_IN serverAddr;
	memset(&serverAddr, 0, sizeof(SOCKADDR_IN));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(SERVER_PORT);
	serverAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	::bind(listenSocket, (struct sockaddr*)&serverAddr, sizeof(SOCKADDR_IN)); //:: 안해도 됨
	listen(listenSocket, SOMAXCONN);

	HANDLE h_iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0);
	CreateIoCompletionPort(reinterpret_cast<HANDLE>(listenSocket), h_iocp, 100000, 0);

	
	SOCKET c_socket;

	EX_OVER a_over;
	a_over.m_op = OP_ACCEPT;
	do_accept(listenSocket,&c_socket, &a_over);

	

	while (true) {
		DWORD num_byte;
		ULONG_PTR i_key;
		WSAOVERLAPPED* over;
		BOOL ret = GetQueuedCompletionStatus(h_iocp, &num_byte, &i_key, &over, INFINITE); //여기서 처리함
		int key = static_cast<int>(i_key);
		if (FALSE == ret) {
			int err = WSAGetLastError();
			error_display("GQCS: ", err);
			disconnect(key);
			continue;
		}

			EX_OVER* ex_over = reinterpret_cast<EX_OVER*>(over);

			switch (ex_over->m_op) {
			case OP_RECV:
			{
				unsigned char* ps = ex_over->m_netbuf; //패킷의 시작주소
				int remain_data = num_byte + players[key].m_prev_recv; //받은 데이터 + 처리하고 남은 데이터

				while (remain_data > 0) {
					int packet_size = ps[0];
					if (packet_size > remain_data) break;

					process_packet(key, ps);
					remain_data -= packet_size;
					ps += packet_size;

				}
				if (remain_data > 0)
					memcpy(ex_over->m_netbuf, ps, remain_data);
				players[key].m_prev_recv = remain_data;
				do_recv(key);
			}
			break;
			case OP_SEND:
				if (num_byte != ex_over->m_wsabuf[0].len)
					disconnect(key);
				delete ex_over;
				break;
			case OP_ACCEPT:
			{
				int p_id = get_new_player_id();
				if (-1 == p_id) {
					closesocket(c_socket);
					do_accept(listenSocket, &c_socket, &a_over);
					continue;
				}

				players[p_id].m_id = p_id;
				players[p_id].m_ingame = false;
				players[p_id].m_prev_recv = 0;
				players[p_id].m_recv_over.m_op = OP_RECV;
				players[p_id].m_s = c_socket;


				CreateIoCompletionPort(reinterpret_cast<HANDLE>(c_socket), h_iocp, p_id, 0);

				//x,y는 여기서 초기화 하는거 바람직하진않음 클라이언트가 로비패킷을 받을 때 초기화

				do_recv(p_id);
				do_accept(listenSocket, &c_socket, &a_over);
				cout << "New Client [" << p_id << "] connected!\n";
			}
			break;
			default:
				cout << "Unknown GQCS ERRor!\n";
				exit(1);

			}

			//cout << "New CLient [" << &clients[client_s].dataBuffer << "] :";
		
	}

	closesocket(listenSocket);
	WSACleanup();
}

void error_display(const char* msg, int err_no)
{
	WCHAR* lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, err_no,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	std::cout << msg;
	std::wcout << L"에러 " << lpMsgBuf << std::endl;
	//while (true);
	LocalFree(lpMsgBuf);
}
