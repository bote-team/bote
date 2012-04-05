// Starmap.cpp: implementation of the CStarmap class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Starmap.h"
#include "botf2Doc.h"
#include "Galaxy\Sector.h"
#include "Galaxy\Anomaly.h"
#include <math.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


// statische Variablen initialisieren
double** CStarmap::m_BadMapModifiers = NULL;//[STARMAP_SECTORS_HCOUNT][STARMAP_SECTORS_VCOUNT];


/**
 * @return <code>-1</code> wenn <code>x &lt; 0</code>, <code>0</code> wenn <code>x == 0</code>, <code>1</code> wenn <code>x &gt; 0</code>
 */
inline int sgn(int x)
{
	if (x == 0)
		return 0;
	else
		return x / abs(x);
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CStarmap::CStarmap(BOOL bAICalculation, char nAIRange) : m_bAICalculation(bAICalculation), m_nAIRange(nAIRange)
{

	if(!m_BadMapModifiers) {
		m_BadMapModifiers=new double*[STARMAP_SECTORS_HCOUNT];
		for(int i=0;i<STARMAP_SECTORS_HCOUNT;i++)
			m_BadMapModifiers[i]=new double[STARMAP_SECTORS_VCOUNT];
		for(int i=0;i<STARMAP_SECTORS_HCOUNT;i++)
			for(int j=0;j<STARMAP_SECTORS_VCOUNT;j++)
				m_BadMapModifiers[i][j]=0;
	}

	ASSERT(nAIRange >= SM_RANGE_FAR && nAIRange <= SM_RANGE_NEAR);

	// KI Berechnung mal auf immer TRUE gestellt. So wird bei einer Spielerrasse auch berechnet, aber nicht angewandt
	m_bAICalculation = TRUE;

	// m_Range komplett mit RANGE_SPACE füllen
	m_Range=new unsigned char*[STARMAP_SECTORS_HCOUNT];
	for(int i=0;i<STARMAP_SECTORS_HCOUNT;i++)
		m_Range[i]=new unsigned char[STARMAP_SECTORS_VCOUNT];
	for(int i=0;i<STARMAP_SECTORS_HCOUNT;i++)
		for(int j=0;j<STARMAP_SECTORS_VCOUNT;j++)
			m_Range[i][j]=SM_RANGE_SPACE;

	//memset(m_Range, SM_RANGE_SPACE, STARMAP_SECTORS_HCOUNT * STARMAP_SECTORS_VCOUNT * sizeof(unsigned char));

	// Standard-Rangemap
	m_RangeMap.range = new unsigned char [49];
	m_RangeMap.w = m_RangeMap.h = 7;
	m_RangeMap.x0 = m_RangeMap.y0 = 3;

	unsigned char rangeMap[] = 
		{0, 0, 1, 1, 1, 0, 0,
		 0, 1, 2, 2, 2, 1, 0,
		 1, 2, 3, 3, 3, 2, 1,
		 1, 2, 3, 3, 3, 2, 1,
		 1, 2, 3, 3, 3, 2, 1,
		 0, 1, 2, 2, 2, 1, 0,
		 0, 0, 1, 1, 1, 0, 0};

	memcpy(m_RangeMap.range, rangeMap, 49 * sizeof(unsigned char));

	// Bewertungen für Sektoren initialisieren
	m_AINeighbourCount=new unsigned char*[STARMAP_SECTORS_HCOUNT];
	for(int i=0;i<STARMAP_SECTORS_HCOUNT;i++)
		m_AINeighbourCount[i]=new unsigned char[STARMAP_SECTORS_VCOUNT];
	for(int i=0;i<STARMAP_SECTORS_HCOUNT;i++)
		for(int j=0;j<STARMAP_SECTORS_VCOUNT;j++)
			m_AINeighbourCount[i][j]=0;
	//memset(m_AINeighbourCount, 0, STARMAP_SECTORS_HCOUNT * STARMAP_SECTORS_VCOUNT * sizeof(unsigned char));

	m_AIRangePoints=new short*[STARMAP_SECTORS_HCOUNT];
	for(int i=0;i<STARMAP_SECTORS_HCOUNT;i++)
		m_AIRangePoints[i]=new short[STARMAP_SECTORS_VCOUNT];
	for(int i=0;i<STARMAP_SECTORS_HCOUNT;i++)
		for(int j=0;j<STARMAP_SECTORS_VCOUNT;j++)
			m_AIRangePoints[i][j]=0;
	//memset(m_AIRangePoints, 0, STARMAP_SECTORS_HCOUNT * STARMAP_SECTORS_VCOUNT * sizeof(short));

	m_AIConnectionPoints=new short*[STARMAP_SECTORS_HCOUNT];
	for(int i=0;i<STARMAP_SECTORS_HCOUNT;i++)
		m_AIConnectionPoints[i]=new short[STARMAP_SECTORS_VCOUNT];
	for(int i=0;i<STARMAP_SECTORS_HCOUNT;i++)
		for(int j=0;j<STARMAP_SECTORS_VCOUNT;j++)
			m_AIConnectionPoints[i][j]=0;
	//memset(m_AIConnectionPoints, 0, STARMAP_SECTORS_HCOUNT * STARMAP_SECTORS_VCOUNT * sizeof(short));

	m_AITargetPoints=new short*[STARMAP_SECTORS_HCOUNT];
	for(int i=0;i<STARMAP_SECTORS_HCOUNT;i++)
		m_AITargetPoints[i]=new short[STARMAP_SECTORS_VCOUNT];
	for(int i=0;i<STARMAP_SECTORS_HCOUNT;i++)
		for(int j=0;j<STARMAP_SECTORS_VCOUNT;j++)
			m_AITargetPoints[i][j]=0;
	//memset(m_AITargetPoints, 0, STARMAP_SECTORS_HCOUNT * STARMAP_SECTORS_VCOUNT * sizeof(short));

	m_AIBadPoints=new short*[STARMAP_SECTORS_HCOUNT];
	for(int i=0;i<STARMAP_SECTORS_HCOUNT;i++)
		m_AIBadPoints[i]=new short[STARMAP_SECTORS_VCOUNT];
	for(int i=0;i<STARMAP_SECTORS_HCOUNT;i++)
		for(int j=0;j<STARMAP_SECTORS_VCOUNT;j++)
			m_AIBadPoints[i][j]=0;
	//memset(m_AIBadPoints, 0, STARMAP_SECTORS_HCOUNT * STARMAP_SECTORS_VCOUNT * sizeof(short));
	
	pathMap=new PathSector*[STARMAP_SECTORS_HCOUNT];
	for(int i=0;i<STARMAP_SECTORS_HCOUNT;i++)
		pathMap[i]=new PathSector[STARMAP_SECTORS_VCOUNT];

}

CStarmap::~CStarmap()
{
	if (m_RangeMap.range) delete[] m_RangeMap.range;

	for (int x = 0; x < STARMAP_SECTORS_HCOUNT; x++)
	{
		delete[] pathMap[x];
		delete[] m_AIBadPoints[x];
		delete[] m_AITargetPoints[x];
		delete[] m_AIConnectionPoints[x];
		delete[] m_AIRangePoints[x];
		delete[] m_AINeighbourCount[x];
		delete[] m_Range[x];
	}

	delete[] pathMap;
	delete[] m_AITargetPoints;
	delete[] m_AIBadPoints;
	delete[] m_AIConnectionPoints;
	delete[] m_AIRangePoints;
	delete[] m_AINeighbourCount;
	delete[] m_Range;
}

void CStarmap::DeleteStatics()
{
	for (int x = 0; x < STARMAP_SECTORS_HCOUNT; x++)
	{
		if (m_BadMapModifiers)
		{
			delete[] m_BadMapModifiers[x];
			m_BadMapModifiers[x] = NULL;
		}
	}

	if (m_BadMapModifiers)
	{
		delete[] m_BadMapModifiers;
		m_BadMapModifiers = NULL;
	}
}

void CStarmap::SetRangeMap(unsigned char rangeMap[], char w, char h, char x0, char y0)
{
	ASSERT(w > 0 && h > 0 && 0 <= x0 && x0 < w && 0 <= y0 && y0 < h);

	if (m_RangeMap.range) delete[] m_RangeMap.range;

	m_RangeMap.range = new unsigned char [w * h];
	memcpy(m_RangeMap.range, rangeMap, w * h * sizeof(unsigned char));
	m_RangeMap.w = w;
	m_RangeMap.h = h;
	m_RangeMap.x0 = x0;
	m_RangeMap.y0 = y0;
}

unsigned char CStarmap::GetRangeMapValue(char x, char y)
{
	x += m_RangeMap.x0;
	y += m_RangeMap.y0;
	ASSERT(0 <= x && x < m_RangeMap.w && 0 <= y && y < m_RangeMap.h);
	return m_RangeMap.range[y * m_RangeMap.w + x];
}

Sector CStarmap::GetClickedSector(const CPoint &pt) const
{
	Sector result;
	result.x = result.y = -1;

	// wenn innerhalb der Karte geklickt, dann Koordinaten umrechnen
	if (PT_IN_RECT(pt, 0, 0, STARMAP_TOTALWIDTH, STARMAP_TOTALHEIGHT))
	{
		result.x = (char)(pt.x / STARMAP_SECTOR_WIDTH);
		result.y = (char)(pt.y / STARMAP_SECTOR_HEIGHT);
		ASSERT_SECTOR_VALID(result);
	}

	return result;
}

void CStarmap::Select(const Sector &sector)
{
	if (sector.x > -1 && sector.y > -1)
	{
		ASSERT_SECTOR_VALID(sector);
		m_Selection = sector;
	}
}

void CStarmap::Deselect()
{
	m_Selection.x = m_Selection.y = -1;
}

CPoint CStarmap::GetSectorCoords(const Sector& sector) const
{
	ASSERT_SECTOR_VALID(sector);
	return CPoint(sector.x * STARMAP_SECTOR_WIDTH, sector.y * STARMAP_SECTOR_HEIGHT);
}

BOOL CStarmap::IsBase(const Sector &sector)
{
	for (SECTORLIST::const_iterator it = m_lBases.begin(); it != m_lBases.end(); ++it)
		if (it->x == sector.x && it->y == sector.y)
			return TRUE;
	return FALSE;
}

void CStarmap::SetFullRangeMap(int nRange/* = SM_RANGE_NEAR*/)
{
	for (int x = 0; x < STARMAP_SECTORS_HCOUNT; x++)
		for (int y = 0; y < STARMAP_SECTORS_VCOUNT; y++)
			m_Range[x][y] = nRange;
}

void CStarmap::AddBase(const Sector &sector, BYTE propTech)
{
	ASSERT_SECTOR_VALID(sector);

	// merken, dass Sektor einen Außenposten besitzt; falls der Außenposten schon vorhanden ist, die folgende
	// Berechnung trotzdem durchführen, da eine andere <code>rangeMap</code> vorgegeben sein könnte.
	if (!IsBase(sector)) m_lBases.push_back(sector);

	// --- Map mit Entfernungen aktualisieren ---
	this->CalcRangeMap(propTech);

	// lokale Rangemap durchlaufen
	for (char x = -m_RangeMap.x0; x < m_RangeMap.w - m_RangeMap.x0; x++)
		for (char y = -m_RangeMap.y0; y < m_RangeMap.h - m_RangeMap.y0; y++)
		{
			Sector pt(sector.x + x, sector.y + y);
			if (PT_IN_RECT(pt, 0, 0, STARMAP_SECTORS_HCOUNT, STARMAP_SECTORS_VCOUNT))
			{
				// Wert überschreiben, wenn der neue Einfluss größer ist
				m_Range[pt.x][pt.y] = max(m_Range[pt.x][pt.y], GetRangeMapValue(x, y));
			}
		}

	// pathStart zurücksetzen, damit sämtliche Wege beim nächsten Aufruf von
	// CalcPath() neu berechnet werden
	pathStart = Sector();

#ifdef DEBUG_AI_BASE_DEMO
	// ACHTUNG: verwenden i. A. falsche RangeMap!
	RecalcRangePoints();
	RecalcConnectionPoints();
	RecalcTargetPoints();
#endif
}

// Gleicht die komplette Rangemap mit der Galaxiemap ab. Gebiete, welche einer Rasse gehören mit der wir einen
// Nichtangriffspakt haben, werden von der Rangemap entfernt. Übergeben werden dafür ein Zeiger auf alle
// Sektoren <code>sectors</code> und ein Wahrheitswert <code>races</code> für alle Rassen, ob wir einen
// Nichtangriffspakt mit dieser Rasse haben.
void CStarmap::SynchronizeWithMap(CSector** sectors/*[][STARMAP_SECTORS_VCOUNT]*/, std::set<CString>* races)
{
	for (int y = 0; y < STARMAP_SECTORS_VCOUNT; y++)
		for (int x = 0; x < STARMAP_SECTORS_HCOUNT; x++)
			if (sectors[x][y].GetOwnerOfSector() != "")
				if (m_Range[x][y] > 0 && races->find(sectors[x][y].GetOwnerOfSector()) != races->end())
					m_Range[x][y] = 0;
}

// Führt für gefährliche Anomalien mathematische Gewichte hinzu, so dass dieser Sektor bei der automatischen
// Wegsuche nicht überflogen wird. Außerdem wird solch ein Sektor auch nicht für einen Außenpostenbau bestimmt.
void CStarmap::SynchronizeWithAnomalies(CSector** sectors/*[][STARMAP_SECTORS_VCOUNT]*/)
{
	for (int y = 0; y < STARMAP_SECTORS_VCOUNT; y++)
		for (int x = 0; x < STARMAP_SECTORS_HCOUNT; x++)
			if (sectors[x][y].GetAnomaly())
				m_BadMapModifiers[x][y] = sectors[x][y].GetAnomaly()->GetWaySearchWeight();		
}

void CStarmap::ClearAll()
{
	// m_Range komplett mit RANGE_SPACE füllen
	//memset(m_Range, SM_RANGE_SPACE, STARMAP_SECTORS_HCOUNT * STARMAP_SECTORS_VCOUNT * sizeof(unsigned char));
	for(int i=0;i<STARMAP_SECTORS_HCOUNT;i++)
		for(int j=0;j<STARMAP_SECTORS_VCOUNT;j++)
			m_Range[i][j]=SM_RANGE_SPACE;
	m_lBases.clear();

	// Bewertungen für Sektoren initialisieren
	//memset(m_AINeighbourCount, 0, STARMAP_SECTORS_HCOUNT * STARMAP_SECTORS_VCOUNT * sizeof(unsigned char));
	for(int i=0;i<STARMAP_SECTORS_HCOUNT;i++)
		for(int j=0;j<STARMAP_SECTORS_VCOUNT;j++)
			m_AINeighbourCount[i][j]=0;

	//memset(m_AIRangePoints, 0, STARMAP_SECTORS_HCOUNT * STARMAP_SECTORS_VCOUNT * sizeof(short));
	for(int i=0;i<STARMAP_SECTORS_HCOUNT;i++)
		for(int j=0;j<STARMAP_SECTORS_VCOUNT;j++)
			m_AIRangePoints[i][j]=0;
	
	
	//memset(m_AIConnectionPoints, 0, STARMAP_SECTORS_HCOUNT * STARMAP_SECTORS_VCOUNT * sizeof(short));
	for(int i=0;i<STARMAP_SECTORS_HCOUNT;i++)
		for(int j=0;j<STARMAP_SECTORS_VCOUNT;j++)
			m_AIConnectionPoints[i][j]=0;

	//memset(m_AITargetPoints, 0, STARMAP_SECTORS_HCOUNT * STARMAP_SECTORS_VCOUNT * sizeof(short));
	for(int i=0;i<STARMAP_SECTORS_HCOUNT;i++)
		for(int j=0;j<STARMAP_SECTORS_VCOUNT;j++)
			m_AITargetPoints[i][j]=0;

	//memset(m_AIBadPoints, 0, STARMAP_SECTORS_HCOUNT * STARMAP_SECTORS_VCOUNT * sizeof(short));
	for(int i=0;i<STARMAP_SECTORS_HCOUNT;i++)
		for(int j=0;j<STARMAP_SECTORS_VCOUNT;j++)
			m_AIBadPoints[i][j]=0;

	m_lAIKnownSystems.clear();
	m_lAITargets.clear();
}

// Kantengewichte; das diagonale Kantengewicht muss höher sein, da ein Weg sonst
// statt geradeaus auch im Zickzack verlaufen könnte
//#define WEIGHT_DIAG 1.41421356	// diagonal	(original von CBot)
#define WEIGHT_DIAG 1.4143			// diagonal (modded bei Sir Pustekuchen)
#define WEIGHT_DIR  1.				// horizontal, vertikal

Sector CStarmap::CalcPath(const Sector &pos, const Sector &target, unsigned char range,
	unsigned char speed, CArray<Sector> &path)
{
	ASSERT_SECTOR_VALID(pos);
	ASSERT_SECTOR_VALID(target);

	// bisherige Einträge von path löschen
	path.RemoveAll();

	// gegebene Parameter prüfen
	if (pos == target								// Start == Ziel
		|| range < 1 || range > 3
		|| m_Range[pos.x][pos.y] < range			// Start außerhalb des Gebiets der Reichweite
		|| m_Range[target.x][target.y] < range		// Ziel außerhalb der Reichweite
		|| speed < 1)
	{
		return Sector();
	}

	// Array zur Berechnung der Koordinaten sämtlicher Nachbarn eines Sektors (schräg/gerade abwechselnd,
	// mit schräg beginnend)
	Sector neighbours[8] = {Sector(-1, -1), Sector(0, -1), Sector(1, -1), Sector(1, 0), Sector(1, 1),
		Sector(0, 1), Sector(-1, 1), Sector(-1, 0)};

	// Berechnung neu beginnen?
	if (pos != pathStart || range != pathRange)
	{
		// pathMap zurücksetzen
		for (int j = 0; j < STARMAP_SECTORS_VCOUNT; j++)
			for (int i = 0; i < STARMAP_SECTORS_HCOUNT; i++)
			{
				/*PathSector *tmp = &(pathMap[i][j]);
				tmp->used = false;
				tmp->distance = 0.;
				tmp->hops = 0;
				tmp->parent.x = tmp->parent.y = -1;
				
				tmp->position.x = i; // für Zugriff aus leafList heraus merken
				tmp->position.y = j;*/
				pathMap[i][j].used=false;
				pathMap[i][j].distance=0;
				pathMap[i][j].hops=0;
				pathMap[i][j].parent.x=-1;
				pathMap[i][j].parent.y=-1;
				pathMap[i][j].position.x=i;
				pathMap[i][j].position.y=j;
			}

		// leaves zurücksetzen
		leaves.Clear();

		// Startknoten zur Liste der auszuwählenden Blätter hinzufügen
		leaves.Add(&(pathMap[pos.x][pos.y]));

		// Parameter merken
		pathStart = pos;
		pathRange = range;
	}

	// ist der Weg zum angegebenen Ziel bereits bekannt?
	if (pathMap[target.x][target.y].parent.x == -1 || pathMap[target.x][target.y].parent.y == -1)
	{
		// kürzeste Wege zu allen anderen Knoten bestimmen, bis uns der Zielknoten über den Weg läuft
		bool found = false;
		while (!found)
		{
			// Zeiger auf ein neues Blatt mit einer kürzesten Gesamtentfernung zum
			// Start-Sektor ermitteln
			PathSector *next = leaves.PopFirst();
			if (!next) return Sector(); // keine Knoten mehr, Zielknoten ist nicht erreichbar
			if (next->used) continue; // Knoten wurde schonmal gewählt

			// Knoten als ausgewählt markieren
			next->used = true;

			// bisher noch nicht ausgewählte Nachbarn innerhalb der Reichweite in leaves
			// eintragen;
			// die Nachbarn müssen auch eingetragen werden, wenn next bereits der Zielknoten ist,
			// da der nächste Aufruf von CalcPath() die Zwischenergebnisse wiederverwendet
			for (int i = 0; i < 8; i++)
			{
				// Koordinaten des Nachbarn ermitteln (Sektoren nur betrachten, wenn sie
				// noch auf der Starmap liegen!)
				Sector npos = next->position + neighbours[i];
				if (!PT_IN_RECT(npos, 0, 0, STARMAP_SECTORS_HCOUNT, STARMAP_SECTORS_VCOUNT))
					continue;

				// nur Nachbarn betrachten, die noch nicht ausgewählt wurden und innerhalb der
				// Reichweite liegen
				PathSector *neighb = &(pathMap[npos.x][npos.y]);
				if (neighb->used || m_Range[npos.x][npos.y] < range)
					continue;

				// kann der Nachbar über next auf einem kürzeren Weg als bisher erreicht werden,
				// dann die bisherige Info überschreiben
				double distance = next->distance + ((i % 2) ? WEIGHT_DIR : WEIGHT_DIAG);
				// Anomalien beachten
				distance += m_BadMapModifiers[next->position.x][next->position.y];				
				
				if (neighb->distance == 0. || distance < neighb->distance)
				{
					// (distance ist für alle anderen Sektoren außer dem Start-Sektor > 0.,
					// der Wert 0. weist darauf hin, dass distance noch nicht gesetzt wurde)

					neighb->distance = distance;
					neighb->hops = next->hops + 1;
					neighb->parent = next->position;

					// den Knoten in leaves neu einsortieren (derselbe Knoten ist evtl. unter
					// einer anderen Entfernung früher bereits einsortiert worden; da nun die
					// Entfernung aber kürzer ist, wird das neu einsortierte Element zuerst
					// gewählt; liefert die Liste eines der vorher eingeordneten Elemente, ist
					// dessen used-Feld bereits true und es wird sofort mit dem nächsten Eintrag
					// fortgesetzt);
					// @TODO besser wäre es, den früher einsortierten Knoten zu entfernen
					leaves.Add(neighb);
				}
			}

			if (next->position == target) found = true; // Zielknoten gefunden
		}
	}

	// Ziel gefunden; Weg vom Ziel bis zum Startknoten zurück verfolgen,
	// dabei von hinten beginnend in Array eintragen
	Sector next = target;
	int idx = pathMap[target.x][target.y].hops;
	ASSERT(idx >= 1);

	path.SetSize(idx); // Größe des Arrays setzen (= Länge des Weges)

	while (next.x > -1 && next.y > -1 && --idx >= 0) // Start-Sektor nicht mit eintragen
	{
		ASSERT_SECTOR_VALID(next);
		path[idx] = next;
		next = pathMap[next.x][next.y].parent;
	}
	ASSERT(idx == -1);	

	// entsprechend speed den nächsten Knoten des Weges zurückgeben; bzw. den Zielknoten,
	// wenn der Weg kürzer ist
	return path[min(speed - 1, path.GetUpperBound())];
}

////////////////////////////////////////////////////////////////////////////
// private Funktionen
////////////////////////////////////////////////////////////////////////////

/// Diese Funktion berechnet die Reichweitenkarte anhand der aktuellen Techstufe <code>propTech</code> und schreibt
/// das Ergebnis in den Parameter <code>rangeMap</code>. Zusätzlich werden Referenzen auf paar Hilfsvariablen
/// übergeben.
void CStarmap::CalcRangeMap(BYTE propTech)
{
	int w = 0, h = 0, x0 = 0, y0 = 0;
	switch (propTech)
	{
	case 0: {
		unsigned char tmpRangeMap[] = 
			{0, 0, 1, 1, 1, 0, 0,
			0, 1, 2, 2, 2, 1, 0,
			1, 2, 3, 3, 3, 2, 1,
			1, 2, 3, 3, 3, 2, 1,
			1, 2, 3, 3, 3, 2, 1,
			0, 1, 2, 2, 2, 1, 0,
			0, 0, 1, 1, 1, 0, 0};
			w = 7; h = 7; x0 = 3; y0 = 3;
			this->SetRangeMap(tmpRangeMap, w, h, x0, y0);
			break;}
	case 1: {
		unsigned char tmpRangeMap[] = 
			{0, 0, 1, 1, 1, 0, 0,
			0, 1, 2, 2, 2, 1, 0,
			1, 2, 3, 3, 3, 2, 1,
			1, 2, 3, 3, 3, 2, 1,
			1, 2, 3, 3, 3, 2, 1,
			0, 1, 2, 2, 2, 1, 0,
			0, 0, 1, 1, 1, 0, 0};
			w = 7; h = 7; x0 = 3; y0 = 3;
			this->SetRangeMap(tmpRangeMap, w, h, x0, y0);
			break;}
	case 2: {
		unsigned char tmpRangeMap[] = 
			{0, 0, 0, 1, 1, 1, 0, 0, 0,
			0, 0, 1, 1, 1, 1, 1, 0, 0,
			0, 1, 1, 2, 2, 2, 1, 1, 0,
			1, 1, 2, 3, 3, 3, 2, 1, 1,
			1, 1, 2, 3, 3, 3, 2, 1, 1,
			1, 1, 2, 3, 3, 3, 2, 1, 1,
			0, 1, 1, 2, 2, 2, 1, 1, 0,
			0, 0, 1, 1, 1, 1, 1, 0, 0,
			0, 0, 0, 1, 1, 1, 0, 0, 0};
			w = 9, h = 9, x0 = 4, y0 = 4;
			this->SetRangeMap(tmpRangeMap, w, h, x0, y0);
			break;}
	case 3: {
		unsigned char tmpRangeMap[] = 
			{0, 0, 0, 1, 1, 1, 0, 0, 0,
			0, 0, 1, 2, 2, 2, 1, 0, 0,
			0, 1, 2, 2, 2, 2, 2, 1, 0,
			1, 2, 2, 3, 3, 3, 2, 2, 1,
			1, 2, 2, 3, 3, 3, 2, 2, 1,
			1, 2, 2, 3, 3, 3, 2, 2, 1,
			0, 1, 2, 2, 2, 2, 2, 1, 0,
			0, 0, 1, 2, 2, 2, 1, 0, 0,
			0, 0, 0, 1, 1, 1, 0, 0, 0};
			w = 9, h = 9, x0 = 4, y0 = 4;
			this->SetRangeMap(tmpRangeMap, w, h, x0, y0);
			break;}
	case 4: {
		unsigned char tmpRangeMap[] = 
			{0, 0, 0, 1, 1, 1, 0, 0, 0,
			0, 0, 1, 2, 2, 2, 1, 0, 0,
			0, 1, 2, 3, 3, 3, 2, 1, 0,
			1, 2, 3, 3, 3, 3, 3, 2, 1,
			1, 2, 3, 3, 3, 3, 3, 2, 1,
			1, 2, 3, 3, 3, 3, 3, 2, 1,
			0, 1, 2, 3, 3, 3, 2, 1, 0,
			0, 0, 1, 2, 2, 2, 1, 0, 0,
			0, 0, 0, 1, 1, 1, 0, 0, 0};
			w = 9, h = 9, x0 = 4, y0 = 4;
			this->SetRangeMap(tmpRangeMap, w, h, x0, y0);
			break;}
	case 5: {
		unsigned char tmpRangeMap[] = 
			{0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0,
			0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0,
			0, 0, 1, 1, 2, 2, 2, 1, 1, 0, 0,
			0, 1, 1, 2, 3, 3, 3, 2, 1, 1, 0,
			1, 1, 2, 3, 3, 3, 3, 3, 2, 1, 1,
			1, 1, 2, 3, 3, 3, 3, 3, 2, 1, 1,
			1, 1, 2, 3, 3, 3, 3, 3, 2, 1, 1,
			0, 1, 1, 2, 3, 3, 3, 2, 1, 1, 0,
			0, 0, 1, 1, 2, 2, 2, 1, 1, 0, 0,
			0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0,
			0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0};
			w = 11, h = 11, x0 = 5, y0 = 5;
			this->SetRangeMap(tmpRangeMap, w, h, x0, y0);
			break;}
	case 6: {
		unsigned char tmpRangeMap[] = 
			{0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0,
			0, 0, 0, 1, 2, 2, 2, 1, 0, 0, 0,
			0, 0, 1, 2, 2, 2, 2, 2, 1, 0, 0,
			0, 1, 2, 2, 3, 3, 3, 2, 2, 1, 0,
			1, 2, 2, 3, 3, 3, 3, 3, 2, 2, 1,
			1, 2, 2, 3, 3, 3, 3, 3, 2, 2, 1,
			1, 2, 2, 3, 3, 3, 3, 3, 2, 2, 1,
			0, 1, 2, 2, 3, 3, 3, 2, 2, 1, 0,
			0, 0, 1, 2, 2, 2, 2, 2, 1, 0, 0,
			0, 0, 0, 1, 2, 2, 2, 1, 0, 0, 0,
			0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0};
			w = 11, h = 11, x0 = 5, y0 = 5;
			this->SetRangeMap(tmpRangeMap, w, h, x0, y0);
			break;}
	case 7: {
		unsigned char tmpRangeMap[] = 
			{0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0,
			0, 0, 0, 1, 2, 2, 2, 1, 0, 0, 0,
			0, 0, 1, 2, 3, 3, 3, 2, 1, 0, 0,
			0, 1, 2, 3, 3, 3, 3, 3, 2, 1, 0,
			1, 2, 3, 3, 3, 3, 3, 3, 3, 2, 1,
			1, 2, 3, 3, 3, 3, 3, 3, 3, 2, 1,
			1, 2, 3, 3, 3, 3, 3, 3, 3, 2, 1,
			0, 1, 2, 3, 3, 3, 3, 3, 2, 1, 0,
			0, 0, 1, 2, 3, 3, 3, 2, 1, 0, 0,
			0, 0, 0, 1, 2, 2, 2, 1, 0, 0, 0,
			0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0};
			w = 11, h = 11, x0 = 5, y0 = 5;
			this->SetRangeMap(tmpRangeMap, w, h, x0, y0);
			break;}
	case 8: {
		unsigned char tmpRangeMap[] = 
			{0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0,
			0, 0, 0, 1, 1, 2, 2, 2, 1, 1, 0, 0, 0,
			0, 0, 1, 1, 2, 3, 3, 3, 2, 1, 1, 0, 0,
			0, 1, 1, 2, 3, 3, 3, 3, 3, 2, 1, 1, 0,
			1, 1, 2, 3, 3, 3, 3, 3, 3, 3, 2, 1, 1,
			1, 1, 2, 3, 3, 3, 3, 3, 3, 3, 2, 1, 1,
			1, 1, 2, 3, 3, 3, 3, 3, 3, 3, 2, 1, 1,
			0, 1, 1, 2, 3, 3, 3, 3, 3, 2, 1, 1, 0,
			0, 0, 1, 1, 2, 3, 3, 3, 2, 1, 1, 0, 0,
			0, 0, 0, 1, 1, 2, 2, 2, 1, 1, 0, 0, 0,
			0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0};
			w = 13, h = 13, x0 = 6, y0 = 6;
			this->SetRangeMap(tmpRangeMap, w, h, x0, y0);
			break;}
	case 9: {
		unsigned char tmpRangeMap[] = 
			{0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 1, 2, 2, 2, 1, 0, 0, 0, 0,
			0, 0, 0, 1, 2, 2, 2, 2, 2, 1, 0, 0, 0,
			0, 0, 1, 2, 2, 3, 3, 3, 2, 2, 1, 0, 0,
			0, 1, 2, 2, 3, 3, 3, 3, 3, 2, 2, 1, 0,
			1, 2, 2, 3, 3, 3, 3, 3, 3, 3, 2, 2, 1,
			1, 2, 2, 3, 3, 3, 3, 3, 3, 3, 2, 2, 1,
			1, 2, 2, 3, 3, 3, 3, 3, 3, 3, 2, 2, 1,
			0, 1, 2, 2, 3, 3, 3, 3, 3, 2, 2, 1, 0,
			0, 0, 1, 2, 2, 3, 3, 3, 2, 2, 1, 0, 0,
			0, 0, 0, 1, 2, 2, 2, 2, 2, 1, 0, 0, 0,
			0, 0, 0, 0, 1, 2, 2, 2, 1, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0};
			w = 13, h = 13, x0 = 6, y0 = 6;
			this->SetRangeMap(tmpRangeMap, w, h, x0, y0);
			break;}
	case 10: {
		unsigned char tmpRangeMap[] = 
			{0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 1, 2, 2, 2, 1, 0, 0, 0, 0,
			0, 0, 0, 1, 2, 3, 3, 3, 2, 1, 0, 0, 0,
			0, 0, 1, 2, 3, 3, 3, 3, 3, 2, 1, 0, 0,
			0, 1, 2, 3, 3, 3, 3, 3, 3, 3, 2, 1, 0,
			1, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2, 1,
			1, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2, 1,
			1, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2, 1,
			0, 1, 2, 3, 3, 3, 3, 3, 3, 3, 2, 1, 0,
			0, 0, 1, 2, 3, 3, 3, 3, 3, 2, 1, 0, 0,
			0, 0, 0, 1, 2, 3, 3, 3, 2, 1, 0, 0, 0,
			0, 0, 0, 0, 1, 2, 2, 2, 1, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0};
			w = 13, h = 13, x0 = 6, y0 = 6;
			this->SetRangeMap(tmpRangeMap, w, h, x0, y0);
			break;}
	 default: {
		unsigned char tmpRangeMap[] = 
			{0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 1, 2, 2, 2, 1, 0, 0, 0, 0,
			0, 0, 0, 1, 2, 3, 3, 3, 2, 1, 0, 0, 0,
			0, 0, 1, 2, 3, 3, 3, 3, 3, 2, 1, 0, 0,
			0, 1, 2, 3, 3, 3, 3, 3, 3, 3, 2, 1, 0,
			1, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2, 1,
			1, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2, 1,
			1, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2, 1,
			0, 1, 2, 3, 3, 3, 3, 3, 3, 3, 2, 1, 0,
			0, 0, 1, 2, 3, 3, 3, 3, 3, 2, 1, 0, 0,
			0, 0, 0, 1, 2, 3, 3, 3, 2, 1, 0, 0, 0,
			0, 0, 0, 0, 1, 2, 2, 2, 1, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0};
			w = 13, h = 13, x0 = 6, y0 = 6;
			this->SetRangeMap(tmpRangeMap, w, h, x0, y0);
			break;}
	}
}

void CStarmap::AddTarget(const Sector &target)
{
	ASSERT_SECTOR_VALID(target);
	ASSERT(m_bAICalculation);
	if (!m_bAICalculation || !PT_IN_RECT(target, 0, 0, STARMAP_SECTORS_HCOUNT, STARMAP_SECTORS_VCOUNT)) return;
	
	// prüfen, ob Ziel bereits in Liste vorhanden ist
	for (SECTORLIST::const_iterator it = m_lAITargets.begin(); it != m_lAITargets.end(); ++it)
		if (*it == target)
			return;

	// sonst hinzufügen
	m_lAITargets.push_back(target);

#ifdef DEBUG_AI_BASE_DEMO
	// Bewertung für Ausbreitungsrichtungen aktualisieren
	// ACHTUNG: Berechnung hier nicht notwendig
	RecalcTargetPoints();
#endif
}

BOOL CStarmap::IsTarget(const Sector &sector)
{
	for (SECTORLIST::const_iterator it = m_lAITargets.begin(); it != m_lAITargets.end(); ++it)
		if (it->x == sector.x && it->y == sector.y)
			return TRUE;
	return FALSE;
}

void CStarmap::AddKnownSystem(const Sector &sector)
{
	ASSERT_SECTOR_VALID(sector);
	ASSERT(m_bAICalculation);
	if (!m_bAICalculation || !PT_IN_RECT(sector, 0, 0, STARMAP_SECTORS_HCOUNT, STARMAP_SECTORS_VCOUNT)) return;
	
	// prüfen, ob Ziel bereits in Liste vorhanden ist
	for (SECTORLIST::const_iterator it = m_lAIKnownSystems.begin(); it != m_lAIKnownSystems.end(); ++it)
		if (*it == sector)
			return;

	// sonst hinzufügen
	m_lAIKnownSystems.push_back(sector);

#ifdef DEBUG_AI_BASE_DEMO
	// Bewertung für Ausbreitungsrichtungen aktualisieren
	// ACHTUNG: Berechnung hier nicht notwendig
	RecalcTargetPoints();
#endif
}

BOOL CStarmap::IsKnownSystem(const Sector &sector)
{
	for (SECTORLIST::const_iterator it = m_lAIKnownSystems.begin(); it != m_lAIKnownSystems.end(); ++it)
		if (it->x == sector.x && it->y == sector.y)
			return TRUE;
	return FALSE;
}

void CStarmap::RecalcRangePoints()
{
	//memset(m_AIRangePoints, 0, STARMAP_SECTORS_HCOUNT * STARMAP_SECTORS_VCOUNT * sizeof(short));
	for(int i=0;i<STARMAP_SECTORS_HCOUNT;i++)
		for(int j=0;j<STARMAP_SECTORS_VCOUNT;j++)
			m_AIRangePoints[i][j]=0;

	// komplette Starmap durchlaufen, Werte aus Effektivitätsgründen nur für Sektoren innerhalb der gegebenen
	// Reichweite berechnen
	for (char x = 0; x < STARMAP_SECTORS_HCOUNT; x++)
		for (char y = 0; y < STARMAP_SECTORS_VCOUNT; y++)
			if (m_Range[x][y] >= m_nAIRange)
			{
				// für Sektoren innerhalb der gegebenen Reichweite Bewertung neu berechnen, die sich ergibt,
				// wenn hier ein Außenposten gebaut würde

				m_AIRangePoints[x][y] = 0;

				// lokale Rangemap durchlaufen
				for (char mx = -m_RangeMap.x0; mx < m_RangeMap.w - m_RangeMap.x0; mx++)
					for (char my = -m_RangeMap.y0; my < m_RangeMap.h - m_RangeMap.y0; my++)
					{
						Sector mpt(x + mx, y + my);
						if (PT_IN_RECT(mpt, 0, 0, STARMAP_SECTORS_HCOUNT, STARMAP_SECTORS_VCOUNT))
						{
							// Gebietszuwachs ermitteln
							m_AIRangePoints[x][y] += max(GetRangeMapValue(mx, my) - m_Range[mpt.x][mpt.y], 0);
						}
					}
			}
}

void CStarmap::RecalcConnectionPoints()
{
	//memset(m_AINeighbourCount, 0, STARMAP_SECTORS_HCOUNT * STARMAP_SECTORS_VCOUNT * sizeof(unsigned char));
	for(int i=0;i<STARMAP_SECTORS_HCOUNT;i++)
		for(int j=0;j<STARMAP_SECTORS_VCOUNT;j++)
			m_AINeighbourCount[i][j]=0;
	//memset(m_AIConnectionPoints, 0, STARMAP_SECTORS_HCOUNT * STARMAP_SECTORS_VCOUNT * sizeof(short));

	for(int i=0;i<STARMAP_SECTORS_HCOUNT;i++)
		for(int j=0;j<STARMAP_SECTORS_VCOUNT;j++)
			m_AIConnectionPoints[i][j]=0;

	// für Sektoren, die außerhalb der Reichweite liegen, die Anzahl der Nachbarn innerhalb der Reichweite neu bestimmen
	for (char x = 0; x < STARMAP_SECTORS_HCOUNT; x++)
		for (char y = 0; y < STARMAP_SECTORS_VCOUNT; y++)
			if (m_Range[x][y] >= m_nAIRange)
			{
				// Nachbaranzahl für Nachbarsektoren von (x, y), die außerhalb der Reichweite liegen, hochzählen
				for (int nx = -1; nx <= 1; nx++)
					for (int ny = -1; ny <= 1; ny++)
					{
						Sector npt(x + nx, y + ny);
						if (PT_IN_RECT(npt, 0, 0, STARMAP_SECTORS_HCOUNT, STARMAP_SECTORS_VCOUNT) &&
							m_Range[npt.x][npt.y] < m_nAIRange)
						{
							// (npt.x, npt.y) ist jetzt immer != (x, y)
							m_AINeighbourCount[npt.x][npt.y]++;
						}
					}
			}

	// Bewertungen für Zusammenhang neu berechnen
	for (char x = 0; x < STARMAP_SECTORS_HCOUNT; x++)
		for (char y = 0; y < STARMAP_SECTORS_VCOUNT; y++)
			if (m_Range[x][y] >= m_nAIRange)
			{
				// für Sektoren (x, y) innerhalb der Reichweite die lokale Rangemap durchlaufen
				for (char mx = -m_RangeMap.x0; mx < m_RangeMap.w - m_RangeMap.x0; mx++)
					for (char my = -m_RangeMap.y0; my < m_RangeMap.h - m_RangeMap.y0; my++)
					{
						Sector mpt(x + mx, y + my);
						if (PT_IN_RECT(mpt, 0, 0, STARMAP_SECTORS_HCOUNT, STARMAP_SECTORS_VCOUNT))
						{
							// für die (x, y) umgebenden Sektoren deren Anzahl der Nachbarn innerhalb der Reichweite
							// verwerten: ist diese Anzahl hoch, befindet sich in der Nähe von (x, y) ein weiteres
							// Gebiet innerhalb der Reichweite, das mit dem Gebiet von (x, y) nicht "direkt" zusammenhängt
							m_AIConnectionPoints[x][y] += m_AINeighbourCount[mpt.x][mpt.y] * GetRangeMapValue(mx, my);
						}
					}
			}
}

#define NEAR_TARGET_DISTANCE	5			///< Entfernung, die für ein Ziel als "nah" interpretiert wird
#define NEAR_SYSTEM_DISTANCE	3			///< Entfernung, die für ein bekanntes System als "nah" interpretiert wird

void CStarmap::RecalcTargetPoints()
{
	//memset(m_AITargetPoints, 0, STARMAP_SECTORS_HCOUNT * STARMAP_SECTORS_VCOUNT * sizeof(short));
	for(int i=0;i<STARMAP_SECTORS_HCOUNT;i++)
		for(int j=0;j<STARMAP_SECTORS_VCOUNT;j++)
			m_AITargetPoints[i][j]=0;

	// abbrechen, wenn keine Ziele angegeben und keine Systeme bekannt sind
	if (m_lAITargets.empty() && m_lAIKnownSystems.empty()) return;

	// alle Sektoren innerhalb der Reichweite durchlaufen
	for (int x = 0; x < STARMAP_SECTORS_HCOUNT; x++)
		for (int y = 0; y < STARMAP_SECTORS_VCOUNT; y++)
			if (m_Range[x][y] >= m_nAIRange)
			{
				// alle Ziele untersuchen
				for (SECTORLIST::const_iterator it = m_lAITargets.begin(); it != m_lAITargets.end(); ++it)
				{
					// Ziel überspringen, wenn es sich innerhalb der gegebenen Reichweite befindet
					if (m_Range[it->x][it->y] >= m_nAIRange) continue;

					// zählen, wieviele Sektoren innerhalb der Reichweite besucht werden, wenn man
					// vom Sektor (x, y) aus horizontal zu (tx, y) und vertikal zu (x, ty) läuft,
					// wobei (tx, ty) die Koordinaten des Zielsektors sind
					bool intersect = false;

					short incx = sgn(it->x - x), px = x + incx;
					while (incx)
					{
						if (m_Range[px][y] >= m_nAIRange)
						{
							intersect = true; break;
						}
						if (px == it->x) break;
						px += incx;
					}

					short incy = sgn(it->y - y), py = y + incy;
					while (incy && !intersect)
					{
						if (m_Range[x][py] >= m_nAIRange)
						{
							intersect = true; break;
						}
						if (py == it->y) break;
						py += incy;
					}

					// Bonus, wenn keine Sektoren innerhalb der Reichweite besucht wurden
					if (!intersect) m_AITargetPoints[x][y] += 20;

					// Entfernung zum Ziel ermitteln (da (x, y) innerhalb der Reichweite und das Ziel außerhalb liegt,
					// ist diese Entfernung >= 1)
					short distance = max(abs(it->x - x), abs(it->y - y));
					if (distance <= NEAR_TARGET_DISTANCE)
						m_AITargetPoints[x][y] += (NEAR_TARGET_DISTANCE - distance + 1) * 5;
				}

				// die bekannten Systeme untersuchen
				for (SECTORLIST::const_iterator it = m_lAIKnownSystems.begin(); it != m_lAIKnownSystems.end(); ++it)
				{
					// System überspringen, wenn es sich bereits innerhalb der gegebenen Reichweite befindet
					if (m_Range[it->x][it->y] >= m_nAIRange) continue;

					// Bonus, wenn der Bau eines Außenpostens in (x, y) das System in die gegebene Reichweite
					// eingliedern würde
					Sector system(it->x - x, it->y - y);
					if (PT_IN_RECT(system, -m_RangeMap.x0, -m_RangeMap.y0, m_RangeMap.w - m_RangeMap.x0, m_RangeMap.h - m_RangeMap.y0)
						&& GetRangeMapValue(system.x, system.y) >= m_nAIRange)
					{
						m_AITargetPoints[x][y] += 30;
					}
					// sonst in Abhängigkeit von der Entfernung zum System
					else
					{
						short distance = max(abs(it->x - x), abs(it->y - y));
						if (distance <= NEAR_SYSTEM_DISTANCE)
							m_AITargetPoints[x][y] += (NEAR_SYSTEM_DISTANCE - distance + 1) * 5;
					}
				}
			}
}

short CStarmap::GetPoints(const Sector &sector)
{
	short points = m_AIRangePoints[sector.x][sector.y];
	if (points > 0)
	{
		// Verbinden von Gebieten und bevorzugte Ausbreitungsrichtung sind nur dort sinnvoll,
		// wo Gebietszuwachs erreicht wird
		points += m_AIConnectionPoints[sector.x][sector.y] + m_AITargetPoints[sector.x][sector.y];
	}
	
	points -= m_AIBadPoints[sector.x][sector.y];
	return points;
	
//	return m_AIRangePoints[sector.x][sector.y];
//	return m_AIConnectionPoints[sector.x][sector.y];
//	return m_AITargetPoints[sector.x][sector.y];
}

void CStarmap::SetBadAIBaseSectors(CSector** sectors/*[][STARMAP_SECTORS_VCOUNT]*/, const CString& race)
{
	//memset(m_AIBadPoints, 0, STARMAP_SECTORS_HCOUNT * STARMAP_SECTORS_VCOUNT * sizeof(short));
	for(int i=0;i<STARMAP_SECTORS_HCOUNT;i++)
		for(int j=0;j<STARMAP_SECTORS_VCOUNT;j++)
			m_AIBadPoints[i][j]=0;

	for (int y = 0 ; y < STARMAP_SECTORS_VCOUNT; y++)
		for (int x = 0; x < STARMAP_SECTORS_HCOUNT; x++)
			if (m_Range[x][y] >= m_nAIRange)
			{
				double dValue = 0.0;
				if (sectors[x][y].GetOwnerOfSector() == race || sectors[x][y].GetOwnerOfSector() == "")
				{
					int number = 0;
					// in einem Umkreis von einem Sektor um den Sektor scannen
					for (int j = -1; j <= 1; j++)
						for (int i = -1; i <= 1; i++)
							if (y + j > -1 && y + j < STARMAP_SECTORS_VCOUNT && x + i > -1 && x + i < STARMAP_SECTORS_HCOUNT)
								if (sectors[x+i][y+j].GetOwnerOfSector() != race && sectors[x+i][y+j].GetOwnerOfSector() != "")
									number++;
					dValue += (50.0 * number);
				}
				else
					dValue += 1000.0;

				if (sectors[x][y].GetAnomaly())
					dValue += sectors[x][y].GetAnomaly()->GetWaySearchWeight() * 100.0;

				if ((double)m_AIBadPoints[x][y] + dValue > MAXSHORT)
					m_AIBadPoints[x][y] = MAXSHORT;
				else
					m_AIBadPoints[x][y] += (short)dValue;				
			}		
}

BaseSector CStarmap::CalcAIBaseSector(double variance)
{
	ASSERT(m_bAICalculation);
	ASSERT(0. <= variance && variance <= 1.);

	if (!m_bAICalculation) return BaseSector();
	variance = min(max(variance, 0.), 1.);

	// Bewertung der Sektoren neu berechnen
	RecalcRangePoints();
	RecalcConnectionPoints();
	RecalcTargetPoints();

	// Sektoren innerhalb der Reichweite mit Gebietszuwachs + zusätzlicher Bewertung in Liste aufnehmen
	std::list<BaseSector> lSectors;
	for (int x = 0; x < STARMAP_SECTORS_HCOUNT; x++)
		for (int y = 0; y < STARMAP_SECTORS_VCOUNT; y++)
			if (m_Range[x][y] >= m_nAIRange)
			{
				BaseSector sector;
				sector.position = Sector(x, y);
				sector.points = GetPoints(sector.position);
				lSectors.push_back(sector);
			}

	// abbrechen, wenn kein Sektor innerhalb der Reichweite existiert
	if (lSectors.empty()) return BaseSector();

	// nach Gesamtbewertung sortieren
	lSectors.sort();

	// die Bewertungsstufen bestimmen, die innerhalb der besten (100 * variance)% liegen
	int count = 0;
	short max_points = -1, old_points = -1;

	for (std::list<BaseSector>::const_iterator it = lSectors.begin(); it != lSectors.end(); ++it)
	{
		if (max_points == -1) max_points = it->points;
		if ((double)(max_points - it->points) / max_points > variance)
			break;
		if (it->points != old_points)
		{
			old_points = it->points;
			count += it->points;
		}
	}

	// eine der Stufen zufällig bestimmen, höhere Stufen mit höherer Wahrscheinlichkeit wählen
	int n = (int)(((double)rand() / RAND_MAX) * count);

	old_points = -1;
	for (std::list<BaseSector>::const_iterator it = lSectors.begin(); it != lSectors.end(); ++it)
	{
		if (it->points != old_points)
		{
			old_points = it->points;
			n -= it->points;
			if (n < 0)
			{
				n = it->points;
				break;
			}
		}
	}

	// Einträge der gewählten Stufe zählen
	count = 0;
	for (std::list<BaseSector>::const_iterator it = lSectors.begin(); it != lSectors.end(); ++it)
	{
		if (it->points == n)
			count++;
		else if (it->points < n)
			break;
	}

	// einen der Einträge gleichwahrscheinlich wählen
	int m = (int)(((double)rand() / RAND_MAX) * count);
	for (std::list<BaseSector>::const_iterator it = lSectors.begin(); it != lSectors.end(); ++it)
	{
		if (it->points == n)
		{
			if (!m--) return *it;
		}
		else if (it->points < n)
			break;
	}

	// bei sonstigen Fehlern
	return BaseSector();
}
