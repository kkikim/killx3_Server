#include "CServer.h"

CServer* CServer::instance = nullptr;

CServer::CServer(){}
CServer::~CServer(){}
CServer* CServer::GetInstance() {
	if (instance == nullptr)
		instance = new CServer();
	return instance;
}
void CServer::Init() {

	wcout.imbue(std::locale("korean"));
	_wsetlocale(LC_ALL, L"korean");

	WSADATA   wsadata;
	WSAStartup(MAKEWORD(2, 2), &wsadata);

	CThreadManager::GetInstance()->Init();
}
void CServer::Run() {
	Init();
	MainLoop();
	Destroy();
}
void CServer::Destroy() {
	if (instance != nullptr)
	{
		delete instance;
		instance = nullptr;
	}
}

void CServer::MainLoop() {
	CThreadManager::GetInstance()->MakeAcceptThread();
	CThreadManager::GetInstance()->MakeWorkerThread();
	CThreadManager::GetInstance()->MakeLogicThread();
	CThreadManager::GetInstance()->MakeTimerThread();
	//CThreadManager::GetInstance()->DatabaseThread();
	CThreadManager::GetInstance()->ThreadJoin();
}


