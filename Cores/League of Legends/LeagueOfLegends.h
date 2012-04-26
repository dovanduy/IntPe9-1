#ifndef LEAGUE_OF_LEGENDS_H
#define LEAGUE_OF_LEGENDS_H

#include <Skeleton.h>
#include <Crypto/Blowfish.h>
#include <General/General.h>
#include <General/Memory.h>
#include <enet/enet.h>

#include <map>
#include <vector>

#define NAKED __declspec(naked)

using std::map;
using std::vector;


//LoL typedefs
typedef char         (__thiscall *SendPacket)(void *p, size_t length, const void *data, uint8 channel, ENetPacketFlag type);
typedef ENetEvent*   (__thiscall *AddEvent)(void *p, ENetEvent *event);
typedef void*        (__cdecl *EnetMalloc)(size_t Size);

extern EnetMalloc enetMalloc;
extern SendPacket lolSendPacket;
extern AddEvent lolAddEvent;

#pragma pack(push,1)
struct ChatPacket
{
	uint32 cmd;
	uint32 type;
	uint32 size;
	uint8 text;
	uint8 *getText()
	{
		return &text;
	}

	static ChatPacket *create(uint8 *text, uint32 length)
	{
		ChatPacket *packet = (ChatPacket*)malloc(sizeof(ChatPacket)+length);
		packet->cmd = 0;
		packet->type = 0;
		packet->size = length;
		memcpy(packet->getText(), text, length+1); //including the 0 byte copy
		return packet;
	}

	/* This will be done by LoL code */
	void destroy()
	{
		free((void*)this);
	}

	uint32 totalLength()
	{
		return sizeof(ChatPacket)+size;
	}
};
#pragma pack(pop)
class LeagueOfLegends : public Skeleton
{
private:
	uint16 _keySize;
	uint8 *_key;
	bool _wrongCommandLine;
	
public:
	Blowfish *blowfish;
	LeagueOfLegends();

	void initialize();
	void finalize();
	char *getName();
	void parsePython(uint8 *script, uint32 length);
	void debugToChat(uint8 *text);

//Static part
	//Custom functions/callbacks
	static void onExit();
	static void addEvent(void *pointer, ENetEvent *event);
	static void recvPacket(uint8 *data, uint32 length, uint8 channel, ENetPacketFlag type = ENET_PACKET_FLAG_NO_ALLOCATE);
	static void sendPacket(uint8* data, uint32 length, uint8 channel, ENetPacketFlag type = ENET_PACKET_FLAG_RELIABLE);


	//Lol steal functions
	static void __stdcall stealRecvPacket(ENetEvent *event);
	static void __stdcall stealAddEvent(void *pointer, ENetEvent *event);
	static void __stdcall stealSendPacket(void *pointer, uint8* data, uint32 length, uint8 channel, ENetPacketFlag flag);

	//Lol variables
	static ENetPeer *addEventPeer;
	static void *pointerAddEvent;
	static void *pointerSendPacket;
	static bool isInjected;
};

extern LeagueOfLegends *leagueOfLegends;

#endif