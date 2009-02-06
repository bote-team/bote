// GenDiploMessage.cpp: Implementierung der Klasse CGenDiploMessage.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Botf2.h"
#include "GenDiploMessage.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

CGenDiploMessage::CGenDiploMessage()
{

}

CGenDiploMessage::~CGenDiploMessage()
{

}

//////////////////////////////////////////////////////////////////////
// sonstige Funktionen
//////////////////////////////////////////////////////////////////////
CString CGenDiploMessage::GenerateMinorAnswer(const CString& minorRaceName, USHORT minorType, short type, BOOL accepted)
{
	CString answer = "";
	int i = 0;
	CString read[14];	// Anzahl der ganzen verschiedenen Nachrichten +1
	CString search;
	search.Format("%s:",minorRaceName);
	search.MakeUpper();
	CString search2;
	switch(minorType)
	{
	case NOTHING_SPECIAL:
		search2 = "NOTHING_SPECIAL:";
		break;
	case FINANCIAL:
		search2 = "FINANCIAL:";
		break;
	case WARLIKE:
		search2 = "WARLIKE:";
		break;
	case FARMER:
		search2 = "FARMER:";
		break;
	case INDUSTRIAL:
		search2 = "INDUSTRIAL:";
		break;
	case SECRET:
		search2 = "SECRET:";
		break;
	case RESEARCHER:
		search2 = "RESEARCHER:";
		break;
	case PRODUCER:
		search2 = "PRODUCER:";
		break;
	case PACIFIST:
		search2 = "PACIFIST:";
		break;
	case SNEAKY:
		search2 = "SNEAKY:";
		break;
	}
	CString csInput;													// auf csInput wird die jeweilige Zeile gespeichert
	CString fileName=*((CBotf2App*)AfxGetApp())->GetPath() + "Data\\Races\\MinorsDiploAnswers.data";	// Name des zu Öffnenden Files 
	CStdioFile file;													// Varibale vom Typ CStdioFile
	if (file.Open(fileName, CFile::modeRead | CFile::typeText))			// Datei wird geöffnet
	{
		while (file.ReadString(csInput))
		{
			// Wenn wir in dem File die Rasse gefunden haben ("RASSENNAME:" gefunden!)
			if (csInput == search || csInput == search2 || i > 0)
			{
				read[i] = csInput;
				i++;
			}
			if (i == 14)
				break;
		}
	}
	else
	{	
		AfxMessageBox("Fehler! Datei \"MinorsDiploAnswers.data\" kann nicht geöffnet werden...");
		exit(1);
	}
	file.Close();							// Datei wird geschlossen
	// Jetzt die einzelnen Werte aus dem read-Feld in die Membervariablen schreiben
	switch (type)
	{
	case TRADE_AGREEMENT:
		{
			if (accepted == TRUE)
				answer = read[1];
			else
				answer = read[7];
			break;
		}
	case FRIENDSHIP_AGREEMENT:
		{
			if (accepted == TRUE)
				answer = read[2];
			else
				answer = read[8];
			break;
		}
	case COOPERATION:
		{
			if (accepted == TRUE)
				answer = read[3];
			else
				answer = read[9];
			break;
		}
	case AFFILIATION:
		{
			if (accepted == TRUE)
				answer = read[4];
			else
				answer = read[10];
			break;
		}
	case MEMBERSHIP:
		{
			if (accepted == TRUE)
				answer = read[5];
			else
				answer = read[11];
			break;
		}
	case CORRUPTION:
		{
			if (accepted == TRUE)
				answer = read[6];
			else
				answer = read[12];
			break;
		}
	case WAR:
		{
			answer = read[13];
			break;
		}
	}
	return answer;
}

CString CGenDiploMessage::GenerateMinorOffer(const CString& minorRaceName, USHORT minorType, short type)
{
	CString answer = "";
	int i = 0;
	CString read[7];	// Anzahl der ganzen verschiedenen Nachrichten +1
	CString search;
	search.Format("%s:",minorRaceName);
	search.MakeUpper();
	CString search2;
	switch(minorType)
	{
	case NOTHING_SPECIAL:
		search2 = "NOTHING_SPECIAL:";
		break;
	case FINANCIAL:
		search2 = "FINANCIAL:";
		break;
	case WARLIKE:
		search2 = "WARLIKE:";
		break;
	case FARMER:
		search2 = "FARMER:";
		break;
	case INDUSTRIAL:
		search2 = "INDUSTRIAL:";
		break;
	case SECRET:
		search2 = "SECRET:";
		break;
	case RESEARCHER:
		search2 = "RESEARCHER:";
		break;
	case PRODUCER:
		search2 = "PRODUCER:";
		break;
	case PACIFIST:
		search2 = "PACIFIST:";
		break;
	case SNEAKY:
		search2 = "SNEAKY:";
		break;
	}
	CString csInput;													// auf csInput wird die jeweilige Zeile gespeichert
	CString fileName=*((CBotf2App*)AfxGetApp())->GetPath() + "Data\\Races\\MinorsDiploOffers.data";	// Name des zu Öffnenden Files 
	CStdioFile file;													// Varibale vom Typ CStdioFile
	if (file.Open(fileName, CFile::modeRead | CFile::typeText))			// Datei wird geöffnet
	{		
		while (file.ReadString(csInput))
		{
			// Wenn wir in dem File die Rasse gefunden haben ("RASSENNAME:" gefunden
			// oder ("RASSENTYP:" gefunden!)
			if (csInput == search || csInput == search2 || i > 0)
			{
				read[i] = csInput;
				i++;
			}
			if (i == 7)
				break;
		}
	}
	else
	{	
		AfxMessageBox("Fehler! Datei \"MinorsDiploOffers.data\" kann nicht geöffnet werden...");
		exit(1);
	}
	file.Close();							// Datei wird geschlossen
	// Jetzt die einzelnen Werte aus dem read-Feld in die Membervariablen schreiben
	switch (type)
	{
	case TRADE_AGREEMENT:
		{
			answer = read[1];
			break;
		}
	case FRIENDSHIP_AGREEMENT:
		{
			answer = read[2];
			break;
		}
	case COOPERATION:
		{
			answer = read[3];
			break;
		}
	case AFFILIATION:
		{
			answer = read[4];
			break;
		}
	case MEMBERSHIP:
		{
			answer = read[5];
			break;
		}
	case WAR:
		{
			answer = read[6];
			break;
		}
	}	
	return answer;
}

CString CGenDiploMessage::GenerateMajorAnswer(USHORT majorRaceNumber, short type, BOOL accepted)
{
	CString answer = "";
	int i = 0;
	CString read[18] = {""};	// Anzahl der ganzen verschiedenen Nachrichten +1
	CString search;
	search.Format("%s:",CMajorRace::GetRaceName(majorRaceNumber));
	search.MakeUpper();
	CString csInput;													// auf csInput wird die jeweilige Zeile gespeichert
	CString fileName=*((CBotf2App*)AfxGetApp())->GetPath() + "Data\\Races\\MajorsDiploAnswers.data";	// Name des zu Öffnenden Files 
	CStdioFile file;													// Varibale vom Typ CStdioFile
	if (file.Open(fileName, CFile::modeRead | CFile::typeText))			// Datei wird geöffnet
	{
		while (file.ReadString(csInput))
		{
			if (csInput == search || i > 0)	// Wenn wir in dem File die Rasse gefunden haben ("RASSENNAME:" gefunden!)
			{
				read[i] = csInput;
				i++;
			}
			if (i == 18)
				break;
		}
	}
	else
	{	
		AfxMessageBox("Fehler! Datei \"MajorsDiploAnswers.data\" kann nicht geöffnet werden...");
		exit(1);
	}
	if (read[0] == "")
	{
		CString s;
		s.Format("Fehler in Datei! Konnte %s nicht in MajorDiploAnswers.data finden", search);
		AfxMessageBox(s);
	}
	
	file.Close();							// Datei wird geschlossen
	// Jetzt die einzelnen Werte aus dem read-Feld in die Membervariablen schreiben
	switch (type)
	{
	case TRADE_AGREEMENT:
		{
			if (accepted == TRUE)
				answer = read[1];
			else
				answer = read[9];
			break;
		}
	case FRIENDSHIP_AGREEMENT:
		{
			if (accepted == TRUE)
				answer = read[2];
			else
				answer = read[10];
			break;
		}
	case COOPERATION:
		{
			if (accepted == TRUE)
				answer = read[3];
			else
				answer = read[11];
			break;
		}
	case AFFILIATION:
		{
			if (accepted == TRUE)
				answer = read[4];
			else
				answer = read[12];
			break;
		}
	case NON_AGGRESSION_PACT:
		{
			if (accepted == TRUE)
				answer = read[5];
			else
				answer = read[13];
			break;
		}
	case DEFENCE_PACT:
		{
			if (accepted == TRUE)
				answer = read[6];
			else
				answer = read[14];
			break;
		}
	case WAR_PACT:
		{
			if (accepted == TRUE)
				answer = read[7];
			else
				answer = read[15];
			break;
		}
	case DIP_REQUEST:
		{
			if (accepted == TRUE)
				answer = read[8];
			else
				answer = read[16];
			break;
		}
	case WAR:
		{
			answer = read[17];
			break;
		}
	}
	return answer;
}

CString CGenDiploMessage::GenerateMajorOffer(USHORT majorRaceNumber, short type, USHORT latinum, USHORT ressources[5], short duration)
{
	CString offer = "";
	int i = 0;
	CString read[13] = {""};	// Anzahl der ganzen verschiedenen Nachrichten +1
	CString search;
	search.Format("%s:",CMajorRace::GetRaceName(majorRaceNumber));
	search.MakeUpper();
	CString csInput;													// auf csInput wird die jeweilige Zeile gespeichert
	CString fileName=*((CBotf2App*)AfxGetApp())->GetPath() + "Data\\Races\\MajorsDiploOffers.data";	// Name des zu Öffnenden Files 
	CStdioFile file;													// Varibale vom Typ CStdioFile
	if (file.Open(fileName, CFile::modeRead | CFile::typeText))		// Datei wird geöffnet
	{
		while (file.ReadString(csInput))
		{
			if (csInput == search || i > 0)	// Wenn wir in dem File die Rasse gefunden haben ("RASSENNAME:" gefunden!)
			{
				read[i] = csInput;
				i++;
			}
			if (i == 13)
				break;
		}
	}
	else
	{	
		AfxMessageBox("Fehler! Datei \"MajorsDiploOffers.data\" kann nicht geöffnet werden...");
		exit(1);
	}
	if (read[0] == "")
	{
		CString s;
		s.Format("Fehler in Datei! Konnte %s nicht in MajorDiploOffers.data finden", search);
		AfxMessageBox(s);
	}
	file.Close();							// Datei wird geschlossen
	// Jetzt die einzelnen Werte aus dem read-Feld in die Membervariablen schreiben
	switch (type)
	{
		case TRADE_AGREEMENT:
		{
			offer = read[1];
			break;
		}
		case FRIENDSHIP_AGREEMENT:
		{
			offer = read[2];
			break;
		}
		case COOPERATION:
		{
			offer = read[3];
			break;
		}
		case AFFILIATION:
		{
			offer = read[4];
			break;
		}
		case MEMBERSHIP:
		{
			offer = read[5];
			break;
		}
		case NON_AGGRESSION_PACT:
		{
			offer = read[6];
			break;
		}
		case DEFENCE_PACT:
		{
			offer = read[7];
			break;
		}
		case WAR_PACT:
		{
			offer = read[8];
			CString temp;
			temp.Format("%s",CMajorRace::GetRaceName(duration));
			offer.Replace("$enemy$",temp);
			break;
		}
		case WAR:
		{
			offer = read[9];
			break;
		}
		case DIP_REQUEST:		// oder Corruption
		{
			CString temp;
			temp.Format("%d",latinum);
			if (duration == -1)
				offer = read[10];	// bei Bestechung einer Minorrace
			else
			{
				offer = read[11];   // bei Forderung an eine andere Majorrace
				CString res = "";
				if (ressources[TITAN] > 0)
				{
					CString s; s.Format("%d",ressources[TITAN]);
					res = CResourceManager::GetString("TITAN_REQUEST", FALSE, s);
				}
				else if (ressources[DEUTERIUM] > 0)
				{
					CString s; s.Format("%d",ressources[DEUTERIUM]);
					res = CResourceManager::GetString("DEUTERIUM_REQUEST", FALSE, s);
				}
				else if (ressources[DURANIUM] > 0)
				{
					CString s; s.Format("%d",ressources[DURANIUM]);
					res = CResourceManager::GetString("DURANIUM_REQUEST", FALSE, s);
				}
				else if (ressources[CRYSTAL] > 0)
				{
					CString s; s.Format("%d",ressources[CRYSTAL]);
					res = CResourceManager::GetString("CRYSTAL_REQUEST", FALSE, s);
				}
				else if (ressources[IRIDIUM] > 0)
				{
					CString s; s.Format("%d",ressources[IRIDIUM]);
					res = CResourceManager::GetString("IRIDIUM_REQUEST", FALSE, s);
				}
				offer.Replace("$ressource$",res);
			}
			offer.Replace("$latinum$",temp);
			break;
		}
		case PRESENT:
		{
			offer = read[12];
			CString temp;
			temp.Format("%d",latinum);
			offer.Replace("$latinum$",temp);
			break;
		}
	}
	// Wenn wir Latinum mit dazugeben
	if (latinum > 0 && type != PRESENT && type != DIP_REQUEST)
	{
		CString temp = offer;
		CString s; s.Format("%d",latinum);
		offer = CResourceManager::GetString("EXTRA_LATINUM", FALSE, temp, s);
	}
	// Wenn wir eine Ressource mit dazugeben
	if ((type != DIP_REQUEST && duration != -1) || duration == -1)
	{
		if (ressources[TITAN] > 0)
		{
			CString s; s.Format("%d",ressources[TITAN]);
			offer = CResourceManager::GetString("EXTRA_TITAN", FALSE, offer, s);
		}
		else if (ressources[DEUTERIUM] > 0)
		{
			CString s; s.Format("%d",ressources[DEUTERIUM]);
			offer = CResourceManager::GetString("EXTRA_DEUTERIUM", FALSE, offer, s);
		}
		else if (ressources[DURANIUM] > 0)
		{
			CString s; s.Format("%d",ressources[DURANIUM]);
			offer = CResourceManager::GetString("EXTRA_DURANIUM", FALSE, offer, s);
		}
		else if (ressources[CRYSTAL] > 0)
		{
			CString s; s.Format("%d",ressources[CRYSTAL]);
			offer = CResourceManager::GetString("EXTRA_CRYSTAL", FALSE, offer, s);
		}
		else if (ressources[IRIDIUM] > 0)
		{
			CString s; s.Format("%d",ressources[IRIDIUM]);
			offer = CResourceManager::GetString("EXTRA_IRIDIUM", FALSE, offer, s);
		}
	}
	if (duration != -1 && type != PRESENT && type != DIP_REQUEST && type != WAR && type != WAR_PACT)
	{
		CString temp;
		if (duration == 0)
			temp = CResourceManager::GetString("UNLIMITED_CONTRACT_DURATION", FALSE, offer);
		else
		{
			CString s; s.Format("%d",duration);	
			temp = CResourceManager::GetString("LIMITED_CONTRACT_DURATION", FALSE, offer, s);
		}
		offer = temp;
	}
	return offer;
}