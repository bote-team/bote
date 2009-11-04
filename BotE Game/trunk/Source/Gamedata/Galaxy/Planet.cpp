// Planet.cpp: Implementierung der Klasse CPlanet.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "stdafx.h"
#include "Planet.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_SERIAL (CPlanet, CObject, 1)
//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

CPlanet::CPlanet()
{
	// Standardwerte initialisieren
	m_bHabitable = TRUE;
	m_bColonisized = FALSE;
	m_bTerraformed = FALSE;
	m_bIsTerraforming = FALSE;
	m_dCurrentHabitant = NULL;
	m_dMaxHabitant = NULL;
	m_iSize = NORMAL;
	m_iType = PLANETCLASS_I;
	m_iPosition = 0;
	m_strName = ""; 
	m_cClass = NULL;
	m_dGrowing = 0;
	m_iGraphicType = rand()%GRAPHICNUMBER;
	m_iNeededTerraformPoints = 0;
	m_iStartTerraformPoints = 0;
	for (int i = 0; i < 8; i++)
		m_bBoni[i] = FALSE;
}

CPlanet::~CPlanet()
{

}
///////////////////////////////////////////////////////////////////////
// Kopierkonstruktor
///////////////////////////////////////////////////////////////////////
CPlanet::CPlanet(const CPlanet & rhs)
{
	m_bColonisized = rhs.m_bColonisized;
	m_bHabitable = rhs.m_bHabitable;
	m_bTerraformed = rhs.m_bTerraformed;
	m_bIsTerraforming = rhs.m_bIsTerraforming;
	m_dCurrentHabitant = rhs.m_dCurrentHabitant;
	m_dMaxHabitant = rhs.m_dMaxHabitant;
	m_iSize = rhs.m_iSize;
	m_iType = rhs.m_iType;
	m_iPosition = rhs.m_iPosition;
	m_strName = rhs.m_strName;
	m_cClass = rhs.m_cClass;
	m_dGrowing = rhs.m_dGrowing;
	m_iGraphicType = rhs.m_iGraphicType;
	m_iNeededTerraformPoints = rhs.m_iNeededTerraformPoints;
	m_iStartTerraformPoints = rhs.m_iStartTerraformPoints;
	for (int i = 0; i < 8; i++)
		m_bBoni[i] = rhs.m_bBoni[i];
}

///////////////////////////////////////////////////////////////////////
// Zuweisungsoperator
///////////////////////////////////////////////////////////////////////
CPlanet & CPlanet::operator=(const CPlanet & rhs)
{
	if (this == &rhs)
		return *this;
	m_bColonisized = rhs.m_bColonisized;
	m_bHabitable = rhs.m_bHabitable;
	m_bTerraformed = rhs.m_bTerraformed;
	m_bIsTerraforming = rhs.m_bIsTerraforming;
	m_dCurrentHabitant = rhs.m_dCurrentHabitant;
	m_dMaxHabitant = rhs.m_dMaxHabitant;
	m_iSize = rhs.m_iSize;
	m_iType = rhs.m_iType;
	m_iPosition = rhs.m_iPosition;
	m_strName = rhs.m_strName;
	m_cClass = rhs.m_cClass;
	m_dGrowing = rhs.m_dGrowing;
	m_iGraphicType = rhs.m_iGraphicType;
	m_iNeededTerraformPoints = rhs.m_iNeededTerraformPoints;
	m_iStartTerraformPoints = rhs.m_iStartTerraformPoints;
	for (int i = 0; i < 8; i++)
		m_bBoni[i] = rhs.m_bBoni[i];
	return *this;
}

///////////////////////////////////////////////////////////////////////
// Speichern / Laden
///////////////////////////////////////////////////////////////////////
void CPlanet::Serialize(CArchive &ar)		
{
	CObject::Serialize(ar);
	// wenn gespeichert wird
	if (ar.IsStoring())
	{
		ar << m_bColonisized;
		ar << m_bHabitable;
		ar << m_bTerraformed;
		ar << m_bIsTerraforming;
		ar << m_dCurrentHabitant;
		ar << m_dMaxHabitant;
		ar << m_iSize;
		ar << m_iType;
		ar << m_iPosition;
		ar << m_strName;
		ar << m_cClass;
		ar << m_dGrowing;
		ar << m_iGraphicType;
		ar << m_iNeededTerraformPoints;
		ar << m_iStartTerraformPoints;
		for (int i = 0; i < 8; i++)
			ar << m_bBoni[i];
	}
	// wenn geladen wird
	if (ar.IsLoading())
	{
		ar >> m_bColonisized;
		ar >> m_bHabitable;
		ar >> m_bTerraformed;
		ar >> m_bIsTerraforming;
		ar >> m_dCurrentHabitant;
		ar >> m_dMaxHabitant;
		ar >> m_iSize;
		ar >> m_iType;
		ar >> m_iPosition;
		ar >> m_strName;
		ar >> m_cClass;
		ar >> m_dGrowing;
		ar >> m_iGraphicType;
		ar >> m_iNeededTerraformPoints;
		ar >> m_iStartTerraformPoints;
		for (int i = 0; i < 8; i++)
			ar >> m_bBoni[i];
	}
}

/// Funktion gibt einen Dateinamen für die Planetengrafik zurück. Dieser wird automatisch aus der Nummer der
/// Grafik und der Planetenklasse generiert.
CString CPlanet::GetGraphicFile() const
{
	if (m_iGraphicType < 10)	// führende NULL beachten
	{
		CString fileName;
		fileName.Format("Planets\\class%c0%d.png", m_cClass, m_iGraphicType);
		return fileName;
	}
	else
	{
		CString fileName;
		fileName.Format("Planets\\class%c%d.png", m_cClass, m_iGraphicType);
		return fileName;
	}
}

// KindOfLastPlanet = welcher Planet wurde als letztes generiert, weil hinter Erde darf ja kein Merkur mehr kommen
// das hat alles was mit den Zonen zu tun
const BYTE CPlanet::GeneratePlanet(BYTE KindOfLastPlanet, CString NameOfSunSystem, BYTE NumberOfPlanet, BOOLEAN MinorRace)
{
	// Standardwerte initialisieren
	m_bHabitable = TRUE;
	m_bColonisized = FALSE;
	m_bTerraformed = FALSE;
	m_bIsTerraforming = FALSE;
	m_dCurrentHabitant = NULL;
	m_dMaxHabitant = NULL;
	m_iSize = NORMAL;
	m_iType = PLANETCLASS_I;
	m_iPosition = 0;
	m_strName = ""; 
	m_cClass = NULL;
	m_dGrowing = 0;
	m_iGraphicType = 0;
	m_iNeededTerraformPoints = 0;
	m_iStartTerraformPoints = 0;
	for (int i = 0; i < 8; i++)
		m_bBoni[i] = FALSE;
	

	BYTE m_iRandomSize = 0;	// Zufallszahl, die die Größe bestimmt (abhängig vom DEFINE!)
	BYTE m_iRandomType = 0;	// Zufallszahl, die den Type bestimmt (abhängug vom DEFINE!)
	
	BYTE Zone = HOT;
	// Ersten Planeten bestimmen, größte Wahrscheinlichkeit für einen Planeten aus heißer Zone
	// nur machen, wenn NumberOfPlanet == 0
	if (NumberOfPlanet == 0)
	{
		BYTE WhatZone = rand()%10;						// Zufallszahl aus 0 bis 9 ermitteln
		if (WhatZone > 2) Zone = HOT;					// Wenn diese größer 2 ist, dann Planet aus heißer Zone
		if (WhatZone > 0 && WhatZone < 3) Zone = TEMPERATE;
		if (WhatZone == 0) Zone = COOL;
	}
	
	// Bestimmen, in welcher Zone der vorherige Planet lag, abhängig vom letzten Planeten
	if (NumberOfPlanet != 0)
	switch (KindOfLastPlanet)
	{
		case PLANETCLASS_A: Zone = COOL;
				break;
		case PLANETCLASS_B: Zone = HOT;
				break;
		case PLANETCLASS_C: Zone = COOL;
				break;
		case PLANETCLASS_E: Zone = TEMPERATE;
				break;
		case PLANETCLASS_F: Zone = TEMPERATE;
				break;
		case PLANETCLASS_G: Zone = TEMPERATE;
				break;
		case PLANETCLASS_H: Zone = HOT;
				break;
		case PLANETCLASS_I: Zone = COOL;
				break;
		case PLANETCLASS_J: Zone = COOL;
				break;
		case PLANETCLASS_K: Zone = TEMPERATE;
				break;
		case PLANETCLASS_L: Zone = TEMPERATE;
				break;
		case PLANETCLASS_M: Zone = TEMPERATE;
				break;
		case PLANETCLASS_N: Zone = HOT;
				break;
		case PLANETCLASS_O: Zone = TEMPERATE;
				break;
		case PLANETCLASS_P: Zone = COOL;
				break;
		case PLANETCLASS_Q: Zone = TEMPERATE;
				break;
		// Das wahrscheinlich nochmal berichtigen um Bug 7 zu lösen
		case PLANETCLASS_R: Zone = rand()%KindOfLastPlanet;	// R Kann überall liegen
				break;
		case PLANETCLASS_S: Zone = COOL;
				break;
		case PLANETCLASS_T: Zone = COOL;
				break;
		case PLANETCLASS_Y: Zone = HOT;
				break;
		default:Zone = COOL;
				break;
		
	}
	// Den ersten Planeten bestimmen, abhängig von der Zone in der er liegen würde
	if (Zone == HOT && NumberOfPlanet == 0)
	{
		int WhatPlanet = rand()%9;	
		if (WhatPlanet >= 0) m_iRandomType = PLANETCLASS_H;
		if (WhatPlanet >= 2) m_iRandomType = PLANETCLASS_B;
		if (WhatPlanet >= 4) m_iRandomType = PLANETCLASS_N;
		if (WhatPlanet >= 6) m_iRandomType = PLANETCLASS_Y;
		if (WhatPlanet >= 8) m_iRandomType = PLANETCLASS_R;
	}
	if (Zone == TEMPERATE && NumberOfPlanet == 0)
	{
		int WhatPlanet = rand()%9;
		if (WhatPlanet == 0) m_iRandomType = PLANETCLASS_E;
		if (WhatPlanet == 1) m_iRandomType = PLANETCLASS_F;
		if (WhatPlanet == 2) m_iRandomType = PLANETCLASS_G;
		if (WhatPlanet == 3) m_iRandomType = PLANETCLASS_K;
		if (WhatPlanet == 4) m_iRandomType = PLANETCLASS_L;
		if (WhatPlanet == 5) m_iRandomType = PLANETCLASS_M;
		if (WhatPlanet == 6) m_iRandomType = PLANETCLASS_O;
		if (WhatPlanet == 7) m_iRandomType = PLANETCLASS_Q;
		if (WhatPlanet == 8) m_iRandomType = PLANETCLASS_R;
	}
	if (Zone == COOL && NumberOfPlanet == 0)
	{
		int WhatPlanet = rand()%8;	
		if (WhatPlanet == 0) m_iRandomType = PLANETCLASS_I;
		if (WhatPlanet == 1) m_iRandomType = PLANETCLASS_J;
		if (WhatPlanet == 2) m_iRandomType = PLANETCLASS_S;
		if (WhatPlanet == 3) m_iRandomType = PLANETCLASS_T;
		if (WhatPlanet == 4) m_iRandomType = PLANETCLASS_A;
		if (WhatPlanet == 5) m_iRandomType = PLANETCLASS_C;
		if (WhatPlanet == 6) m_iRandomType = PLANETCLASS_P;
		if (WhatPlanet == 7) m_iRandomType = PLANETCLASS_R;
	}
	// Jetzt Planetenklassen bestimmen, abhängig von der Zone und dem vorherigen Planeten (NumberOfPlanet > 0)
	if (Zone == HOT && NumberOfPlanet >= 1)	// Es könnten alle Klassen auftreten, aber halt mit unterschiedlicher Wahrscheinlichkeit
	{
		int WhatPlanet = rand()%75;
		// Planeten aus kalter Zone
		if (WhatPlanet == 0) m_iRandomType = PLANETCLASS_I;
		if (WhatPlanet == 1) m_iRandomType = PLANETCLASS_J;
		if (WhatPlanet == 2) m_iRandomType = PLANETCLASS_S;
		if (WhatPlanet == 3) m_iRandomType = PLANETCLASS_T;
		if (WhatPlanet == 4) m_iRandomType = PLANETCLASS_A;
		if (WhatPlanet == 5) m_iRandomType = PLANETCLASS_C;
		if (WhatPlanet == 6) m_iRandomType = PLANETCLASS_P;
		// Planeten aus gemäßigter Zone
		if (WhatPlanet > 6) m_iRandomType = PLANETCLASS_E;
		if (WhatPlanet > 11) m_iRandomType = PLANETCLASS_F;
		if (WhatPlanet > 16) m_iRandomType = PLANETCLASS_G;
		if (WhatPlanet > 21) m_iRandomType = PLANETCLASS_K;
		if (WhatPlanet > 26) m_iRandomType = PLANETCLASS_L;
		if (WhatPlanet > 31) m_iRandomType = PLANETCLASS_M;	// seltener
		if (WhatPlanet > 35) m_iRandomType = PLANETCLASS_O;
		if (WhatPlanet > 40) m_iRandomType = PLANETCLASS_Q;	// selten
		if (WhatPlanet > 43) m_iRandomType = PLANETCLASS_R; // selten
		// Planeten aus heißer Zone
		if (WhatPlanet > 46) m_iRandomType = PLANETCLASS_H;
		if (WhatPlanet > 53) m_iRandomType = PLANETCLASS_B;
		if (WhatPlanet > 60) m_iRandomType = PLANETCLASS_N;
		if (WhatPlanet > 67) m_iRandomType = PLANETCLASS_Y;
	}
	else if (Zone == TEMPERATE && NumberOfPlanet >= 1)
	{
		int WhatPlanet = rand()%47;
		// Planeten aus kalter Zone
		if (WhatPlanet == 0) m_iRandomType = PLANETCLASS_I;
		if (WhatPlanet == 1) m_iRandomType = PLANETCLASS_J;
		if (WhatPlanet == 2) m_iRandomType = PLANETCLASS_S;
		if (WhatPlanet == 3) m_iRandomType = PLANETCLASS_T;
		if (WhatPlanet == 4) m_iRandomType = PLANETCLASS_A;
		if (WhatPlanet == 5) m_iRandomType = PLANETCLASS_C;
		if (WhatPlanet == 6) m_iRandomType = PLANETCLASS_P;
		// Planeten aus gemäßigter Zone
		if (WhatPlanet > 6) m_iRandomType = PLANETCLASS_E;
		if (WhatPlanet > 11) m_iRandomType = PLANETCLASS_F;
		if (WhatPlanet > 16) m_iRandomType = PLANETCLASS_G;
		if (WhatPlanet > 21) m_iRandomType = PLANETCLASS_K;
		if (WhatPlanet > 26) m_iRandomType = PLANETCLASS_L;
		if (WhatPlanet > 31) m_iRandomType = PLANETCLASS_M;	// seltener
		if (WhatPlanet > 35) m_iRandomType = PLANETCLASS_O;
		if (WhatPlanet > 40) m_iRandomType = PLANETCLASS_Q;
		if (WhatPlanet > 43) m_iRandomType = PLANETCLASS_R;
	}
	else if (Zone == COOL && NumberOfPlanet >= 1)
	{
		int WhatPlanet = rand()%18;
		if (WhatPlanet == 0) m_iRandomType = PLANETCLASS_I;
		if (WhatPlanet == 1) m_iRandomType = PLANETCLASS_J;
		if (WhatPlanet == 2) m_iRandomType = PLANETCLASS_S;
		if (WhatPlanet == 3) m_iRandomType = PLANETCLASS_T;
		if (WhatPlanet >= 4) m_iRandomType = PLANETCLASS_A;
		if (WhatPlanet >= 8) m_iRandomType = PLANETCLASS_C;
		if (WhatPlanet >= 12) m_iRandomType = PLANETCLASS_P;
		if (WhatPlanet >= 17) m_iRandomType = PLANETCLASS_R;
	}
	
	float Habitants = 0.0f;
	m_iPosition = NumberOfPlanet+1;
	m_iRandomSize = rand()%3;					// mod der Anzahl, die im zugehörigen ENUM stehen
	m_iGraphicType = rand()%GRAPHICNUMBER;		// eine der Graphiken zufällig wählen
		
	m_iSize = m_iRandomSize;
	m_iType = m_iRandomType;
	// Bestimmen, ob der Planet überhaupt bewohnbar ist! A,B,E,I,J,S,T,Y
	if (m_iRandomType == PLANETCLASS_A || m_iRandomType == PLANETCLASS_B || m_iRandomType == PLANETCLASS_E
		|| m_iRandomType == PLANETCLASS_I || m_iRandomType == PLANETCLASS_J || m_iRandomType == PLANETCLASS_S
		|| m_iRandomType == PLANETCLASS_T || m_iRandomType == PLANETCLASS_Y)
		m_bHabitable = FALSE;
	if (m_iRandomType == PLANETCLASS_M)		// Ein erdähnlicher Planet ist schon terraformed
		m_bTerraformed = TRUE;
	if (m_iRandomType == PLANETCLASS_I || m_iRandomType == PLANETCLASS_J || m_iRandomType == PLANETCLASS_S || m_iRandomType == PLANETCLASS_T)
		m_iSize = GIANT;			// Gasriesen sind immer GROß
	if (m_bHabitable == FALSE)
		m_dMaxHabitant = 0;
	else
	{	
		if (m_iRandomType < PLANETCLASS_A)	// Ab Klasse A sind alle Planeten eh nicht mehr kolonisierbar
		{
			// Einen Muliplikator anlegen (0,8 ; 1,0 ; 1,2)
			float multi = 0.0f;
			// Welcher Multiplikator soll gewählt werden?
			int miniRandom = rand()%3;
			if (miniRandom == 0) multi = 0.8f;
			else if (miniRandom == 1) multi = 1.0f;
			else if (miniRandom == 2) multi = 1.2f;
				
			// Zufallszahl ermitteln, abhängig von der max. Einwohnerzahl, die später draufaddiert wird
			int random = rand()%(((m_iRandomSize+1)*(12-m_iRandomType))/6 + 1);
			// Max. Einwohner werden berechnet, inkl. "random " Zufallszahl
			Habitants = ((((m_iRandomSize+1)*(12-m_iRandomType)+random)*1000)*multi*0.5f);
			// z.B. für kleine Erde (M): 1*12+z.B. 2 = 11 * 1000 = 9000 -> 11000 Mill. Einwohner (11 Mrd.)
			m_dMaxHabitant = Habitants;
			m_dMaxHabitant = m_dMaxHabitant/1000;

			// hier noch die benötigten Terraformingpunkte berechnen. Umso besser die Klasse des Planeten, umso weniger
			// Punkte brauchen wir. Klasse M braucht gar keine Terraformpunkte weil er ja schon geterraformt ist
			m_iNeededTerraformPoints = (int)(m_iRandomType * (m_iRandomSize + 1) * (rand()%11 +5) / 10);
			if (m_iNeededTerraformPoints == 0 && m_iRandomType != PLANETCLASS_M)
				m_iNeededTerraformPoints++;
			m_iStartTerraformPoints = m_iNeededTerraformPoints;
		}
		else
			m_dMaxHabitant = 0;
	}
	// Wenn eine MinorRace da ist, dann ein paar Planeten schon geterraformt machen
	if (MinorRace == TRUE && m_bHabitable == TRUE)
	{
		BYTE random;
		random = rand()%5+1;
		if (random >= 5)
		{
			m_bTerraformed = TRUE;
			m_iNeededTerraformPoints = 0;
			m_iStartTerraformPoints = 0;
		}
	}
	// Wenn eine MinorRace in dem System ist, dann gleich ein paar Einwohner generieren
	if (MinorRace == TRUE && m_bTerraformed == TRUE)
	{
		BYTE randDiv;
		randDiv = rand()%8+1;
		m_dCurrentHabitant = m_dMaxHabitant/randDiv;
		m_bColonisized = TRUE;
	}
// M,O,L,P,H,Q,K,G,R,F,C,N,A,B,E,N,Y,I,J,S,T
	if (m_iType == 0) {m_cClass = 'M';}
	else if (m_iType == 1) {m_cClass = 'O';}
	else if (m_iType == 2) {m_cClass = 'L';}
	else if (m_iType == 3) {m_cClass = 'P';}
	else if (m_iType == 4) {m_cClass = 'H';}
	else if (m_iType == 5) {m_cClass = 'Q';}
	else if (m_iType == 6) {m_cClass = 'K';}
	else if (m_iType == 7) {m_cClass = 'G';}
	else if (m_iType == 8) {m_cClass = 'R';}
	else if (m_iType == 9) {m_cClass = 'F';}
	else if (m_iType == 10) {m_cClass = 'C';}
	else if (m_iType == 11) {m_cClass = 'N';}
	else if (m_iType == 12) {m_cClass = 'A';}
	else if (m_iType == 13) {m_cClass = 'B';}
	else if (m_iType == 14) {m_cClass = 'E';}
	else if (m_iType == 15) {m_cClass = 'Y';}
	else if (m_iType == 16) {m_cClass = 'I';}
	else if (m_iType == 17) {m_cClass = 'J';}
	else if (m_iType == 18) {m_cClass = 'S';}
	else if (m_iType == 19) {m_cClass = 'T';}
	
	// Wachstumsprozent des Planeten berechnen
	SetPlanetGrowth();
	
	// Namen für den Planeten geben, besteht aus Namen des Sonnensystems und der Nummer
	m_strName.Format("%s %i",NameOfSunSystem,m_iPosition);
	
	//m_strName.Format("%s %i\nmax. Einwohner: %.3lf Mrd.\nTyp: %i\nGröße: %i\nMultiplikator: %.2lf\nRandomwert: %.2lf\nZone: %i",NameOfSunSystem,NumberOfPlanet+1,m_iMaxHabitant,m_iType,m_iSize,mult,randomwert,Zone);
	//AfxMessageBox(m_strName);
	//CString s;
	//s.Format("%i",m_dMaxHabitant);
	//AfxMessageBox(s);
	
	// eventuelle Boni durch den Planeten berechnen
	GenerateBoni();
	// Damit bei Klasse R keine Fehler auftreten und der nächste Planet in der richtigen Zone liegt
	if (m_cClass == 'R')
		return KindOfLastPlanet;
	return m_iType;
}

void CPlanet::DrawPlanet(Graphics &g, CRect planetRect, CGraphicPool* graphicPool)
{
	ASSERT(graphicPool);

	int x = planetRect.right;
	int y = planetRect.top+15;
	int y_Boni = y-23;
	
	Bitmap* planet = NULL;
	planet = graphicPool->GetGDIGraphic("Planets\\"+m_strName+".png");
	
	// Konnte keine spezielle Planetengrafik gefunden werden, so wird eine zufällige Grafik ausgewählt
	if (planet == NULL)
		planet = graphicPool->GetGDIGraphic(GetGraphicFile());
	
	if (planet)
	{	
		RectF r;
		switch (m_iSize)
		{
		case SMALL:
			r = RectF((REAL)x, (REAL)y, 45, 45);	break;
		case NORMAL:
			r = RectF((REAL)x, (REAL)y, 60, 60);	break;
		case BIG:
			r = RectF((REAL)x, (REAL)y, 80, 80);	break;
		case GIANT:
			{
				if (m_strName == "Saturn")
					r = RectF((REAL)x - 10, (REAL)y + 28, 145, 84);
				else
					r = RectF((REAL)x, (REAL)y, 125, 125);					
				break;
			}
		}
		g.DrawImage(planet, r);
		planet = NULL;	
	}	
			
	// Namen des Planeten mit anzeigen, später noch in separate Funktion machen
	if (m_iSize == GIANT)
		{x+=58;y+=130;}		// Bitmapgröße (x-Richtung/2)-5, y-Richtung +5
	else if (m_iSize == BIG)
		{x+=35;y+=85;}
	else if (m_iSize == NORMAL)
		{x+=25;y+=65;}
	else if (m_iSize == SMALL)
		{x+=18;y+=50;}	
	planetRect.SetRect(x,y,x+10,y+15);
	
	Color c;
	Gdiplus::Font font(L"Nina", 10);
	StringFormat format;
	format.SetAlignment(StringAlignmentCenter);
	SolidBrush brush(c);
	
	if (m_bHabitable == FALSE)
		c.SetFromCOLORREF(RGB(0,0,255));
	else if (m_bTerraformed == TRUE)
	{
		if (m_dCurrentHabitant > 0.0f)
			c.SetFromCOLORREF(RGB(0,255,30));
		else
			c.SetFromCOLORREF(RGB(180,255,180));
	}
	else if (m_bIsTerraforming == TRUE)
	{
		c.SetFromCOLORREF(RGB(200,200,0));
		brush.SetColor(c);
		// prozentuale Angabe mitmachen
		CString s;
		s.Format("%d %%",100 - (short)(this->m_iNeededTerraformPoints * 100 / this->m_iStartTerraformPoints));
		g.DrawString(s.AllocSysString(), -1, &font, RectF((REAL)planetRect.left-10, (REAL)planetRect.bottom, (REAL)planetRect.Width()+30, (REAL)planetRect.Height()+15), &format, &brush);
	}
	else
		c.SetFromCOLORREF(RGB(255,40,40));
	
	CString s;
	s.Format("%c", m_cClass);
	brush.SetColor(c);
	g.DrawString(s.AllocSysString(), -1, &font, RectF((REAL)planetRect.left, (REAL)planetRect.top, (REAL)planetRect.Width(), (REAL)planetRect.Height()), &format, &brush);
		
	// Symbole für eventuell vohandene Boni zeichnen
	// erstmal schauen, wieviele Boni überhaupt vorhanden sind
	BYTE n = 0;
	for (int i = 0; i < 8; i++)
		n += m_bBoni[i];
	x -= (n*9-4);
	
	Bitmap* graphic = NULL;
	for (int i = 0; i < 8; i++)
		if (m_bBoni[i])
		{
			switch(i)
			{
				case TITAN:		graphic = graphicPool->GetGDIGraphic("Other\\titanSmall.png"); break;
				case DEUTERIUM: graphic = graphicPool->GetGDIGraphic("Other\\deuteriumSmall.png"); break;
				case DURANIUM:	graphic = graphicPool->GetGDIGraphic("Other\\duraniumSmall.png"); break;
				case CRYSTAL:	graphic = graphicPool->GetGDIGraphic("Other\\crystalSmall.png"); break;
				case IRIDIUM:	graphic = graphicPool->GetGDIGraphic("Other\\iridiumSmall.png"); break;
				case DILITHIUM: graphic = graphicPool->GetGDIGraphic("Other\\Dilithium.png"); break;
				case 6:			graphic = graphicPool->GetGDIGraphic("Other\\foodSmall.png"); break;
				case 7:			graphic = graphicPool->GetGDIGraphic("Other\\energySmall.png"); break;
			}
			if (graphic)
			{
				g.DrawImage(graphic, x, y_Boni, 20, 16);
				x += 18;
				graphic = NULL;
			}
		}
}

void CPlanet::PlanetGrowth()
{
	float tempCurrentHabitant = m_dCurrentHabitant;
	if (m_dCurrentHabitant < m_dMaxHabitant)
		m_dCurrentHabitant = m_dCurrentHabitant+m_dCurrentHabitant*m_dGrowing/100;
	if (m_dCurrentHabitant < (tempCurrentHabitant + 0.1f) && m_bColonisized == TRUE)
		m_dCurrentHabitant = tempCurrentHabitant + 0.1f;	// immer minimales Wachstum von 0.1 Mrd. pro Runde
	if (m_dCurrentHabitant > m_dMaxHabitant)		
		m_dCurrentHabitant = m_dMaxHabitant;				// Wenn MaxHabitant erreicht ist, natürlich kein Wachstum mehr
}

/// Subtrahiert "sub" von den Terraformpoints, bei kleiner 0 wird der Plani auf m_bTerraformed = TRUE gesetzt und ein
/// TRUE zurückgegeben, ansonsten wird ein FALSE zurückgegeben
/// Wenn der Planet noch nicht fertig geterraformt ist, wird m_bIsTerraforming auf TRUE gesetzt
BOOLEAN CPlanet::SetNeededTerraformPoints(BYTE sub)	
{
	if ((m_iNeededTerraformPoints - sub) < 0)
		sub = m_iNeededTerraformPoints;
	m_iNeededTerraformPoints -= sub;	
	if (m_iNeededTerraformPoints <= 0)
	{
		m_bTerraformed = TRUE;
		return TRUE;	
	}
	else
	{
		m_bIsTerraforming = TRUE;
		return FALSE;
	}
}

/// Diese Funktion generiert einen eventuellen Bonus anhand einer speziellen Wahrscheinlichkeitstabelle.
void CPlanet::GenerateBoni()
{
	// die Wahrscheinlichkeitstablle wird angelegt
	// Matrix aus allen Planetenklassen und aller möglichen Boni
	BYTE probs[PLANETCLASS_T+1][8] = {

	// Testweise die Dilithiumprozente um 10 erhöht (nur dort wo nicht NULL drin stand)
	//		TITAN		DEUTERIUM	DURANIUM	KRISTALLE	IRIDIUM		DILITHIUM	FOOD		ENERGY
	//+-----------------------------------------------------------------------------------------------+
	/*M*/	0,			10,			0,			0,			0,			11,			50,			0,
	/*O*/	0,			50,			0,			0,			0,			0,			25,			0,
	/*L*/	0,			5,			0,			0,			0,			0,			10,			0,	
	/*P*/	5,			0,			0,			5,			5,			15,			0,			50,
	/*H*/	0,			0,			0,			0,			50,			0,			0,			50,
	/*Q*/	0,			0,			0,			50,			0,			15,			5,			0,
	/*K*/	5,			0,			5,			0,			0,			15,			0,			0,
	/*G*/	10,			0,			10,			25,			0,			20,			0,			0,
	/*R*/	0,			0,			50,			0,			0,			15,			0,			0,
	/*F*/	25,			0,			25,			0,			0,			35,			0,			0,
	/*C*/	50,			0,			0,			0,			25,			60,			0,			0,
	/*N*/	0,			25,			0,			0,			0,			60,			0,			50,
	/*A*/	0,			0,			0,			0,			0,			0,			0,			0,
	/*B*/	0,			0,			0,			0,			0,			0,			0,			0,
	/*E*/	0,			0,			0,			0,			0,			0,			0,			0,
	/*Y*/	0,			0,			0,			0,			0,			0,			0,			0,
	/*I*/	0,			0,			0,			0,			0,			0,			0,			0,
	/*J*/	0,			0,			0,			0,			0,			0,			0,			0,
	/*S*/	0,			0,			0,			0,			0,			0,			0,			0,
	/*T*/	0,			0,			0,			0,			0,			0,			0,			0
	};
	// Zahl[0,99] generieren und vergleichen
	for (int i = TITAN; i < 8; i++)
	{
		if (rand()%100 >= (100 - probs[this->m_iType][i]))
			m_bBoni[i] = TRUE;
		else
			m_bBoni[i] = FALSE;
	}
}

void CPlanet::SetBoni(BOOLEAN titan, BOOLEAN deuterium, BOOLEAN duranium, BOOLEAN crystal, BOOLEAN iridium, BOOLEAN dilithium,
		BOOLEAN food, BOOLEAN energy)
{
	m_bBoni[TITAN] = titan;
	m_bBoni[DEUTERIUM] = deuterium;
	m_bBoni[DURANIUM] = duranium;
	m_bBoni[CRYSTAL] = crystal;
	m_bBoni[IRIDIUM] = iridium;
	m_bBoni[DILITHIUM] = dilithium;
	m_bBoni[6] = food;
	m_bBoni[7] = energy;
}

/// Die Resetfunktion für die CPlanet Klasse, welche alle Werte wieder auf Ausgangswerte setzt.
void CPlanet::Reset()
{
	m_bHabitable = TRUE;
	m_bColonisized = FALSE;
	m_bTerraformed = FALSE;
	m_bIsTerraforming = FALSE;
	m_dCurrentHabitant = NULL;
	m_dMaxHabitant = NULL;
	m_iSize = NORMAL;
	m_iType = PLANETCLASS_A;
	m_iPosition = 0;
	m_strName = ""; 
	m_cClass = NULL;
	m_dGrowing = 0;
	m_iGraphicType = 0;
	m_iNeededTerraformPoints = 0;
	m_iStartTerraformPoints = 0;
	for (int i = 0; i < 8; i++)
		m_bBoni[i] = FALSE;
}
