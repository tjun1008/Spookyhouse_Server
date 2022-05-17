#pragma once


void do_accept(SOCKET s_socket, EX_OVER* a_over);
void display_error(const char* msg, int err_no);

void worker();
void process_packet(int p_id, unsigned char* packet);
void send_pc_logout(int c_id, int p_id);
void disconnect(int p_id);
void send_packet(int p_id, void* buf);
