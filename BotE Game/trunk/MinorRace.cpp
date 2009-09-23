// MinorRace.cpp: Implementierung der Klasse CMinorRace.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Botf2.h"
#include "MinorRace.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_SERIAL (CMinorRace, CObject, 1)

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

CMinorRace::CMinorRace(void)
{
	this->Reset();
}

CMinorRace::~CMinorRace(void)
{

}

//////////////////////////////////////////////////////////////////////
// Kopierkonstruktor
//////////////////////////////////////////////////////////////////////
CMinorRace::CMinorRace(const CMinorRace & rhs)
{
	m_Attributes = rhs.m_Attributes;
	m_pKO = rhs.m_pKO;
	m_strRaceName = rhs.m_strRaceName;
	m_strHomePlanet = rhs.m_strHomePlanet;
	m_strRaceDescription = rhs.m_strRaceDescription;
	m_strGraphicName = rhs.m_strGraphicName;
	m_iTechnologicalProgress = rhs.m_iTechnologicalProgress;
	m_iKind = rhs.m_iKind;
	m_iCorruptibility = rhs.m_iCorruptibility;
	for (int i = 0; i < 6; i++)
	{
		m_byMajorRelations[i] = rhs.m_byMajorRelations[i];
		m_iDiplomacyStatus[i] = rhs.m_iDiplomacyStatus[i];
		m_iRelationPoints[i] = rhs.m_iRelationPoints[i];
	}
	for (int i = 0; i < 5; i++)
		m_iResourceStorage[i] = rhs.m_iResourceStorage[i];
}

//////////////////////////////////////////////////////////////////////
// Zuweisungsoperator
//////////////////////////////////////////////////////////////////////
CMinorRace & CMinorRace::operator=(const CMinorRace & rhs)
{
	if (this == &rhs)
		return *this;
	m_Attributes = rhs.m_Attributes;
	m_pKO = rhs.m_pKO;
	m_strRaceName = rhs.m_strRaceName;
	m_strHomePlanet = rhs.m_strHomePlanet;
	m_strRaceDescription = rhs.m_strRaceDescription;
	m_strGraphicName = rhs.m_strGraphicName;
	m_iTechnologicalProgress = rhs.m_iTechnologicalProgress;
	m_iKind = rhs.m_iKind;
	m_iCorruptibility = rhs.m_iCorruptibility;
	for (int i = 0; i < 6; i++)
	{
		m_byMajorRelations[i] = rhs.m_byMajorRelations[i];
		m_iDiplomacyStatus[i] = rhs.m_iDiplomacyStatus[i];
		m_iRelationPoints[i] = rhs.m_iRelationPoints[i];
	}
	for (int i = 0; i < 5; i++)
		m_iResourceStorage[i] = rhs.m_iResourceStorage[i];
	return *this;
}

///////////////////////////////////////////////////////////////////////
// Speichern / Laden
///////////////////////////////////////////////////////////////////////
void CMinorRace::Serialize(CArchive &ar)		
{
	CObject::Serialize(ar);
	// wenn gespeichert wird
	if (ar.IsStoring())
	{
		ar << m_Attributes;
		ar << m_pKO;
		ar << m_strHomePlanet;
		for (int i = 0; i < 6; i++)
		{
			ar << m_byMajorRelations[i];
			ar << m_iDiplomacyStatus[i];
			ar << m_iRelationPoints[i];
		}
		for (int i = TITAN; i <= IRIDIUM; i++)
			ar << m_iResourceStorage[i];
	}
	// wenn geladen wird
	if (ar.IsLoading())
	{
		ar >> m_Attributes;
		ar >> m_pKO;
		ar >> m_strHomePlanet;
		for (int i = 0; i < 6; i++)
		{
			ar >> m_byMajorRelations[i];
			ar >> m_iDiplomacyStatus[i];
			ar >> m_iRelationPoints[i];
		}
		for (int i = TITAN; i <= IRIDIUM; i++)
			ar >> m_iResourceStorage[i];
		
		// Wenn geladen wird können feste Eigenschaften der Minorrace eingelesen werden
		int i = 0;
		CString read[14];
		CString search;
		search.Format("%s:",m_strHomePlanet);
		search.MakeUpper();
		CString csInput;												// auf csInput wird die jeweilige Zeile gespeichert
		CString fileName=*((CBotf2App*)AfxGetApp())->GetPath() + "Data\\Races\\MinorRaces.data";		// Name des zu Öffnenden Files 
		CStdioFile file;												// Varibale vom Typ CStdioFile
		if (file.Open(fileName, CFile::shareDenyNone | CFile::modeRead | CFile::typeBinary))	// Datei wird geöffnet
		{
			while (file.ReadString(csInput))
			{
				if (csInput == search || i > 0)	// Wenn wir in dem File die Rasse gefunden haben ("PLANETENNAME:" gefunden!)
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
			AfxMessageBox("ERROR! Could not open file \"MinorRaces.data\"...");
			exit(1);
		}
		file.Close();							// Datei wird geschlossen
		if (read[13].GetLength() > 1)
			AfxMessageBox("ERROR! wrong file or old version of \"MinorRaces.data\"...");
		// Jetzt die einzelnen Werte aus dem read-Feld in die Membervariablen schreiben
		m_strRaceName = read[1];
		m_strRaceDescription = read[2];
		m_strGraphicName = read[3];
		m_iTechnologicalProgress = atoi(read[10]);
		m_iKind = atoi(read[11]);
		m_iCorruptibility = atoi(read[13]);
	}
}

//////////////////////////////////////////////////////////////////////
// sonstige Funktionen
//////////////////////////////////////////////////////////////////////
void CMinorRace::GenerateMinorRace(const CString &HomeplanetName)
{
	int i = 0;
	CString read[14];
	m_strHomePlanet = HomeplanetName;
	CString search;
	search.Format("%s:", HomeplanetName);
	search.MakeUpper();
	CString csInput;											// auf csInput wird die jeweilige Zeile gespeichert
	CString fileName = *((CBotf2App*)AfxGetApp())->GetPath() + "Data\\Races\\MinorRaces.data";	// Name des zu Öffnenden Files 
	CStdioFile file;											// Varibale vom Typ CStdioFile
	if (file.Open(fileName, CFile::modeRead | CFile::typeBinary))	// Datei wird geöffnet
	{
		while (file.ReadString(csInput))
		{
			if (csInput == search || i > 0)	// Wenn wir in dem File die Rasse gefunden haben ("PLANETENNAME:" gefunden!)
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
		AfxMessageBox("ERROR! Could not open file \"MinorRaces.data\"...");
		exit(1);
	}
	file.Close();							// Datei wird geschlossen
	if (read[13].GetLength() > 1)
		AfxMessageBox("ERROR! wrong file or old version of \"MinorRaces.data\"...");
	// Jetzt die einzelnen Werte aus dem read-Feld in die Membervariablen schreiben
	m_strRaceName = read[1];
	m_strRaceDescription = read[2];
	m_strGraphicName = read[3];
	for (i = 0; i < 6; i++)
	{
		m_byMajorRelations[i] = atoi(read[4 + i]);
		// jetzt die Beziehungen zufällig modifizieren, damit bei jedem Spielstart nicht genau die gleichen Werte auftreten
		USHORT newRelationship = 0;
		for (int j = 0; j < 15; j++)
			newRelationship += rand()%((m_byMajorRelations[i] + 1) * 2);
		m_byMajorRelations[i] = newRelationship / 15;
		if (m_byMajorRelations[i] > 100)
			m_byMajorRelations[i] = 100;
	}
	m_iTechnologicalProgress = atoi(read[10]);
	m_iKind = atoi(read[11]);
	SetSpaceflightNation(atoi(read[12]));
	m_iCorruptibility = atoi(read[13]);
}

BYTE CMinorRace::GetRelationshipToMajorRace(USHORT race) const 
{
	ASSERT(race >= HUMAN && race <= DOMINION);
	return m_byMajorRelations[race-1];
}

void CMinorRace::SetRelationshipToMajorRace(USHORT race, short newRelationAdd)
{
	ASSERT(race >= HUMAN && race <= DOMINION);
	
	if (m_byMajorRelations[race-1] + newRelationAdd > 100)
		m_byMajorRelations[race-1] = 100;
	else if ((short)m_byMajorRelations[race-1] + newRelationAdd < 0)
		m_byMajorRelations[race-1] = 0;
	else
		m_byMajorRelations[race-1] += newRelationAdd;
}

// Funktion berechnet die neue Beziehung zu allen Rassen, nachdem sie von einer Rasse ein Latinumgeschenk
// bekommen hat.
void CMinorRace::GiveLatinumPresent(USHORT race, short diplomacyBonus, USHORT latinum)
{
	ASSERT(race >= HUMAN && race <= DOMINION);

	// race -> Rasse, die Latinumgeschenk gegeben hat
	// latinum -> Menge des Latinums (max. 5000)

	/* Berechnen wieviel das Latinum eigentlich wert ist, wenn wir die langjährigen Beziehungen zu anderen
	   Majorraces mit einberechnen. Das bedeutet, umso länger jemand anderes diese Rasse kennt, umso mehr
	   RelationPoints wird diese bei der Rasse haben und umso schwerer wird es für andere Rassen die Beziehung
	   zu dieser durch Geschenke zu verändern. Wir suchen uns also die Rasse, welche die meistens Relation-
	   Points bei dieser hat (außer die Geldgeberrasse) und subtrahieren von diesem Wert unsere Beziehungs-
	   punkte. Den dadurch erhaltenen Wert (nur positiv) ziehen wir vom latinum ab. 
	*/
	int relationPoints = 0;
	// höchsten Wert (außer eigenen) suchen
	for (int i = 0; i < 6; i++)
		if (race != i+1)
			if (m_iRelationPoints[i] > relationPoints)
				relationPoints = m_iRelationPoints[i];
	// eigene RelationPoints mit einberechnen. Weil wenn wir selber eine gute Beziehung zu der Rasse haben,
	// dann soll unser Geld auch was wert sein
	if (m_iRelationPoints[race-1] > relationPoints)
		relationPoints = 0;
	else
		relationPoints -= m_iRelationPoints[race-1];			
	// Grenzen einhalten
	if (relationPoints >= latinum)
		latinum = 1;
	else
		latinum -= relationPoints;
	/*CString s;
	s.Format("Geld: %d\nRelationPoints: %d\nFöd-relationpoints: %d\nFer-relationpoints: %d\nKli-relationpoints: %d\nRom-relationpoints: %d\nCar-relationpoints: %d\nDom-relationpoints: %d\n",
		latinum,relationPoints,m_iRelationPoints[0],m_iRelationPoints[1],m_iRelationPoints[2],m_iRelationPoints[3],m_iRelationPoints[4],m_iRelationPoints[5]);
	AfxMessageBox(s);*/

	// Berechnen welche obere Grenze wir durch die Menge Latinum bekommen
	// Der berechnete Wert ist zufällig, innerhalb einiger Parameter, die durch latinum vorgegeben sind
	USHORT randomMoney = 0;
	// Schleife von 3 Durchläufen -> Zufallsvariablendurchnschitt, damit wir bei 5000 Credits nur sehr unwahrscheinlich
	// mal nen Wert von unter 1000 oder so bekommen
	for (int i = 0; i < 3; i++)
		randomMoney += (rand()%(latinum*2)+1);
	randomMoney = (USHORT)(randomMoney/3);

	// Wert, den wir mit randomMoney erreichen müssen, um die Beziehung zu verbessern
	USHORT neededValue = DIPLOMACY_PRESENT_VALUE;	// 200 ist "normal"
	if (m_iKind == WARLIKE)
		neededValue += 100;		// bei ner kriegerischen Minorrace ist der Wert höher
	else if (m_iKind == FINANCIAL)
		neededValue -= 100;		// bei ner finanziellen Minorrace ist der Wert niedriger

	/*	Jetzt neededValue noch modifizieren mit der aktuellen Beziehung der Geldgeberrasse.
		Alles über 50% wird auf die neededValue draufgerechnet, alles unter 50% wird von
		neededValue abgezogen. Außerdem werden noch die diplomatischen Fähigkeiten der
		einzelnen Majorraces mit einbezogen. So ist neededValue bei der Föderation nur 90%
		des normalen Wertes, während er bei den Romulanern 110% beträgt.
	*/

	neededValue += m_byMajorRelations[race-1] - 50;
	neededValue = USHORT(neededValue * (100 - diplomacyBonus) / 100);
	
	/*	Jetzt berechnen, um wieviel sich die Beziehung verändern würde */
	float bonus = 0.0f;
	if (randomMoney >= neededValue)
		bonus = randomMoney / neededValue;	// Wieviel Prozent bekommen wir als Bonus dazu

	/*	Jetzt den Modifikator der Bestechlichkeit der kleinen Rasse mit einbeziehen. Das bedeutet,
		ob der bonus so bleibt oder nicht. Bei großer Resistenz wird bonus nochmal verringert und
		wenn die Rasse sich von Geld gut beeinflussen läßt, dann wird bonus sogar erhöht
	*/
	switch (m_iCorruptibility)
	{
	case 0:	// kaum
		{
			bonus *= 0.5f;
			break;
		}
	case 1:	// wenig
		{
			bonus *= 0.75f;
			break;
		}
	// Bei 2 verändert sich ja nichts
	case 3:	// viel
		{
			bonus *= 1.25f;
			break;
		}
	case 4:
		{
			bonus *= 1.5f;
			break;
		}
	}

	/*	Nun den bonus runden und wir haben den Prozentwert, um wieviel sich die Beziehung verbessert. */
	m_byMajorRelations[race-1] += (USHORT)bonus;
	if (m_byMajorRelations[race-1] > 100)
	{
		bonus -= m_byMajorRelations[race-1] - 100;
		m_byMajorRelations[race-1] = 100;
	}
	
	// Jetzt die Funktion aufrufen, die die Beziehung zu den anderen Rassen um den
	// Prozentwert verringert, den die Geldgeberrasse bekommen hat.
	this->CalculateDiplomaticFalloutFromPresent(race, (USHORT)bonus);
}


/*	Funktion berechnet wieviel unsere gegebenen Ressourcen in Latinum wert sind.
	Am Ende dieser Funktion wird die Fkt. GiveLatinumPresent mit dem berechneten
	Wert des Latinums aufgerufen bzw. wenn es ein Bestechungsversuch ist wird
	der Wert zurückgegeben	
*/
USHORT CMinorRace::GiveResourcePresent(USHORT race, USHORT* resource, BOOLEAN corruption)
{
	ASSERT(race >= HUMAN && race <= DOMINION);

	/*	Titan wird getauscht im Verhältnis 1000 zu 200
		Deuterium wird getauscht im Verhältnis 1000 zu 225
		Duranium wird getauscht im Verhältnis 1000 zu 250
		Crystal wird getauscht im Verhältnis 1000 zu 300
		Iridium wird getauscht im Verhältnis 1000 zu 400	*/
	double value = 0;
	double div = 0;
	USHORT whichResource;
	if (resource[0] != 0)		// Titan übergeben?
	{
		value = resource[0]/5;
		whichResource = 0;
		div = m_iResourceStorage[whichResource] / 2000;
		m_iResourceStorage[0] += resource[0];
	}
	else if (resource[1] != 0)	// Deuterium übergeben?
	{
		value = resource[1]/4.5;
		whichResource = 1;
		div = m_iResourceStorage[whichResource] / 2000;
		m_iResourceStorage[1] += resource[1];
	}
	else if (resource[2] != 0)	// Duranium übergeben?
	{
		value = resource[2]/4;
		whichResource = 2;
		div = m_iResourceStorage[whichResource] / 2000;
		m_iResourceStorage[2] += resource[2];
	}
	else if (resource[3] != 0)	// Kristalle übergeben?
	{
		value = resource[3]/3.25;
		whichResource = 3;
		div = m_iResourceStorage[whichResource] / 2000;
		m_iResourceStorage[3] += resource[3];
	}
	else if (resource[4] != 0)	// Iridium übergeben?
	{
		value = resource[4]/2.5;
		whichResource = 4;
		div = m_iResourceStorage[whichResource] / 2000;
		m_iResourceStorage[4] += resource[4];
	}
	/*	So, nun gibts aber nicht immer diesen Betrag! Dafür gibt es
		Variablen, in denen steht, wieviel Rohstoffe in dem System der
		kleinen Rasse vorhanden/gelagert sind.

		Dafür berechnen wir einen Teiler. Wir nehmen die aktuelle
		Menge des jeweiligen Rohstoffes, die in dem System gelagert ist.
		Diese Menge teilen wir z.B. durch 2000 und erhalten einen Teiler.
		Mit diesem Teiler wird der erhaltene Wert des Rohstoffes in Latinum
		geteilt. Dieser nun erhaltene Wert gibt den letztendlichen
		Wert Latinum an, den die Rasse bekommt.	*/

	if (div < 1)
		div = 1;
	value = value / div;
	
	// Wert nochmal modifizieren, aufgrund der Rassenart
	if (m_iKind == PRODUCER)
		value = value * 1.5;
	else if (m_iKind == INDUSTRIAL)
		value = value * 1.35;
	else if (m_iKind == WARLIKE)
		value = value * 1.2;
	else if (m_iKind == FINANCIAL)
		value = value * 1.1;
	else if (m_iKind == SECRET)
		value = value * 0.9;
	else if (m_iKind == FARMER)
		value = value * 0.8;

	// Funktion aufrufen und den value Wert übergeben
	if (value > 0)
	{/*
		if (corruption == FALSE)
			this->GiveLatinumPresent(race,(USHORT)value);
		else
			return (USHORT)value;*/
		return (USHORT)value;
	}
	return 0;
}

// Funktion berechnet unseren Bestechungsversuch
BOOLEAN CMinorRace::TryCorruption(USHORT race, USHORT latinum, CEmpire* empires, USHORT corruptedMajor)
{
	ASSERT(race >= HUMAN && race <= DOMINION);

	/* Jetzt noch die angesammelten RelationPoints der corruptedMajor verringern. 
	   Somit haben wir hier die Möglichkeit, diese Punkte durch Bestechung zu verringern.
	   Bis jetzt wird der Wert noch durch nichts modifiziert. Mal schaun ob dies noch
	   kommt.
	   zum Algorithmus:
	   ich berechne latinum/15 und ziehe den erhaltenen Wert von den RelationPoints ab
	*/
	m_iRelationPoints[corruptedMajor-1] = m_iRelationPoints[corruptedMajor-1] - (latinum/15);
	if (m_iRelationPoints[corruptedMajor-1] < 0)
		m_iRelationPoints[corruptedMajor-1] = 0;

	/* Okay, der Betrag des Latinum kanns zwischen 0 und ca. 13.000 liegen
	   dieser zusätzliche Betrag kann die Bestechung erleichtern.
	   Bei erfolgreicher Bestechung wird der Vertrag einer Majorrace gecancelt
	   und ihre angesammelten RelationPoints verringern sich.
	   Bei nicht erfolgreicher Bestechung wird der Vertrag nicht gecancelt, aber
	   auch hier werden die RelationPoints gecancelt
	
	   zum Algorithmus:
	   Ich laufe latinum/250 mal die Schleife mit folgender Rechnung durch:
	   meine Beziehung wird ins Verhältnis zur Beziehung der corruptedMajor
	   gesetzt, d.h. Beziehung[race] - Beziehung[corruptedMajor].
	   Der erhaltene Wert wird auf rand()%100 addiert. Außerdem wird noch
	   latinum/750 draufgerechnet. Wenn dieser Gesamtwert
	   größer als 90 ist, dann war die Bestechung erfolgreich.
	   Aber nicht immer 90, sondern Wert ist noch abhängig von der Bestechlichkeit der
	   kleinen Rasse.
	*/
	short relationdiff = 0;
	short value = 0;
	USHORT corruptionValue = 90;
	switch (m_iCorruptibility)
	{
	case 0:	// kaum
		{
			corruptionValue = 80;
			break;
		}
	case 1:	// wenig
		{
			corruptionValue = 85;
			break;
		}
	// Bei 2 verändert sich ja nichts
	case 3:	// viel
		{
			corruptionValue = 95;
			break;
		}
	case 4:
		{
			corruptionValue = 100;
			break;
		}
	}
	for (int i = 0; i <= (USHORT)latinum/250; i++)
	{
		relationdiff = m_byMajorRelations[race-1] - m_byMajorRelations[corruptedMajor-1];
		value = rand()%100 + relationdiff + (short)latinum/750;
		// war die Bestechung erfolgreich?
		if (value > corruptionValue)
			break;		
	}
	if (value > corruptionValue)			
	{	// war erfolgreich
		CMessage message;
		CString s;
		switch (m_iDiplomacyStatus[corruptedMajor-1])
		{
			case TRADE_AGREEMENT:
				{
					m_iDiplomacyStatus[corruptedMajor-1] = NO_AGREEMENT;
					s = CResourceManager::GetString("CANCEL_TRADE_AGREEMENT", FALSE, m_strRaceName);
					message.GenerateMessage(s,DIPLOMACY,"",0,0);
					empires[corruptedMajor].AddMessage(message);
					break;
				}
			case FRIENDSHIP_AGREEMENT:
				{
					m_iDiplomacyStatus[corruptedMajor-1] = NO_AGREEMENT;
					s = CResourceManager::GetString("CANCEL_FRIENDSHIP", FALSE, m_strRaceName);
					message.GenerateMessage(s,DIPLOMACY,"",0,0);
					empires[corruptedMajor].AddMessage(message);
					break;
				}
			case COOPERATION:
				{
					m_iDiplomacyStatus[corruptedMajor-1] = NO_AGREEMENT;
					s = CResourceManager::GetString("CANCEL_COOPARATION", FALSE, m_strRaceName);
					message.GenerateMessage(s,DIPLOMACY,"",0,0);
					empires[corruptedMajor].AddMessage(message);
					break;
				}
			case AFFILIATION:
				{
					m_iDiplomacyStatus[corruptedMajor-1] = NO_AGREEMENT;
					s = CResourceManager::GetString("CANCEL_AFFILIATION", FALSE, m_strRaceName);
					message.GenerateMessage(s,DIPLOMACY,"",0,0);
					empires[corruptedMajor].AddMessage(message);
					break;
				}
			case MEMBERSHIP:
				{
					m_iDiplomacyStatus[corruptedMajor-1] = NO_AGREEMENT;
					s = CResourceManager::GetString("CANCEL_MEMBERSHIP", FALSE, m_strRaceName);
					message.GenerateMessage(s,DIPLOMACY,"",0,0);
					empires[corruptedMajor].AddMessage(message);
					break;
				}
		}
		s = CResourceManager::GetString("CORRUPTION_SUCCESS", FALSE, m_strRaceName);
		message.GenerateMessage(s,DIPLOMACY,"",0,0);
		empires[race].AddMessage(message);
		return TRUE;
	}
	else
	{	// war nicht erfolgreich
		CString s;
		CString major;
		CMessage message;
		// nicht immer bekommt die corruptedRace die Nachricht, sondern nur wenn siehe "if"
		if (value < m_byMajorRelations[corruptedMajor-1]-10)
		{
			switch (race)
			{
				case 1:	{major = CResourceManager::GetString("ARTICLE_RACE1_EMPIRE", TRUE); break;}
				case 2:	{major = CResourceManager::GetString("ARTICLE_RACE2_EMPIRE", TRUE); break;}
				case 3:	{major = CResourceManager::GetString("ARTICLE_RACE3_EMPIRE", TRUE); break;}
				case 4:	{major = CResourceManager::GetString("ARTICLE_RACE4_EMPIRE", TRUE); break;}
				case 5:	{major = CResourceManager::GetString("ARTICLE_RACE5_EMPIRE", TRUE); break;}
				case 6:	{major = CResourceManager::GetString("ARTICLE_RACE6_EMPIRE", TRUE); break;}
			}
			s = CResourceManager::GetString("TRYED_CORRUPTION", FALSE, major, m_strRaceName);
			message.GenerateMessage(s,DIPLOMACY,"",0,0);
			empires[corruptedMajor].AddMessage(message);
		}
		s = CResourceManager::GetString("CORRUPTION_FAILED", FALSE, m_strRaceName);
		message.GenerateMessage(s,DIPLOMACY,"",0,0);
		empires[race].AddMessage(message);
		return FALSE;
	}
}

// Funktion berechnet die diplomatischen Auswirkungen auf die anderen Rassen, wenn wir die Beziehung
// durch ein Geschenk z.B. verbessert haben.
// Diese Funktion wird am Ende von GiveLatinumPresent() aufgerufen und zieht die Prozentpunkte, die die
// Geldgeberrasse dazugewonnen hat von den anderen Rassen ab...
void CMinorRace::CalculateDiplomaticFalloutFromPresent(USHORT race, USHORT bonus)
{
	ASSERT(race >= HUMAN && race <= DOMINION);

	// wir ziehen den Bonuswert von den Bezihungen prozenzweise ab.
	// Es wird immer bei der schlechtesten Beziehung zuerst abgezogen, dann
	// bei der nächst schlechtesten usw.
	// Wenn wir keine Bonuswertpunkte mehr abziehen können, ist die Funktion beendet
	BYTE lowestValue = 100;
	short lastValue = -1;
	BOOLEAN changed = TRUE;
	/*CString s;
	s.Format("Bonus: %d\nFödbez: %d - %d\nFerbez: %d - %d\nKlibez: %d - %d\nRombez: %d - %d\nCarbez: %d - %d\nDombez: %d - %d",
		bonus,relations[0],m_bKnownByMajorRace[0],relations[1],m_bKnownByMajorRace[1],relations[2],m_bKnownByMajorRace[2],
		relations[3],m_bKnownByMajorRace[3],relations[4],m_bKnownByMajorRace[4],relations[5],m_bKnownByMajorRace[5]);
	AfxMessageBox(s);*/

	// Anzahl der anderen bekannten Rassen
	short knownRaces = 0;
	for (int i = HUMAN; i <= DOMINION; i++)
		if (GetKnownByMajorRace(i) == TRUE && i != race)
			knownRaces++;

	// Wenn die kleine Rasse nicht alle kennt, dann den bonus bisl kleiner machen.
	// weil wenn sie nur eine kennt, dann würde der alles abgezogen werden, ist vlt.
	// ein bissl zu krass
	if (knownRaces == 1)
		bonus = bonus / 2;
			
	while (bonus > 0 && changed == TRUE)
	{
		// kleinsten Beziehungswert suchen von bekannten Rassen, außer Geldgeberrasse
		for (int i = HUMAN; i <= DOMINION; i++)
			if (i != race && GetKnownByMajorRace(i) == TRUE && lowestValue > m_byMajorRelations[i-1] && lastValue < m_byMajorRelations[i-1])
				lowestValue = m_byMajorRelations[i];
		changed = FALSE;
		for (int i = HUMAN; i <= DOMINION; i++)
			if (m_byMajorRelations[i-1] == lowestValue && bonus > 0 && i != race && GetKnownByMajorRace(i) == TRUE)
			{
				if (m_byMajorRelations[i] > 0)
					m_byMajorRelations[i]--;
				bonus--;
				changed = TRUE;
			}
		lastValue = lowestValue;
		lowestValue = 100;
		if (changed == FALSE && knownRaces >= 0)
		{
			changed = TRUE;
			lastValue = -1;
			knownRaces--;
		}
	}	
}

// Funktion berechnet ob die Rasse unser diplomatisches Angebot (Freundschaft, Kooperation usw.) annimmt oder ablehnt.
// Wenn sie annimmt, dann ändert sich der Status, wenn sie ablehnt, dann könnte sich unsere Beziehung sogar verschlechtern
// Wenn das Angebot angenommen wurde liefert die Fkt. TRUE zurück, ansonsten FALSE
BOOLEAN CMinorRace::CalculateDiplomaticOffer(USHORT race, short offer, USHORT fromMinor)
{
	ASSERT(race >= HUMAN && race <= DOMINION);

	if (GetSubjugated())
		return FALSE;
	
	if (offer == WAR)
	{
		m_byMajorRelations[race-1] = 0;
		m_iDiplomacyStatus[race-1] = offer;
		return TRUE;
	}
	else if (offer == NO_AGREEMENT)	// entspricht den Vertrag aufzulösen
	{
		m_iDiplomacyStatus[race-1] = offer;
		return TRUE;
	}

	// Wenn das Angebot nicht beachtet wurde, d.h. nicht angenommen oder abgelehnt wurde
	if (fromMinor == NOT_REACTED)
		return FALSE;
	
	// Checken ob wir ein Angebot überhaupt machen können, z.B. wenn eine andere Hauptrasse
	// z.B. Mitgliedschaft mit einer Minorrace hat, dann können wir ihr kein Angebot machen, außer
	// Krieg erklären, Geschenke geben und Bestechen
	short status = 0;
	for (int i = 1; i <= 6; i++)
		if (i != race)
		{
			short temp = m_iDiplomacyStatus[i-1];
			if (temp > status)
				status = temp;
		}
	if ((offer == COOPERATION || offer == AFFILIATION || offer == MEMBERSHIP) && status > FRIENDSHIP_AGREEMENT)
		return FALSE;
	if (offer == TRADE_AGREEMENT && status > AFFILIATION)
		return FALSE;
	if (offer == FRIENDSHIP_AGREEMENT && status > COOPERATION)
		return FALSE;

	// Wenn wir das Angebot wahrmachen könnten, berechnen ob es klappt
	BYTE neededRelation;

	if (offer == TRADE_AGREEMENT)
	{
		switch(m_iKind)
		{
		case FINANCIAL: {neededRelation = 30; break;}
		case SECRET: {neededRelation = 50; break;}
		default: neededRelation = 40;
		}
	}
	else if (offer == FRIENDSHIP_AGREEMENT)
	{
		switch(m_iKind)
		{
		case PACIFIST: {neededRelation = 40; break;}
		case SECRET: {neededRelation = 60; break;}
		default: neededRelation = 50;
		}
	}
	else if (offer == COOPERATION)
	{
		switch(m_iKind)
		{
		case PACIFIST: {neededRelation = 60; break;}
		case RESEARCHER: {neededRelation = 60; break;}
		case SECRET: {neededRelation = 75; break;}
		default: neededRelation = 65;
		}
	}
	else if (offer == AFFILIATION)
	{
		switch(m_iKind)
		{
		case SECRET: {neededRelation = 95; break;}
		default: neededRelation = 85;
		}
	}
	else if (offer == MEMBERSHIP)
	{
		switch(m_iKind)
		{
		case PACIFIST: {neededRelation = 90; break;}
		case FARMER: {neededRelation = 95; break;}
		case SECRET: {neededRelation = 110; break;}
		default: neededRelation = 100;
		}
	}

	double multi = m_byMajorRelations[race-1] + 100;
	multi = multi/100;
	double value = m_byMajorRelations[race-1] * multi;
	USHORT forRandom = (USHORT)value;
	if (forRandom == 0)
		forRandom++;
	USHORT temp = 0;
	for (int i = 0; i < 5; i++)
		temp += rand()%forRandom+1;
	forRandom = (USHORT)(temp/5);
/*	CString s;
	s.Format("Beziehung: %d\nNeededRelation: %d\nMulti: %lf\nforRandom: %d",relations[race-1],neededRelation,multi,forRandom);
	AfxMessageBox(s);
*/	
	// Wenn wir den Status erfolgreich ändern oder die kleine Rasse selbst hat uns das Angebot gemacht
	if ((forRandom >= neededRelation || fromMinor == TRUE) && fromMinor != DECLINED)
	{
		// Wenn der aktuelle Vertrag nicht höherwertiger ist als der angebotene, dann diplomatischen Status ändern
		if (m_iDiplomacyStatus[race-1] >= offer)
			return FALSE;
		else
		{
			m_iDiplomacyStatus[race-1] = offer;
			return TRUE;
		}
	}
	// Wenn wir es nicht schaffen den Status zu ändern, so könnte sich die Beziehung verschlechtern
	else
	{
		short relationMali = 1+offer - m_iCorruptibility;
		if (relationMali > 0)
			relationMali = rand()%relationMali;
		else
			return FALSE;
		if ((m_byMajorRelations[race-1] - relationMali) < 0)
			m_byMajorRelations[race-1] = 0;
		else
			m_byMajorRelations[race-1] -= relationMali;
		return FALSE;
	}
}

/*	Funktion wird für jede Minorrace in der NextRound() Funktion der Doc-Klasse aufgerufen.
	Diese Funktion berechnet, ob die MinorRace der MajorRace ein Angebot aufgrund der 
	Beziehung macht, oder ob sie ihr sogar den Krieg erklärt
*/
short CMinorRace::PerhapsMakeOfferToMajorRace(BYTE race)
{
	if (GetKnownByMajorRace(race) == TRUE && GetSubjugated() == FALSE)
	{
		USHORT relation  = this->GetRelationshipToMajorRace(race);
		// jetzt zum Algorithmus:
		/*	Aufgrund der Beziehung zur Majorrace macht die Minorrace dieser vielleicht ein diplomatisches
			Angebot
		*/
		
		// Checken ob wir ein Angebot überhaupt machen können, z.B. wenn eine andere Hauptrasse
		// z.B. Mitgliedschaft mit der Minorrace hat, dann können wir ihr kein Angebot machen
		short status = 0;
		for (int i = 1; i <= 6; i++)
			if (i != race)
			{
				short temp = m_iDiplomacyStatus[i-1];
				if (temp > status)
					status = temp;
			}
		double multi = relation+100;
		multi = multi/100;
		double value = relation*multi;
		USHORT forRandom = (USHORT)value;
		if (forRandom == 0)
			forRandom++;
		USHORT temp = 0;
		for (int i = 0; i < 5; i++)
			temp += rand()%forRandom+1;
		forRandom = (USHORT)(temp/5);
		// Jetzt haben wir nen Randomwert, wenn dieser einen zufälligen Wert überschreitet, bietet die
		// Minorrace zu 33% etwas an
		short offer = 0;
		if (forRandom > 125 && m_iDiplomacyStatus[race-1] < MEMBERSHIP && status <= FRIENDSHIP_AGREEMENT)
			offer = MEMBERSHIP;
		else if (forRandom > 110 && m_iDiplomacyStatus[race-1] < AFFILIATION && status <= FRIENDSHIP_AGREEMENT)
			offer = AFFILIATION;
		else if (forRandom > 95 && m_iDiplomacyStatus[race-1] < COOPERATION && status <= FRIENDSHIP_AGREEMENT)
			offer = COOPERATION;
		else if (forRandom > 80 && m_iDiplomacyStatus[race-1] < FRIENDSHIP_AGREEMENT && status <= COOPERATION)
			offer = FRIENDSHIP_AGREEMENT;
		else if (forRandom > 60 && m_iDiplomacyStatus[race-1] < TRADE_AGREEMENT && status <= AFFILIATION)
			offer = TRADE_AGREEMENT;
		
		if (m_iDiplomacyStatus[race-1] >= offer)
			offer = 0;

		// Das könnte man noch von der Art der Rasse abhängig machen, kriegerische Rassen erklären eher Krieg,
		// pazifistische Rasse erklären gar keinen Krieg oder so ähnlich -> hier gemacht
		USHORT minRel = 10;
		if (m_iKind == WARLIKE)			// Wir lieben Krieg
			minRel = 35;
		else if (m_iKind == FINANCIAL)	// Krieg ist gut fürs Geschäft
			minRel = 20;
		if (forRandom < 3 && relation < minRel && m_iDiplomacyStatus[race-1] != WAR && m_iKind != PACIFIST)
			offer = WAR;																	// nie bei Pazifisten
		
		// hier Algo um zu 33% was anzubieten
		forRandom = rand()%3;
		if (forRandom > 0)
			offer = 0;
		return offer;
	}
	return 0;
}

// Fkt. checkt die diplomatische Konsistenz und generiert bei Kündigungen auch die entsprechende Nachricht
void CMinorRace::CheckDiplomaticConsistence(CEmpire* empires)
{
	for (int i = 0; i < 6; i++)
	{
		// Wenn die Minorrace unterworfen wurde, so wird jeglicher Vertrag gekündigt
		if (GetSubjugated())
		{
			CString s = "";
			switch (m_iDiplomacyStatus[i])
			{
				case TRADE_AGREEMENT: {s = CResourceManager::GetString("CANCEL_TRADE_AGREEMENT", FALSE, m_strRaceName); break;}
				case FRIENDSHIP_AGREEMENT: {s = CResourceManager::GetString("CANCEL_FRIENDSHIP", FALSE, m_strRaceName);	break;}
				case COOPERATION: {s = CResourceManager::GetString("CANCEL_COOPERATION", FALSE, m_strRaceName);	break;}
				case AFFILIATION: {s = CResourceManager::GetString("CANCEL_AFFILIATION", FALSE, m_strRaceName);	break;}
				case MEMBERSHIP: {s = CResourceManager::GetString("CANCEL_MEMBERSHIP", FALSE, m_strRaceName); break;}
			}
			if (m_iDiplomacyStatus[i] != WAR)
			{
				m_iDiplomacyStatus[i] = NO_AGREEMENT;
				if (!s.IsEmpty())
				{
					CMessage message;
					message.GenerateMessage(s,DIPLOMACY,"",0,0);
					empires[i+1].AddMessage(message);
				}
			}
		}
		if (m_iDiplomacyStatus[i] == MEMBERSHIP)
			for (int j = 0; j < 6; j++)
			{
				if (m_iDiplomacyStatus[j] == TRADE_AGREEMENT)
				{
					m_iDiplomacyStatus[j] = NO_AGREEMENT;
					CMessage message;
					CString s = CResourceManager::GetString("CANCEL_TRADE_AGREEMENT", FALSE, m_strRaceName);
					message.GenerateMessage(s,DIPLOMACY,"",0,0);
					empires[j+1].AddMessage(message);
				}
				if (m_iDiplomacyStatus[j] == FRIENDSHIP_AGREEMENT)
				{
					m_iDiplomacyStatus[j] = NO_AGREEMENT;
					CMessage message;
					CString s = CResourceManager::GetString("CANCEL_FRIENDSHIP", FALSE, m_strRaceName);
					message.GenerateMessage(s,DIPLOMACY,"",0,0);
					empires[j+1].AddMessage(message);
				}
			}
		if (m_iDiplomacyStatus[i] == AFFILIATION)
			for (int j = 0; j < 6; j++)
				if (m_iDiplomacyStatus[j] == FRIENDSHIP_AGREEMENT)
				{
					m_iDiplomacyStatus[j] = NO_AGREEMENT;
					CMessage message;
					CString s = CResourceManager::GetString("CANCEL_FRIENDSHIP", FALSE, m_strRaceName);
					message.GenerateMessage(s,DIPLOMACY,"",0,0);
					empires[j+1].AddMessage(message);
				}
	}
}

// Fkt. checkt die Beziehungen zu den Hauptrasse, wenn diese zuweit von Vertrag abweicht könnte gekündigt werden
void CMinorRace::PerhapsCancelAgreement(CEmpire* empires)
{
	for (int i = 0; i < 6; i++)
		if (m_byMajorRelations[i] < m_iDiplomacyStatus[i]*12 && m_iDiplomacyStatus[i] >= 2 && m_iDiplomacyStatus[i] <= 6 && GetKnownByMajorRace(i+1) == TRUE)
		{
			// Wenn das zutrifft, dann nicht unbedingt kündigen, nur zu 20% wird gekündigt
			int perhaps = rand()%5;
			if (perhaps == 0)
			{
				// Jetzt wird gekündigt
				CString s;
				CMessage message;
				switch (m_iDiplomacyStatus[i])
				{
				case TRADE_AGREEMENT:
					{
						m_iDiplomacyStatus[i] = NO_AGREEMENT;
						s = CResourceManager::GetString("CANCEL_TRADE_AGREEMENT", FALSE, m_strRaceName);
						message.GenerateMessage(s,DIPLOMACY,"",0,0);
						empires[i+1].AddMessage(message);
						break;
					}
				case FRIENDSHIP_AGREEMENT:
					{
						m_iDiplomacyStatus[i] = NO_AGREEMENT;
						s = CResourceManager::GetString("CANCEL_FRIENDSHIP", FALSE, m_strRaceName);
						message.GenerateMessage(s,DIPLOMACY,"",0,0);
						empires[i+1].AddMessage(message);
						break;
					}
				case COOPERATION:
					{
						m_iDiplomacyStatus[i] = NO_AGREEMENT;
						s = CResourceManager::GetString("CANCEL_COOPERATION", FALSE, m_strRaceName);
						message.GenerateMessage(s,DIPLOMACY,"",0,0);
						empires[i+1].AddMessage(message);
						break;
					}
				case AFFILIATION:
					{
						m_iDiplomacyStatus[i] = NO_AGREEMENT;
						s = CResourceManager::GetString("CANCEL_AFFILIATION", FALSE, m_strRaceName);
						message.GenerateMessage(s,DIPLOMACY,"",0,0);
						empires[i+1].AddMessage(message);
						break;
					}
				case MEMBERSHIP:
					{
						m_iDiplomacyStatus[i] = NO_AGREEMENT;
						s = CResourceManager::GetString("CANCEL_MEMBERSHIP", FALSE, m_strRaceName);
						message.GenerateMessage(s,DIPLOMACY,"",0,0);
						empires[i+1].AddMessage(message);
						break;
					}
				}
			}
		}
}

/*	Funktion berechnet, wieviel Rohstoffe pro Runde (also bei jedem NextRound() aufrufen) von der
	Minorrace verbraucht werden. Wenn die Minorrace die Möglichkeit hat, den jeweiligen Rohstoff
	selbst abzubauen, damm ist der Verbrauch natürlich geringer. Um rauszukriegen, ob die Rasse
	den Rohstoff selbst abbauen zu können, wird der Sektor mitübergeben, weil in diesem die ganzen
	Planeten des Systems gespeichert sind.
*/
void CMinorRace::ConsumeRessources(CSector* sector)
{
	// bewohnbar sind:    C,F,G,H,K,L,M,N,O,P,Q,R
	BOOLEAN exist[5] = {0,0,0,0,0};
	sector->GetAvailableResources(exist);
	
	short div;
	unsigned int value;
	
	// Titan
	exist[0] == TRUE ? div = 1500 : div = 4000;
	value = rand()%div;
	if (value > 3000) value = 3000;
	m_iResourceStorage[0] -= value;
	if (m_iResourceStorage[0] < 0) m_iResourceStorage[0] = 0;
/*
	CString s;
	s.Format("Name der Rasse: %s\nTitanabbau: %d\nVerbrauch: %d\nLager: %d",m_strRaceName,exist[0],value,m_iRessourceStorage[0]);
	AfxMessageBox(s);
*/
	
	// Deuterium
	exist[1] == TRUE ? div = 1500 : div = 4000;
	value = rand()%div;
	if (value > 3000) value = 3000;
	m_iResourceStorage[1] -= value;
	if (m_iResourceStorage[1] < 0) m_iResourceStorage[1] = 0;

	// Duranium
	exist[2] == TRUE ? div = 1500 : div = 4000;
	value = rand()%div;
	if (value > 3000) value = 3000;
	m_iResourceStorage[2] -= value;
	if (m_iResourceStorage[2] < 0) m_iResourceStorage[2] = 0;

	// Kristalle
	exist[3] == TRUE ? div = 1500 : div = 4000;
	value = rand()%div;
	if (value > 3000) value = 3000;
	m_iResourceStorage[3] -= value;
	if (m_iResourceStorage[3] < 0) m_iResourceStorage[3] = 0;

	// Iridium
	exist[4] == TRUE ? div = 1500 : div = 4000;
	value = rand()%div;
	if (value > 3000) value = 3000;
	m_iResourceStorage[4] -= value;
	if (m_iResourceStorage[4] < 0) m_iResourceStorage[4] = 0;
}

// Funktion berechnet wieviele Beziehungspunkte eine Majorrace für längere Beziehungsdauer bekommt.
// Umso mehr Punkte sie hat, umso schwerer wird es für andere Majorraces diese Rasse durch Geschenke
// zu beeinflussen
void CMinorRace::CalculateRelationPoints()
{
	for (int i = 0; i < DOMINION; i++)
	{
		// bei keinem aktuell laufendem Vertrag verringern sich die gesammelten Punkte langsam
		if (m_iDiplomacyStatus[i] == NO_AGREEMENT)
			m_iRelationPoints[i] -= rand()%80+1;
		else if (m_iDiplomacyStatus[i] == FRIENDSHIP_AGREEMENT)
			m_iRelationPoints[i] += 10;
		else if (m_iDiplomacyStatus[i] == COOPERATION)
			m_iRelationPoints[i] += 20;
		else if (m_iDiplomacyStatus[i] == AFFILIATION)
			m_iRelationPoints[i] += 30;
		else if (m_iDiplomacyStatus[i] == MEMBERSHIP)
		{
			m_iRelationPoints[i] += 40;
			// bei einer Mitgliedschaft erhöht sich womoglich auch die Beziehung ein wenig
			this->SetRelationshipToMajorRace(i+1, rand()%2);
		}
		// bei Krieg werden alle Punkte gelöscht
		else if (m_iDiplomacyStatus[i] == WAR)
			m_iRelationPoints[i] = 0;
		// Obergrenze setzen, einfach mal auf 5000
		if (m_iRelationPoints[i] > 5000)
			m_iRelationPoints[i] = 5000;
		// wenn es unter null sein sollte
		if (m_iRelationPoints[i] < 0)
			m_iRelationPoints[i] = 0;
	}
}

/// Diese Funktion läßt die Minorrace mit einer gewissen Wahrscheinlichkeit einen weiteren Planeten in ihrem System
/// kolonisieren.
/// @param planets ist das Feld der Planeten in dem Sektor der Minorrace.
/// @return <code>true</code> wenn ein neuer Planet kolonisiert wurde, ansonsten <code>false</code>
BOOLEAN CMinorRace::PerhapsExtend(CArray<CPlanet>* planets)
{
	bool colonized = false;
	for (int i = 0; i < planets->GetSize(); i++)
	{
		// ist der Planet noch nicht geterraformt
		if (planets->GetAt(i).GetColonized() == FALSE && planets->GetAt(i).GetHabitable() == TRUE && planets->GetAt(i).GetTerraformed() == FALSE)
		{
			// mit einer gewissen Wahrscheinlichkeit wird der Planet geterraformt und kolonisiert
			if (rand()%200 >= (200 - (m_iTechnologicalProgress+1)))
			{
				colonized = true;
				planets->ElementAt(i).SetNeededTerraformPoints(planets->ElementAt(i).GetNeededTerraformPoints());
				planets->ElementAt(i).SetTerraformed(TRUE);
				planets->ElementAt(i).SetColonisized(TRUE);
				planets->ElementAt(i).SetIsTerraforming(FALSE);
				if (planets->GetAt(i).GetMaxHabitant() < 1.0f)
					planets->ElementAt(i).SetCurrentHabitant(planets->GetAt(i).GetMaxHabitant());
				else
					planets->ElementAt(i).SetCurrentHabitant(1.0f);
			}
		}
		// ist der Planet schon geterraformt
		else if (planets->GetAt(i).GetColonized() == FALSE && planets->GetAt(i).GetTerraformed() == TRUE)
		{
			// dann wird mit einer größeren Wahrscheinlichkeit kolonisiert
			if (rand()%200 >= (200 - 3*(m_iTechnologicalProgress+1)))
			{
				colonized = true;
				planets->ElementAt(i).SetColonisized(TRUE);
				if (planets->GetAt(i).GetMaxHabitant() < 1.0f)
					planets->ElementAt(i).SetCurrentHabitant(planets->GetAt(i).GetMaxHabitant());
				else
					planets->ElementAt(i).SetCurrentHabitant(1.0f);
			}
		}
	}
	return colonized;
}

void CMinorRace::Reset()
{
	m_Attributes = 0;
	m_pKO.x = m_pKO.y = 0;			// Koordinaten der Rasse
	m_strRaceName = "";				// Der Name der Rasse
	m_strHomePlanet = "";			// Der Name des Heimatplaneten der Minorrace
	m_strRaceDescription = "";		// Die Beschreibung der Minorrace
	m_strGraphicName = "";			// Der Name der zugehörigen Grafikdatei
	m_iTechnologicalProgress = 0;	// wie fortschrittlich ist die Minorrace?
	m_iKind = 0;					// Art der Minorrace (landwirtschaftlich, kriegerish, industriell usw.)
	m_iCorruptibility = 0;			// wie stark ändert sich die Beziehung beim Geschenke geben?
	for (int i = 0; i < 6; i++)
	{	
		m_byMajorRelations[i] = 0;
		m_iDiplomacyStatus[i] = NO_AGREEMENT;	// Diplomatischer Status gegenüber den Hauptrassen (z.B. Nichtangriffspakt, Krieg, Bündnis usw.)
		m_iRelationPoints[i] = 0;				// Punkte für längere gute Beziehung, umso mehr, so schwerer ist es für andere Majors die Rasse wegzukaufen
	}
	for (int i = TITAN; i <= IRIDIUM; i++)		// Menge der gelagerten Rohstoffe
		m_iResourceStorage[i] = 0;
}