#pragma once

#include "Global.h"


void do_accept(SOCKET s_socket, EX_OVER* a_over);
void display_error(const char* msg, int err_no);
int get_new_player_id();
void ReadPacket(const UINT& id, const DWORD& transferred, EX_OVER* ex_over);
void AcceptPacket(const UINT& p_id, SOCKET c_socket, EX_OVER* ex_over);


void worker();
void process_packet(int p_id, unsigned char* packet);
void send_pc_logout(int c_id, int p_id);
void disconnect(int p_id);
void send_packet(int p_id, void* buf);
void send_login_info(int p_id);
void send_pc_lobby(int c_id, int p_id);
void send_ready(int c_id, int p_id);
void send_start(int p_id);
void send_key_info(int p_id);
void send_move_packet(int c_id, int p_id);
bool is_npc(int id);
void player_move(int p_id, c2s_packet_move* move_packet);
void send_pc_login(int c_id, int p_id);
