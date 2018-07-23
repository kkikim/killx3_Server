#pragma once

#define	_XM_NO_INTRINSICS_
#include <WinSock2.h>
#include <Windows.h>
#include <stdio.h>
#include <mutex>
#include <set>
#include <list>
#include <vector>
#include <d3d11.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>

#define MAX_BUFF_SIZE			4000
#define MAX_PACKET_SIZE			255
#define BUFSIZE					1024	

#define MY_SERVER_PORT			4000
#define MAX_USER				100
#define WORKER_THREAD_NUM		4		

#define MAX_ROOM		100

//-----OVERLAP_STATE-----
#define OP_RECV					1
#define OP_SEND					2
//---------CAVE MAP-----
#define OP_MOVE					3
#define OP_NPC					4
#define OP_SEND_NPC_DIR			5
#define OP_NPC_DEATH			6
//---------ICE MAP-----
#define OP_MOVE_ICE				7
#define OP_NPC_ICE				8
#define OP_SEND_NPC_DIR_ICE		9
#define OP_NPC_DEATH_ICE		10
//--------REMAIN MAP------
#define OP_MOVE_REMAIN			11
#define OP_NPC_REMAIN			12
#define OP_SEND_NPC_DIR_REMAIN	13
#define OP_NPC_DEATH_REMAIN		14

#define OP_CAVELOBBY_BACK		20
#define OP_ICELOBBY_BACK		21
#define OP_REMAINLOBBY_BACK		22


//-----PACKET_STATE-----
//--------Client->Sever------

#define CS_KEY					0
#define CS_ANGLE				1
#define CS_COLLISION			2
#define CS_NPC_DAMAGE			3
#define CS_TRICK				4

//--------Sever->Client------
#define SC_POS					0
#define SC_KEY					1
#define SC_ANGLE				2
#define SC_PUT_PLAYER			3
#define SC_COLLISION			4
#define SC_NPC_POS				5
#define SC_NPC_DIR				6
#define SC_NPC_DEATH			7
#define SC_NPC_DAMAGE			8
#define SC_TRICK				9
#define SC_LOBBY_BACK			10


#define MAP_CAVE				100
#define MAP_CAVE_STAGE1			101
#define MAP_CAVE_STAGE2			102
#define MAP_CAVE_STAGE3			103
#define MAP_CAVE_STAGE1_CLEAR   104
#define MAP_CAVE_STAGE2_CLEAR	105
#define MAP_CAVE_STAGE3_CLEAR	106

#define MAP_ICE					110
#define MAP_ICE_STAGE1			111
#define MAP_ICE_STAGE2			112
#define MAP_ICE_STAGE3			113
#define MAP_ICE_STAGE1_CLEAR	114
#define MAP_ICE_STAGE2_CLEAR	115
#define MAP_ICE_STAGE3_CLEAR	116

#define MAP_REMAIN				120
#define MAP_REMAIN_STAGE1		121
#define MAP_REMAIN_STAGE2		122
#define MAP_REMAIN_STAGE3		123
#define MAP_REMAIN_STAGE1_CLEAR	124
#define MAP_REMAIN_STAGE2_CLEAR	125
#define MAP_REMAIN_STAGE3_CLEAR	126

//------PLAYER CHARTYPE----------
#define CHARACTER_WARRIOR		51
#define CHARACTER_PALADIN		52
#define CHARACTER_KNIGHT		53

//------PLAYER_KEY_STATE---------
#define NONE_KEY		0
#define UP_KEY			1
#define DOWN_KEY		2
#define SHIFT_KEY		3
#define ATTACK_KEY		4
#define BACKSHIFT_KEY	5
#define SPACE_KEY		6
#define KNIGHT_DEFENCE_KEY		7
#define PALADIN_DEFENCE_KEY		8
#define ROLLEND			9
#define KNIGHT_DEFENCE_KEY_UP	10
#define PALADIN_DEFENCE_KEY_UP	11

//-----NPC_TYPE-----
#define GOLEM			90
#define SPIDER			91
#define GOBLIN			92
#define BOSS			93

//------CAVE_STAGE1_PLAYERS_POSITION-------
//184.0f, 0.0f, 205.0f
#define PLAYER1_POS_X		126.0f
#define PLAYER1_POS_Y		0.0f
#define PLAYER1_POS_Z		20.0f
#define PLAYER1_POS_ANGLE	0.0f

#define PLAYER2_POS_X 		133.0f
#define PLAYER2_POS_Y 		0.0f
#define PLAYER2_POS_Z		20.0f
#define PLAYER2_POS_ANGLE	0.0f

//#define CAVE_STAGE1_PLAYER1_POS_X		182.0f
//#define CAVE_STAGE1_PLAYER1_POS_Y		1.0f
//#define CAVE_STAGE1_PLAYER1_POS_Z		202.0f
//#define CAVE_STAGE1_PLAYER1_POS_ANGLE	0.0f
//
//#define CAVE_STAGE1_PLAYER2_POS_X 		186.0f
//#define CAVE_STAGE1_PLAYER2_POS_Y 		1.0f
//#define CAVE_STAGE1_PLAYER2_POS_Z		202.0f
//#define CAVE_STAGE1_PLAYER2_POS_ANGLE	0.0f

//-----CAVE_STAGE2_PLAYER_POSITION---------


//-----CAVE_STAGE1_NPC_POSITION-----


//-----CAVE_STAGE1_NPC_DestinationPoint-----


//-----NPC_FSM_STATE------
#define IDLE			0
#define TRACKING		1
#define ATTACK			2
#define GOBACK			3

//NPC Ani
#define NPC_WALK			1
#define NPC_ATTACK			2
#define NPC_DEAD			4


	

using namespace std;
using namespace DirectX;
using namespace DirectX::PackedVector;

#pragma comment(lib, "d3d11")
#include "Math.h"
struct CLIENT;

enum Event_Type {	E_MOVE,			E_NMOVE ,
					E_MOVE_ICE,		E_NMOVE_ICE,
					E_MOVE_REMAIN,	E_NMOVE_REMAIN};

typedef struct PLAYER {			//int y추가
	Vec3			m_vPosition;
	Vec3			m_vDir;
	FLOAT			m_fAngle;
	Vec3			m_vAngle;
	Vec3			m_vAxis[3];
	float			m_fAxisLength[3];
} PLAYER;

typedef struct OverlapEx
{
	WSAOVERLAPPED	original_overlap;
	int				operation;
	WSABUF			recv_buff;
	unsigned char	socket_buff[MAX_BUFF_SIZE];
	int				packet_size;
	BYTE			event_type;
	int				event_target;
	int				ID;
	BYTE			MapType;
}OverlapEx;

// 클라이언트는 확장 오버랩 구조체를 가져야 한다.
typedef struct CLIENT {
	bool			is_connected;			//접속여부
	bool			is_gaming;				//게임중인지
	bool			Collision;
	BYTE			gaming_character;		//어떤 캐릭터로 게임중인지.
	BYTE			gamingMapType;			//어떤맵에서 게임중인지.
	BYTE			animationState;
	BYTE			key_state;			
	unsigned char	packet[MAX_BUFF_SIZE];      //조각난 패킷들이 조합되는 공간
	int				id;						
	int				roomNumber;				//room Vector의 인덱스로 사용하자
	int				previous_data_size;
	float			Speed;
	Vec3			character_direction;	//캐릭터의 방향
	SOCKET			sock;
	PLAYER			player;
	OverlapEx		recv_overlap;
	vector<int>		roomList;					//같이 게임하고 있는애들, 나자신포함
}CLIENT;

//static HANDLE				hIOCP;

#pragma pack (push, 1)

struct InitPos
{
	WORD id;
	BYTE CharacterType;
	Vec3 Position;
};


struct NPCInitPos {			//WORD 0 ~ 약65000
	WORD npcID;				//00000  // 000 - 방번호, 0 스테이지번호, 0 - NPC고유번호
	Vec3 NpcPosition;		//
	Vec3 Dir;
	//bool m_bAlive;
	//int m_iHp;
	//int m_iAnimationType;
	//bool m_bOneLoop;
};

//Server->Client
struct sc_packet_scene {
	BYTE size;
	BYTE type;
	InitPos initPosition[2];	
	NPCInitPos initNPCPosition[4];
};
struct sc_packet_pos {
	BYTE size;
	BYTE type;
	WORD id;
	Vec3 pos;
};
struct sc_packet_angle {
	BYTE size;
	BYTE type;
	Vec3 Direction;
};

struct sc_packet_npc_dir {
	BYTE size;
	BYTE type;
	BYTE ani;
	WORD id;
	Vec3 Direction;
	Vec3 Pos;
};

struct sc_packet_put_player {
	BYTE size;
	BYTE type;
	WORD id;
};
struct sc_packet_key{
	BYTE size;
	BYTE type;			//KEY_TYPE
	WORD id;
	BYTE key;
	float angle;
	Vec3 direction;		//Direction
};
struct NPC_DATA 
{
	WORD id;
	Vec3 pos;
};
struct sc_packet_npc_pos {
	BYTE size;
	BYTE type;
	NPC_DATA npcs_data[4];
};

//Client->Server
struct cs_packet_map {			//맵 선택 데이터 전송
	BYTE size;
	BYTE type;
	BYTE character_type;
};

struct cs_packet_key {
	BYTE size;
	BYTE type;				//KEY_STATE
	BYTE key;
	Vec3 direction;
};
struct cs_packet_DirToAngle {
	BYTE size;
	BYTE type;
	float fY;
	Vec3 Direction;
};

struct sc_packet_DirToAngle
{
	BYTE size;
	BYTE type;
	WORD id;
	float Angle;
	Vec3 Dir;
};

struct cs_packet_collision
{
	BYTE size;
	BYTE type;
	Vec3 CollPos;
};

struct sc_packet_collision
{
	BYTE size;
	BYTE type;
	WORD m_Id;
	Vec3 PrevPos;
};

struct cs_packet_NpcDamage
{
	BYTE size;
	BYTE type;
	WORD m_Id;
	int m_iDamage;
};

struct sc_packet_NpcDeath
{
	BYTE size;
	BYTE type;
	WORD m_Id;
	bool bAlive;
	bool bRemove;
};

struct sc_packet_NPCDamage
{
	BYTE size;
	BYTE type;
	WORD m_Id;
};

struct cs_camera_effect
{
	BYTE size;
	BYTE type;
	WORD m_Id;
};

struct sc_camera_effect
{
	BYTE size;
	BYTE type;
	WORD m_Id;
};

struct sc_lobby_back
{
	BYTE size;
	BYTE type;
};

#pragma pack (pop)