#pragma once



class IocpServer
{
public:
	IocpServer();
	~IocpServer();

	bool Initialize();
	bool StartWorkerThreads();
	void Finalize();
	bool StartAcceptLoop();

	void CreateObjectPool();

public:
	// worker thread¿ë
	static void WorkerThread(LPVOID arg);
	void WorkerThreadFunc();

	void send_pc_lobby(int c_id, int p_id);
	void send_packet(int p_id, void* buf);
	void Disconnect(int p_id);
	void send_pc_logout(int c_id, int p_id);

private:
	HANDLE	mIOCP;
	SOCKET	mListenSocket;

	vector<thread>			mWorkerThreads;
};

//extern IocpServer* GIocpServer;
