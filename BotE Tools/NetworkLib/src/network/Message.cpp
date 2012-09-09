#include "StdAfx.h"
#include "Message.h"

#include "ChosenRaces.h"
#include "RequestRace.h"
#include "BeginGame.h"
#include "NextRound.h"
#include "EndOfRound.h"
#include "ChatMsg.h"

#define BEGIN_CREATEMESSAGE_LIST() \
	switch (nID) {
#define END_CREATEMESSAGE_LIST() \
	default: ASSERT(FALSE); return NULL; }
#define CREATEMESSAGE_ENTRY(id, cls) \
	case (id): return new (cls)();

namespace network
{
	CMessage::CMessage(BYTE nID) : m_nID(nID)
	{
	}

	CMessage::~CMessage()
	{
	}

	CMessage *CMessage::CreateMessage(BYTE nID)
	{
		BEGIN_CREATEMESSAGE_LIST()
		CREATEMESSAGE_ENTRY(MSGID_CHOSENRACES, CChosenRaces)
		CREATEMESSAGE_ENTRY(MSGID_REQUESTRACE, CRequestRace)
		CREATEMESSAGE_ENTRY(MSGID_BEGINGAME, CBeginGame)
		CREATEMESSAGE_ENTRY(MSGID_NEXTROUND, CNextRound)
		CREATEMESSAGE_ENTRY(MSGID_ENDOFROUND, CEndOfRound)
		CREATEMESSAGE_ENTRY(MSGID_CHAT, CChatMsg)
		END_CREATEMESSAGE_LIST()
	}

};
