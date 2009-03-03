// FileReader.cpp: Implementierung der Klasse CFileReader.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "stdafx.h"
#include "FileReader.h"
#include "array_sort.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

CFileReader::CFileReader()
{


}

CFileReader::~CFileReader()
{

}

void CFileReader::ReadDataFromFile(CArray<CMinorRace,CMinorRace>* m_MinorInfos)
{
	int i = 0;
	if (m_MinorInfos != NULL)
		m_MinorInfos->RemoveAll();
	CMinorRace info;
	CString data[14];
	CString csInput;					// auf csInput wird die jeweilige Zeile gespeichert
	CString fileName="MinorRaces.data";	// Name des zu Öffnenden Files 
	CStdioFile file;											// Varibale vom Typ CStdioFile
	if (file.Open(fileName, CFile::modeRead | CFile::typeBinary))	// Datei wird geöffnet
	{
		while (file.ReadString(csInput))
		{
			data[i++] = csInput;
			if (i == 14)
			{
				i = 0;
				// Doppelpunkt bei Namen des Heimatsystems entferneen
				if (!data[0].IsEmpty())
				{
					data[0].Delete(data[0].GetLength()-1);
					// Den ersten Buchstaben merken, den Rest klein schreiben
					char first = data[0].GetAt(0);
					data[0].MakeLower();
					data[0].SetAt(0,first);
				}
				info.SetHomePlanet(data[0]);
				info.SetRaceName(data[1]);
				info.SetRaceDescription(data[2]);
				info.SetGraphicName(data[3]);
				for (int j = HUMAN; j <= DOMINION; j++)
					info.SetRelationship(j,atoi(data[j+3]));
				info.SetTechnologicalProgress(atoi(data[10]));
				info.SetKind(atoi(data[11]));
				info.SetSpaceflightNation(atoi(data[12]));
				info.SetCorruptibility(atoi(data[13]));
				m_MinorInfos->Add(info);
			}
		}
	}
	else
	{	
		AfxMessageBox("Fehler! Datei \"MinorRaces.data\" kann nicht geöffnet werden...");
		exit(1);
	}
	file.Close();							// Datei wird geschlossen
}

void CFileReader::WriteDataToFile(CArray<CMinorRace,CMinorRace>* m_MinorInfos)
{
	// Sortierung
	c_arraysort<CArray<CMinorRace,CMinorRace>,CMinorRace>(*m_MinorInfos,sort_asc);
	int i = 0;
	CString s;
	CString fileName="MinorRaces.data";		// Name des zu Öffnenden Files 
	CStdioFile file;						// Varibale vom Typ CStdioFile
	if (file.Open(fileName, CFile::typeBinary | CFile::modeCreate | CFile::modeWrite))		// Datei wird geöffnet
	{
		for (i = 0; i < m_MinorInfos->GetSize(); i++)
		{
			// Systemnamen groß schreiben und Doppelpunkt an das Ende stellen
			s.Format("%s:\n",m_MinorInfos->GetAt(i).GetHomeplanetName());
			s.MakeUpper();
			file.WriteString(s);
			s.Format("%s\n",m_MinorInfos->GetAt(i).GetRaceName());
			file.WriteString(s);
			s.Format("%s\n",m_MinorInfos->GetAt(i).GetRaceDescription());
			file.WriteString(s);
			s.Format("%s\n", m_MinorInfos->GetAt(i).GetGraphicName());
			file.WriteString(s);
			for (int j = HUMAN; j <= DOMINION; j++)
			{
				s.Format("%d\n",m_MinorInfos->GetAt(i).GetRelationshipToMajorRace(j));
				file.WriteString(s);				
			}
			s.Format("%d\n",m_MinorInfos->GetAt(i).GetTechnologicalProgress());
			file.WriteString(s);
			s.Format("%d\n",m_MinorInfos->GetAt(i).GetKind());
			file.WriteString(s);
			s.Format("%d\n",m_MinorInfos->GetAt(i).GetSpaceflightNation());
			file.WriteString(s);
			s.Format("%d\n",m_MinorInfos->GetAt(i).GetCorruptibility());
			file.WriteString(s);
		}
	}
	else
	{	
		AfxMessageBox("Fehler! Datei \"MinorRaces.data\" kann nicht geschrieben werden...");
		exit(1);
	}
	file.Close();							// Datei wird geschlossen

	// Jetzt noch das RacePlanetNames.data schreiben
	fileName.Format("RacePlanetNames.data");
	if (file.Open(fileName, CFile::typeText | CFile::modeCreate | CFile::modeWrite))		// Datei wird geöffnet
	{
		for (i = 0; i < m_MinorInfos->GetSize(); i++)
		{
			s.Format("%s\n",m_MinorInfos->GetAt(i).GetHomeplanetName());
			file.WriteString(s);			
		}
	}
	else
	{	
		AfxMessageBox("Fehler! Datei \"RacePlanetNames.data\" kann nicht geschrieben werden...");
		exit(1);
	}
	file.Close();							// Datei wird geschlossen


}