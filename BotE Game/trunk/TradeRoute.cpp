#include "stdafx.h"
#include "TradeRoute.h"
#include "MajorRace.h"

IMPLEMENT_SERIAL (CTradeRoute, CObject, 1)
//////////////////////////////////////////////////////////////////////
// Konstruktion und Destruktion
//////////////////////////////////////////////////////////////////////
CTradeRoute::CTradeRoute(void)
{
	m_KO = CPoint(0,0);
	m_iLatinum = 0;
	m_iDuration = 0;
}

CTradeRoute::~CTradeRoute(void)
{
}

//////////////////////////////////////////////////////////////////////
// Kopierkonstruktor
//////////////////////////////////////////////////////////////////////
CTradeRoute::CTradeRoute(const CTradeRoute &rhs)
{
	m_KO = rhs.m_KO;
	m_iLatinum = rhs.m_iLatinum;
	m_iDuration = rhs.m_iDuration;
}

//////////////////////////////////////////////////////////////////////
// Zuweisungsoperator
//////////////////////////////////////////////////////////////////////
CTradeRoute & CTradeRoute::operator=(const CTradeRoute & rhs)
{
	if (this == &rhs)
		return *this;
	m_KO = rhs.m_KO;
	m_iLatinum = rhs.m_iLatinum;
	m_iDuration = rhs.m_iDuration;
	return *this;
}

///////////////////////////////////////////////////////////////////////
// Speichern / Laden
///////////////////////////////////////////////////////////////////////
void CTradeRoute::Serialize(CArchive &ar)		
{
	CObject::Serialize(ar);
	// wenn gespeichert wird
	if (ar.IsStoring())
	{
		ar << m_KO;
		ar << m_iLatinum;
		ar << m_iDuration;
	}
	// wenn geladen wird
	if (ar.IsLoading())
	{
		ar >> m_KO;
		ar >> m_iLatinum;
		ar >> m_iDuration;
	}
}

//////////////////////////////////////////////////////////////////////
// sonstige Funktionen
//////////////////////////////////////////////////////////////////////
/// Funktion legt die Handelsroute zum System mit der Koordinate <code>ko</code> an.
void CTradeRoute::GenerateTradeRoute(CPoint ko)
{
	m_KO = ko;
	m_iDuration = 20;
}

/// Funktion gibt das Latinum inkl. Boni, welches man durch die Handelsroute bekommt zurück
USHORT CTradeRoute::GetLatinum(short boni) const
{
	return (USHORT)(m_iLatinum + m_iLatinum * boni/100);
}

/// Funktion überprüft, ob die Handelsroute noch Bestand haben darf und setzt das Latinum, welches
/// diese Handelsroute fabriziert. Dabei werden noch keinerlei Boni auf die Latinumproduktion angerechnet.
/// Die Funktion gibt einen Wahrheitswert zurück, der sagt, ob die Handelsroute noch Bestand haben darf.
BOOLEAN CTradeRoute::CheckTradeRoute(BYTE owner, CSector* dest, CMajorRace* majors, MinorRaceArray* minors)
{
	// wurde der Zielsektor durch uns gescannt
	if (dest->GetScanned(owner) == FALSE)
		return FALSE;	
	// zu allererst das Latinum berechnen
	float habitants = dest->GetCurrentHabitants();
	// wenn keine Leute in dem System leben, so gibt es auch keine Handelsroute.
	if (habitants == 0.0f)
		return FALSE;
	
	// zufällig wird das Latinum bei der Handelsroute noch modifiziert (+-)20%
	// also (rand()%41 + 80) / 100 -> [0,40]+80 = [80,120] / 100 -> [0.8,1.2]
	float mod = (float)((float)(rand()%41 + 80) / 100);

	m_iLatinum = (USHORT)(habitants * mod);
	// gehört der Zielsektor einer Majorrace und nicht uns?
	if (dest->GetOwnerOfSector() != NOBODY && dest->GetOwnerOfSector() != UNKNOWN && dest->GetOwnerOfSector() != owner)
	{
		if (majors[dest->GetOwnerOfSector()].GetDiplomacyStatus(owner) >= TRADE_AGREEMENT)
			return TRUE;
	}
	// gehört der Zielsektor einer Minorrace
	else if (dest->GetMinorRace() == TRUE)
	{
		// Minorrace in dem Array suchen
		int i;
		for (i = 0; i < minors->GetSize(); i++)
			if (minors->GetAt(i).GetRaceKO() == dest->GetKO())
			{
				if (minors->GetAt(i).GetDiplomacyStatus(owner) >= TRADE_AGREEMENT && minors->GetAt(i).GetDiplomacyStatus(owner) != MEMBERSHIP)
					return TRUE;
				else
					return FALSE;
			}
	}
	return FALSE;
}

/// Diese Funktion verbessert manchmal die Beziehung zu der Minorrace, die in dem betroffenem Sektor lebt.
/// Als Parameter wird hierfür der Besitzer der Handerlsroute, ein Zeiger auf den Sektor <code>dest</code>
/// und ein Zeiger auf das Feld mit allen Minorraces <code>minors</code> übergeben.
void CTradeRoute::PerhapsChangeRelationship(BYTE owner, const CSector* dest, MinorRaceArray* minors)
{
	if (dest->GetMinorRace() == TRUE)
	{
		// Minorrace in dem Array suchen
		int i;
		for (i = 0; i < minors->GetSize(); i++)
			if (minors->GetAt(i).GetRaceKO() == dest->GetKO())
				break;
		short relAdd = rand()%(minors->GetAt(i).GetCorruptibility()+1);
		if (dest->GetOwned())
			relAdd /= 2;
	/*	CString s;
		s.Format("Verbesserung um %d Punkte",relAdd);
		AfxMessageBox(s);*/
		minors->GetAt(i).SetRelationshipToMajorRace(owner, relAdd);
	}
}

/// Funktion zeichnet die Handelsroute auf der Galaxiekarte
void CTradeRoute::DrawTradeRoute(CDC* pDC, CPoint start, BYTE owner)
{
	COLORREF color;
	if (m_iDuration >= 0)
	{
		switch (owner)
		{
		case HUMAN:		color = RGB(50,150,255); break;
		case FERENGI:	color = RGB(155,155,0); break;
		case KLINGON:	color = RGB(165,0,0);	  break;
		case ROMULAN:	color = RGB(0,125,0); break;
		case CARDASSIAN:color = RGB(130,0,130); break;
		case DOMINION:	color = RGB(80,195,245); break;
		default: color = RGB(255,255,255);
		}
	}
	else
		color = RGB(255,0,255);
	CPen pen(PS_DOT, 0, color);
	pDC->SelectObject(&pen);
	pDC->MoveTo(start.x*STARMAP_SECTOR_WIDTH+STARMAP_SECTOR_WIDTH/2,start.y*STARMAP_SECTOR_HEIGHT+STARMAP_SECTOR_HEIGHT/2);
	pDC->LineTo(m_KO.x*STARMAP_SECTOR_WIDTH+STARMAP_SECTOR_WIDTH/2,m_KO.y*STARMAP_SECTOR_HEIGHT+STARMAP_SECTOR_HEIGHT/2);	
}