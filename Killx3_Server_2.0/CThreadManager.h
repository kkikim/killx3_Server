#pragma once
#include <WinSock2.h>
#include <Windows.h>
#include <thread>
#include <vector>
#include <mutex>
#include <iostream>
#include <queue>
#include <chrono>
#include "protocol.h"
#include "PacketMgr.h"
using namespace std;
using namespace chrono;

struct Timer_Event {
	int object_id;
	high_resolution_clock::time_point exec_time; // 이 이벤트가 언제 실행되야 하는가
	Event_Type event; // 게임을 확장하며 무브외에 다른것들이 추가될 것
	int roomNum;
};

class comparison {
	bool reverse;
public:
	comparison() {}
	bool operator() (const Timer_Event first, const Timer_Event second) const
	{
		return first.exec_time > second.exec_time;
	}
};


class CThreadManager
{
public:
	static CThreadManager*		GetInstance();

	void						Init();

	void						MakeAcceptThread();
	static void					AcceptThread();

	void						MakeTimerThread();
	static void					TimerThread();

	void						MakeLogicThread();
	static void					LogicThread();

	void						MakeWorkerThread();
	static void					WorkerThread(int);

	void						MakeDBThread();
	static void					DatabaseThread();

	void						ThreadJoin();
	static void					error_display(char *msg, int err_num);

	HANDLE						GetIOCPHandle();

	//priority_queue <Timer_Event, vector<Timer_Event>, comparison> GetTimerQueue();
	static priority_queue <Timer_Event, vector<Timer_Event>, comparison> timer_queue;
	static mutex				tq_lock;

private:
	CThreadManager();
	CThreadManager(const CThreadManager& other) {}
	~CThreadManager();
	static CThreadManager* instance;

	vector<thread*>				workThreadVector;			//WorkThread Vector
	thread*						pAcceptThread;				//AcceptThread
	thread*						pTimerThread;				//TiemrThread
	thread*						pLogicThread;				//Logic Threads
	thread*						pDBThread;

	static CLIENT				clients[MAX_USER];

	static SOCKET accept_socket;

	static HANDLE				hIOCP;
	static int					mCurrentUserNum;

	static mutex				m_mutex;
	
	static mutex				accept_mutex;
};

