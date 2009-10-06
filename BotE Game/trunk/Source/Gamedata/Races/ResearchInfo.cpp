// ResearchInfo.cpp: Implementierung der Klasse CResearchInfo.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ResearchInfo.h"
#include "Botf2.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


IMPLEMENT_SERIAL (CResearchInfo, CObject, 1)

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

CResearchInfo::CResearchInfo()
{
	m_iCurrentComplex = -1;			// kein Complex gewählt
	m_bChoiceTaken = FALSE;
	for (int i = 0; i < NoUC; i++)
		m_ResearchComplex[i].Reset();
	for (int i = 0; i < 6; i++)
	{
		m_strTechName[i] = "";
		m_strTechDescription[i] = "";
	}
}

CResearchInfo::~CResearchInfo()
{

}

//////////////////////////////////////////////////////////////////////
// Kopierkonstruktor
//////////////////////////////////////////////////////////////////////
CResearchInfo::CResearchInfo(const CResearchInfo & rhs)
{
	m_bChoiceTaken = rhs.m_bChoiceTaken;
	m_iCurrentComplex = rhs.m_iCurrentComplex;
	for (int i = 0; i < NoUC; i++)
		m_ResearchComplex[i] = rhs.m_ResearchComplex[i];
	for (int i = 0; i < 6; i++)
	{
		m_strTechName[i] = rhs.m_strTechName[i];
		m_strTechDescription[i] = rhs.m_strTechDescription[i];
	}
}

//////////////////////////////////////////////////////////////////////
// Zuweisungsoperator
//////////////////////////////////////////////////////////////////////
CResearchInfo & CResearchInfo::operator=(const CResearchInfo & rhs)
{
	if (this == &rhs)
		return *this;
	m_bChoiceTaken = rhs.m_bChoiceTaken;
	m_iCurrentComplex = rhs.m_iCurrentComplex;
	for (int i = 0; i < NoUC; i++)
		m_ResearchComplex[i] = rhs.m_ResearchComplex[i];
	for (int i = 0; i < 6; i++)
	{
		m_strTechName[i] = rhs.m_strTechName[i];
		m_strTechDescription[i] = rhs.m_strTechDescription[i];
	}
	return *this;
}

///////////////////////////////////////////////////////////////////////
// Speichern / Laden
///////////////////////////////////////////////////////////////////////
void CResearchInfo::Serialize(CArchive &ar)		
{
	CObject::Serialize(ar);
	// Namen und Techbeschreibungen werden beim Laden neu eingelesen. Dies wird in der Research-Klasse gemacht.	
	for (int i = 0; i < NoUC; i++)
		m_ResearchComplex[i].Serialize(ar);
	// wenn gespeichert wird
	if (ar.IsStoring())
	{
		ar << m_iCurrentComplex;
		ar << m_bChoiceTaken;
	}
	// wenn geladen wird
	if (ar.IsLoading())
	{
		ar >> m_iCurrentComplex;
		ar >> m_bChoiceTaken;
	}
}

//////////////////////////////////////////////////////////////////////
// sonstige Funktionen
//////////////////////////////////////////////////////////////////////

/// Diese Funktion wählt zufällig ein Unique-Themengebiet aus den noch nicht erforschten Komplexen aus.
/// Vor Aufruf der Funktion sollte überprüft werden, dass nicht schon alle Komplexe erforscht wurden, da
/// es sonst zum Absturz des Programms kommen könnte.
void CResearchInfo::ChooseUniqueResearch(void)
{
	int count = 0;
	while (1)
	{
		BYTE random = rand()%NoUC;
		if (m_ResearchComplex[random].m_byComplexStatus == NOTRESEARCHED)
		{
			m_ResearchComplex[random].m_byComplexStatus = RESEARCHING;
			m_iCurrentComplex = random;
			m_bChoiceTaken = FALSE;
			// Wenn wir einen Komplex ausgewählt haben, diesen erst generieren
			m_ResearchComplex[random].GenerateComplex(random);
			break;
		}
		count++;
		if (count == MAXBYTE)
			break;
	}
}

/// Diese Funktion ändert den Status des aktuellen Komplexes. Dabei ändert sie gleichzeitig auch den Status
/// der zuvor gewählten Wahlmöglichkeit. Als Parameter wird dabei ein neuer Status <code>newstatus</code>
/// übergeben.
void CResearchInfo::ChangeStatusOfComplex(BYTE newstatus)
{
	m_ResearchComplex[m_iCurrentComplex].m_byComplexStatus = newstatus;
	// Wenn der neue Status RESEARCHED ist
	if (newstatus == RESEARCHED)
	{
		// Alle drei möglichen Gebiete durchgehen
		for (int i = 0; i < 3; i++)
			// Wenn gerade das Gebiet i erforscht wird
			if (m_ResearchComplex[m_iCurrentComplex].m_byFieldStatus[i] == RESEARCHING)
			{
				// alle anderen Gebiete auf "nicht erforscht" setzen
				memset(m_ResearchComplex[m_iCurrentComplex].m_byFieldStatus, NOTRESEARCHED, 3);
				// gerade erforschtes Gebiet auf "erforscht" setzen
				m_ResearchComplex[m_iCurrentComplex].m_byFieldStatus[i] = RESEARCHED;
				break;
			}
		// kein Komplex mehr ausgewählt
		m_iCurrentComplex = -1; 
	}
}

/// Diese Funktion wählt eine der drei Möglichkeiten der Uniqueforschung aus. Dafür muss man das Gebiet, welches
/// erforscht werden soll mit dem Parameter <code>possibility</code> übergeben. Genaueres steht in der Definition
/// dieser Funktion.
void CResearchInfo::SetUniqueResearchChoosePossibility(BYTE possibility)
{
	/*
		für die 1. Wahlmöglichkeit	-> possibility == 1
		für die 2. Wahlmöglichkeit	-> possibility == 2
		für die 3. Wahlmöglichkeit	-> possibility == 3
	*/
	if (m_iCurrentComplex != -1 && m_bChoiceTaken == FALSE)
	{
		// alle Gebiete erstmal auf nicht erforscht setzen
		memset(m_ResearchComplex[m_iCurrentComplex].m_byFieldStatus, NOTRESEARCHED, 3);
		// gewähltes Gebiet auf "wird erforscht" setzen
		m_ResearchComplex[m_iCurrentComplex].m_byFieldStatus[possibility-1] = RESEARCHING;
		m_bChoiceTaken = TRUE;
	}
}

/// Diese Funktion ermittelt den Namen und die Beschreibung einer bestimmten Technologie, an der gerade geforscht
/// wird. Dies wird in den Attributen <code>m_strTechName</code> und <code>m_strTechDescription</code> gespeichert.
/// Als Parameter müssen dafür die jeweilige Technologie <code>tech</code> und die Stufe <code>level</code>, die
/// aktuell erforscht wird übergeben werden.
void CResearchInfo::SetTechInfos(BYTE tech, BYTE level)
{
	m_strTechName[tech] = "";
	m_strTechDescription[tech] = "";
	
	CResearchInfo::GetTechInfos(tech, level, m_strTechName[tech], m_strTechDescription[tech]);
}

/// Diese Funktion ermittelt den Namen und die Beschreibung einer bestimmten Technologie
/// Dies wird in den Parametern <code>m_sTechName</code> und <code>m_sTechDesc</code> gespeichert.
/// Als Parameter müssen dafür die jeweilige Technologie <code>tech</code> und die Stufe <code>level</code>
/// übergeben werden.
void CResearchInfo::GetTechInfos(BYTE tech, BYTE level, CString& sTechName, CString& sTechDesc)
{
	if (level > NoTL)
		return;

	int i = 0;
	int j = tech * 22 + level * 2;
	CString csInput;											// auf csInput wird die jeweilige Zeile gespeichert
	CString fileName=*((CBotf2App*)AfxGetApp())->GetPath() + "Data\\Names\\Techs.data";		// Name des zu öffnenden Files 
	CStdioFile file;											// Varibale vom Typ CStdioFile
	if (file.Open(fileName, CFile::shareDenyNone | CFile::modeRead | CFile::typeText))	// Datei wird geöffnet
	{
		while (file.ReadString(csInput))
		{
			if (i == j)
				sTechName = csInput;
			else if (i-1 == j)
			{
				sTechDesc = csInput;
				break;
			}
			i++;
		}
	}
	else
	{	
		MYTRACE(MT::LEVEL_ERROR, "Could not open file \"Techs.data\"...\n");
		AfxMessageBox("ERROR! Could not open file \"Techs.data\"...\n(Maybe check your installation directory...)");
	}
	// Datei wird geschlossen
	file.Close();
}