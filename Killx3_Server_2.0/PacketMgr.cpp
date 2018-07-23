#include "PacketMgr.h"
#include "CaveNormalNPC.h"
#include "CaveMiddleNPC.h"
#include "CaveBossNPC.h"
#include "IceNormalNPC.h"
#include "IceMiddleNPC.h"
#include "IceBossNPC.h"
#include "RemainNormalNPC.h"
#include "RemainMiddleNPC.h"
#include "RemainBossNPC.h"

CPacketMgr* CPacketMgr::instance = nullptr;

CPacketMgr* CPacketMgr::GetInstance() 
{
	if (instance == nullptr)
		instance = new CPacketMgr();

	return instance;
}
CPacketMgr::CPacketMgr()
{
	for (int i = 0; i < 2; ++i)
		memset(&KeyState[i], 0, sizeof(bool));

	//Room 데이터  미리 할당
	mCaveRoom.resize(MAX_ROOM);
	mIceRoom.resize(MAX_ROOM);
	mRemainRoom.resize(MAX_ROOM);
}
CPacketMgr::~CPacketMgr(){}

void CPacketMgr::SendPacket(int id, unsigned char* packet, CLIENT * clients)
{
	// 아래처럼 지역변수로 놔두면 안된다. 데이터 다 보내기도 전에 소멸되므로
	// OverlapEx send_over;
	OverlapEx* send_over = new OverlapEx;
	memset(send_over, 0, sizeof(OverlapEx));			// 꼭 해줘야한다!!!!!!!!!!!
	send_over->operation = OP_SEND;
	send_over->recv_buff.buf = reinterpret_cast<CHAR*>(send_over->socket_buff);
	send_over->recv_buff.len = packet[0];
	memcpy(send_over->socket_buff, packet, packet[0]);

	int result = WSASend(clients[id].sock, &send_over->recv_buff, 1, NULL, 0, &send_over->original_overlap, NULL);
	if ((0 != result) && (WSA_IO_PENDING != result))
	{
		int error_num = WSAGetLastError();

		if (WSA_IO_PENDING != error_num)
			error_display("SendPacket : WSASend", error_num);
		while (true);
	}
}
CRoom* CPacketMgr::GetRoom(int roomNum)
{
	return &mCaveRoom[roomNum];
}
CRoom* CPacketMgr::GetIceRoom(int roomNum)
{
	return &mIceRoom[roomNum];
}
CRoom* CPacketMgr::GetRemainRoom(int roomNum)
{
	return &mRemainRoom[roomNum];
}

void CPacketMgr::SetKeyState(int i, bool enable)
{
	KeyState[i] = enable;
}

bool CPacketMgr::GetKeyState(int i)
{
	return KeyState[i];
}

void CPacketMgr::ProcessPacket(int id, unsigned char * packet , CLIENT * clients)
{
	unsigned char packet_type = packet[1];

	switch (packet_type)
	{
		//클라이언트로부터 온 패킷을 기반으로 방향을 결정해주는것이 좋을듯. ->
		//클라이언트 구조체에 Vec3 방향 변수 추가
		//키패킷이오면 방향만 바꿔주면 되지 않나

		case CS_KEY:
		{
			cs_packet_key *key_packet = reinterpret_cast<cs_packet_key*>(packet);
			clients[id].key_state = key_packet->key;
			if(key_packet->key != 10 || key_packet->key != 11)
				clients[id].character_direction = key_packet->direction;
			
			Vec3 vPivotDir = Vec3(0.0f, 0.0f, 1.0f);
			float fAngle = clients[id].character_direction.GetAngle(vPivotDir);
			Vec3 vAxis = clients[id].character_direction.Cross(vPivotDir);
			vAxis = vAxis.Normalize();
			
			if (key_packet->key != NONE_KEY)
				SetKeyState(id, true);

				if (key_packet->key == NONE_KEY)
				SetKeyState(id, false);

			// 회전방향
			fAngle *= (vAxis.y < 0.0f) ? 1.0f : -1.0f;

			switch (key_packet->key)
			{
			case NONE_KEY:
			{
				clients[id].Speed = 0.0f;
				break;
			}
			case UP_KEY: 
			{
				if (clients[id].gaming_character == CHARACTER_WARRIOR)
					clients[id].Speed = 10.0f;
				else if (clients[id].gaming_character == CHARACTER_KNIGHT)
					clients[id].Speed = 7.5f;
				else if (clients[id].gaming_character == CHARACTER_PALADIN)
					clients[id].Speed = 6.0f;

				break;
			}
			case DOWN_KEY: 
			{
				if (clients[id].gaming_character == CHARACTER_WARRIOR)
					clients[id].Speed = -10.0f;
				else if (clients[id].gaming_character == CHARACTER_KNIGHT)
					clients[id].Speed = -7.5f;
				else if (clients[id].gaming_character == CHARACTER_PALADIN)
					clients[id].Speed = -6.0f;

				break;
			}
			case SHIFT_KEY:
			{
				if (clients[id].gaming_character == CHARACTER_WARRIOR)
					clients[id].Speed = 10.0f;
				else if (clients[id].gaming_character == CHARACTER_KNIGHT)
					clients[id].Speed = 7.5f;
				else if (clients[id].gaming_character == CHARACTER_PALADIN)
					clients[id].Speed = 6.0f;

				clients[id].Speed *= 1.5f;

				break;
			}
			
			case ATTACK_KEY:
			{
				clients[id].Speed = 0.0f;

				break;
			}
			case BACKSHIFT_KEY:
			{
				if (clients[id].gaming_character == CHARACTER_WARRIOR)
					clients[id].Speed = 10.0f;
				else if (clients[id].gaming_character == CHARACTER_KNIGHT)
					clients[id].Speed = 7.5f;
				else if (clients[id].gaming_character == CHARACTER_PALADIN)
					clients[id].Speed = 6.0f;

				clients[id].Speed *= 1.5f;
				clients[id].character_direction *= -1.0f;

				break;
			}

			case SPACE_KEY:
			{
				clients[0].player.m_vPosition = Vec3(124.0f, 1.0f, 20.0f);
				clients[0].character_direction = Vec3(0.0f, 0.0f, 1.0f);

				clients[1].player.m_vPosition = Vec3(128.0f, 1.0f, 20.0f);
				clients[1].character_direction = Vec3(0.0f, 0.0f, 1.0f);

				break;
			}

			case KNIGHT_DEFENCE_KEY:
			{
				clients[id].Speed = 0.0f;
				break;
			}

			case PALADIN_DEFENCE_KEY:
			{
				clients[id].Speed = 0.0f;
				break;

			}

			case KNIGHT_DEFENCE_KEY_UP:
			{
				if (clients[id].gaming_character == CHARACTER_WARRIOR)
					clients[id].Speed = 10.0f;
				else if (clients[id].gaming_character == CHARACTER_KNIGHT)
					clients[id].Speed = 7.5f;
				else if (clients[id].gaming_character == CHARACTER_PALADIN)
					clients[id].Speed = 6.0f;
				
				break;
			}

			case PALADIN_DEFENCE_KEY_UP:
			{
				if (clients[id].gaming_character == CHARACTER_WARRIOR)
					clients[id].Speed = 10.0f;
				else if (clients[id].gaming_character == CHARACTER_KNIGHT)
					clients[id].Speed = 7.5f;
				else if (clients[id].gaming_character == CHARACTER_PALADIN)
					clients[id].Speed = 6.0f;

				break;
			}
			case ROLLEND:
			{
				clients[id].Speed = 0.0f;

				break;
			}
			default:
				break;
			}

			//Broading Cast
			cout << key_packet->key << endl;
			sc_packet_key anglePacket;
			anglePacket.size = sizeof(sc_packet_key);
			anglePacket.type = SC_KEY;
			anglePacket.id = id;
			anglePacket.key = key_packet->key;
			anglePacket.angle = fAngle;
			anglePacket.direction = key_packet->direction;
		
			for (int i = 0; i < 2; ++i)
			{
				//cout << i << "번 각도 : " << fAngle << endl;
				SendPacket(clients[i].roomList[i], 
					reinterpret_cast<unsigned char*>(&anglePacket),
					clients);
			}
			break;
		}

		case CS_ANGLE:
		{
			cs_packet_DirToAngle *key_packet = reinterpret_cast<cs_packet_DirToAngle*>(packet);
			if(GetKeyState(id))
				clients[id].character_direction = key_packet->Direction;
			else
			{
				clients[id].Speed = 0.0f;
			}

			clients[id].player.m_vPosition.y = key_packet->fY;

			cs_packet_DirToAngle AngleToDirPacket;
			AngleToDirPacket.size = sizeof(cs_packet_DirToAngle);
			AngleToDirPacket.type = key_packet->type;
			AngleToDirPacket.Direction = key_packet->Direction;

			// 보낼때는 다른거 맨들어서 보냅시다 Angle 있는걸로
			Vec3 vPivotDir = Vec3(0.0f, 0.0f, 1.0f);
			float fAngle = clients[id].character_direction.GetAngle(vPivotDir);
			Vec3 vAxis = clients[id].character_direction.Cross(vPivotDir);
			vAxis = vAxis.Normalize();

			// 회전방향
			fAngle *= (vAxis.y < 0.0f) ? 1.0f : -1.0f;
			
			sc_packet_DirToAngle AnglePacket;
			AnglePacket.size = sizeof(AnglePacket);
			AnglePacket.id = id;
			AnglePacket.type = SC_ANGLE;
			AnglePacket.Angle = fAngle;
			AnglePacket.Dir = key_packet->Direction;

			for (int i = 0; i < 2; ++i)
			{
				SendPacket(clients[i].roomList[i],
					reinterpret_cast<unsigned char*>(&AnglePacket),
					clients);
			}

			break;
		}

		case CS_COLLISION:
		{
			cs_packet_collision *key_packet = reinterpret_cast<cs_packet_collision*>(packet);
			clients[id].player.m_vPosition = key_packet->CollPos;
			clients[id].Speed = 0.0f;

			sc_packet_collision CollisionPacket;
			CollisionPacket.size = sizeof(sc_packet_collision);
			CollisionPacket.type = SC_COLLISION;
			CollisionPacket.m_Id = id;
			CollisionPacket.PrevPos = clients[id].player.m_vPosition;

			for (int i = 0; i < 2; ++i)
			{
				SendPacket(clients[i].roomList[i],
					reinterpret_cast<unsigned char*>(&CollisionPacket),
					clients);
			}

			break;
		}

		case CS_NPC_DAMAGE:
		{
			cs_packet_NpcDamage *key_packet = reinterpret_cast<cs_packet_NpcDamage*>(packet);
			WORD NpcId = key_packet->m_Id;
			int iDamage = key_packet->m_iDamage;

			if(true == GetRoom(0)->GetRoomState())
				CPacketMgr::GetInstance()->GetRoom(0)->NPCHp(NpcId, iDamage);
			else if (true == GetIceRoom(0)->GetRoomState())
				CPacketMgr::GetInstance()->GetIceRoom(0)->NPCHp(NpcId, iDamage);
			else if (true == GetRemainRoom(0)->GetRoomState())
				CPacketMgr::GetInstance()->GetRemainRoom(0)->NPCHp(NpcId, iDamage);

			sc_packet_NPCDamage NPCDamage;
			NPCDamage.size = sizeof(sc_packet_NPCDamage);
			NPCDamage.type = SC_NPC_DAMAGE;
			NPCDamage.m_Id = NpcId;

			for (int i = 0; i < 2; ++i)
			{
				SendPacket(clients[i].roomList[i],
					reinterpret_cast<unsigned char*>(&NPCDamage),
					clients);
			}

			break;
		}

		case CS_TRICK:
		{
			cs_camera_effect *key_packet = reinterpret_cast<cs_camera_effect*>(packet);
			WORD NpcId = key_packet->m_Id;

			sc_camera_effect EffectCamera;
			EffectCamera.size = sizeof(sc_camera_effect);
			EffectCamera.type = SC_TRICK;
			EffectCamera.m_Id = key_packet->m_Id;

			for (int i = 0; i < 2; ++i)
			{
				SendPacket(clients[i].roomList[i],
					reinterpret_cast<unsigned char*>(&EffectCamera),
					clients);
			}

			break;
		}

		case MAP_CAVE:
		{
			if (clients[id].is_gaming == false)
			{
				clients[id].gaming_character = packet[2];

				if (clients[id].gaming_character == CHARACTER_WARRIOR)
					clients[id].Speed = 10.0f;
				else if (clients[id].gaming_character == CHARACTER_KNIGHT)
					clients[id].Speed = 7.5f;
				else if (clients[id].gaming_character == CHARACTER_PALADIN)
					clients[id].Speed = 6.0f;

				MakeCaveRoom(id, packet_type, clients);
			}
			break;
		}
		case MAP_CAVE_STAGE1_CLEAR:
		{
			cout << "Cave Stage1->Cave Stage2 \n";
			CaveNextStage(id,packet_type,clients);
			break;
		}
		case MAP_CAVE_STAGE2_CLEAR:
		{
			cout << "Cave Stage2->Cave Stage3 \n";
			CaveNextStage(id, packet_type, clients);
			break;
		}
		case MAP_CAVE_STAGE3_CLEAR:
		{
			//로비로 씬전환 패킷 보낸 이후 
			//방데이터 초기화
			break;
		}
		//---------ICE STAGE--------
		case MAP_ICE: 
		{
			if (clients[id].is_gaming == false)
			{
				clients[id].gaming_character = packet[2];

				if (clients[id].gaming_character == CHARACTER_WARRIOR)
					clients[id].Speed = 10.0f;
				else if (clients[id].gaming_character == CHARACTER_KNIGHT)
					clients[id].Speed = 7.5f;
				else if (clients[id].gaming_character == CHARACTER_PALADIN)
					clients[id].Speed = 6.0f;

				MakeIceRoom(id, packet_type, clients);
			}

			break;
		}
		case MAP_ICE_STAGE1_CLEAR:
		{
			cout << "ICE_Stage1->Ice_Stage2 \n";
			IceNextStage(id, packet_type, clients);
			break;
		}
		case MAP_ICE_STAGE2_CLEAR:
		{
			cout << "ICE_Stage2->Ice_Stage3 \n";
			IceNextStage(id, packet_type, clients);
			break;
		}
		case MAP_ICE_STAGE3_CLEAR:
		{
			break;
		}
		//------REMAIN STAGE-----
		case MAP_REMAIN:
		{
			if (clients[id].is_gaming == false)
			{
				clients[id].gaming_character = packet[2];

				if (clients[id].gaming_character == CHARACTER_WARRIOR)
					clients[id].Speed = 10.0f;
				else if (clients[id].gaming_character == CHARACTER_KNIGHT)
					clients[id].Speed = 7.5f;
				else if (clients[id].gaming_character == CHARACTER_PALADIN)
					clients[id].Speed = 6.0f;

				MakeRemainRoom(id, packet_type, clients);
			}
			break;
		}
		case MAP_REMAIN_STAGE1_CLEAR:
		{
			cout << "Remain_Stage1->Remain_Stage2 \n";
			RemainNextStage(id, packet_type, clients);
			break;
		}
		case MAP_REMAIN_STAGE2_CLEAR:
		{
			cout << "Remain_Stage2->Remain_Stage3 \n";
			RemainNextStage(id, packet_type, clients);
			break;
		}
		case MAP_REMAIN_STAGE3_CLEAR:
		{
			break;
		}
		default:
		{
			cout << "Unknown Packet Type Detexcted!!" << endl;
			//exit(-1);
		}
	}
}
void CPacketMgr::MakeCaveRoom(int now_ID, unsigned char packet, CLIENT * clients)
{
	// TODO : 2,3,4,인용에 각각에 맞게 수정 필요
	cout << "MAKE CAVE ROOM \n";
	for (int i = 0; i < 1; ++i)
	{
		cout << "MAKE CAVE ROOM1 \n";
		printf("State1 : %d\n", mCaveRoom[i].GetRoomState());
		if (false == mCaveRoom[i].GetRoomState())		//true 게임중 , false 게임 안하는방
		{
			cout << "MAKE CAVE ROOM12 \n";
			clients[now_ID].roomNumber = i;
			
			clients[now_ID].gamingMapType = MAP_CAVE;

			if (true == mCaveRoom[i].FindClient(now_ID))
				return;

			mCaveRoom[i].PushClient(&clients[now_ID]);

			switch (mCaveRoom[i].CheckFullRoom())
			{
			case true: 
				{
				cout << "MAKE CAVE ROOM123 \n";
					mCaveRoom[i].Init();
				 
					int roomNumber = clients[now_ID].roomNumber;
					//Init Packet

					sc_packet_scene scene_packet;
					scene_packet.size = sizeof(sc_packet_scene);
					scene_packet.type = MAP_CAVE_STAGE1;
					scene_packet.initPosition[0].id = (WORD)mCaveRoom[i].GetClientID(0);
					scene_packet.initPosition[0].Position = Vec3(PLAYER1_POS_X, PLAYER1_POS_Y, PLAYER1_POS_Z);
					scene_packet.initPosition[0].CharacterType = clients[mCaveRoom[i].GetClientID(0)].gaming_character;

					scene_packet.initPosition[1].id = (WORD)mCaveRoom[i].GetClientID(1);
					scene_packet.initPosition[1].Position = Vec3(PLAYER2_POS_X, PLAYER2_POS_Y, PLAYER2_POS_Z);
					scene_packet.initPosition[1].CharacterType = clients[mCaveRoom[i].GetClientID(1)].gaming_character;

					scene_packet.initNPCPosition[0].npcID = 0;
					scene_packet.initNPCPosition[0].NpcPosition = Vec3{ 120.0f,1.8f,60.0f };

					scene_packet.initNPCPosition[1].npcID = 1;
					scene_packet.initNPCPosition[1].NpcPosition = Vec3{ 123.0f,2.5f,119.0f };

					scene_packet.initNPCPosition[2].npcID = 2;
					scene_packet.initNPCPosition[2].NpcPosition = Vec3{ 171.0f,2.2f,179.0f };

					scene_packet.initNPCPosition[3].npcID = 3;
					scene_packet.initNPCPosition[3].NpcPosition = Vec3{ 182.0f,0.52f,210.0f };

					//Init Player&NPC Info
					clients[mCaveRoom[i].GetClientID(0)].player.m_vPosition.x = PLAYER1_POS_X;
					clients[mCaveRoom[i].GetClientID(0)].player.m_vPosition.y = PLAYER1_POS_Y;
					clients[mCaveRoom[i].GetClientID(0)].player.m_vPosition.z = PLAYER1_POS_Z;

					clients[mCaveRoom[i].GetClientID(1)].player.m_vPosition.x = PLAYER2_POS_X;
					clients[mCaveRoom[i].GetClientID(1)].player.m_vPosition.y = PLAYER2_POS_Y;
					clients[mCaveRoom[i].GetClientID(1)].player.m_vPosition.z = PLAYER2_POS_Z;

					clients[mCaveRoom[i].GetClientID(0)].player.m_fAngle = PLAYER1_POS_ANGLE;
					clients[mCaveRoom[i].GetClientID(1)].player.m_fAngle = PLAYER2_POS_ANGLE;

					//NPC들 아이디 셋팅
					mCaveRoom[i].GetCaveStage1_NPC_Vector()[0]->SetID(0);
					mCaveRoom[i].GetCaveStage1_NPC_Vector()[1]->SetID(1);
					mCaveRoom[i].GetCaveStage1_NPC_Vector()[2]->SetID(2);
					mCaveRoom[i].GetCaveStage1_NPC_Vector()[3]->SetID(3);

					//방 번호 셋팅
					mCaveRoom[i].GetCaveStage1_NPC_Vector()[0]->SetRoomNumber(roomNumber);
					mCaveRoom[i].GetCaveStage1_NPC_Vector()[1]->SetRoomNumber(roomNumber);
					mCaveRoom[i].GetCaveStage1_NPC_Vector()[2]->SetRoomNumber(roomNumber);
					mCaveRoom[i].GetCaveStage1_NPC_Vector()[3]->SetRoomNumber(roomNumber);

					//좌표 셋팅
					mCaveRoom[i].GetCaveStage1_NPC_Vector()[0]->SetDestPoint(Vec3(128.0f, 1.0f, 65.0f));
					mCaveRoom[i].GetCaveStage1_NPC_Vector()[0]->SetDestPoint(Vec3(112.0f, 1.0f, 55.0f));
					mCaveRoom[i].GetCaveStage1_NPC_Vector()[0]->SetDestPoint(Vec3(127.0f, 1.0f, 57.5f));
					mCaveRoom[i].GetCaveStage1_NPC_Vector()[0]->SetDestPoint(Vec3(114.0f, 1.0f, 62.5f));

					mCaveRoom[i].GetCaveStage1_NPC_Vector()[1]->SetDestPoint(Vec3(129.0f, 2.5f, 115.0f));
					mCaveRoom[i].GetCaveStage1_NPC_Vector()[1]->SetDestPoint(Vec3(129.0f, 2.5f, 128.0f));
					mCaveRoom[i].GetCaveStage1_NPC_Vector()[1]->SetDestPoint(Vec3(120.0f, 2.5f, 116.0f));
					mCaveRoom[i].GetCaveStage1_NPC_Vector()[1]->SetDestPoint(Vec3(116.0f, 2.5f, 121.0f));

					//3번째몬스터는 말뚝임
					
					mCaveRoom[i].GetCaveStage1_NPC_Vector()[3]->SetDestPoint(Vec3(176.0f, 0.52f, 205.0f));
					mCaveRoom[i].GetCaveStage1_NPC_Vector()[3]->SetDestPoint(Vec3(184.0f, 0.52f, 219.0f));
					mCaveRoom[i].GetCaveStage1_NPC_Vector()[3]->SetDestPoint(Vec3(188.0f, 0.52f, 205.0f));
					mCaveRoom[i].GetCaveStage1_NPC_Vector()[3]->SetDestPoint(Vec3(178.0f, 0.52f, 215.0f));

					for (int j = 0; j < 4; ++j)
					{
						if (j != 2){
							Vec3 tempVarDir =
								mCaveRoom[i].GetCaveStage1_NPC_Vector()[j]->GetDestPoint().front() -
								mCaveRoom[i].GetCaveStage1_NPC_Vector()[j]->GetInitPos();

							mCaveRoom[i].GetCaveStage1_NPC_Vector()[j]->SetDir(tempVarDir.Normalize());

							scene_packet.initNPCPosition[j].Dir = tempVarDir.Normalize();
						}
						else {
							mCaveRoom[i].GetCaveStage1_NPC_Vector()[j]->SetDir(Vec3(0, 0, 1));
							scene_packet.initNPCPosition[j].Dir = Vec3{ 0, 0, 1 };
						}
					}

					for (int j = 0; j < 2; ++j){
						SendPacket(mCaveRoom[i].GetClientID(j),
							reinterpret_cast<unsigned char*>(&scene_packet),
							clients);
					}

					clients[mCaveRoom[i].GetClientID(0)].is_gaming = true;
					clients[mCaveRoom[i].GetClientID(1)].is_gaming = true;

					mCaveRoom[i].SetRoomState(true);
					mCaveRoom[i].SetClientsNumber();

					mCaveRoom[i].SetStageState(1);			//1 넣으면 1탄 중, 2넣으면 2탄중..

					//-----Timer Event Setting-----
					Timer_Event player1Event;
					player1Event.event = E_MOVE;
					player1Event.exec_time = high_resolution_clock::now() + 3s;
					player1Event.object_id = mCaveRoom[i].GetClientID(0);
					player1Event.roomNum = roomNumber;

					Timer_Event player2Event;
					player2Event.event = E_MOVE;
					player2Event.exec_time = high_resolution_clock::now() + 3s;
					player2Event.object_id = mCaveRoom[i].GetClientID(1);
					player2Event.roomNum = roomNumber;

					Timer_Event Stage1_NPC_Event;
					Stage1_NPC_Event.event = E_NMOVE;
					Stage1_NPC_Event.exec_time = high_resolution_clock::now() + 1s;
					Stage1_NPC_Event.object_id = roomNumber;
					Stage1_NPC_Event.roomNum = roomNumber;

					CThreadManager::GetInstance()->tq_lock.lock();
					CThreadManager::GetInstance()->timer_queue.push(player1Event);
					CThreadManager::GetInstance()->timer_queue.push(player2Event);

					CThreadManager::GetInstance()->timer_queue.push(Stage1_NPC_Event);

					CThreadManager::GetInstance()->tq_lock.unlock();
					break;
				}
			}
		}
	}
}
void CPacketMgr::CaveNextStage(int now_ID, unsigned char  packet, CLIENT * clients)
{
	//TODO : KEySTATE변환
	
	if (true == mCaveRoom[clients[now_ID].roomNumber].StageCheck())
	{
		//스테이지 변환 전송
		mCaveRoom[clients[now_ID].roomNumber].SetStageCheckVar();
		if (packet == MAP_CAVE_STAGE1_CLEAR)
		{
			int roomNumber = clients[now_ID].roomNumber;
			sc_packet_scene scene_packet;
			scene_packet.size = sizeof(sc_packet_scene);
			scene_packet.type = MAP_CAVE_STAGE2;

			//0번
			scene_packet.initPosition[0].id =
				(WORD)mCaveRoom[clients[now_ID].roomNumber].GetClientID(0);
			scene_packet.initPosition[0].Position =
				Vec3(PLAYER1_POS_X, PLAYER1_POS_Y, PLAYER1_POS_Z);
			scene_packet.initPosition[0].CharacterType =
				clients[mCaveRoom[roomNumber].GetClientID(0)].gaming_character;

			//1번
			scene_packet.initPosition[1].id =
				(WORD)mCaveRoom[clients[now_ID].roomNumber].GetClientID(1);
			scene_packet.initPosition[1].Position =
				Vec3(PLAYER2_POS_X, PLAYER2_POS_Y, PLAYER2_POS_Z);
			scene_packet.initPosition[1].CharacterType =
				clients[mCaveRoom[roomNumber].GetClientID(1)].gaming_character;

			scene_packet.initNPCPosition[0].npcID = 0;
			scene_packet.initNPCPosition[0].NpcPosition = Vec3{ 106.0f, 1.06f, 74.0f };

			scene_packet.initNPCPosition[1].npcID = 1;
			scene_packet.initNPCPosition[1].NpcPosition = Vec3{ 142.6f, 1.98f, 90.0f };

			scene_packet.initNPCPosition[2].npcID = 2;
			scene_packet.initNPCPosition[2].NpcPosition = Vec3{ 102.0f, 3.02f, 150.0f };

			clients[mCaveRoom[roomNumber].GetClientID(0)].player.m_vPosition.x = PLAYER1_POS_X;
			clients[mCaveRoom[roomNumber].GetClientID(0)].player.m_vPosition.y = PLAYER1_POS_Y;
			clients[mCaveRoom[roomNumber].GetClientID(0)].player.m_vPosition.z = PLAYER1_POS_Z;
			clients[mCaveRoom[roomNumber].GetClientID(0)].player.m_fAngle = PLAYER1_POS_ANGLE;
			clients[mCaveRoom[roomNumber].GetClientID(0)].key_state = NONE_KEY;

			clients[mCaveRoom[roomNumber].GetClientID(1)].player.m_vPosition.x = PLAYER2_POS_X;
			clients[mCaveRoom[roomNumber].GetClientID(1)].player.m_vPosition.y = PLAYER2_POS_Y;
			clients[mCaveRoom[roomNumber].GetClientID(1)].player.m_vPosition.z = PLAYER2_POS_Z;
			clients[mCaveRoom[roomNumber].GetClientID(1)].player.m_fAngle = PLAYER2_POS_ANGLE;
			clients[mCaveRoom[roomNumber].GetClientID(1)].key_state = NONE_KEY;


			mCaveRoom[roomNumber].GetCaveStage2_NPC_Vector()[0]->SetID(0);
			mCaveRoom[roomNumber].GetCaveStage2_NPC_Vector()[1]->SetID(1);
			mCaveRoom[roomNumber].GetCaveStage2_NPC_Vector()[2]->SetID(2);

			//방 번호 셋팅
			mCaveRoom[roomNumber].GetCaveStage2_NPC_Vector()[0]->SetRoomNumber(roomNumber);
			mCaveRoom[roomNumber].GetCaveStage2_NPC_Vector()[1]->SetRoomNumber(roomNumber);
			mCaveRoom[roomNumber].GetCaveStage2_NPC_Vector()[2]->SetRoomNumber(roomNumber);
			Sleep(1);
			//좌표 셋팅
			mCaveRoom[roomNumber].GetCaveStage2_NPC_Vector()[0]->SetDestPoint(Vec3(112.0f, 1.06f, 80.0f));
			mCaveRoom[roomNumber].GetCaveStage2_NPC_Vector()[0]->SetDestPoint(Vec3(110.0f, 1.06f, 66.0f));
			mCaveRoom[roomNumber].GetCaveStage2_NPC_Vector()[0]->SetDestPoint(Vec3(114.0f, 1.06f, 67.5f));
			mCaveRoom[roomNumber].GetCaveStage2_NPC_Vector()[0]->SetDestPoint(Vec3(102.0f, 1.06f, 75.5f));

			mCaveRoom[roomNumber].GetCaveStage2_NPC_Vector()[1]->SetDestPoint(Vec3(132.0f, 1.98f, 85.0f));
			mCaveRoom[roomNumber].GetCaveStage2_NPC_Vector()[1]->SetDestPoint(Vec3(145.0f, 1.98f, 97.0f));
			mCaveRoom[roomNumber].GetCaveStage2_NPC_Vector()[1]->SetDestPoint(Vec3(145.0f, 1.98f, 100.0f));
			mCaveRoom[roomNumber].GetCaveStage2_NPC_Vector()[1]->SetDestPoint(Vec3(135.0f, 1.98f, 82.0f));

			mCaveRoom[roomNumber].GetCaveStage2_NPC_Vector()[2]->SetDestPoint(Vec3(106.0f, 3.02f, 145.0f));
			mCaveRoom[roomNumber].GetCaveStage2_NPC_Vector()[2]->SetDestPoint(Vec3(106.0f, 3.02f, 155.0f));
			mCaveRoom[roomNumber].GetCaveStage2_NPC_Vector()[2]->SetDestPoint(Vec3(95.0f, 3.02f, 147.0f));
			mCaveRoom[roomNumber].GetCaveStage2_NPC_Vector()[2]->SetDestPoint(Vec3(95.0f, 3.02f, 153.0f));

			mCaveRoom[roomNumber].SetStageState(2);			//1 넣으면 1탄 중, 2넣으면 2탄중..

			for (int j = 0; j < 3; ++j)
			{
				Vec3 tempVarDir =
					mCaveRoom[roomNumber].GetCaveStage2_NPC_Vector()[j]->GetDestPoint().front() -
					mCaveRoom[roomNumber].GetCaveStage2_NPC_Vector()[j]->GetInitPos();

				mCaveRoom[roomNumber].GetCaveStage2_NPC_Vector()[j]->SetDir(tempVarDir.Normalize());

				scene_packet.initNPCPosition[j].Dir = tempVarDir.Normalize();
			}

			clients[mCaveRoom[roomNumber].GetClientID(0)].is_gaming = true;
			clients[mCaveRoom[roomNumber].GetClientID(1)].is_gaming = true;

			mCaveRoom[roomNumber].SetRoomState(true);
			mCaveRoom[roomNumber].SetClientsNumber();

			for (int i = 0; i < 2; ++i)
			{
				SendPacket(mCaveRoom[clients[now_ID].roomNumber].GetClientID(i),
					reinterpret_cast<unsigned char*>(&scene_packet),
					clients);
			}

			//-----Timer Event Setting-----
			Timer_Event Stage2_NPC_Event;
			Stage2_NPC_Event.event = E_NMOVE;
			Stage2_NPC_Event.exec_time = high_resolution_clock::now() + 1s;
			Stage2_NPC_Event.object_id = roomNumber;
			Stage2_NPC_Event.roomNum = roomNumber;

			CThreadManager::GetInstance()->tq_lock.lock();
			CThreadManager::GetInstance()->timer_queue.push(Stage2_NPC_Event);
			CThreadManager::GetInstance()->tq_lock.unlock();
		}
		////////////////////////////////////////////////////////////////
		else if (packet == MAP_CAVE_STAGE2_CLEAR)
		{
			int roomNumber = clients[now_ID].roomNumber;

			sc_packet_scene scene_packet;
			scene_packet.size = sizeof(sc_packet_scene);
			scene_packet.type = MAP_CAVE_STAGE3;

			//0번
			scene_packet.initPosition[0].id =
				(WORD)mCaveRoom[clients[now_ID].roomNumber].GetClientID(0);
			scene_packet.initPosition[0].Position =
				Vec3(PLAYER1_POS_X, PLAYER1_POS_Y, PLAYER1_POS_Z);
			scene_packet.initPosition[0].CharacterType =
				clients[mCaveRoom[roomNumber].GetClientID(0)].gaming_character;

			//1번
			scene_packet.initPosition[1].id =
				(WORD)mCaveRoom[clients[now_ID].roomNumber].GetClientID(1);
			scene_packet.initPosition[1].Position =
				Vec3(PLAYER2_POS_X, PLAYER2_POS_Y, PLAYER2_POS_Z);
			scene_packet.initPosition[1].CharacterType =
				clients[mCaveRoom[roomNumber].GetClientID(1)].gaming_character;

			scene_packet.initNPCPosition[0].npcID = 0;
			scene_packet.initNPCPosition[0].NpcPosition = Vec3{ 128.0f, 0.0f, 220.0f };

			clients[mCaveRoom[roomNumber].GetClientID(0)].player.m_vPosition.x = PLAYER1_POS_X;
			clients[mCaveRoom[roomNumber].GetClientID(0)].player.m_vPosition.y = PLAYER1_POS_Y;
			clients[mCaveRoom[roomNumber].GetClientID(0)].player.m_vPosition.z = PLAYER1_POS_Z;
			clients[mCaveRoom[roomNumber].GetClientID(0)].player.m_fAngle = PLAYER1_POS_ANGLE;
			clients[mCaveRoom[roomNumber].GetClientID(0)].key_state = NONE_KEY;

			clients[mCaveRoom[roomNumber].GetClientID(1)].player.m_vPosition.x = PLAYER2_POS_X;
			clients[mCaveRoom[roomNumber].GetClientID(1)].player.m_vPosition.y = PLAYER2_POS_Y;
			clients[mCaveRoom[roomNumber].GetClientID(1)].player.m_vPosition.z = PLAYER2_POS_Z;
			clients[mCaveRoom[roomNumber].GetClientID(1)].player.m_fAngle = PLAYER2_POS_ANGLE;
			clients[mCaveRoom[roomNumber].GetClientID(1)].key_state = NONE_KEY;

			mCaveRoom[roomNumber].GetCaveStage3_NPC_Vector()[0]->SetID(0);

			//방 번호 셋팅
			mCaveRoom[roomNumber].GetCaveStage3_NPC_Vector()[0]->SetRoomNumber(roomNumber);
			Sleep(1);
			//좌표 셋팅
			mCaveRoom[roomNumber].GetCaveStage3_NPC_Vector()[0]->SetDestPoint(Vec3(132.0f, 0.0f, 210.0f));
			mCaveRoom[roomNumber].GetCaveStage3_NPC_Vector()[0]->SetDestPoint(Vec3(132.0f, 0.0f, 230.0f));
			mCaveRoom[roomNumber].GetCaveStage3_NPC_Vector()[0]->SetDestPoint(Vec3(124.0f, 0.0f, 210.0f));
			mCaveRoom[roomNumber].GetCaveStage3_NPC_Vector()[0]->SetDestPoint(Vec3(124.0f, 0.0f, 230.0f));

			mCaveRoom[roomNumber].SetStageState(3);			//1 넣으면 1탄 중, 2넣으면 2탄중..

			for (int j = 0; j < 1; ++j)
			{
				Vec3 tempVarDir =
					mCaveRoom[roomNumber].GetCaveStage3_NPC_Vector()[j]->GetDestPoint().front() -
					mCaveRoom[roomNumber].GetCaveStage3_NPC_Vector()[j]->GetInitPos();

				mCaveRoom[roomNumber].GetCaveStage3_NPC_Vector()[j]->SetDir(tempVarDir.Normalize());

				scene_packet.initNPCPosition[j].Dir = tempVarDir.Normalize();
			}

			clients[mCaveRoom[roomNumber].GetClientID(0)].is_gaming = true;
			clients[mCaveRoom[roomNumber].GetClientID(1)].is_gaming = true;

			cout << "짜라잔" << endl;
			mCaveRoom[roomNumber].SetRoomState(true);
			mCaveRoom[roomNumber].SetClientsNumber();

			for (int i = 0; i < 2; ++i)
			{
				SendPacket(mCaveRoom[clients[now_ID].roomNumber].GetClientID(i),
					reinterpret_cast<unsigned char*>(&scene_packet),
					clients);
			}

			//-----Timer Event Setting-----
			Timer_Event Stage3_NPC_Event;
			Stage3_NPC_Event.event = E_NMOVE;
			Stage3_NPC_Event.exec_time = high_resolution_clock::now() + 1s;
			Stage3_NPC_Event.object_id = roomNumber;
			Stage3_NPC_Event.roomNum = roomNumber;

			CThreadManager::GetInstance()->tq_lock.lock();
			CThreadManager::GetInstance()->timer_queue.push(Stage3_NPC_Event);
			CThreadManager::GetInstance()->tq_lock.unlock();
		}

	}
}

void CPacketMgr::MakeIceRoom(int now_ID, unsigned char packet, CLIENT * clients)
{
	// TODO : 2,3,4,인용에 각각에 맞게 수정 필요
	cout << "MAKE Ice ROOM \n";
	for (int i = 0; i < 1; ++i)
	{
		if (false == mIceRoom[i].GetRoomState())		//true 게임중 , false 게임 안하는방
		{
			clients[now_ID].roomNumber = i;

			clients[now_ID].gamingMapType = MAP_ICE;

			if (true == mIceRoom[i].FindClient(now_ID))
				return;

			mIceRoom[i].PushClient(&clients[now_ID]);

			switch (mIceRoom[i].CheckFullRoom())
			{
			case true:
			{
				mIceRoom[i].InitIce();

				int roomNumber = clients[now_ID].roomNumber;
				//Init Packet

				sc_packet_scene scene_packet;
				scene_packet.size = sizeof(sc_packet_scene);
				scene_packet.type = MAP_ICE_STAGE1;
				scene_packet.initPosition[0].id = (WORD)mIceRoom[i].GetClientID(0);
				scene_packet.initPosition[0].Position = Vec3(PLAYER1_POS_X, PLAYER1_POS_Y, PLAYER1_POS_Z);
				scene_packet.initPosition[0].CharacterType = clients[mIceRoom[i].GetClientID(0)].gaming_character;

				scene_packet.initPosition[1].id = (WORD)mIceRoom[i].GetClientID(1);
				scene_packet.initPosition[1].Position = Vec3(PLAYER2_POS_X, PLAYER2_POS_Y, PLAYER2_POS_Z);
				scene_packet.initPosition[1].CharacterType = clients[mIceRoom[i].GetClientID(1)].gaming_character;

				scene_packet.initNPCPosition[0].npcID = 0;
				scene_packet.initNPCPosition[0].NpcPosition = Vec3{ 120.0f,1.8f,60.0f };

				scene_packet.initNPCPosition[1].npcID = 1;
				scene_packet.initNPCPosition[1].NpcPosition = Vec3{ 123.0f,2.5f,119.0f };

				scene_packet.initNPCPosition[2].npcID = 2;
				scene_packet.initNPCPosition[2].NpcPosition = Vec3{ 171.0f,2.2f,179.0f };

				scene_packet.initNPCPosition[3].npcID = 3;
				scene_packet.initNPCPosition[3].NpcPosition = Vec3{ 182.0f,0.52f,210.0f };

				//Init Player&NPC Info
				clients[mIceRoom[i].GetClientID(0)].player.m_vPosition.x = PLAYER1_POS_X;
				clients[mIceRoom[i].GetClientID(0)].player.m_vPosition.y = PLAYER1_POS_Y;
				clients[mIceRoom[i].GetClientID(0)].player.m_vPosition.z = PLAYER1_POS_Z;

				clients[mIceRoom[i].GetClientID(1)].player.m_vPosition.x = PLAYER2_POS_X;
				clients[mIceRoom[i].GetClientID(1)].player.m_vPosition.y = PLAYER2_POS_Y;
				clients[mIceRoom[i].GetClientID(1)].player.m_vPosition.z = PLAYER2_POS_Z;

				clients[mIceRoom[i].GetClientID(0)].player.m_fAngle = PLAYER1_POS_ANGLE;
				clients[mIceRoom[i].GetClientID(1)].player.m_fAngle = PLAYER2_POS_ANGLE;

				//NPC들 아이디 셋팅GetIceStage1_NPC_Vector
				mIceRoom[i].GetIceStage1_NPC_Vector()[0]->SetID(0);
				mIceRoom[i].GetIceStage1_NPC_Vector()[1]->SetID(1);
				mIceRoom[i].GetIceStage1_NPC_Vector()[2]->SetID(2);
				mIceRoom[i].GetIceStage1_NPC_Vector()[3]->SetID(3);

				//방 번호 셋팅
				mIceRoom[i].GetIceStage1_NPC_Vector()[0]->SetRoomNumber(roomNumber);
				mIceRoom[i].GetIceStage1_NPC_Vector()[1]->SetRoomNumber(roomNumber);
				mIceRoom[i].GetIceStage1_NPC_Vector()[2]->SetRoomNumber(roomNumber);
				mIceRoom[i].GetIceStage1_NPC_Vector()[3]->SetRoomNumber(roomNumber);

				//좌표 셋팅
				mIceRoom[i].GetIceStage1_NPC_Vector()[0]->SetDestPoint(Vec3(115.0f, 0.0f, 95.0f));
				mIceRoom[i].GetIceStage1_NPC_Vector()[0]->SetDestPoint(Vec3(110.0f, 0.0f, 105.0f));
				mIceRoom[i].GetIceStage1_NPC_Vector()[0]->SetDestPoint(Vec3(100.0f, 0.0f, 107.5f));

				mIceRoom[i].GetIceStage1_NPC_Vector()[1]->SetDestPoint(Vec3(145.0f, 0.0f, 67.5f));
				mIceRoom[i].GetIceStage1_NPC_Vector()[1]->SetDestPoint(Vec3(150.0f, 0.0f, 70.0f));
				mIceRoom[i].GetIceStage1_NPC_Vector()[1]->SetDestPoint(Vec3(155.0f, 0.0f, 77.5f));

				mIceRoom[i].GetIceStage1_NPC_Vector()[2]->SetDestPoint(Vec3(165.0f, 0.0f, 145.0f));
				mIceRoom[i].GetIceStage1_NPC_Vector()[2]->SetDestPoint(Vec3(155.0f, 0.0f, 140.0f));
				mIceRoom[i].GetIceStage1_NPC_Vector()[2]->SetDestPoint(Vec3(145.0f, 0.0f, 150.0f));

				mIceRoom[i].GetIceStage1_NPC_Vector()[3]->SetDestPoint(Vec3(132.5f, 0.0f, 215.0f));
				mIceRoom[i].GetIceStage1_NPC_Vector()[3]->SetDestPoint(Vec3(115.0f, 0.0f, 210.0f));
				mIceRoom[i].GetIceStage1_NPC_Vector()[3]->SetDestPoint(Vec3(127.5f, 0.0f, 220.0f));
				
				for (int j = 0; j < 4; ++j)
				{
					Vec3 tempVarDir =
						mIceRoom[i].GetIceStage1_NPC_Vector()[j]->GetDestPoint().front() -
						mIceRoom[i].GetIceStage1_NPC_Vector()[j]->GetInitPos();

					mIceRoom[i].GetIceStage1_NPC_Vector()[j]->SetDir(tempVarDir.Normalize());

					scene_packet.initNPCPosition[j].Dir = tempVarDir.Normalize();
				}

				for (int j = 0; j < 2; ++j) {
					SendPacket(mIceRoom[i].GetClientID(j),
						reinterpret_cast<unsigned char*>(&scene_packet),
						clients);
				}

				clients[mIceRoom[i].GetClientID(0)].is_gaming = true;
				clients[mIceRoom[i].GetClientID(1)].is_gaming = true;

				mIceRoom[i].SetRoomState(true);
				mIceRoom[i].SetClientsNumber();

				mIceRoom[i].SetStageState(1);			//1 넣으면 1탄 중, 2넣으면 2탄중..

														//-----Timer Event Setting-----
				Timer_Event player1Event;
				player1Event.event = E_MOVE_ICE;
				player1Event.exec_time = high_resolution_clock::now() + 3s;
				player1Event.object_id = mIceRoom[i].GetClientID(0);
				player1Event.roomNum = roomNumber;

				Timer_Event player2Event;
				player2Event.event = E_MOVE_ICE;
				player2Event.exec_time = high_resolution_clock::now() + 3s;
				player2Event.object_id = mIceRoom[i].GetClientID(1);
				player2Event.roomNum = roomNumber;

				Timer_Event Stage1_NPC_Event;
				Stage1_NPC_Event.event = E_NMOVE_ICE;
				Stage1_NPC_Event.exec_time = high_resolution_clock::now() + 1s;
				Stage1_NPC_Event.object_id = roomNumber;
				Stage1_NPC_Event.roomNum = roomNumber;

				CThreadManager::GetInstance()->tq_lock.lock();
				CThreadManager::GetInstance()->timer_queue.push(player1Event);
				CThreadManager::GetInstance()->timer_queue.push(player2Event);

				CThreadManager::GetInstance()->timer_queue.push(Stage1_NPC_Event);

				CThreadManager::GetInstance()->tq_lock.unlock();
				break;
			}
			}
		}
	}
}
void CPacketMgr::IceNextStage(int now_ID, unsigned char  packet, CLIENT * clients)
{
	//TODO : KEySTATE변환

	if (true == mIceRoom[clients[now_ID].roomNumber].StageCheck())
	{
		//스테이지 변환 전송
		mIceRoom[clients[now_ID].roomNumber].SetStageCheckVar();
		if (packet == MAP_ICE_STAGE1_CLEAR)
		{
			int roomNumber = clients[now_ID].roomNumber;
			sc_packet_scene scene_packet;
			scene_packet.size = sizeof(sc_packet_scene);
			scene_packet.type = MAP_ICE_STAGE2;

			//0번
			scene_packet.initPosition[0].id =
				(WORD)mIceRoom[clients[now_ID].roomNumber].GetClientID(0);
			scene_packet.initPosition[0].Position =
				Vec3(PLAYER1_POS_X, PLAYER1_POS_Y, PLAYER1_POS_Z);
			scene_packet.initPosition[0].CharacterType =
				clients[mIceRoom[roomNumber].GetClientID(0)].gaming_character;

			//1번
			scene_packet.initPosition[1].id =
				(WORD)mIceRoom[clients[now_ID].roomNumber].GetClientID(1);
			scene_packet.initPosition[1].Position =
				Vec3(PLAYER2_POS_X, PLAYER2_POS_Y, PLAYER2_POS_Z);
			scene_packet.initPosition[1].CharacterType =
				clients[mIceRoom[roomNumber].GetClientID(1)].gaming_character;

			scene_packet.initNPCPosition[0].npcID = 0;
			scene_packet.initNPCPosition[0].NpcPosition = Vec3{ 100.0f, 0.0f, 90.0f };

			scene_packet.initNPCPosition[1].npcID = 1;
			scene_packet.initNPCPosition[1].NpcPosition = Vec3{ 150.0f, 0.0f, 120.0f };

			scene_packet.initNPCPosition[2].npcID = 2;
			scene_packet.initNPCPosition[2].NpcPosition = Vec3{ 130.0f, 0.0f, 180.0f };

			clients[mIceRoom[roomNumber].GetClientID(0)].player.m_vPosition.x = PLAYER1_POS_X;
			clients[mIceRoom[roomNumber].GetClientID(0)].player.m_vPosition.y = PLAYER1_POS_Y;
			clients[mIceRoom[roomNumber].GetClientID(0)].player.m_vPosition.z = PLAYER1_POS_Z;
			clients[mIceRoom[roomNumber].GetClientID(0)].player.m_fAngle = PLAYER1_POS_ANGLE;
			clients[mIceRoom[roomNumber].GetClientID(0)].key_state = NONE_KEY;

			clients[mIceRoom[roomNumber].GetClientID(1)].player.m_vPosition.x = PLAYER2_POS_X;
			clients[mIceRoom[roomNumber].GetClientID(1)].player.m_vPosition.y = PLAYER2_POS_Y;
			clients[mIceRoom[roomNumber].GetClientID(1)].player.m_vPosition.z = PLAYER2_POS_Z;
			clients[mIceRoom[roomNumber].GetClientID(1)].player.m_fAngle = PLAYER2_POS_ANGLE;
			clients[mIceRoom[roomNumber].GetClientID(1)].key_state = NONE_KEY;


			mIceRoom[roomNumber].GetIceStage2_NPC_Vector()[0]->SetID(0);
			mIceRoom[roomNumber].GetIceStage2_NPC_Vector()[1]->SetID(1);
			mIceRoom[roomNumber].GetIceStage2_NPC_Vector()[2]->SetID(2);

			//방 번호 셋팅
			mIceRoom[roomNumber].GetIceStage2_NPC_Vector()[0]->SetRoomNumber(roomNumber);
			mIceRoom[roomNumber].GetIceStage2_NPC_Vector()[1]->SetRoomNumber(roomNumber);
			mIceRoom[roomNumber].GetIceStage2_NPC_Vector()[2]->SetRoomNumber(roomNumber);
			Sleep(1);
			//좌표 셋팅
			mIceRoom[roomNumber].GetIceStage2_NPC_Vector()[0]->SetDestPoint(Vec3(110.0f, 0.0f, 80.0f));
			mIceRoom[roomNumber].GetIceStage2_NPC_Vector()[0]->SetDestPoint(Vec3(115.0f, 0.0f, 75.0f));
			mIceRoom[roomNumber].GetIceStage2_NPC_Vector()[0]->SetDestPoint(Vec3(107.0f, 0.0f, 80.0f));
			mIceRoom[roomNumber].GetIceStage2_NPC_Vector()[0]->SetDestPoint(Vec3(100.0f, 0.0f, 90.0f));

			mIceRoom[roomNumber].GetIceStage2_NPC_Vector()[1]->SetDestPoint(Vec3(155.0f, 0.0f, 125.0f));
			mIceRoom[roomNumber].GetIceStage2_NPC_Vector()[1]->SetDestPoint(Vec3(147.0f, 0.0f, 120.0f));
			mIceRoom[roomNumber].GetIceStage2_NPC_Vector()[1]->SetDestPoint(Vec3(149.0f, 0.0f, 122.0f));
			mIceRoom[roomNumber].GetIceStage2_NPC_Vector()[1]->SetDestPoint(Vec3(154.0f, 0.0f, 120.0f));

			mIceRoom[roomNumber].GetIceStage2_NPC_Vector()[2]->SetDestPoint(Vec3(125.0f, 0.0f, 175.0f));
			mIceRoom[roomNumber].GetIceStage2_NPC_Vector()[2]->SetDestPoint(Vec3(132.0f, 0.0f, 182.0f));
			mIceRoom[roomNumber].GetIceStage2_NPC_Vector()[2]->SetDestPoint(Vec3(135.0f, 0.0f, 179.0f));
			mIceRoom[roomNumber].GetIceStage2_NPC_Vector()[2]->SetDestPoint(Vec3(130.0f, 0.0f, 181.0f));

			mIceRoom[roomNumber].SetStageState(2);			//1 넣으면 1탄 중, 2넣으면 2탄중..

			for (int j = 0; j < 3; ++j)
			{
				Vec3 tempVarDir =
					mIceRoom[roomNumber].GetIceStage2_NPC_Vector()[j]->GetDestPoint().front() -
					mIceRoom[roomNumber].GetIceStage2_NPC_Vector()[j]->GetInitPos();

				mIceRoom[roomNumber].GetIceStage2_NPC_Vector()[j]->SetDir(tempVarDir.Normalize());

				scene_packet.initNPCPosition[j].Dir = tempVarDir.Normalize();
			}

			clients[mIceRoom[roomNumber].GetClientID(0)].is_gaming = true;
			clients[mIceRoom[roomNumber].GetClientID(1)].is_gaming = true;

			mIceRoom[roomNumber].SetRoomState(true);
			mIceRoom[roomNumber].SetClientsNumber();

			for (int i = 0; i < 2; ++i)
			{
				SendPacket(mIceRoom[clients[now_ID].roomNumber].GetClientID(i),
					reinterpret_cast<unsigned char*>(&scene_packet),
					clients);
			}

			//-----Timer Event Setting-----
			Timer_Event Stage2_NPC_Event;
			Stage2_NPC_Event.event = E_NMOVE_ICE;
			Stage2_NPC_Event.exec_time = high_resolution_clock::now() + 1s;
			Stage2_NPC_Event.object_id = roomNumber;
			Stage2_NPC_Event.roomNum = roomNumber;

			CThreadManager::GetInstance()->tq_lock.lock();
			CThreadManager::GetInstance()->timer_queue.push(Stage2_NPC_Event);
			CThreadManager::GetInstance()->tq_lock.unlock();
		}
		////////////////////////////////////////////////////////////////
		else if (packet == MAP_ICE_STAGE2_CLEAR)
		{
			int roomNumber = clients[now_ID].roomNumber;

			sc_packet_scene scene_packet;
			scene_packet.size = sizeof(sc_packet_scene);
			scene_packet.type = MAP_ICE_STAGE3;

			//0번
			scene_packet.initPosition[0].id =
				(WORD)mIceRoom[clients[now_ID].roomNumber].GetClientID(0);
			scene_packet.initPosition[0].Position =
				Vec3(PLAYER1_POS_X, PLAYER1_POS_Y, PLAYER1_POS_Z);
			scene_packet.initPosition[0].CharacterType =
				clients[mIceRoom[roomNumber].GetClientID(0)].gaming_character;

			//1번
			scene_packet.initPosition[1].id =
				(WORD)mIceRoom[clients[now_ID].roomNumber].GetClientID(1);
			scene_packet.initPosition[1].Position =
				Vec3(PLAYER2_POS_X, PLAYER2_POS_Y, PLAYER2_POS_Z);
			scene_packet.initPosition[1].CharacterType =
				clients[mIceRoom[roomNumber].GetClientID(1)].gaming_character;

			scene_packet.initNPCPosition[0].npcID = 0;
			scene_packet.initNPCPosition[0].NpcPosition = Vec3{ 128.0f, 0.0f, 130.0f };

			clients[mIceRoom[roomNumber].GetClientID(0)].player.m_vPosition.x = PLAYER1_POS_X;
			clients[mIceRoom[roomNumber].GetClientID(0)].player.m_vPosition.y = PLAYER1_POS_Y;
			clients[mIceRoom[roomNumber].GetClientID(0)].player.m_vPosition.z = PLAYER1_POS_Z;
			clients[mIceRoom[roomNumber].GetClientID(0)].player.m_fAngle = PLAYER1_POS_ANGLE;
			clients[mIceRoom[roomNumber].GetClientID(0)].key_state = NONE_KEY;

			clients[mIceRoom[roomNumber].GetClientID(1)].player.m_vPosition.x = PLAYER2_POS_X;
			clients[mIceRoom[roomNumber].GetClientID(1)].player.m_vPosition.y = PLAYER2_POS_Y;
			clients[mIceRoom[roomNumber].GetClientID(1)].player.m_vPosition.z = PLAYER2_POS_Z;
			clients[mIceRoom[roomNumber].GetClientID(1)].player.m_fAngle = PLAYER2_POS_ANGLE;
			clients[mIceRoom[roomNumber].GetClientID(1)].key_state = NONE_KEY;

			mIceRoom[roomNumber].GetIceStage3_NPC_Vector()[0]->SetID(0);

			//방 번호 셋팅
			mIceRoom[roomNumber].GetIceStage3_NPC_Vector()[0]->SetRoomNumber(roomNumber);
			Sleep(1);
			//좌표 셋팅
			// 128.0f, 0.0f, 130.0f
			mIceRoom[roomNumber].GetIceStage3_NPC_Vector()[0]->SetDestPoint(Vec3(135.0f, 0.0f, 130.0f));
			mIceRoom[roomNumber].GetIceStage3_NPC_Vector()[0]->SetDestPoint(Vec3(120.0f, 0.0f, 130.0f));
			mIceRoom[roomNumber].GetIceStage3_NPC_Vector()[0]->SetDestPoint(Vec3(140.0f, 0.0f, 90.0f));
			mIceRoom[roomNumber].GetIceStage3_NPC_Vector()[0]->SetDestPoint(Vec3(110.0f, 0.0f, 90.0f));

			mIceRoom[roomNumber].SetStageState(3);			//1 넣으면 1탄 중, 2넣으면 2탄중..

			for (int j = 0; j < 1; ++j)  
			{
				Vec3 tempVarDir =
					mIceRoom[roomNumber].GetIceStage3_NPC_Vector()[j]->GetDestPoint().front() -
					mIceRoom[roomNumber].GetIceStage3_NPC_Vector()[j]->GetInitPos();

				mIceRoom[roomNumber].GetIceStage3_NPC_Vector()[j]->SetDir(tempVarDir.Normalize());

				scene_packet.initNPCPosition[j].Dir = tempVarDir.Normalize();
			}

			clients[mIceRoom[roomNumber].GetClientID(0)].is_gaming = true;
			clients[mIceRoom[roomNumber].GetClientID(1)].is_gaming = true;

			mIceRoom[roomNumber].SetRoomState(true);
			mIceRoom[roomNumber].SetClientsNumber();

			for (int i = 0; i < 2; ++i)
			{
				SendPacket(mIceRoom[clients[now_ID].roomNumber].GetClientID(i),
					reinterpret_cast<unsigned char*>(&scene_packet),
					clients);
			}

			//-----Timer Event Setting-----
			Timer_Event Stage3_NPC_Event;
			Stage3_NPC_Event.event = E_NMOVE_ICE;
			Stage3_NPC_Event.exec_time = high_resolution_clock::now() + 1s;
			Stage3_NPC_Event.object_id = roomNumber;
			Stage3_NPC_Event.roomNum = roomNumber;

			CThreadManager::GetInstance()->tq_lock.lock();
			CThreadManager::GetInstance()->timer_queue.push(Stage3_NPC_Event);
			CThreadManager::GetInstance()->tq_lock.unlock();
		}

	}
}

void CPacketMgr::MakeRemainRoom(int now_ID, unsigned char packet, CLIENT * clients)
{
	// TODO : 2,3,4,인용에 각각에 맞게 수정 필요
	cout << "MAKE Remain ROOM \n";
	for (int i = 0; i < 1; ++i)
	{
		if (false == mRemainRoom[i].GetRoomState())		//true 게임중 , false 게임 안하는방
		{
			clients[now_ID].roomNumber = i;

			clients[now_ID].gamingMapType = MAP_REMAIN;

			if (true == mRemainRoom[i].FindClient(now_ID))
				return;

			mRemainRoom[i].PushClient(&clients[now_ID]);

			switch (mRemainRoom[i].CheckFullRoom())
			{
			case true:
			{
				mRemainRoom[i].InitRemain();

				int roomNumber = clients[now_ID].roomNumber;
				//Init Packet

				sc_packet_scene scene_packet;
				scene_packet.size = sizeof(sc_packet_scene);
				scene_packet.type = MAP_REMAIN_STAGE1;
				scene_packet.initPosition[0].id = (WORD)mRemainRoom[i].GetClientID(0);
				scene_packet.initPosition[0].Position = Vec3(PLAYER1_POS_X, PLAYER1_POS_Y, PLAYER1_POS_Z);
				scene_packet.initPosition[0].CharacterType = clients[mRemainRoom[i].GetClientID(0)].gaming_character;

				scene_packet.initPosition[1].id = (WORD)mRemainRoom[i].GetClientID(1);
				scene_packet.initPosition[1].Position = Vec3(PLAYER2_POS_X, PLAYER2_POS_Y, PLAYER2_POS_Z);
				scene_packet.initPosition[1].CharacterType = clients[mRemainRoom[i].GetClientID(1)].gaming_character;

				scene_packet.initNPCPosition[0].npcID = 0;
				scene_packet.initNPCPosition[0].NpcPosition = Vec3{ 120.0f, 0.0f, 55.0f };

				scene_packet.initNPCPosition[1].npcID = 1;
				scene_packet.initNPCPosition[1].NpcPosition = Vec3{ 84.0f, 0.0f, 150.0f };

				scene_packet.initNPCPosition[2].npcID = 2;
				scene_packet.initNPCPosition[2].NpcPosition = Vec3{ 172.0f, 0.0f, 150.0f };

				scene_packet.initNPCPosition[3].npcID = 3;
				scene_packet.initNPCPosition[3].NpcPosition = Vec3{ 120.0f, 0.0f, 204.0f };

				//Init Player&NPC Info
				clients[mRemainRoom[i].GetClientID(0)].player.m_vPosition.x = PLAYER1_POS_X;
				clients[mRemainRoom[i].GetClientID(0)].player.m_vPosition.y = PLAYER1_POS_Y;
				clients[mRemainRoom[i].GetClientID(0)].player.m_vPosition.z = PLAYER1_POS_Z;

				clients[mRemainRoom[i].GetClientID(1)].player.m_vPosition.x = PLAYER2_POS_X;
				clients[mRemainRoom[i].GetClientID(1)].player.m_vPosition.y = PLAYER2_POS_Y;
				clients[mRemainRoom[i].GetClientID(1)].player.m_vPosition.z = PLAYER2_POS_Z;

				clients[mRemainRoom[i].GetClientID(0)].player.m_fAngle = PLAYER1_POS_ANGLE;
				clients[mRemainRoom[i].GetClientID(1)].player.m_fAngle = PLAYER2_POS_ANGLE;

				//NPC들 아이디 셋팅
				mRemainRoom[i].GetRemainStage1_NPC_Vector()[0]->SetID(0);
				mRemainRoom[i].GetRemainStage1_NPC_Vector()[1]->SetID(1);
				mRemainRoom[i].GetRemainStage1_NPC_Vector()[2]->SetID(2);
				mRemainRoom[i].GetRemainStage1_NPC_Vector()[3]->SetID(3);

				//방 번호 셋팅
				mRemainRoom[i].GetRemainStage1_NPC_Vector()[0]->SetRoomNumber(roomNumber);
				mRemainRoom[i].GetRemainStage1_NPC_Vector()[1]->SetRoomNumber(roomNumber);
				mRemainRoom[i].GetRemainStage1_NPC_Vector()[2]->SetRoomNumber(roomNumber);
				mRemainRoom[i].GetRemainStage1_NPC_Vector()[3]->SetRoomNumber(roomNumber);

				//좌표 셋팅
				mRemainRoom[i].GetRemainStage1_NPC_Vector()[0]->SetDestPoint(Vec3(129.0f, 0.0f, 62.0f));
				mRemainRoom[i].GetRemainStage1_NPC_Vector()[0]->SetDestPoint(Vec3(120.0f, 0.0f, 68.0f));
				mRemainRoom[i].GetRemainStage1_NPC_Vector()[0]->SetDestPoint(Vec3(110.0f, 0.0f, 58.5f));
				mRemainRoom[i].GetRemainStage1_NPC_Vector()[0]->SetDestPoint(Vec3(119.0f, 0.0f, 55.5f));

				mRemainRoom[i].GetRemainStage1_NPC_Vector()[1]->SetDestPoint(Vec3(80.0f, 0.0f, 145.0f));
				mRemainRoom[i].GetRemainStage1_NPC_Vector()[1]->SetDestPoint(Vec3(75.0f, 0.0f, 155.0f));
				mRemainRoom[i].GetRemainStage1_NPC_Vector()[1]->SetDestPoint(Vec3(81.0f, 0.0f, 148.0f));
				mRemainRoom[i].GetRemainStage1_NPC_Vector()[1]->SetDestPoint(Vec3(75.0f, 0.0f, 154.0f));

				mRemainRoom[i].GetRemainStage1_NPC_Vector()[2]->SetDestPoint(Vec3(162.0f, 0.0f, 142.0f));
				mRemainRoom[i].GetRemainStage1_NPC_Vector()[2]->SetDestPoint(Vec3(170.0f, 0.0f, 150.0f));
				mRemainRoom[i].GetRemainStage1_NPC_Vector()[2]->SetDestPoint(Vec3(180.0f, 0.0f, 142.0f));
				mRemainRoom[i].GetRemainStage1_NPC_Vector()[2]->SetDestPoint(Vec3(170.0f, 0.0f, 150.0f));

				mRemainRoom[i].GetRemainStage1_NPC_Vector()[3]->SetDestPoint(Vec3(125.0f, 0.52f, 208.0f));
				mRemainRoom[i].GetRemainStage1_NPC_Vector()[3]->SetDestPoint(Vec3(115.0f, 0.52f, 212.0f));
				mRemainRoom[i].GetRemainStage1_NPC_Vector()[3]->SetDestPoint(Vec3(124.0f, 0.52f, 205.0f));
				mRemainRoom[i].GetRemainStage1_NPC_Vector()[3]->SetDestPoint(Vec3(115.0f, 0.52f, 200.0f));

				for (int j = 0; j < 4; ++j)
				{
					Vec3 tempVarDir =
						mRemainRoom[i].GetRemainStage1_NPC_Vector()[j]->GetDestPoint().front() -
						mRemainRoom[i].GetRemainStage1_NPC_Vector()[j]->GetInitPos();

					mRemainRoom[i].GetRemainStage1_NPC_Vector()[j]->SetDir(tempVarDir.Normalize());

					scene_packet.initNPCPosition[j].Dir = tempVarDir.Normalize();
				}

				for (int j = 0; j < 2; ++j) 
				{
					SendPacket(mRemainRoom[i].GetClientID(j),
						reinterpret_cast<unsigned char*>(&scene_packet),
						clients);
				}

				clients[mRemainRoom[i].GetClientID(0)].is_gaming = true;
				clients[mRemainRoom[i].GetClientID(1)].is_gaming = true;

				mRemainRoom[i].SetRoomState(true);
				mRemainRoom[i].SetClientsNumber();

				mRemainRoom[i].SetStageState(1);			//1 넣으면 1탄 중, 2넣으면 2탄중..

														//-----Timer Event Setting-----
				Timer_Event player1Event;
				player1Event.event = E_MOVE_REMAIN;
				player1Event.exec_time = high_resolution_clock::now() + 3s;
				player1Event.object_id = mRemainRoom[i].GetClientID(0);
				player1Event.roomNum = roomNumber;

				Timer_Event player2Event;
				player2Event.event = E_MOVE_REMAIN;
				player2Event.exec_time = high_resolution_clock::now() + 3s;
				player2Event.object_id = mRemainRoom[i].GetClientID(1);
				player2Event.roomNum = roomNumber;

				Timer_Event Stage1_NPC_Event;
				Stage1_NPC_Event.event = E_NMOVE_REMAIN;
				Stage1_NPC_Event.exec_time = high_resolution_clock::now() + 1s;
				Stage1_NPC_Event.object_id = roomNumber;
				Stage1_NPC_Event.roomNum = roomNumber;

				CThreadManager::GetInstance()->tq_lock.lock();
				CThreadManager::GetInstance()->timer_queue.push(player1Event);
				CThreadManager::GetInstance()->timer_queue.push(player2Event);

				CThreadManager::GetInstance()->timer_queue.push(Stage1_NPC_Event);

				CThreadManager::GetInstance()->tq_lock.unlock();
				break;
			}
			}
		}
	}
}
void CPacketMgr::RemainNextStage(int now_ID, unsigned char  packet, CLIENT * clients)
{
	//TODO : KEySTATE변환

	if (true == mRemainRoom[clients[now_ID].roomNumber].StageCheck())
	{
		//스테이지 변환 전송
		mRemainRoom[clients[now_ID].roomNumber].SetStageCheckVar();
		if (packet == MAP_REMAIN_STAGE1_CLEAR)
		{
			int roomNumber = clients[now_ID].roomNumber;
			sc_packet_scene scene_packet;
			scene_packet.size = sizeof(sc_packet_scene);
			scene_packet.type = MAP_REMAIN_STAGE2;

			//0번
			scene_packet.initPosition[0].id =
				(WORD)mRemainRoom[clients[now_ID].roomNumber].GetClientID(0);
			scene_packet.initPosition[0].Position =
				Vec3(PLAYER1_POS_X, PLAYER1_POS_Y, PLAYER1_POS_Z);
			scene_packet.initPosition[0].CharacterType =
				clients[mRemainRoom[roomNumber].GetClientID(0)].gaming_character;

			//1번
			scene_packet.initPosition[1].id =
				(WORD)mRemainRoom[clients[now_ID].roomNumber].GetClientID(1);
			scene_packet.initPosition[1].Position =
				Vec3(PLAYER2_POS_X, PLAYER2_POS_Y, PLAYER2_POS_Z);
			scene_packet.initPosition[1].CharacterType =
				clients[mRemainRoom[roomNumber].GetClientID(1)].gaming_character;

			scene_packet.initNPCPosition[0].npcID = 0;
			scene_packet.initNPCPosition[0].NpcPosition = Vec3{ 128.0f, 0.0f, 80.0f };

			scene_packet.initNPCPosition[1].npcID = 1;
			scene_packet.initNPCPosition[1].NpcPosition = Vec3{ 165.0f, 0.0f, 178.0f };

			scene_packet.initNPCPosition[2].npcID = 2;
			scene_packet.initNPCPosition[2].NpcPosition = Vec3{ 91.0f, 0.0f, 178.0f };

			clients[mRemainRoom[roomNumber].GetClientID(0)].player.m_vPosition.x = PLAYER1_POS_X;
			clients[mRemainRoom[roomNumber].GetClientID(0)].player.m_vPosition.y = PLAYER1_POS_Y;
			clients[mRemainRoom[roomNumber].GetClientID(0)].player.m_vPosition.z = PLAYER1_POS_Z;
			clients[mRemainRoom[roomNumber].GetClientID(0)].player.m_fAngle = PLAYER1_POS_ANGLE;
			clients[mRemainRoom[roomNumber].GetClientID(0)].key_state = NONE_KEY;

			clients[mRemainRoom[roomNumber].GetClientID(1)].player.m_vPosition.x = PLAYER2_POS_X;
			clients[mRemainRoom[roomNumber].GetClientID(1)].player.m_vPosition.y = PLAYER2_POS_Y;
			clients[mRemainRoom[roomNumber].GetClientID(1)].player.m_vPosition.z = PLAYER2_POS_Z;
			clients[mRemainRoom[roomNumber].GetClientID(1)].player.m_fAngle = PLAYER2_POS_ANGLE;
			clients[mRemainRoom[roomNumber].GetClientID(1)].key_state = NONE_KEY;


			mRemainRoom[roomNumber].GetRemainStage2_NPC_Vector()[0]->SetID(0);
			mRemainRoom[roomNumber].GetRemainStage2_NPC_Vector()[1]->SetID(1);
			mRemainRoom[roomNumber].GetRemainStage2_NPC_Vector()[2]->SetID(2);

			//방 번호 셋팅
			mRemainRoom[roomNumber].GetRemainStage2_NPC_Vector()[0]->SetRoomNumber(roomNumber);
			mRemainRoom[roomNumber].GetRemainStage2_NPC_Vector()[1]->SetRoomNumber(roomNumber);
			mRemainRoom[roomNumber].GetRemainStage2_NPC_Vector()[2]->SetRoomNumber(roomNumber);
			Sleep(1);

			//좌표 셋팅
			mRemainRoom[roomNumber].GetRemainStage2_NPC_Vector()[0]->SetDestPoint(Vec3(136.0f, 0.0f, 72.0f));
			mRemainRoom[roomNumber].GetRemainStage2_NPC_Vector()[0]->SetDestPoint(Vec3(132.0f, 0.0f, 81.0f));
			mRemainRoom[roomNumber].GetRemainStage2_NPC_Vector()[0]->SetDestPoint(Vec3(124.0f, 0.0f, 80.0f));
			mRemainRoom[roomNumber].GetRemainStage2_NPC_Vector()[0]->SetDestPoint(Vec3(127.0f, 0.0f, 75.0f));

			mRemainRoom[roomNumber].GetRemainStage2_NPC_Vector()[1]->SetDestPoint(Vec3(156.0f, 0.0f, 176.0f));
			mRemainRoom[roomNumber].GetRemainStage2_NPC_Vector()[1]->SetDestPoint(Vec3(159.0f, 0.0f, 185.0f));
			mRemainRoom[roomNumber].GetRemainStage2_NPC_Vector()[1]->SetDestPoint(Vec3(163.0f, 0.0f, 183.0f));
			mRemainRoom[roomNumber].GetRemainStage2_NPC_Vector()[1]->SetDestPoint(Vec3(168.0f, 0.0f, 177.0f));

			mRemainRoom[roomNumber].GetRemainStage2_NPC_Vector()[2]->SetDestPoint(Vec3(101.0f, 0.0f, 175.0f));
			mRemainRoom[roomNumber].GetRemainStage2_NPC_Vector()[2]->SetDestPoint(Vec3(102.0f, 0.0f, 182.0f));
			mRemainRoom[roomNumber].GetRemainStage2_NPC_Vector()[2]->SetDestPoint(Vec3(90.0f, 0.0f, 184.0f));
			mRemainRoom[roomNumber].GetRemainStage2_NPC_Vector()[2]->SetDestPoint(Vec3(95.0f, 0.0f, 174.0f));

			mRemainRoom[roomNumber].SetStageState(2);			//1 넣으면 1탄 중, 2넣으면 2탄중..

			for (int j = 0; j < 3; ++j)
			{
				Vec3 tempVarDir =
					mRemainRoom[roomNumber].GetRemainStage2_NPC_Vector()[j]->GetDestPoint().front() -
					mRemainRoom[roomNumber].GetRemainStage2_NPC_Vector()[j]->GetInitPos();

				mRemainRoom[roomNumber].GetRemainStage2_NPC_Vector()[j]->SetDir(tempVarDir.Normalize());

				scene_packet.initNPCPosition[j].Dir = tempVarDir.Normalize();
			}

			clients[mRemainRoom[roomNumber].GetClientID(0)].is_gaming = true;
			clients[mRemainRoom[roomNumber].GetClientID(1)].is_gaming = true;

			mRemainRoom[roomNumber].SetRoomState(true);
			mRemainRoom[roomNumber].SetClientsNumber();

			for (int i = 0; i < 2; ++i)
			{
				SendPacket(mRemainRoom[clients[now_ID].roomNumber].GetClientID(i),
					reinterpret_cast<unsigned char*>(&scene_packet),
					clients);
			}

			//-----Timer Event Setting-----
			Timer_Event Stage2_NPC_Event;
			Stage2_NPC_Event.event = E_NMOVE_REMAIN;
			Stage2_NPC_Event.exec_time = high_resolution_clock::now() + 1s;
			Stage2_NPC_Event.object_id = roomNumber;
			Stage2_NPC_Event.roomNum = roomNumber;

			CThreadManager::GetInstance()->tq_lock.lock();
			CThreadManager::GetInstance()->timer_queue.push(Stage2_NPC_Event);
			CThreadManager::GetInstance()->tq_lock.unlock();
		}
		////////////////////////////////////////////////////////////////
		else if (packet == MAP_REMAIN_STAGE2_CLEAR)
		{
			int roomNumber = clients[now_ID].roomNumber;

			sc_packet_scene scene_packet;
			scene_packet.size = sizeof(sc_packet_scene);
			scene_packet.type = MAP_REMAIN_STAGE3;

			//0번
			scene_packet.initPosition[0].id =
				(WORD)mRemainRoom[clients[now_ID].roomNumber].GetClientID(0);
			scene_packet.initPosition[0].Position =
				Vec3(PLAYER1_POS_X, PLAYER1_POS_Y, PLAYER1_POS_Z);
			scene_packet.initPosition[0].CharacterType =
				clients[mRemainRoom[roomNumber].GetClientID(0)].gaming_character;

			//1번
			scene_packet.initPosition[1].id =
				(WORD)mRemainRoom[clients[now_ID].roomNumber].GetClientID(1);
			scene_packet.initPosition[1].Position =
				Vec3(PLAYER2_POS_X, PLAYER2_POS_Y, PLAYER2_POS_Z);
			scene_packet.initPosition[1].CharacterType =
				clients[mRemainRoom[roomNumber].GetClientID(1)].gaming_character;

			scene_packet.initNPCPosition[0].npcID = 0;
			scene_packet.initNPCPosition[0].NpcPosition = Vec3{ 128.0f, 0.0f, 180.0f };

			clients[mRemainRoom[roomNumber].GetClientID(0)].player.m_vPosition.x = PLAYER1_POS_X;
			clients[mRemainRoom[roomNumber].GetClientID(0)].player.m_vPosition.y = PLAYER1_POS_Y;
			clients[mRemainRoom[roomNumber].GetClientID(0)].player.m_vPosition.z = PLAYER1_POS_Z;
			clients[mRemainRoom[roomNumber].GetClientID(0)].player.m_fAngle = PLAYER1_POS_ANGLE;
			clients[mRemainRoom[roomNumber].GetClientID(0)].key_state = NONE_KEY;

			clients[mRemainRoom[roomNumber].GetClientID(1)].player.m_vPosition.x = PLAYER2_POS_X;
			clients[mRemainRoom[roomNumber].GetClientID(1)].player.m_vPosition.y = PLAYER2_POS_Y;
			clients[mRemainRoom[roomNumber].GetClientID(1)].player.m_vPosition.z = PLAYER2_POS_Z;
			clients[mRemainRoom[roomNumber].GetClientID(1)].player.m_fAngle = PLAYER2_POS_ANGLE;
			clients[mRemainRoom[roomNumber].GetClientID(1)].key_state = NONE_KEY;

			mRemainRoom[roomNumber].GetRemainStage3_NPC_Vector()[0]->SetID(0);

			//방 번호 셋팅
			mRemainRoom[roomNumber].GetRemainStage3_NPC_Vector()[0]->SetRoomNumber(roomNumber);
			Sleep(1);
			//좌표 셋팅
			mRemainRoom[roomNumber].GetRemainStage3_NPC_Vector()[0]->SetDestPoint(Vec3(136.0f, 0.0f, 165.0f));
			mRemainRoom[roomNumber].GetRemainStage3_NPC_Vector()[0]->SetDestPoint(Vec3(126.0f, 0.0f, 180.0f));
			mRemainRoom[roomNumber].GetRemainStage3_NPC_Vector()[0]->SetDestPoint(Vec3(116.0f, 0.0f, 165.0f));
			mRemainRoom[roomNumber].GetRemainStage3_NPC_Vector()[0]->SetDestPoint(Vec3(126.0f, 0.0f, 175.0f));

			mRemainRoom[roomNumber].SetStageState(3);			//1 넣으면 1탄 중, 2넣으면 2탄중..

			for (int j = 0; j < 1; ++j)
			{
				Vec3 tempVarDir =
					mRemainRoom[roomNumber].GetRemainStage3_NPC_Vector()[j]->GetDestPoint().front() -
					mRemainRoom[roomNumber].GetRemainStage3_NPC_Vector()[j]->GetInitPos();

				mRemainRoom[roomNumber].GetRemainStage3_NPC_Vector()[j]->SetDir(tempVarDir.Normalize());

				scene_packet.initNPCPosition[j].Dir = tempVarDir.Normalize();
			}

			clients[mRemainRoom[roomNumber].GetClientID(0)].is_gaming = true;
			clients[mRemainRoom[roomNumber].GetClientID(1)].is_gaming = true;

			mRemainRoom[roomNumber].SetRoomState(true);
			mRemainRoom[roomNumber].SetClientsNumber();

			for (int i = 0; i < 2; ++i)
			{
				SendPacket(mRemainRoom[clients[now_ID].roomNumber].GetClientID(i),
					reinterpret_cast<unsigned char*>(&scene_packet),
					clients);
			}

			//-----Timer Event Setting-----
			Timer_Event Stage3_NPC_Event;
			Stage3_NPC_Event.event = E_NMOVE_REMAIN;
			Stage3_NPC_Event.exec_time = high_resolution_clock::now() + 1s;
			Stage3_NPC_Event.object_id = roomNumber;
			Stage3_NPC_Event.roomNum = roomNumber;

			CThreadManager::GetInstance()->tq_lock.lock();
			CThreadManager::GetInstance()->timer_queue.push(Stage3_NPC_Event);
			CThreadManager::GetInstance()->tq_lock.unlock();
		}

	}
}
void CPacketMgr::error_display(char *msg, int err_num)
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