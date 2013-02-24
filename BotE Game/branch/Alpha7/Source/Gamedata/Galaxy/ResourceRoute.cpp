#include "stdafx.h"
#include "ResourceRoute.h"
#include "Sector.h"
#include "Races\Major.h"

IMPLEMENT_SERIAL (CResourceRoute, CObject, 1)
//////////////////////////////////////////////////////////////////////
// Konstruktion und Destruktion
//////////////////////////////////////////////////////////////////////
CResourceRoute::CResourceRoute(void) :
	m_byPercent(0),
	m_byResource(TITAN),
	m_KO(-1, -1)
{
}

CResourceRoute::~CResourceRoute(void)
{
}

//////////////////////////////////////////////////////////////////////
// Kopierkonstruktor
//////////////////////////////////////////////////////////////////////
CResourceRoute::CResourceRoute(const CResourceRoute &rhs)
{
	m_KO = rhs.m_KO;
	m_byPercent = rhs.m_byPercent;
	m_byResource = rhs.m_byResource;
}

//////////////////////////////////////////////////////////////////////
// Zuweisungsoperator
//////////////////////////////////////////////////////////////////////
CResourceRoute & CResourceRoute::operator=(const CResourceRoute & rhs)
{
	if (this == &rhs)
		return *this;
	m_KO = rhs.m_KO;
	m_byPercent = rhs.m_byPercent;
	m_byResource = rhs.m_byResource;
	return *this;
}

///////////////////////////////////////////////////////////////////////
// Speichern / Laden
///////////////////////////////////////////////////////////////////////
void CResourceRoute::Serialize(CArchive &ar)
{
	CObject::Serialize(ar);
	// wenn gespeichert wird
	if (ar.IsStoring())
	{
		ar << m_KO;
		ar << m_byPercent;
		ar << m_byResource;
	}
	// wenn geladen wird
	if (ar.IsLoading())
	{
		ar >> m_KO;
		ar >> m_byPercent;
		ar >> m_byResource;
	}
}

//////////////////////////////////////////////////////////////////////
// sonstige Funktionen
//////////////////////////////////////////////////////////////////////
/// Funktion überprüft, ob die Ressourcenroute noch Bestand haben darf.
BOOLEAN CResourceRoute::CheckResourceRoute(const CString& sOwner, CSector* dest)
{
	if (sOwner != dest->GetOwnerOfSector())
		return FALSE;

	// wenn keine Leute in dem System leben, so gibt es auch keine Route.
	float habitants = dest->GetCurrentHabitants();
	if (habitants == 0.0f)
		return FALSE;

	return TRUE;
}

// Funktion zeichnet die Ressourcensroute auf der Galaxiekarte. Übergeben werden dafür die Koordinate des
// Zielsystems <code>dest</code> sowie der Besitzer des Startsystems <code>owner</code>.
void CResourceRoute::DrawResourceRoute(CDC* pDC, CPoint dest, const CMajor* pMajor)
{
	COLORREF color = pMajor->GetDesign()->m_clrRouteColor;

	CPen pen(PS_SOLID, 0, color);
	pDC->SelectObject(&pen);
	// Zeichnung mit Hilfe von Bezierkurve und 4 Punkten:
	// 1. Punkt: Startpunkt
	// 2. Punkt: Mittlerer Punkt, der nicht auf der direkten Geraden zwischen 1. und 3. Punkt liegt -> berechnen
	// 3. Punkt: Mittlerer Punkt, der nicht auf der direkten Geraden zwischen 1. und 3. Punkt liegt -> berechnen
	// 4. Punkt: Zielpunkt
	CPoint p[4];
	p[0] = CPoint(m_KO.x*STARMAP_SECTOR_WIDTH+STARMAP_SECTOR_WIDTH/2,m_KO.y*STARMAP_SECTOR_HEIGHT+STARMAP_SECTOR_HEIGHT/2);
	p[3] = CPoint(dest.x*STARMAP_SECTOR_WIDTH+STARMAP_SECTOR_WIDTH/2,dest.y*STARMAP_SECTOR_HEIGHT+STARMAP_SECTOR_HEIGHT/2);

	// mittlere Punkte berechnen
	int x1 = p[0].x;
	int x2 = p[3].x;
	int y1 = p[0].y;
	int y2 = p[3].y;
	p[1].x = (x1 + x2) / 2;
	p[1].y = (y1 + y2) / 2;
	// mittlere Punkte leicht auf ihrer Ebene verschieben, damit eine Kurve entsteht
	p[1].x += (p[0].y - p[3].y) / 4;
	p[1].y += (p[0].x - p[3].x) / 4;
	p[2] = p[1];
	pDC->PolyBezier(p, 4);

//	pDC->Rectangle(p[0].x, p[0].y, p[0].x + 5, p[0].y + 5);
//	pDC->Rectangle(p[1].x, p[1].y, p[1].x + 5, p[1].y + 5);
//	pDC->Rectangle(p[2].x, p[2].y, p[2].x + 5, p[2].y + 5);
//	pDC->Rectangle(p[3].x, p[3].y, p[3].x + 5, p[3].y + 5);
//	pDC->MoveTo(m_KO.x*STARMAP_SECTOR_WIDTH+STARMAP_SECTOR_WIDTH/2,m_KO.y*STARMAP_SECTOR_HEIGHT+STARMAP_SECTOR_HEIGHT/2);
//	pDC->LineTo(dest.x*STARMAP_SECTOR_WIDTH+STARMAP_SECTOR_WIDTH/2,dest.y*STARMAP_SECTOR_HEIGHT+STARMAP_SECTOR_HEIGHT/2);

	pen.DeleteObject();
}
