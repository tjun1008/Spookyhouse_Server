#include "Player.h"
#include "ThreadFunctions.h"

Player::Player() :last_move_time(0)
{
	m_state = STATE_FREE;

}

Player::~Player()
{

}

void Player::ReadPacket(const UINT id, const int transferred, EX_OVER* ex_over)
{
	unsigned char* ps = ex_over->m_netbuf;
	int remain_data = transferred + m_prev_recv;
	while (remain_data > 0) {
		int packet_size = ps[0];
		if (packet_size > remain_data) break;
		process_packet(id, ps);
		remain_data -= packet_size;
		ps += packet_size;
	}
	if (remain_data > 0)
		memcpy(ex_over->m_netbuf, ps, remain_data);
	m_prev_recv = remain_data;
	//do_recv(key);
}

void Player::RecvPacket(int p_id)
{
	//SESSION& pl = players[p_id];
	EX_OVER& r_over = m_recv_over;
	// r_over.m_op = OP_RECV;
	memset(&r_over.m_over, 0, sizeof(r_over.m_over));
	r_over.m_wsabuf[0].buf = reinterpret_cast<CHAR*>(r_over.m_netbuf) + m_prev_recv;
	r_over.m_wsabuf[0].len = MAX_BUFFER - m_prev_recv;
	DWORD r_flag = 0;
	WSARecv(m_s, r_over.m_wsabuf, 1, 0, &r_flag, &r_over.m_over, 0);
}

void Player::Disconnect(int p_id, array <Object*, MAX_USER + 1> players)
{
	{
		//lock_guard<mutex> guard0{ players[p_id].m_lock };
		m_lock.lock();
		m_state = STATE_CONNECTED;
		closesocket(m_s);
		m_state = STATE_FREE;
		m_viewlist.clear();
		m_lock.unlock();
		//players.erase(p_id);
	}

	unordered_set<int> old_vl;
	m_vl.lock();
	old_vl = m_viewlist;
	m_vl.unlock();

	for (auto& c_id : old_vl) {
		send_pc_logout(dynamic_cast<Player*>(players[c_id])->GetID(), p_id);
	}
}

void Player::AcceptPacket(const UINT& id, SOCKET c_socket)
{
	m_s = c_socket;
	m_recv_over.m_op = OP_RECV;
	m_prev_recv = 0;

}

void Player::do_recv(int p_id)
{
	EX_OVER& r_over = m_recv_over;
	// r_over.m_op = OP_RECV;
	memset(&r_over.m_over, 0, sizeof(r_over.m_over));
	r_over.m_wsabuf[0].buf = reinterpret_cast<CHAR*>(r_over.m_netbuf) + m_prev_recv;
	r_over.m_wsabuf[0].len = MAX_BUFFER - m_prev_recv;
	DWORD r_flag = 0;
	WSARecv(m_s, r_over.m_wsabuf, 1, 0, &r_flag, &r_over.m_over, 0);
}

