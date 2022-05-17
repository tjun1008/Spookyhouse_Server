#include "iocp_server.h"



int main()
{
	
	IocpServer* ex_server = new IocpServer;

	if (ex_server->Initialize())
	{
		cout << "server start\n";
		ex_server->Progress();
	}
	
	delete ex_server;

}