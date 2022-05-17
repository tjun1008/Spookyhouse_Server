#pragma once

#include "stdafx.h"

constexpr int NUM_THREADS = 4;

enum OP_TYPE { OP_RECV, OP_SEND, OP_ACCEPT };
enum S_STATE { STATE_FREE, STATE_CONNECTED, STATE_INGAME };

struct EX_OVER {
	WSAOVERLAPPED	m_over;
	WSABUF			m_wsabuf[1];
	unsigned char	m_netbuf[MAX_BUFFER];
	OP_TYPE			m_op;
	SOCKET m_csocket;
};