#include "StdAfx.h"
#include "ChatMsg.h"

namespace network
{
	CChatMsg::CChatMsg() : CMessage(MSGID_CHAT), m_sSenderName(""), m_nSender(RACE_NONE), m_sMsg("")
	{
	}

	CChatMsg::CChatMsg(CString msg) : CMessage(MSGID_CHAT), m_sSenderName(""),
		m_nSender(RACE_NONE), m_sMsg(msg)
	{
		m_sMsg.Trim();
		ASSERT(m_sMsg.GetLength());
	}

	CChatMsg::CChatMsg(CString msg, RACE receiver) : CMessage(MSGID_CHAT), m_sSenderName(""),
		m_nSender(RACE_NONE), m_sMsg(msg)
	{
		m_sMsg.Trim();
		ASSERT(m_sMsg.GetLength());
		AddReceiver(receiver);
	}

	CChatMsg::~CChatMsg()
	{
	}

	RACE CChatMsg::GetReceiver(int idx) const
	{
		POSITION pos = m_lReceivers.FindIndex(idx);
		if (!pos) return RACE_NONE;
		else return m_lReceivers.GetAt(pos);
	}

	void CChatMsg::AddReceiver(RACE race)
	{
		// falls RACE_ALL eingetragen wird, werden alle anderen Einträge entfernt bzw.
		// nicht mehr aufgenommen

		if (race == RACE_ALL)
		{
			m_lReceivers.RemoveAll();
			m_lReceivers.AddTail(RACE_ALL);
		}
		else if (RACE_FIRST <= race && race < RACE_LAST &&
			(m_lReceivers.GetCount() == 0 || m_lReceivers.GetHead() != RACE_ALL))
		{
			if (!m_lReceivers.Find(race)) m_lReceivers.AddTail(race);
		}
	}

	void CChatMsg::SetSender(RACE nSender)
	{
		ASSERT((RACE_FIRST <= nSender && nSender < RACE_LAST) || nSender == RACE_NONE);
		m_nSender = nSender;
	}

	void CChatMsg::SetSenderName(CString strSender)
	{
		m_sSenderName = strSender;
		m_sSenderName.Trim();
	}

	void CChatMsg::Serialize(CArchive &ar)
	{
		if (ar.IsStoring())
		{
			ar << m_sSenderName;
			ar << (BYTE)m_nSender;

			ASSERT(m_lReceivers.GetCount() <= 255);
			ar << (BYTE)m_lReceivers.GetCount();
			for (POSITION pos = m_lReceivers.GetHeadPosition(); pos; m_lReceivers.GetNext(pos))
				ar << (BYTE)m_lReceivers.GetAt(pos);

			ar << m_sMsg;
		}
		else
		{
			ar >> m_sSenderName;
			BYTE nSender;
			ar >> nSender;
			SetSender((RACE)nSender);

			BYTE nCount;
			ar >> nCount;
			m_lReceivers.RemoveAll();
			for (; nCount; nCount--)
			{
				BYTE nReceiver;
				ar >> nReceiver;
				AddReceiver((RACE)nReceiver);
			}

			ar >> m_sMsg;
		}
	}

	BOOL CChatMsg::IsForAllClients() const
	{
		return (m_lReceivers.GetCount() > 0 && m_lReceivers.GetHead() == RACE_ALL);
	}

};
