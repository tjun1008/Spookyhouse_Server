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
		strcpy_s(m_name, name);
	}
	void SetHost(const bool state) { host = state; }
	void SetReady(const bool state) { ready = state; }

	void SetFlashlight(const bool n) { flashlight = n; }

public:
	const SOCKET& GetSocket() const { return m_s; }
	const S_STATE GetState() const { return m_state; }
	const char* GetGameID() const { return m_name; }
	const bool GetHost() const { return host; }
	const bool GetReady() const { return ready; }
	const bool GetFlashlight() const { return flashlight; }

	
};




