#include "stdafx.h"
#include "iocp_server.h"



int main()
{

	IocpServer* ex_server = new IocpServer;

	if (false == ex_server->Initialize())
		return -1;

	if (false == ex_server->StartWorkerThreads())
		return -1;

	cout << "spookyhouse server start\n";

	if (false == ex_server->StartAcceptLoop())
		return -1;

	ex_server->Finalize();

	cout << "End Server\n";

	delete ex_server;

}