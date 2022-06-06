#include "stdafx.h" //맨 위에 놓기 제발!!
#include "iocp_server.h"
#include "Global.h"


//IocpServer* GIocpServer = nullptr;

IocpServer::IocpServer() : mIOCP(NULL), mListenSocket(NULL), mWorkerThreads()
{
}


IocpServer::~IocpServer()
{
}

bool IocpServer::Initialize()
{
	wcout.imbue(locale("korean"));

	CreateObjectPool();

	// begin : GetSystemInfo로 cpu의 갯수를 확인합시다

	/*
	SYSTEM_INFO si;
	GetSystemInfo(&si);
	ThreadCount = si.dwNumberOfProcessors;

	// int ThreadCount

	*/

	// 1. winsock initializing 
	WSADATA WSAData;
	if (WSAStartup(MAKEWORD(2, 2), &WSAData) != 0)
		return false;


	// 2. Create I/O Completion Port

	mIOCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	if (mIOCP == NULL)
		return false;


	/// 3. create TCP socket
	mListenSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (mListenSocket == INVALID_SOCKET)
		return false;



	// 4. iocp 객체와 소켓 연결

	HANDLE handle = CreateIoCompletionPort((HANDLE)mListenSocket, mIOCP, 0, 0);
	if (handle != mIOCP)
	{
		printf_s("[DEBUG] listen socket IOCP register error: %d\n", GetLastError());
		return false;
	}

	// 5. 서버 정보 객체 설정

	SOCKADDR_IN serverAddr;
	memset(&serverAddr, 0, sizeof(SOCKADDR_IN));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(SERVER_PORT);
	serverAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY); //IP주소를 자동으로 찾아서 대입

	//bind 에러 방지
	int opt = 1;
	setsockopt(mListenSocket, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof(int));

	// 6. bind

	if (SOCKET_ERROR == bind(mListenSocket, (struct sockaddr*)&serverAddr, sizeof(SOCKADDR_IN)))
		return false;


	return true;

}

bool IocpServer::StartWorkerThreads()
{
	mWorkerThreads.reserve(NUM_THREADS);
	for (int i = 0; i < NUM_THREADS; ++i)
		mWorkerThreads.emplace_back(thread{ WorkerThread, (LPVOID)this });

	for (auto& th : mWorkerThreads) th.join();

	return true;
}

//쓰레드에서 멤버 변수를 사용하기 위해, 클래스 내부에서 워커쓰레드에 필요한 함수를 호출

void IocpServer::WorkerThread(LPVOID arg)
{
	IocpServer* pServer = static_cast<IocpServer*>(arg);
	pServer->WorkerThreadFunc();


};

void IocpServer::WorkerThreadFunc()
{
	//스레드 생성중~~

	while (true) {
		DWORD num_byte;
		ULONG_PTR i_key;
		//WSAOVERLAPPED* over;

		EX_OVER* over;
		BOOL ret = GetQueuedCompletionStatus(mIOCP, &num_byte, &i_key, reinterpret_cast<LPOVERLAPPED*>(&over), INFINITE);
		int key = static_cast<int> (i_key);
		if (FALSE == ret) {
			int err = WSAGetLastError();
			//display_error("GQCS : ", err);
			Disconnect(key);
			continue;
		}

		EX_OVER* ex_over = reinterpret_cast<EX_OVER*>(over);

	}
}

void IocpServer::Disconnect(int p_id)
{
	Player* player = dynamic_cast<Player*>(players[p_id]);

	
	player->m_lock.lock();
	player->SetState(false);
	closesocket(player->GetSocket());
	player->m_lock.unlock();
	//players.erase(p_id);

	for (int i = 0; i < players.size(); ++i)
	{
		Player* pl = dynamic_cast<Player*>(players[i]);

		if (pl->GetID() == p_id) continue;

		if (false != pl->GetState())
			send_pc_logout(pl->GetID(), p_id);

	}

}

bool IocpServer::StartAcceptLoop()
{

	SOCKET clientSocket{};

	///7.listen
	if (SOCKET_ERROR == listen(mListenSocket, SOMAXCONN))
		return false;

	
	

	while (true)
	{
		clientSocket = accept(mListenSocket, NULL, NULL);
		if (clientSocket == INVALID_SOCKET)
		{
			cout << "invalid socket\n";
			continue;
		}

		// id 받아오는 작업

		int new_id = -1;
		
		for (int i = 0; i < MAX_USER; ++i)
		{
			// id는 비어있는값으로 세팅해줍니다
			if (false == players[i]->GetState())
			{
				new_id = i;
				break;
			}
		}

		if (cur_player >= 4)
		{
			closesocket(mListenSocket);
			cout << "Max User" << endl;
		}

		if (true == bIsgameStart) {
			closesocket(mListenSocket);
			cout << "Game Already Start" << endl;

		}

		cout << "[" << new_id << "] 접속" << endl;

		Player* player = dynamic_cast<Player*>(players[new_id]);

		if (cur_player == 0)
		{
			player->SetHost(true);
		}
		else
		{
			player->SetHost(false);
		}

		cur_player++;

		// id, socket 상태 설정
		player->SetState(STATE_INGAME);
		player->SetSocket(clientSocket);
		player->SetId(new_id);

		//해당 클라이언트 값을 본인, 상대한테 전한다

		send_pc_lobby(new_id, new_id);

		for (int i = 0; i < players.size(); ++i)
		{
			Player* pl = dynamic_cast<Player*>(players[i]);

			if (pl->GetID() == new_id) continue;

			if (false != pl->GetState()) continue;

			send_pc_lobby(pl->GetID(), new_id);
			send_pc_lobby(new_id, pl->GetID());

		}

		CreateIoCompletionPort(reinterpret_cast<HANDLE>(clientSocket), mIOCP, new_id, 0);
	}

	return true;

}

void IocpServer::CreateObjectPool()
{
	//탈출키 초기화
	KeyLocation loc1[6]; // = { {730, -1870, 70},{799, -1870, 70},{1661, -2061, 89},{1661, -1983, 70},{1428, 1512, 145},{1587, 1519, 144} };
	KeyLocation loc2[9];
	KeyLocation loc3[12];

	ifstream file;

	file.open("KeyLocation.txt");

	if (file.is_open() == false)
	{
		cout << "파일을 열지 못했습니다" << endl;
	}
	for (int i = 0; i < 6; ++i)
	{
		file >> loc1[i].x >> loc1[i].y >> loc1[i].z;
	}

	for (int i = 0; i < 9; ++i)
	{
		file >> loc2[i].x >> loc2[i].y >> loc2[i].z;
	}

	for (int i = 0; i < 12; ++i)
	{
		file >> loc3[i].x >> loc3[i].y >> loc3[i].z;
	}
	// 파일불러오기 확인 완료

	srand(time(NULL));

	int x1, x2, x3;

	x1 = rand() % 6;
	x2 = rand() % 9;
	x3 = rand() % 12;

	EscapeKey.push_back(KeyLocation(loc1[x1].x, loc1[x1].y, loc1[x1].z));
	EscapeKey.push_back(KeyLocation(loc2[x2].x, loc2[x2].y, loc2[x2].z));
	EscapeKey.push_back(KeyLocation(loc3[x3].x, loc3[x3].y, loc3[x3].z));


	cout << EscapeKey[0].x << " " << EscapeKey[0].y << " " << EscapeKey[0].z << endl;
	cout << EscapeKey[1].x << " " << EscapeKey[1].y << " " << EscapeKey[1].z << endl;
	cout << EscapeKey[2].x << " " << EscapeKey[2].y << " " << EscapeKey[2].z << endl;


	//클라이언트 초기화

	for (int i = 0; i < MAX_USER; ++i)
	{
		// 생성자 호출됨
		players[i] = new Player;
		//auto& pl = players[i];
		//players[i]->SetId(i);
		//pl.SetId(i);

	}

}

void IocpServer::Finalize()
{
	closesocket(mListenSocket);
	CloseHandle(mIOCP);

	// 윈속 종료
	WSACleanup();
}

void IocpServer::send_pc_lobby(int c_id, int p_id)
{
	s2c_packet_lobby packet;
	packet.id = p_id;
	packet.size = sizeof(packet);
	packet.type = S2C_PACKET_LOBBY;

	packet.host = dynamic_cast<Player*>(players[p_id])->GetHost();
	send_packet(c_id, &packet);
}

void IocpServer::send_packet(int p_id, void* buf)
{
	EX_OVER* s_over = new EX_OVER;

	unsigned char packet_size = reinterpret_cast<unsigned char*>(buf)[0];
	s_over->m_op = OP_SEND;
	memset(&s_over->m_over, 0, sizeof(s_over->m_over));
	memcpy(s_over->m_netbuf, buf, packet_size);
	s_over->m_wsabuf[0].buf = reinterpret_cast<char*>(s_over->m_netbuf);
	s_over->m_wsabuf[0].len = packet_size;

	WSASend(dynamic_cast<Player*>(players[p_id])->GetSocket(), s_over->m_wsabuf, 1, 0, 0, &s_over->m_over, 0);
}

void IocpServer::send_pc_logout(int c_id, int p_id)
{
	s2c_packet_pc_logout packet;
	packet.id = p_id;
	packet.size = sizeof(packet);
	packet.type = S2C_PACKET_PC_LOGOUT;
	send_packet(c_id, &packet);
}