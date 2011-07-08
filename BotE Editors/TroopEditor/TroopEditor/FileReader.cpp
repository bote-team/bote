// FileReader.cpp: Implementierung der Klasse CFileReader.
//
//////////////////////////////////////////////////////////////////////

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

CString CFileReader::GetString(CString target)
{
	CString actualTarget;
	actualTarget.Format("%s%s%s","[",target,"]");
	CString csInput;					
	CString fileName="StringTable.txt";
	CStdioFile file;											
	if (file.Open(fileName, CFile::modeRead | CFile::typeText))
	{
		while(file.ReadString(csInput))
		{
			if(csInput==actualTarget) 
			{
				file.ReadString(csInput);
				file.Close();
				return csInput;
			}

		}

	}else{
		AfxMessageBox("Fehler! Datei \"Stringtable.txt\" kann nicht geöffnet werden...");
		exit(1);

	}
	file.Close();
	return target;
}

void CFileReader::ReadDataFromFile(CArray<CTroopInfo,CTroopInfo>* m_TroopInfos)
{
	int i = 0;
	if (m_TroopInfos != NULL)
		m_TroopInfos->RemoveAll();
	CTroopInfo info;
	CString data[20];
	CString csInput;					// auf csInput wird die jeweilige Zeile gespeichert
	CString fileName="Troops.data";	// Name des zu Öffnenden Files 
	CStdioFile file;											// Varibale vom Typ CStdioFile
	if (file.Open(fileName, CFile::modeRead | CFile::typeText))	// Datei wird geöffnet
	{
		while (file.ReadString(csInput))
		{
			data[i++] = csInput;
			if (i == 20)
			{
				i = 0;
				// Doppelpunkt bei Namen des Heimatsystems entferneen
				info.SetOwner(data[0]);
				info.SetName(data[1]);
				info.SetDescription(data[2]);
				info.SetPower(atoi(data[3]));
				info.SetMaintenanceCosts(atoi(data[4]));
				for (int j = 0; j < 6; j++)
					info.SetNeededTechlevel(j,atoi(data[5+j]));
				for (int j = TITAN; j < DILITHIUM;j++)
					info.SetNeededRessource(j,atoi(data[11+j]));
				info.SetNeededIndustry(atoi(data[16]));
				info.SetID(atoi(data[17]));
				info.SetSize(atoi(data[18]));
				info.SetMoralValue(atoi(data[19]));
				m_TroopInfos->Add(info);
			}
		}
	}
	else
	{	
		AfxMessageBox("Fehler! Datei \"Troop.data\" kann nicht geöffnet werden...");
		exit(1);
	}
	file.Close();							// Datei wird geschlossen
	c_arraysort<CArray<CTroopInfo,CTroopInfo>,CTroopInfo>(*m_TroopInfos,sort_asc);//sortieren
	
}

void CFileReader::WriteDataToFile(CArray<CTroopInfo,CTroopInfo>* m_TroopInfos)
{
	// Sortierung
	c_arraysort<CArray<CTroopInfo,CTroopInfo>,CTroopInfo>(*m_TroopInfos,sort_asc);
	int i = 0;
	CString s;
	CString fileName="Troops.data";		// Name des zu Öffnenden Files 
	CStdioFile file; // Varibale vom Typ CStdioFile
	if (file.Open(fileName, CFile::typeText | CFile::modeCreate | CFile::modeWrite))		// Datei wird geöffnet
	{
		for (i = 0; i < m_TroopInfos->GetSize(); i++)
		{
			
			s.Format("%s\n",m_TroopInfos->GetAt(i).GetOwner());
			file.WriteString(s);
			s.Format("%s\n",m_TroopInfos->GetAt(i).GetName());
			file.WriteString(s);
			s.Format("%s\n",m_TroopInfos->GetAt(i).GetDescription());
			file.WriteString(s);
			s.Format("%d\n",m_TroopInfos->GetAt(i).GetPower());
			file.WriteString(s);
			s.Format("%d\n",m_TroopInfos->GetAt(i).GetMaintenanceCosts());
			file.WriteString(s);
			for(int j=0;j<6;j++)
			{
				s.Format("%d\n",m_TroopInfos->GetAt(i).GetNeededTechlevel(j));
				file.WriteString(s);
			}
			for(int j=TITAN;j<DILITHIUM;j++)
			{
				s.Format("%d\n",m_TroopInfos->GetAt(i).GetNeededRessource(j));
				file.WriteString(s);
			}
			s.Format("%d\n",m_TroopInfos->GetAt(i).GetNeededIndustry());
			file.WriteString(s);
			s.Format("%d\n",m_TroopInfos->GetAt(i).GetID());
			file.WriteString(s);
			s.Format("%d\n",m_TroopInfos->GetAt(i).GetSize());
			file.WriteString(s);
			s.Format("%d\n",m_TroopInfos->GetAt(i).GetMoralValue());
			file.WriteString(s);
		}
	}
	else
	{	
		AfxMessageBox("Fehler! Datei \"Troop.data\" kann nicht geschrieben werden...");
		exit(1);
	}
	file.Close();							// Datei wird geschlossen

	


}