#include "StdAfx.h"
#include "FireArc.h"
#include "AssertBotE.h"

IMPLEMENT_SERIAL (CFireArc, CObject, 1)

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////
CFireArc::CFireArc(void) : m_nMountPos(0), m_nAngle(90)
{
}

CFireArc::CFireArc(USHORT mountPos, USHORT angle) : m_nMountPos(mountPos), m_nAngle(angle)
{
	AssertBotE(m_nMountPos <= 360);
	AssertBotE(m_nAngle <= 360);
}

CFireArc::~CFireArc(void)
{
}

//////////////////////////////////////////////////////////////////////
// Kopierkonstruktor
//////////////////////////////////////////////////////////////////////
CFireArc::CFireArc(const CFireArc & rhs)
{
	m_nMountPos		= rhs.m_nMountPos;
	m_nAngle		= rhs.m_nAngle;
}

//////////////////////////////////////////////////////////////////////
// Zuweisungsoperator
//////////////////////////////////////////////////////////////////////
CFireArc & CFireArc::operator=(const CFireArc & rhs)
{
	if (this == &rhs)
		return *this;
	m_nMountPos		= rhs.m_nMountPos;
	m_nAngle		= rhs.m_nAngle;
	return *this;
}

///////////////////////////////////////////////////////////////////////
// Speichern / Laden
///////////////////////////////////////////////////////////////////////
void CFireArc::Serialize(CArchive &ar)
{
	CObject::Serialize(ar);
	// wenn gespeichert wird
	if (ar.IsStoring())
	{
		ar << m_nMountPos;
		ar << m_nAngle;
	}
	// wenn geladen wird
	else if (ar.IsLoading())
	{
		ar >> m_nMountPos;
		ar >> m_nAngle;
	}
}

///////////////////////////////////////////////////////////////////////
// sonstige Funktionen
///////////////////////////////////////////////////////////////////////
/// Funktion zum Setzen der Feuerwinkelwerte.
/// @param nMountPos Position am Schiff (0 entspricht Front, 90 entspricht rechts, 180 Heck und 270 links)
/// @param nAngle Öffnungswinkel in °
void CFireArc::SetValues(USHORT nMountPos, USHORT nAngle)
{
	m_nMountPos	= nMountPos;
	m_nAngle	= nAngle;

	AssertBotE(m_nMountPos <= 360);
	AssertBotE(m_nAngle <= 360);
}
