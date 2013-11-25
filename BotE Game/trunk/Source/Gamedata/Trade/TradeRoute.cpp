#include "stdafx.h"
#include "TradeRoute.h"
#include "BotEDoc.h"
#include "Races\RaceController.h"


IMPLEMENT_SERIAL (CTradeRoute, CObject, 1)
//////////////////////////////////////////////////////////////////////
// Konstruktion und Destruktion
//////////////////////////////////////////////////////////////////////
CTradeRoute::CTradeRoute(void)
{
	m_KO = CPoint(0,0);
	m_iCredits = 0;
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
	m_iCredits = rhs.m_iCredits;
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
	m_iCredits = rhs.m_iCredits;
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
		ar << m_iCredits;
		ar << m_iDuration;
	}
	// wenn geladen wird
	if (ar.IsLoading())
	{
		ar >> m_KO;
		ar >> m_iCredits;
		ar >> m_iDuration;
	}
}

//////////////////////////////////////////////////////////////////////
// sonstige Funktionen
//////////////////////////////////////////////////////////////////////
/// Funktion legt die Handelsroute zum System mit der Koordinate <code>ko</code> an.
void CTradeRoute::GenerateTradeRoute(CPoint ko)
{
	AssertBotE(ko != CPoint(-1, -1));
	m_KO = ko;
	m_iDuration = 20;
}

/// Funktion gibt das Credits inkl. Boni, welches man durch die Handelsroute bekommt zurück
USHORT CTradeRoute::GetCredits(short boni) const
{
	return (USHORT)(m_iCredits + m_iCredits * boni/100);
}

/// Funktion überprüft, ob die Handelsroute noch Bestand haben darf und setzt die Credits, welche
/// diese Handelsroute fabriziert. Dabei werden noch keinerlei Boni auf die Creditsproduktion angerechnet.
/// Die Funktion gibt einen Wahrheitswert zurück, der sagt, ob die Handelsroute noch Bestand haben darf.
bool CTradeRoute::CheckTradeRoute(const CPoint& pFrom, const CPoint& pDest, CBotEDoc* pDoc)
{
	AssertBotE(pDoc);

	const CSystem* pDestSystem = &(pDoc->GetSystem(pDest.x, pDest.y));
	const CString&  sOwner = pDoc->GetSystem(pFrom.x, pFrom.y).OwnerID();

	// wurde der Zielsektor durch uns gescannt
	if (!pDestSystem->GetScanned(sOwner))
		return false;

	// zu allererst das Credits berechnen
	float habitants = pDestSystem->GetCurrentHabitants();
	// wenn keine Leute in dem System leben, so gibt es auch keine Handelsroute.
	if (habitants == 0.0f)
		return false;

	// zufällig wird das Credits bei der Handelsroute noch modifiziert (+-)20%
	// also (rand()%41 + 80) / 100 -> [0,40]+80 = [80,120] / 100 -> [0.8,1.2]
	float mod = (float)((float)(rand()%41 + 80) / 100);

	m_iCredits = (USHORT)(habitants * mod);
	// minimal wird 1 Credit zurückgegeben
	m_iCredits = max(m_iCredits, 1);

	// gehört der Zielsektor einer Majorrace und nicht uns?
	if (pDestSystem->Majorized() && pDestSystem->OwnerID() != sOwner)
	{
		if (pDestSystem->Owner()->GetAgreement(sOwner) >= DIPLOMATIC_AGREEMENT::TRADE)
			return true;
	}
	// gehört der Zielsektor einer Minorrace
	else if (!pDestSystem->Free() && pDestSystem->Owner()->IsMinor())
	{
		const RacePtr& race = pDestSystem->Owner();
		if (race->GetAgreement(sOwner) >= DIPLOMATIC_AGREEMENT::TRADE && race->GetAgreement(sOwner) != DIPLOMATIC_AGREEMENT::MEMBERSHIP)
			return true;
		else
			return false;
	}

	return false;
}

/// Diese Funktion verbessert manchmal die Beziehung zu der Minorrace, die in dem betroffenem Sektor lebt.
/// Als Parameter wird hierfür die Ausgangskoordinate der Handerlsroute, die Zielkoordinate <code>pDest</code>
/// und ein Zeiger auf das Dokument <code>pDoc</code> übergeben.
void CTradeRoute::PerhapsChangeRelationship(const CPoint& pFrom, const CPoint& pDest, CBotEDoc* pDoc)
{
	AssertBotE(pDoc);

	CSystem* pDestSector = &(pDoc->GetSystem(pDest.x, pDest.y));
	AssertBotE(pDestSector);

	const CString&  sOwner = pDoc->GetSystem(pFrom.x, pFrom.y).OwnerID();
	AssertBotE(!sOwner.IsEmpty());

	if (pDestSector->IndependentMinor())
	{
		AssertBotE(!pDestSector->Free());
		// Minorrace holen
		CMinor* pMinor = dynamic_cast<CMinor*>(pDestSector->Owner().get());
		AssertBotE(pMinor);
		short relAdd = rand()%(pMinor->GetCorruptibility()+1);
		/*CString s;
		s.Format("Verbesserung um %d Punkte",relAdd);
		AfxMessageBox(s);*/
		pMinor->SetRelation(sOwner, relAdd);
	}
}

/// Funktion zeichnet die Handelsroute auf der Galaxiekarte
void CTradeRoute::DrawTradeRoute(CDC* pDC, CPoint start, const CMajor* pMajor)
{
	COLORREF color = RGB(255,0,255);
	if (m_iDuration >= 0)
		color = pMajor->GetDesign()->m_clrRouteColor;

	CPen pen(PS_DOT, 0, color);
	pDC->SelectObject(&pen);
	pDC->MoveTo(start.x*STARMAP_SECTOR_WIDTH+STARMAP_SECTOR_WIDTH/2,start.y*STARMAP_SECTOR_HEIGHT+STARMAP_SECTOR_HEIGHT/2);
	pDC->LineTo(m_KO.x*STARMAP_SECTOR_WIDTH+STARMAP_SECTOR_WIDTH/2,m_KO.y*STARMAP_SECTOR_HEIGHT+STARMAP_SECTOR_HEIGHT/2);
	pen.DeleteObject();
}
