// GenShipName.cpp: Implementierung der Klasse CGenShipName.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GenShipName.h"
#include "Botf2.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_SERIAL (CGenShipName, CObject, 1)
//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

CGenShipName::CGenShipName()
{
	// Standardnamen festlegen, alle Namen von Planeten werden aus Datei eingelesen
	CString csInput;						// auf csInput wird die jeweilige Zeile gespeichert
	CStdioFile file;						// Varibale vom Typ CStdioFile
	// Föderationsschiffnamen einlesen
	CString fileName=*((CBotf2App*)AfxGetApp())->GetPath() + "Data\\Names\\Race1ShipNames.data";	// Name des zu Öffnenden Files 
	if (file.Open(fileName, CFile::modeRead | CFile::typeText))	// Datei wird geöffnet
	{
		while (file.ReadString(csInput))
			m_ShipNames[HUMAN].Add(csInput);	// Konnte erfolgreich gelesen werden wird die jeweilige
	}
	else
	{	
		AfxMessageBox("Fehler! Datei \"Race1ShipNames.data\" kann nicht geöffnet werden...");
		exit(1);
	}
	file.Close();
	
	// Ferengi Schiffsnamen einlesen
	fileName=*((CBotf2App*)AfxGetApp())->GetPath() + "Data\\Names\\Race2ShipNames.data";	// Name des zu Öffnenden Files 
	if (file.Open(fileName, CFile::modeRead | CFile::typeText))	// Datei wird geöffnet
	{
		while (file.ReadString(csInput))
			m_ShipNames[FERENGI].Add(csInput);	// Konnte erfolgreich gelesen werden wird die jeweilige
	}
	else
	{	
		AfxMessageBox("Fehler! Datei \"Race2ShipNames.data\" kann nicht geöffnet werden...");
		exit(1);
	}
	file.Close();

	// Klingonische Schiffsnamen einlesen
	fileName=*((CBotf2App*)AfxGetApp())->GetPath() + "Data\\Names\\Race3ShipNames.data";	// Name des zu Öffnenden Files 
	if (file.Open(fileName, CFile::modeRead | CFile::typeText))	// Datei wird geöffnet
	{
		while (file.ReadString(csInput))
			m_ShipNames[KLINGON].Add(csInput);	// Konnte erfolgreich gelesen werden wird die jeweilige
	}
	else
	{	
		AfxMessageBox("Fehler! Datei \"Race3ShipNames.data\" kann nicht geöffnet werden...");
		exit(1);
	}
	file.Close();

	// Romulanische Schiffsnamen einlesen
	fileName=*((CBotf2App*)AfxGetApp())->GetPath() + "Data\\Names\\Race4ShipNames.data";	// Name des zu Öffnenden Files 
	if (file.Open(fileName, CFile::modeRead | CFile::typeText))	// Datei wird geöffnet
	{
		while (file.ReadString(csInput))
			m_ShipNames[ROMULAN].Add(csInput);	// Konnte erfolgreich gelesen werden wird die jeweilige
	}
	else
	{	
		AfxMessageBox("Fehler! Datei \"Race4ShipNames.data\" kann nicht geöffnet werden...");
		exit(1);
	}
	file.Close();

	// Cardassianische Schiffsnamen einlesen
	fileName=*((CBotf2App*)AfxGetApp())->GetPath() + "Data\\Names\\Race5ShipNames.data";	// Name des zu Öffnenden Files 
	if (file.Open(fileName, CFile::modeRead | CFile::typeText))	// Datei wird geöffnet
	{
		while (file.ReadString(csInput))
			m_ShipNames[CARDASSIAN].Add(csInput);	// Konnte erfolgreich gelesen werden wird die jeweilige
	}
	else
	{	
		AfxMessageBox("Fehler! Datei \"Race5ShipNames.data\" kann nicht geöffnet werden...");
		exit(1);
	}
	file.Close();

	// Dominion Schiffsnamen einlesen
	fileName=*((CBotf2App*)AfxGetApp())->GetPath() + "Data\\Names\\Race6ShipNames.data";	// Name des zu Öffnenden Files 
	if (file.Open(fileName, CFile::modeRead | CFile::typeText))	// Datei wird geöffnet
	{
		while (file.ReadString(csInput))
			m_ShipNames[DOMINION].Add(csInput);	// Konnte erfolgreich gelesen werden wird die jeweilige
	}
	else
	{	
		AfxMessageBox("Fehler! Datei \"Race6ShipNames.data\" kann nicht geöffnet werden...");
		exit(1);
	}
	file.Close();

	for (int i = NOBODY; i <= DOMINION; i++)
		m_iCounter[i] = 64;		// 65 == ASCII-Wert für 'A'; hier eins weniger
}

CGenShipName::~CGenShipName()
{
	for (int i = 0; i <= DOMINION; i++)
	{
		m_ShipNames[i].RemoveAll();
		m_UsedNames[i].RemoveAll();
	}
}

///////////////////////////////////////////////////////////////////////
// Speichern / Laden
///////////////////////////////////////////////////////////////////////
void CGenShipName::Serialize(CArchive &ar)		
{
	CObject::Serialize(ar);
	// wenn gespeichert wird
	if (ar.IsStoring())
	{
		for (int i = 0; i <= DOMINION; i++)
			ar << m_iCounter[i];
	}
	// wenn geladen wird
	else if (ar.IsLoading())
	{
		for (int i = 0; i <= DOMINION; i++)
		{
			ar >> m_iCounter[i];
			m_ShipNames[i].RemoveAll();
			m_UsedNames[i].RemoveAll();
		}
	}
	for (int i = 0; i <= DOMINION; i++)
	{		
		m_ShipNames[i].Serialize(ar);
		m_UsedNames[i].Serialize(ar);
	}
}

/// Diese Funktion generiert einen einmaligen Schiffsnamen. Als Parameter werden dafür die Rasse <code>race</code>
/// und ein Parameter, welcher angibt ob es sich um eine Station handelt <code>station</code> übergeben.
CString CGenShipName::GenerateShipName(BYTE race, BOOLEAN station)
{
	if (race == UNKNOWN)
		return "Minorship";
	if (m_ShipNames[race].GetSize() == 0)
	{
		for (int i = 0; i < m_UsedNames[race].GetSize(); i++)
			m_ShipNames[race].Add(m_UsedNames[race].GetAt(i));
		m_UsedNames[race].RemoveAll();
		if (m_ShipNames[race].GetSize() == 0)
			return "noname";
		m_iCounter[race]++;
		// Wenn der ASCII Wert vom Z erreicht wurde, so fängt es automatisch wieder mit A an
		if (m_iCounter[race] > 90)	// 90 entspricht Z
			m_iCounter[race] = 65;	// 65 entspricht A
	}
	int random = rand()%m_ShipNames[race].GetSize();
	CString name;		
	if (!station)
	{
		if (m_iCounter[race] > 64)
			name.Format("%s %c",m_ShipNames[race].GetAt(random),m_iCounter[race]);
		else
			name.Format("%s",m_ShipNames[race].GetAt(random));			
	}
	else
		name.Format("%s Station", m_ShipNames[race].GetAt(random));
	m_UsedNames[race].Add(m_ShipNames[race].GetAt(random));
	m_ShipNames[race].RemoveAt(random);
	return name;
}
