#pragma once

#pragma comment(lib, "ws2_32")
#include <iostream>
#include <WinSock2.h>
#include "CThreadManager.h"
using namespace std;
class CServer
{
public:
	static CServer* GetInstance();
	void Run();
	void Init();
	void Destroy();
	void MainLoop();
private:
	CServer();
	~CServer();
	CServer(const CServer& other) {}
	static CServer* instance;
};

