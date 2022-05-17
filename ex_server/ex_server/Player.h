#pragma once

#include "Object.h"

class Player :public Object
{
private:
	SOCKET   m_s;

	unordered_set <int> m_viewlist;
	mutex m_vl;

	int last_move_time;

	//lobby
	bool host;
	bool ready;

	// etc
	bool flashlight;


public:
	Player();
	virtual ~Player();
	void RecvPacket(int p_id);
	void ReadPacket(const UINT id, const int transferred, EX_OVER* ex_over);
	void AcceptPacket(const UINT& id, SOCKET c_socket);
	void Disconnect(int p_id, array <Object*, MAX_USER + 1> players);
	void do_recv(int p_id);


public:
	void Mutex_lock() { m_lock.lock(); }
	void Mutex_unlock() { m_lock.unlock(); }
	void SetState(S_STATE state) { m_state = state; }
	void SetGameID(char* name) {
		strcpy(m_name, name);
	}
	void SetHost(const bool state) { host = state; }
	void SetReady(const bool state) { ready = state; }

	void SetFlashlight(const bool n) { flashlight = n; }

public:
	SOCKET& GetSocket() { return m_s; }
	S_STATE GetState() { return m_state; }
	char* GetGameID() { return m_name; }
	bool GetHost() { return host; }
	bool GetReady() { return ready; }
	bool GetFlashlight()  { return flashlight; }

	
};




