#include "CThreadManager.h"
#include "CaveNormalNPC.h"
#include "CaveMiddleNPC.h"
#include "CaveBossNPC.h"
#include "IceNormalNPC.h"
#include "IceMiddleNPC.h"
#include "IceBossNPC.h"
#include "RemainNormalNPC.h"
#include "RemainMiddleNPC.h"
#include "RemainBossNPC.h"


//--------static 변수들..
mutex   CThreadManager::m_mutex;
mutex	CThreadManager::tq_lock;
mutex	CThreadManager::accept_mutex;
HANDLE	CThreadManager::hIOCP;
int		CThreadManager::mCurrentUserNum;
CThreadManager* CThreadManager::instance = nullptr;
SOCKET CThreadManager::accept_socket;
CLIENT CThreadManager::clients[];
priority_queue <Timer_Event, vector<Timer_Event>, comparison> CThreadManager::timer_queue;
CThreadManager::CThreadManager() { 
	hIOCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, 0);	
	mCurrentUserNum = 0; 
}
CThreadManager::~CThreadManager(){}
CThreadManager* CThreadManager::GetInstance() {
	if (instance == nullptr)
		instance = new CThreadManager();
	return instance;
}
void CThreadManager::error_display(char *msg, int err_num)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, err_num,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	printf("[%s] %s", msg, (char *)lpMsgBuf);
	LocalFree(lpMsgBuf);
}
void CThreadManager::Init() {
	sockaddr_in listen_addr;
	// Overlapped 소켓으로 받을 것이므로 옵션 추가
	accept_socket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);

	ZeroMemory(&listen_addr, sizeof(listen_addr));
	listen_addr.sin_family = AF_INET;
	listen_addr.sin_addr.s_addr = htonl(INADDR_ANY);      //모든 클라이언트의 주소에서 받아라
	listen_addr.sin_port = htons(MY_SERVER_PORT);
	ZeroMemory(&listen_addr.sin_zero, 8);

	//Nagle Off
	int option = 1; // 1: 비활성화, 0: 활성화
	setsockopt(accept_socket, IPPROTO_TCP, TCP_NODELAY, (const char*)&option, sizeof(option));

	::bind(accept_socket, reinterpret_cast<SOCKADDR*>(&listen_addr), sizeof(listen_addr));

	listen(accept_socket, 10);

	//클라이언트 구조체들 초기화작업
	for (int i = 0; i < MAX_USER; ++i)
	{
		clients[i].recv_overlap.recv_buff.buf = reinterpret_cast<CHAR *>(clients[i].recv_overlap.socket_buff);
		clients[i].recv_overlap.recv_buff.len = MAX_BUFF_SIZE;
		clients[i].recv_overlap.operation = OP_RECV;

		clients[i].is_connected = false;
		clients[i].Collision = false;
	}
}
void CThreadManager::MakeAcceptThread() {
	cout << "Create Accept Thread..." << endl;
	pAcceptThread = new thread{ AcceptThread };
	cout << "Create Accept Thread Complete.." << endl;
}
void CThreadManager::MakeWorkerThread() {
	cout << "Create Worekr Thread.. \n";
	for (int i = 0; i < WORKER_THREAD_NUM; ++i)
	{
		workThreadVector.push_back(new thread{ WorkerThread,i + 1 });
	}
	cout << "Create Worker Thread Complete... \n";
}
void CThreadManager::MakeTimerThread() {
	cout << "Create Accept Thread..." << endl;
	pTimerThread = new thread{ TimerThread };
	cout << "Create Accept Thread Complete.." << endl;
}
void CThreadManager::MakeLogicThread() {
	cout << "Create LogicThread..." << endl;
	pLogicThread = new thread{ LogicThread };
	cout << "Create Logic Thread Complete.." << endl;
}
void CThreadManager::MakeDBThread() {
	cout << "Create LogicThread..." << endl;
	pDBThread = new thread{ DatabaseThread };
	cout << "Create Logic Thread Complete.." << endl;
}
void CThreadManager::ThreadJoin()
{
	cout << "Threads Join..." << endl;
	pAcceptThread->join();
	pTimerThread->join();
	pLogicThread->join();
	//pDBThread->join();
	for (auto pThread : workThreadVector)
	{
		pThread->join();
		delete pThread;
	}
}

void CThreadManager::AcceptThread() {

	m_mutex.lock();
	cout << "Accept Thread Working.. \n";
	m_mutex.unlock();

	while (true)
	{
		sockaddr_in client_addr;
		int add_size = sizeof(client_addr);

		SOCKET new_client = ::WSAAccept(accept_socket, reinterpret_cast<SOCKADDR*>(&client_addr), &add_size, NULL, NULL);

		// 새로운 아이디 할당
		int new_id = -1;
		for (int i = 0; i < MAX_USER; ++i){
			if (false == clients[i].is_connected){
				new_id = i;
				break;
			}
		}

		if (-1 == new_id){
			cout << "The Server is Full of User" << endl;
			closesocket(new_client);
			continue;
		}
		// 입출력 포트와 클라이언트 연결
		
		CreateIoCompletionPort(reinterpret_cast<HANDLE>(new_client), hIOCP, new_id, 0);

		cout << "Player " << new_id << " Connected " << endl;

		mCurrentUserNum++;
		// 재활용 될 소켓이므로 초기화해주어야 한다.
		clients[new_id].sock = new_client;
		clients[new_id].is_connected = true;
		clients[new_id].is_gaming = false;
		clients[new_id].id = new_id;

		clients[new_id].recv_overlap.operation = OP_RECV;
		clients[new_id].recv_overlap.packet_size = 0;
		clients[new_id].previous_data_size = 0;

		sc_packet_put_player put_player_pacekt;
		put_player_pacekt.id = (WORD)new_id;
		put_player_pacekt.size = (BYTE)sizeof(sc_packet_put_player);
		put_player_pacekt.type = (BYTE)SC_PUT_PLAYER;

		cout << new_id << " Connect \n";

		CPacketMgr::GetInstance()->
			SendPacket(new_id, reinterpret_cast<unsigned char*>(&put_player_pacekt), clients);

		DWORD flags = 0;
		int result = WSARecv(new_client, &clients[new_id].recv_overlap.recv_buff, 1, NULL, &flags, &clients[new_id].recv_overlap.original_overlap, NULL);

		if (0 != result) {
			int error_num = WSAGetLastError();
			if (WSA_IO_PENDING != error_num) {
				error_display("AcceptThread : WSARecv ", error_num);
			}
		}
	}
}
void CThreadManager::WorkerThread(int threadID) {
	m_mutex.lock();
	cout << "[" << threadID << "]" << "Worker Thread Working.... \n";
	m_mutex.unlock();

	DWORD io_Size;
	unsigned long key;
	OverlapEx* overlap;
	bool bResult;

	while (true)
	{
		bResult = GetQueuedCompletionStatus
		(hIOCP, &io_Size, &key, reinterpret_cast<LPOVERLAPPED*>(&overlap), INFINITE);

		if (false == bResult)
		{
			//Error 처리
			cout << "Worker_Thread Error - GetQueuedCompletionStatus 의 결과가 false" << endl;
		}
		if (0 == io_Size)
		{
			//TODO : 종료 부분 설계
			//같이 게임하던놈 메인으로 보내고
			//게임중이던 방 초기화시키기
		
			continue;
		}
		switch (overlap->operation)
		{
		case OP_RECV:
		{
			unsigned char* pBuff = overlap->socket_buff;
			int remained = io_Size;

			//남은 데이터 사이즈만큼 순회하면서 처리해라.
			while (0 < remained)
			{
				if (clients[key].recv_overlap.packet_size == 0)
				{
					clients[key].recv_overlap.packet_size = pBuff[0];      //모든 패킷의 맨 앞 칸은 사이즈이다.
				}

				// 이전에 받은 패킷과 총량을 비교하여 남은 데이터 조사
				int required = clients[key].recv_overlap.packet_size - clients[key].previous_data_size;

				// 패킷 완성
				if (remained >= required) 
				{
					Sleep(1);
					//지난번에 받은 데이터 뒷부분에 복사
					memcpy(clients[key].packet + clients[key].previous_data_size, pBuff, required);

					CPacketMgr::GetInstance()->ProcessPacket(key, clients[key].packet, clients);

					remained -= required;
					pBuff += required;
					clients[key].recv_overlap.packet_size = 0;
					clients[key].previous_data_size = 0;
				}
				else 
				{
					Sleep(1);
					memcpy(clients[key].packet + clients[key].previous_data_size, pBuff, remained);
					//미완성 패킷의 사이즈가 remained만큼 증가했다.
					clients[key].previous_data_size += remained;
					remained = 0;
					pBuff++;
				}
			}
			DWORD flags = 0;
			WSARecv(clients[key].sock, &clients[key].recv_overlap.recv_buff, 1, NULL, &flags, reinterpret_cast<LPWSAOVERLAPPED>(&clients[key].recv_overlap), NULL);

			break;
		}
		case OP_SEND:
		{
			if (io_Size != overlap->recv_buff.len)
			{
				cout << "Send Error.. \n";
				closesocket(clients[key].sock);
				clients[key].is_connected = false;
			}
			delete overlap;
			break;
		}
		case OP_MOVE:
		{
			sc_packet_pos pos_packet;
			pos_packet.size = sizeof(sc_packet_pos);
			pos_packet.type = SC_POS;
			pos_packet.pos = clients[key].player.m_vPosition;
			pos_packet.id = key;
			
			for (int i = 0; i < 2; ++i)
			{	
				CPacketMgr::GetInstance()->	SendPacket(clients[key].roomList[i],
						reinterpret_cast<unsigned char*>(&pos_packet), clients);
			}


			Timer_Event playerEvent;
			playerEvent.event = E_MOVE;
			playerEvent.exec_time = high_resolution_clock::now() + 3s;
			playerEvent.object_id = key;
			playerEvent.roomNum = clients[key].roomNumber;

			tq_lock.lock();
			timer_queue.push(playerEvent);
			tq_lock.unlock();
			

			delete overlap;
			break;
		}
		case OP_NPC:
		{
			sc_packet_npc_pos npc_pos_packet;
			npc_pos_packet.size = sizeof(sc_packet_npc_pos);
			npc_pos_packet.type = SC_NPC_POS;

			if (1 == CPacketMgr::GetInstance()->GetRoom(key)->GetCurrentStage())
			{
				npc_pos_packet.npcs_data[0].id = 0;
				npc_pos_packet.npcs_data[0].pos
					= CPacketMgr::GetInstance()->GetRoom(key)->GetCaveStage1_NPC_Vector()[0]->GetPos();

				npc_pos_packet.npcs_data[1].id = 1;
				npc_pos_packet.npcs_data[1].pos
					= CPacketMgr::GetInstance()->GetRoom(key)->GetCaveStage1_NPC_Vector()[1]->GetPos();

				npc_pos_packet.npcs_data[2].id = 2;
				npc_pos_packet.npcs_data[2].pos
					= CPacketMgr::GetInstance()->GetRoom(key)->GetCaveStage1_NPC_Vector()[2]->GetPos();

				npc_pos_packet.npcs_data[3].id = 3;
				npc_pos_packet.npcs_data[3].pos
					= CPacketMgr::GetInstance()->GetRoom(key)->GetCaveStage1_NPC_Vector()[3]->GetPos();

				for (int i = 0; i < 2; ++i){
					CPacketMgr::GetInstance()->SendPacket(CPacketMgr::GetInstance()->GetRoom(key)->GetClientID(i),
						reinterpret_cast<unsigned char*>(&npc_pos_packet), clients);
				}


				/*for (int i = 0; i < 4; ++i)
				{
					if (false ==
						CPacketMgr::GetInstance()->GetRoom(key)->GetStage1_NPC_Vector()[i]->GetAlive())
						Dead_NPC_Num++;
				}*/


				Timer_Event Stage1_NPC_Event;
				Stage1_NPC_Event.event = E_NMOVE;
				Stage1_NPC_Event.exec_time = high_resolution_clock::now() + 1s;
				Stage1_NPC_Event.object_id = key;
				Stage1_NPC_Event.roomNum = key;

				tq_lock.lock();
				timer_queue.push(Stage1_NPC_Event);
				tq_lock.unlock();

			}

			else if (2 == CPacketMgr::GetInstance()->GetRoom(key)->GetCurrentStage()) 
			{
				npc_pos_packet.npcs_data[0].id = 0;
				npc_pos_packet.npcs_data[0].pos
					= CPacketMgr::GetInstance()->GetRoom(key)->GetCaveStage2_NPC_Vector()[0]->GetPos();

				npc_pos_packet.npcs_data[1].id = 1;
				npc_pos_packet.npcs_data[1].pos
					= CPacketMgr::GetInstance()->GetRoom(key)->GetCaveStage2_NPC_Vector()[1]->GetPos();

				npc_pos_packet.npcs_data[2].id = 2;
				npc_pos_packet.npcs_data[2].pos
					= CPacketMgr::GetInstance()->GetRoom(key)->GetCaveStage2_NPC_Vector()[2]->GetPos();

				for (int i = 0; i < 2; ++i) {
					CPacketMgr::GetInstance()->SendPacket(CPacketMgr::GetInstance()->GetRoom(key)->GetClientID(i),
						reinterpret_cast<unsigned char*>(&npc_pos_packet), clients);
				}


				/*for (int i = 0; i < 3; ++i) 
				{
					if (false ==
						CPacketMgr::GetInstance()->GetRoom(key)->GetStage2_NPC_Vector()[i]->GetAlive())
				}*/


				Timer_Event Stage2_NPC_Event;
				Stage2_NPC_Event.event = E_NMOVE;
				Stage2_NPC_Event.exec_time = high_resolution_clock::now() + 1s;
				Stage2_NPC_Event.object_id = key;
				Stage2_NPC_Event.roomNum = key;

				tq_lock.lock();
				timer_queue.push(Stage2_NPC_Event);
				tq_lock.unlock();
			}

			else if (3 == CPacketMgr::GetInstance()->GetRoom(key)->GetCurrentStage())
			{
				npc_pos_packet.npcs_data[0].id = 0;
				npc_pos_packet.npcs_data[0].pos
					= CPacketMgr::GetInstance()->GetRoom(key)->GetCaveStage3_NPC_Vector()[0]->GetPos();

				for (int i = 0; i < 2; ++i) 
				{
					CPacketMgr::GetInstance()->SendPacket(CPacketMgr::GetInstance()->GetRoom(key)->GetClientID(i),
						reinterpret_cast<unsigned char*>(&npc_pos_packet), clients);
				}

					Timer_Event Stage3_NPC_Event;
					Stage3_NPC_Event.event = E_NMOVE;
					Stage3_NPC_Event.exec_time = high_resolution_clock::now() + 1s;
					Stage3_NPC_Event.object_id = key;
					Stage3_NPC_Event.roomNum = key;

					tq_lock.lock();
					timer_queue.push(Stage3_NPC_Event);
					tq_lock.unlock();
				
			}

			delete overlap;
			break;
		}
		case OP_SEND_NPC_DIR: 
		{
			sc_packet_npc_dir NPCDirPacket;
			NPCDirPacket.size = sizeof(sc_packet_npc_dir);
			NPCDirPacket.type = SC_NPC_DIR;
			NPCDirPacket.id = overlap->ID; 

			int StageNum = CPacketMgr::GetInstance()->GetRoom(key)->GetCurrentStage();

			if (StageNum == 1)
			{
				NPCDirPacket.Pos = 
					CPacketMgr::GetInstance()->GetRoom(key)->GetCaveStage1_NPC_Vector()[overlap->ID]->GetPos();
				NPCDirPacket.Direction = 
					CPacketMgr::GetInstance()->GetRoom(key)->GetCaveStage1_NPC_Vector()[overlap->ID]->GetDir();
				NPCDirPacket.ani = 
					CPacketMgr::GetInstance()->GetRoom(key)->GetCaveStage1_NPC_Vector()[overlap->ID]->GetAniState();

				for (int i = 0; i < 2; ++i)
				{
					CPacketMgr::GetInstance()->SendPacket(CPacketMgr::GetInstance()->GetRoom(key)->GetClientID(i),
						reinterpret_cast<unsigned char*>(&NPCDirPacket), clients);
				}
			}
			else if (StageNum == 2) 
			{
				NPCDirPacket.Pos =
					CPacketMgr::GetInstance()->GetRoom(key)->GetCaveStage2_NPC_Vector()[overlap->ID]->GetPos();
				NPCDirPacket.Direction =
					CPacketMgr::GetInstance()->GetRoom(key)->GetCaveStage2_NPC_Vector()[overlap->ID]->GetDir();
				NPCDirPacket.ani = 
					CPacketMgr::GetInstance()->GetRoom(key)->GetCaveStage2_NPC_Vector()[overlap->ID]->GetAniState();

				for (int i = 0; i < 2; ++i) 
				{
					CPacketMgr::GetInstance()->SendPacket(CPacketMgr::GetInstance()->GetRoom(key)->GetClientID(i),
						reinterpret_cast<unsigned char*>(&NPCDirPacket), clients);
				}
			}
			else 
			{

				NPCDirPacket.Pos =
					CPacketMgr::GetInstance()->GetRoom(key)->GetCaveStage3_NPC_Vector()[0]->GetPos();
				NPCDirPacket.Direction =
					CPacketMgr::GetInstance()->GetRoom(key)->GetCaveStage3_NPC_Vector()[0]->GetDir();
				NPCDirPacket.ani = 
					CPacketMgr::GetInstance()->GetRoom(key)->GetCaveStage3_NPC_Vector()[0]->GetAniState();

				for (int i = 0; i < 2; ++i)
				{
					CPacketMgr::GetInstance()->SendPacket(CPacketMgr::GetInstance()->GetRoom(key)->GetClientID(i),
						reinterpret_cast<unsigned char*>(&NPCDirPacket), clients);
				}

			}
	
			delete overlap;
			break;
		}
		case OP_MOVE_ICE:
		{
			sc_packet_pos pos_packet;
			pos_packet.size = sizeof(sc_packet_pos);
			pos_packet.type = SC_POS;
			pos_packet.pos = clients[key].player.m_vPosition;
			pos_packet.id = key;

			for (int i = 0; i < 2; ++i)
			{
				CPacketMgr::GetInstance()->SendPacket(clients[key].roomList[i],
					reinterpret_cast<unsigned char*>(&pos_packet), clients);
			}

			Sleep(1);

			Timer_Event playerEvent;
			playerEvent.event = E_MOVE_ICE;
			playerEvent.exec_time = high_resolution_clock::now() + 3s;
			playerEvent.object_id = key;
			playerEvent.roomNum = clients[key].roomNumber;

			tq_lock.lock();
			timer_queue.push(playerEvent);
			tq_lock.unlock();

			delete overlap;
			break;
		}
		case OP_NPC_ICE:
		{
			sc_packet_npc_pos npc_pos_packet;
			npc_pos_packet.size = sizeof(sc_packet_npc_pos);
			npc_pos_packet.type = SC_NPC_POS;

			Sleep(1);

			if (1 == CPacketMgr::GetInstance()->GetIceRoom(key)->GetCurrentStage())
			{
				npc_pos_packet.npcs_data[0].id = 0;
				npc_pos_packet.npcs_data[0].pos
					= CPacketMgr::GetInstance()->GetIceRoom(key)->GetIceStage1_NPC_Vector()[0]->GetPos();

				npc_pos_packet.npcs_data[1].id = 1;
				npc_pos_packet.npcs_data[1].pos
					= CPacketMgr::GetInstance()->GetIceRoom(key)->GetIceStage1_NPC_Vector()[1]->GetPos();

				npc_pos_packet.npcs_data[2].id = 2;
				npc_pos_packet.npcs_data[2].pos
					= CPacketMgr::GetInstance()->GetIceRoom(key)->GetIceStage1_NPC_Vector()[2]->GetPos();

				npc_pos_packet.npcs_data[3].id = 3;
				npc_pos_packet.npcs_data[3].pos
					= CPacketMgr::GetInstance()->GetIceRoom(key)->GetIceStage1_NPC_Vector()[3]->GetPos();

				for (int i = 0; i < 2; ++i) {
					CPacketMgr::GetInstance()->SendPacket(CPacketMgr::GetInstance()->GetIceRoom(key)->GetClientID(i),
						reinterpret_cast<unsigned char*>(&npc_pos_packet), clients);
				}


				/*for (int i = 0; i < 4; ++i)
				{
				if (false ==
				CPacketMgr::GetInstance()->GetRoom(key)->GetStage1_NPC_Vector()[i]->GetAlive())
				Dead_NPC_Num++;
				}*/

				Timer_Event Stage1_NPC_Event;
				Stage1_NPC_Event.event = E_NMOVE_ICE;
				Stage1_NPC_Event.exec_time = high_resolution_clock::now() + 1s;
				Stage1_NPC_Event.object_id = key;
				Stage1_NPC_Event.roomNum = key;

				tq_lock.lock();
				timer_queue.push(Stage1_NPC_Event);
				tq_lock.unlock();

			}

			else if (2 == CPacketMgr::GetInstance()->GetIceRoom(key)->GetCurrentStage())
			{
				npc_pos_packet.npcs_data[0].id = 0;
				npc_pos_packet.npcs_data[0].pos
					= CPacketMgr::GetInstance()->GetIceRoom(key)->GetIceStage2_NPC_Vector()[0]->GetPos();

				npc_pos_packet.npcs_data[1].id = 1;
				npc_pos_packet.npcs_data[1].pos
					= CPacketMgr::GetInstance()->GetIceRoom(key)->GetIceStage2_NPC_Vector()[1]->GetPos();

				npc_pos_packet.npcs_data[2].id = 2;
				npc_pos_packet.npcs_data[2].pos
					= CPacketMgr::GetInstance()->GetIceRoom(key)->GetIceStage2_NPC_Vector()[2]->GetPos();

				for (int i = 0; i < 2; ++i) {
					CPacketMgr::GetInstance()->SendPacket(CPacketMgr::GetInstance()->GetIceRoom(key)->GetClientID(i),
						reinterpret_cast<unsigned char*>(&npc_pos_packet), clients);
				}


				/*for (int i = 0; i < 3; ++i)
				{
				if (false ==
				CPacketMgr::GetInstance()->GetRoom(key)->GetStage2_NPC_Vector()[i]->GetAlive())
				}*/


				Timer_Event Stage2_NPC_Event;
				Stage2_NPC_Event.event = E_NMOVE_ICE;
				Stage2_NPC_Event.exec_time = high_resolution_clock::now() + 1s;
				Stage2_NPC_Event.object_id = key;
				Stage2_NPC_Event.roomNum = key;

				tq_lock.lock();
				timer_queue.push(Stage2_NPC_Event);
				tq_lock.unlock();
			}

			else if (3 == CPacketMgr::GetInstance()->GetIceRoom(key)->GetCurrentStage())
			{
				npc_pos_packet.npcs_data[0].id = 0;
				npc_pos_packet.npcs_data[0].pos
					= CPacketMgr::GetInstance()->GetIceRoom(key)->GetIceStage3_NPC_Vector()[0]->GetPos();


				for (int i = 0; i < 2; ++i)
				{
					CPacketMgr::GetInstance()->SendPacket(CPacketMgr::GetInstance()->GetIceRoom(key)->GetClientID(i),
						reinterpret_cast<unsigned char*>(&npc_pos_packet), clients);
				}

				Timer_Event Stage3_NPC_Event;
				Stage3_NPC_Event.event = E_NMOVE_ICE;
				Stage3_NPC_Event.exec_time = high_resolution_clock::now() + 1s;
				Stage3_NPC_Event.object_id = key;
				Stage3_NPC_Event.roomNum = key;

				tq_lock.lock();
				timer_queue.push(Stage3_NPC_Event);
				tq_lock.unlock();

			}
			delete overlap;
			break;
		}
		case OP_SEND_NPC_DIR_ICE:
		{
			sc_packet_npc_dir NPCDirPacket;
			NPCDirPacket.size = sizeof(sc_packet_npc_dir);
			NPCDirPacket.type = SC_NPC_DIR;
			NPCDirPacket.id = overlap->ID; 

			int StageNum = CPacketMgr::GetInstance()->GetIceRoom(key)->GetCurrentStage();

			if (StageNum == 1)
			{
				NPCDirPacket.Pos = 
					CPacketMgr::GetInstance()->GetIceRoom(key)->GetIceStage1_NPC_Vector()[overlap->ID]->GetPos();
				NPCDirPacket.Direction = 
					CPacketMgr::GetInstance()->GetIceRoom(key)->GetIceStage1_NPC_Vector()[overlap->ID]->GetDir();
				NPCDirPacket.ani = 
					CPacketMgr::GetInstance()->GetIceRoom(key)->GetIceStage1_NPC_Vector()[overlap->ID]->GetAniState();

				for (int i = 0; i < 2; ++i)
				{
					CPacketMgr::GetInstance()->SendPacket(CPacketMgr::GetInstance()->GetIceRoom(key)->GetClientID(i),
						reinterpret_cast<unsigned char*>(&NPCDirPacket), clients);
				}
			}
			else if (StageNum == 2) 
			{
				NPCDirPacket.Pos =
					CPacketMgr::GetInstance()->GetIceRoom(key)->GetIceStage2_NPC_Vector()[overlap->ID]->GetPos();
				NPCDirPacket.Direction =
					CPacketMgr::GetInstance()->GetIceRoom(key)->GetIceStage2_NPC_Vector()[overlap->ID]->GetDir();
				NPCDirPacket.ani = 
					CPacketMgr::GetInstance()->GetIceRoom(key)->GetIceStage2_NPC_Vector()[overlap->ID]->GetAniState();

				for (int i = 0; i < 2; ++i) 
				{
					CPacketMgr::GetInstance()->SendPacket(CPacketMgr::GetInstance()->GetIceRoom(key)->GetClientID(i),
						reinterpret_cast<unsigned char*>(&NPCDirPacket), clients);
				}
			}

			else 
			{
				NPCDirPacket.Pos =
					CPacketMgr::GetInstance()->GetIceRoom(key)->GetIceStage3_NPC_Vector()[0]->GetPos();
				NPCDirPacket.Direction =
					CPacketMgr::GetInstance()->GetIceRoom(key)->GetIceStage3_NPC_Vector()[0]->GetDir();
				NPCDirPacket.ani = 
					CPacketMgr::GetInstance()->GetIceRoom(key)->GetIceStage3_NPC_Vector()[0]->GetAniState();

				for (int i = 0; i < 2; ++i)
				{
					CPacketMgr::GetInstance()->SendPacket(CPacketMgr::GetInstance()->GetIceRoom(key)->GetClientID(i),
						reinterpret_cast<unsigned char*>(&NPCDirPacket), clients);
				}
			}
			delete overlap;
			break;
		}
		case OP_MOVE_REMAIN:
		{
			sc_packet_pos pos_packet;
			pos_packet.size = sizeof(sc_packet_pos);
			pos_packet.type = SC_POS;
			pos_packet.pos = clients[key].player.m_vPosition;
			pos_packet.id = key;

			for (int i = 0; i < 2; ++i)
			{
				CPacketMgr::GetInstance()->SendPacket(clients[key].roomList[i],
					reinterpret_cast<unsigned char*>(&pos_packet), clients);
			}

			Sleep(1);

			Timer_Event playerEvent;
			playerEvent.event = E_MOVE_REMAIN;
			playerEvent.exec_time = high_resolution_clock::now() + 3s;
			playerEvent.object_id = key;
			playerEvent.roomNum = clients[key].roomNumber;

			tq_lock.lock();
			timer_queue.push(playerEvent);
			tq_lock.unlock();

			delete overlap;
			break;
		}
		case OP_NPC_REMAIN:
		{
			sc_packet_npc_pos npc_pos_packet;
			npc_pos_packet.size = sizeof(sc_packet_npc_pos);
			npc_pos_packet.type = SC_NPC_POS;

			Sleep(1);

			if (1 == CPacketMgr::GetInstance()->GetRemainRoom(key)->GetCurrentStage())
			{
				npc_pos_packet.npcs_data[0].id = 0;
				npc_pos_packet.npcs_data[0].pos
					= CPacketMgr::GetInstance()->GetRemainRoom(key)->GetRemainStage1_NPC_Vector()[0]->GetPos();

				npc_pos_packet.npcs_data[1].id = 1;
				npc_pos_packet.npcs_data[1].pos
					= CPacketMgr::GetInstance()->GetRemainRoom(key)->GetRemainStage1_NPC_Vector()[1]->GetPos();

				npc_pos_packet.npcs_data[2].id = 2;
				npc_pos_packet.npcs_data[2].pos
					= CPacketMgr::GetInstance()->GetRemainRoom(key)->GetRemainStage1_NPC_Vector()[2]->GetPos();

				npc_pos_packet.npcs_data[3].id = 3;
				npc_pos_packet.npcs_data[3].pos
					= CPacketMgr::GetInstance()->GetRemainRoom(key)->GetRemainStage1_NPC_Vector()[3]->GetPos();

				for (int i = 0; i < 2; ++i) 
				{
					CPacketMgr::GetInstance()->SendPacket(CPacketMgr::GetInstance()->GetRemainRoom(key)->GetClientID(i),
						reinterpret_cast<unsigned char*>(&npc_pos_packet), clients);
				}


				/*for (int i = 0; i < 4; ++i)
				{
				if (false ==
				CPacketMgr::GetInstance()->GetRoom(key)->GetStage1_NPC_Vector()[i]->GetAlive())
				Dead_NPC_Num++;
				}*/

				Timer_Event Stage1_NPC_Event;
				Stage1_NPC_Event.event = E_NMOVE_REMAIN;
				Stage1_NPC_Event.exec_time = high_resolution_clock::now() + 1s;
				Stage1_NPC_Event.object_id = key;
				Stage1_NPC_Event.roomNum = key;

				tq_lock.lock();
				timer_queue.push(Stage1_NPC_Event);
				tq_lock.unlock();

			}

			else if (2 == CPacketMgr::GetInstance()->GetRemainRoom(key)->GetCurrentStage())
			{
				npc_pos_packet.npcs_data[0].id = 0;
				npc_pos_packet.npcs_data[0].pos
					= CPacketMgr::GetInstance()->GetRemainRoom(key)->GetRemainStage2_NPC_Vector()[0]->GetPos();

				npc_pos_packet.npcs_data[1].id = 1;
				npc_pos_packet.npcs_data[1].pos
					= CPacketMgr::GetInstance()->GetRemainRoom(key)->GetRemainStage2_NPC_Vector()[1]->GetPos();

				npc_pos_packet.npcs_data[2].id = 2;
				npc_pos_packet.npcs_data[2].pos
					= CPacketMgr::GetInstance()->GetRemainRoom(key)->GetRemainStage2_NPC_Vector()[2]->GetPos();

				for (int i = 0; i < 2; ++i) {
					CPacketMgr::GetInstance()->SendPacket(CPacketMgr::GetInstance()->GetRemainRoom(key)->GetClientID(i),
						reinterpret_cast<unsigned char*>(&npc_pos_packet), clients);
				}


				/*for (int i = 0; i < 3; ++i)
				{
				if (false ==
				CPacketMgr::GetInstance()->GetRoom(key)->GetStage2_NPC_Vector()[i]->GetAlive())
				}*/


				Timer_Event Stage2_NPC_Event;
				Stage2_NPC_Event.event = E_NMOVE_REMAIN;
				Stage2_NPC_Event.exec_time = high_resolution_clock::now() + 1s;
				Stage2_NPC_Event.object_id = key;
				Stage2_NPC_Event.roomNum = key;

				tq_lock.lock();
				timer_queue.push(Stage2_NPC_Event);
				tq_lock.unlock();
			}

			else if (3 == CPacketMgr::GetInstance()->GetRemainRoom(key)->GetCurrentStage())
			{
				npc_pos_packet.npcs_data[0].id = 0;
				npc_pos_packet.npcs_data[0].pos
					= CPacketMgr::GetInstance()->GetRemainRoom(key)->GetRemainStage3_NPC_Vector()[0]->GetPos();

				for (int i = 0; i < 2; ++i)
				{
					CPacketMgr::GetInstance()->SendPacket(CPacketMgr::GetInstance()->GetRemainRoom(key)->GetClientID(i),
						reinterpret_cast<unsigned char*>(&npc_pos_packet), clients);
				}

				Timer_Event Stage3_NPC_Event;
				Stage3_NPC_Event.event = E_NMOVE_REMAIN;
				Stage3_NPC_Event.exec_time = high_resolution_clock::now() + 1s;
				Stage3_NPC_Event.object_id = key;
				Stage3_NPC_Event.roomNum = key;

				tq_lock.lock();
				timer_queue.push(Stage3_NPC_Event);
				tq_lock.unlock();

			}
			delete overlap;
			break;
		}
		case OP_SEND_NPC_DIR_REMAIN:
		{
			sc_packet_npc_dir NPCDirPacket;
			NPCDirPacket.size = sizeof(sc_packet_npc_dir);
			NPCDirPacket.type = SC_NPC_DIR;
			NPCDirPacket.id = overlap->ID;

			int StageNum = CPacketMgr::GetInstance()->GetRemainRoom(key)->GetCurrentStage();

			if (StageNum == 1)
			{
				NPCDirPacket.Pos =
					CPacketMgr::GetInstance()->GetRemainRoom(key)->GetRemainStage1_NPC_Vector()[overlap->ID]->GetPos();
				NPCDirPacket.Direction =
					CPacketMgr::GetInstance()->GetRemainRoom(key)->GetRemainStage1_NPC_Vector()[overlap->ID]->GetDir();
				NPCDirPacket.ani =
					CPacketMgr::GetInstance()->GetRemainRoom(key)->GetRemainStage1_NPC_Vector()[overlap->ID]->GetAniState();

				for (int i = 0; i < 2; ++i)
				{
					CPacketMgr::GetInstance()->SendPacket(CPacketMgr::GetInstance()->GetRemainRoom(key)->GetClientID(i),
						reinterpret_cast<unsigned char*>(&NPCDirPacket), clients);
				}
			}
			else if (StageNum == 2)
			{
				NPCDirPacket.Pos =
					CPacketMgr::GetInstance()->GetRemainRoom(key)->GetRemainStage2_NPC_Vector()[overlap->ID]->GetPos();
				NPCDirPacket.Direction =
					CPacketMgr::GetInstance()->GetRemainRoom(key)->GetRemainStage2_NPC_Vector()[overlap->ID]->GetDir();
				NPCDirPacket.ani =
					CPacketMgr::GetInstance()->GetRemainRoom(key)->GetRemainStage2_NPC_Vector()[overlap->ID]->GetAniState();

				for (int i = 0; i < 2; ++i)
				{
					CPacketMgr::GetInstance()->SendPacket(CPacketMgr::GetInstance()->GetRemainRoom(key)->GetClientID(i),
						reinterpret_cast<unsigned char*>(&NPCDirPacket), clients);
				}
			}

			else
			{
				NPCDirPacket.Pos =
					CPacketMgr::GetInstance()->GetRemainRoom(key)->GetRemainStage3_NPC_Vector()[0]->GetPos();
				NPCDirPacket.Direction =
					CPacketMgr::GetInstance()->GetRemainRoom(key)->GetRemainStage3_NPC_Vector()[0]->GetDir();
				NPCDirPacket.ani =
					CPacketMgr::GetInstance()->GetRemainRoom(key)->GetRemainStage3_NPC_Vector()[0]->GetAniState();

				for (int i = 0; i < 2; ++i)
				{
					CPacketMgr::GetInstance()->SendPacket(CPacketMgr::GetInstance()->GetRemainRoom(key)->GetClientID(i),
						reinterpret_cast<unsigned char*>(&NPCDirPacket), clients);
				}
			}
			delete overlap;
			break;
		}
		case OP_NPC_DEATH:
		{
			sc_packet_NpcDeath NPCDeathPacket;
			NPCDeathPacket.size = sizeof(sc_packet_NpcDeath);
			NPCDeathPacket.type = SC_NPC_DEATH;
			NPCDeathPacket.m_Id = overlap->ID;
			NPCDeathPacket.bAlive = false;
			
			if(overlap->MapType == MAP_CAVE)
				NPCDeathPacket.bRemove = CPacketMgr::GetInstance()->GetRoom(key)->GetCaveStage1_NPC_Vector()[overlap->ID]->GetRemove();
			else if (overlap->MapType == MAP_ICE)
				NPCDeathPacket.bRemove = CPacketMgr::GetInstance()->GetIceRoom(key)->GetIceStage1_NPC_Vector()[overlap->ID]->GetRemove();
			else if (overlap->MapType == MAP_REMAIN)
				NPCDeathPacket.bRemove = CPacketMgr::GetInstance()->GetRemainRoom(key)->GetRemainStage1_NPC_Vector()[overlap->ID]->GetRemove();

			if (MAP_CAVE == overlap->MapType)
			{
				for (int i = 0; i < 2; ++i)
				{
					CPacketMgr::GetInstance()->SendPacket(CPacketMgr::GetInstance()->GetRoom(key)->GetClientID(i),
						reinterpret_cast<unsigned char*>(&NPCDeathPacket), clients);
				}
			}

			else if (MAP_ICE == overlap->MapType)
			{
				for (int i = 0; i < 2; ++i)
				{
					CPacketMgr::GetInstance()->SendPacket(CPacketMgr::GetInstance()->GetIceRoom(key)->GetClientID(i),
						reinterpret_cast<unsigned char*>(&NPCDeathPacket), clients);
				}
			}

			else if (MAP_REMAIN == overlap->MapType)
			{
				for (int i = 0; i < 2; ++i)
				{
					CPacketMgr::GetInstance()->SendPacket(CPacketMgr::GetInstance()->GetRemainRoom(key)->GetClientID(i),
						reinterpret_cast<unsigned char*>(&NPCDeathPacket), clients);
				}
			}
			break;
		}

		case OP_CAVELOBBY_BACK:
		{
			sc_lobby_back LobbyBack;
			LobbyBack.size = sizeof(sc_lobby_back);
			LobbyBack.type = SC_LOBBY_BACK;
			
			//방데이터 초기화

			for (int i = 0; i < 2; ++i)
			{
				CPacketMgr::GetInstance()->SendPacket(CPacketMgr::GetInstance()->GetRoom(key)->GetClientID(i),
					reinterpret_cast<unsigned char*>(&LobbyBack), clients);
			}


			CPacketMgr::GetInstance()->GetRoom(key)->DestroyData();

			break;
		}
		
		case OP_ICELOBBY_BACK:
		{
			sc_lobby_back LobbyBack;
			LobbyBack.size = sizeof(sc_lobby_back);
			LobbyBack.type = SC_LOBBY_BACK;

			//방데이터 초기화
			for (int i = 0; i < 2; ++i)
			{
				CPacketMgr::GetInstance()->SendPacket(CPacketMgr::GetInstance()->GetIceRoom(key)->GetClientID(i),
					reinterpret_cast<unsigned char*>(&LobbyBack), clients);
			}

			CPacketMgr::GetInstance()->GetIceRoom(key)->DestroyData();

			break;
		}

		case OP_REMAINLOBBY_BACK:
		{
			sc_lobby_back LobbyBack;
			LobbyBack.size = sizeof(sc_lobby_back);
			LobbyBack.type = SC_LOBBY_BACK;

			//방데이터 초기화
			
			for (int i = 0; i < 2; ++i)
			{
				CPacketMgr::GetInstance()->SendPacket(CPacketMgr::GetInstance()->GetRemainRoom(key)->GetClientID(i),
					reinterpret_cast<unsigned char*>(&LobbyBack), clients);
			}
			
			CPacketMgr::GetInstance()->GetRemainRoom(key)->DestroyData();

			break;
		}

		default:
		{
			cout<<"OP : " << overlap->operation << endl;

			cout << "Unknown Event on Worker_Thread" << endl;
			while (true);
			break;
		}
		}
	}
}
void CThreadManager::TimerThread() {
	for (;;) 
	{
		Sleep(10);
		for(;;)
		{
			tq_lock.lock();
			if (0 == timer_queue.size())
			{
				tq_lock.unlock();
				break;
			}
			Timer_Event t = timer_queue.top(); 
			if (t.exec_time > high_resolution_clock::now()) 
			{
				tq_lock.unlock();
				break; 
			}
			timer_queue.pop();
			tq_lock.unlock();
			OverlapEx *over = new OverlapEx;

			/*if (E_MOVE == t.event)
				over->event_type = OP_MOVE;*/

			OverlapEx *send_over = new OverlapEx;

			switch (t.event)
			{
				case E_MOVE:
				{
					memset(send_over, 0, sizeof(OverlapEx));
					send_over->operation = OP_MOVE;
					send_over->MapType = MAP_CAVE;
					PostQueuedCompletionStatus(hIOCP, 1, t.object_id, &(send_over->original_overlap));
					break;
				}
				case E_NMOVE:
				{
					memset(send_over, 0, sizeof(OverlapEx));
					send_over->operation = OP_NPC;
					send_over->MapType = MAP_CAVE;
					PostQueuedCompletionStatus(hIOCP, 1, t.object_id, &(send_over->original_overlap));
					break;
				}
				case E_MOVE_ICE:
				{
					memset(send_over, 0, sizeof(OverlapEx));
					send_over->operation = OP_MOVE_ICE;
					send_over->MapType = MAP_ICE;
					PostQueuedCompletionStatus(hIOCP, 1, t.object_id, &(send_over->original_overlap));
					break;
				}
				case E_NMOVE_ICE:
				{
					memset(send_over, 0, sizeof(OverlapEx));
					send_over->operation = OP_NPC_ICE;
					send_over->MapType = MAP_ICE;
					PostQueuedCompletionStatus(hIOCP, 1, t.object_id, &(send_over->original_overlap));
					break;
				}
				case E_MOVE_REMAIN:
				{
					memset(send_over, 0, sizeof(OverlapEx));
					send_over->operation = OP_MOVE_REMAIN;
					send_over->MapType = MAP_REMAIN;
					PostQueuedCompletionStatus(hIOCP, 1, t.object_id, &(send_over->original_overlap));
					break;
				}
				case E_NMOVE_REMAIN:
				{
					memset(send_over, 0, sizeof(OverlapEx));
					send_over->operation = OP_NPC_REMAIN;
					send_over->MapType = MAP_REMAIN;
					PostQueuedCompletionStatus(hIOCP, 1, t.object_id, &(send_over->original_overlap));
					break;
				}
				default:
				{
					cout << "Unknown Event Type \n;";
					break;
				}
			}
		}
	}
}
void CThreadManager::LogicThread() {
	LARGE_INTEGER	m_tSecond;
	LARGE_INTEGER	m_tCount;

	volatile float m_fDeltaTime;
	
	QueryPerformanceFrequency((LARGE_INTEGER*)&m_tSecond);
	QueryPerformanceCounter((LARGE_INTEGER*)&m_tCount);

	while (true)
	{
		LARGE_INTEGER tCount = {};

		QueryPerformanceCounter(&tCount);

		m_fDeltaTime = ((tCount.QuadPart - m_tCount.QuadPart) / (float)m_tSecond.QuadPart);

		m_tCount = tCount;

		for (int i = 0; i < MAX_ROOM; ++i)
		{
			//Cave Update
			if (true == CPacketMgr::GetInstance()->GetRoom(i)->GetRoomState())
			{
				//거리=시간*속력

				//씬전환시 데이터 레이스 발생할수도

				clients[CPacketMgr::GetInstance()->GetRoom(i)->GetClientID(0)].player.m_vPosition
					+= clients[CPacketMgr::GetInstance()->GetRoom(i)->GetClientID(0)].character_direction * m_fDeltaTime
					* clients[CPacketMgr::GetInstance()->GetRoom(i)->GetClientID(0)].Speed;
				
				clients[CPacketMgr::GetInstance()->GetRoom(i)->GetClientID(1)].player.m_vPosition
					+= clients[CPacketMgr::GetInstance()->GetRoom(i)->GetClientID(1)].character_direction * m_fDeltaTime
					* clients[CPacketMgr::GetInstance()->GetRoom(i)->GetClientID(1)].Speed;

				CPacketMgr::GetInstance()->GetRoom(i)->NPCUpdate(m_fDeltaTime,hIOCP,clients);
			}

			//Ice Update
			if (true == CPacketMgr::GetInstance()->GetIceRoom(i)->GetRoomState())
			{
				//거리=시간*속력

				//씬전환시 데이터 레이스 발생할수도

				clients[CPacketMgr::GetInstance()->GetIceRoom(i)->GetClientID(0)].player.m_vPosition
					+= clients[CPacketMgr::GetInstance()->GetIceRoom(i)->GetClientID(0)].character_direction * m_fDeltaTime
					* clients[CPacketMgr::GetInstance()->GetIceRoom(i)->GetClientID(0)].Speed;

				clients[CPacketMgr::GetInstance()->GetIceRoom(i)->GetClientID(1)].player.m_vPosition
					+= clients[CPacketMgr::GetInstance()->GetIceRoom(i)->GetClientID(1)].character_direction * m_fDeltaTime
					* clients[CPacketMgr::GetInstance()->GetIceRoom(i)->GetClientID(1)].Speed;

				CPacketMgr::GetInstance()->GetIceRoom(i)->NPCUpdate(m_fDeltaTime, hIOCP, clients);
			}

			//Remain Update
			if (true == CPacketMgr::GetInstance()->GetRemainRoom(i)->GetRoomState())
			{
				//거리=시간*속력

				//씬전환시 데이터 레이스 발생할수도

				clients[CPacketMgr::GetInstance()->GetRemainRoom(i)->GetClientID(0)].player.m_vPosition
					+= clients[CPacketMgr::GetInstance()->GetRemainRoom(i)->GetClientID(0)].character_direction * m_fDeltaTime
					* clients[CPacketMgr::GetInstance()->GetRemainRoom(i)->GetClientID(0)].Speed;

				clients[CPacketMgr::GetInstance()->GetRemainRoom(i)->GetClientID(1)].player.m_vPosition
					+= clients[CPacketMgr::GetInstance()->GetRemainRoom(i)->GetClientID(1)].character_direction * m_fDeltaTime
					* clients[CPacketMgr::GetInstance()->GetRemainRoom(i)->GetClientID(1)].Speed;

				CPacketMgr::GetInstance()->GetRemainRoom(i)->NPCUpdate(m_fDeltaTime, hIOCP, clients);
			}
		}
	}	
}
void CThreadManager::DatabaseThread() {
	while (true) {
		Sleep(1000);
	}
}

HANDLE CThreadManager::GetIOCPHandle() {
	return hIOCP;
}
