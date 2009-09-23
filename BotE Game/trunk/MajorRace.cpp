// MajorRace.cpp: Implementierung der Klasse CMajorRace.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MajorRace.h"
#include "GenDiploMessage.h"
#include "Botf2.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_SERIAL (CMajorRace, CObject, 1)
//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

CMajorRace::CMajorRace(void)
{
	Reset();
}

CMajorRace::~CMajorRace(void)
{

}
//////////////////////////////////////////////////////////////////////
// Kopierkonstruktor
//////////////////////////////////////////////////////////////////////
CMajorRace::CMajorRace(const CMajorRace & rhs)
{
	m_Attributes = rhs.m_Attributes;
	m_iRaceNumber = rhs.m_iRaceNumber;
	m_iRaceProberty = rhs.m_iRaceProberty;
	m_iDiplomacyBonus = rhs.m_iDiplomacyBonus;
	for (int i = 0; i <= DOMINION; i++)
	{
		m_iRelationshipOtherMajor[i] = rhs.m_iRelationshipOtherMajor[i];
		m_iDiplomacyStatus[i] = rhs.m_iDiplomacyStatus[i];
		m_iDurationOfAgreement[i] = rhs.m_iDurationOfAgreement[i];
		m_iDurationOfDefencePact[i] = rhs.m_iDurationOfDefencePact[i];
	}
}

//////////////////////////////////////////////////////////////////////
// Zuweisungsoperator
//////////////////////////////////////////////////////////////////////
CMajorRace & CMajorRace::operator=(const CMajorRace & rhs)
{
	if (this == &rhs)
		return *this;
	m_Attributes = rhs.m_Attributes;
	m_iRaceNumber = rhs.m_iRaceNumber;
	m_iRaceProberty = rhs.m_iRaceProberty;
	m_iDiplomacyBonus = rhs.m_iDiplomacyBonus;
	for (int i = 0; i <= DOMINION; i++)
	{
		m_iRelationshipOtherMajor[i] = rhs.m_iRelationshipOtherMajor[i];
		m_iDiplomacyStatus[i] = rhs.m_iDiplomacyStatus[i];
		m_iDurationOfAgreement[i] = rhs.m_iDurationOfAgreement[i];
		m_iDurationOfDefencePact[i] = rhs.m_iDurationOfDefencePact[i];
	}
	return *this;
}

///////////////////////////////////////////////////////////////////////
// Speichern / Laden
///////////////////////////////////////////////////////////////////////
void CMajorRace::Serialize(CArchive &ar)		
{
	CObject::Serialize(ar);
	m_MoralObserver.Serialize(ar);
	// wenn gespeichert wird
	if (ar.IsStoring())
	{
		ar << m_Attributes;
		ar << m_iRaceNumber;
		ar << m_iRaceProberty;
		ar << m_iDiplomacyBonus;
		for (int i = 0; i < 7 ; i++)
		{
			ar << m_iRelationshipOtherMajor[i];	
			ar << m_iDiplomacyStatus[i];
			ar << m_iDurationOfAgreement[i];
			ar << m_iDurationOfDefencePact[i];
		}	
	}
	// wenn geladen wird
	if (ar.IsLoading())
	{
		ar >> m_Attributes;
		ar >> m_iRaceNumber;
		ar >> m_iRaceProberty;
		ar >> m_iDiplomacyBonus;
		for (int i = 0; i < 7 ; i++)
		{
			ar >> m_iRelationshipOtherMajor[i];	
			ar >> m_iDiplomacyStatus[i];
			ar >> m_iDurationOfAgreement[i];
			ar >> m_iDurationOfDefencePact[i];
		}			
	}
}

//////////////////////////////////////////////////////////////////////
// sonstige Funktionen
//////////////////////////////////////////////////////////////////////

/// Diese Funktion liefert den Namen des Grafikfiles für die Majorrace.
CString CMajorRace::GetGraphicFileName()
{
	CString read[5];
	ReadDataFromFile(read);
	
	read[0].MakeLower();
	CString firstLetter = (CString)read[0].GetAt(0);
	firstLetter.MakeUpper();
	read[0].Delete(0);
	read[0].Insert(0, firstLetter);
	return read[0];
}

/// Funktion gibt den Namen des Heimatsystems der Rasse zurück.
CString CMajorRace::GetHomeSystemName()
{
	CString read[5];
	this->ReadDataFromFile(read);
	return read[1];
}

/// Funktion gibt mir die Beschreibung der Hauptrasse zurück.
CString CMajorRace::GetRaceDescription()
{
	CString read[5];
	this->ReadDataFromFile(read);
	return read[2];
}

/// Funktion gibt die Anzahl dem Imperium bekannter MajorRaces zurück. Unser Imperium selbst wird nicht mitgezählt.
USHORT CMajorRace::GetNumberOfKnownMajorRaces(USHORT playersRace)
{
	USHORT number = 0;
	for (int i = HUMAN; i <= DOMINION; i++)
		if (GetKnownMajorRace(i) == TRUE && i != playersRace)
			number++;
	return number;			
}

/// Funktion liefert den gleichen Namen wie <code>GetGraphicFileName()</code>. Nur ist diese hier eine statische Funktion.
CString CMajorRace::GetRaceName(USHORT NumberOfRace)
{
	CString name;
	switch(NumberOfRace)
	{
	case HUMAN:		{return name = CResourceManager::GetString("RACE1"); break;}
	case FERENGI:	{return name = CResourceManager::GetString("RACE2"); break;}
	case KLINGON:	{return name = CResourceManager::GetString("RACE3"); break;}
	case ROMULAN:	{return name = CResourceManager::GetString("RACE4"); break;}
	case CARDASSIAN:{return name = CResourceManager::GetString("RACE5"); break;}
	case DOMINION:	{return name = CResourceManager::GetString("RACE6"); break;}
	}
	return name = "";
}

/// Funktion verringert die Anzahl der noch verbleibenden Runden der laufenden Verträge um eins.
/// Außer der Vertrag läuft auf unbestimmte Zeit.
BOOLEAN CMajorRace::DekrementDurationOfAllAgreements(CEmpire* EmpireOfMajor)
{
	BOOLEAN end = FALSE;
	/* Hier die Anzahl der Runden um eins verringern, außer der Wert steht schon auf
	   NULL, das bedeutet der Vertrag läuft auf unbestimmte Zeit -> unbegrenzt.
	   Wenn wir verringern und auf einen Wert von "1" kommen, dann wird der Vertrag aufgelöst
	   und eine Nachricht an unser Imperium versandt
	*/
	for (int i = HUMAN; i <= DOMINION; i++)
	{
		if (m_iDurationOfAgreement[i] > 1 && i != m_iRaceNumber) // Dauer 1 runterzählen
			m_iDurationOfAgreement[i]--;
		else if (m_iDurationOfAgreement[i] == 1 && i != m_iRaceNumber) // bei eins Vertrag auflösen und Nachricht senden
		{
			CString agreement;
			CString race;
			switch (m_iDiplomacyStatus[i])
			{
			case TRADE_AGREEMENT: {agreement = CResourceManager::GetString("TRADE_AGREEMENT"); break;}
			case FRIENDSHIP_AGREEMENT: {agreement = CResourceManager::GetString("FRIENDSHIP"); break;}
			case COOPERATION: {agreement = CResourceManager::GetString("COOPERATION"); break;}
			case AFFILIATION: {agreement = CResourceManager::GetString("AFFILIATION"); break;}
			case NON_AGGRESSION_PACT: {agreement = CResourceManager::GetString("NON_AGGRESSION"); break;}
			}
			switch (i)
			{
			case HUMAN: {race = CResourceManager::GetString("TO_RACE1_EMPIRE"); break;}
			case FERENGI: {race = CResourceManager::GetString("TO_RACE2_EMPIRE"); break;}
			case KLINGON: {race = CResourceManager::GetString("TO_RACE3_EMPIRE"); break;}
			case ROMULAN: {race = CResourceManager::GetString("TO_RACE4_EMPIRE"); break;}
			case CARDASSIAN: {race = CResourceManager::GetString("TO_RACE5_EMPIRE"); break;}
			case DOMINION: {race = CResourceManager::GetString("TO_RACE6_EMPIRE"); break;}
			}
			CString s = CResourceManager::GetString("CONTRACT_ENDED", FALSE, agreement, race);
			CMessage message;
			message.GenerateMessage(s,DIPLOMACY,"",0,FALSE);
			EmpireOfMajor->AddMessage(message);
			m_iDiplomacyStatus[i] = NO_AGREEMENT;
			m_iDurationOfAgreement[i] = 0;
			end = TRUE;
		}
		// Hier seperat die Dauer eines Verteidigungspaktes berechnen
		if (m_iDurationOfDefencePact[i] > 1 && i != m_iRaceNumber) // Dauer 1 runterzählen
			m_iDurationOfDefencePact[i]--;
		else if (m_iDurationOfDefencePact[i] == 1 && i != m_iRaceNumber) // bei eins Vertrag auflösen und Nachricht senden
		{
			CString race;
			switch (i)
			{
			case HUMAN: {race = CResourceManager::GetString("TO_RACE1_EMPIRE"); break;}
			case FERENGI: {race = CResourceManager::GetString("TO_RACE2_EMPIRE"); break;}
			case KLINGON: {race = CResourceManager::GetString("TO_RACE3_EMPIRE"); break;}
			case ROMULAN: {race = CResourceManager::GetString("TO_RACE4_EMPIRE"); break;}
			case CARDASSIAN: {race = CResourceManager::GetString("TO_RACE5_EMPIRE"); break;}
			case DOMINION: {race = CResourceManager::GetString("TO_RACE6_EMPIRE"); break;}
			}
			CString s = CResourceManager::GetString("DEFENCE_PACT_ENDED", FALSE, race);
			CMessage message;
			message.GenerateMessage(s,DIPLOMACY,"",0,FALSE);
			EmpireOfMajor->AddMessage(message);
			SetDefencePact(i, FALSE);
			m_iDurationOfDefencePact[i] = 0;
			end = TRUE;
		}
	}
	return end;
}

/// Funktion setzt die neue Beziehung zur Majorrace.
void CMajorRace::SetRelationshipToMajorRace(USHORT race, short newRelationAdd)
{
	if ((m_iRelationshipOtherMajor[race] + newRelationAdd) > 100)
		m_iRelationshipOtherMajor[race] = 100;
	else if ((m_iRelationshipOtherMajor[race] + newRelationAdd) < 0)
		m_iRelationshipOtherMajor[race] = 0;
	else
		m_iRelationshipOtherMajor[race] += newRelationAdd;
}

/// Funktion setzt den neuen Status zur Majorrace, wenn wir aber Krieg erklären, so wird der 
/// möglicherweise vorhandene Verteidigungspakt aufgehoben.
void CMajorRace::SetDiplomaticStatus(USHORT race, short newDiplomacyStatus)
{
	if (newDiplomacyStatus == DEFENCE_PACT) 
	{
		this->SetDefencePact(race,TRUE);
		if (m_iDiplomacyStatus[race] == WAR)
			newDiplomacyStatus = NO_AGREEMENT;
		else if (m_iDiplomacyStatus[race] == AFFILIATION)
			newDiplomacyStatus = AFFILIATION;
	}
	else if (newDiplomacyStatus == WAR_PACT)
		newDiplomacyStatus = WAR;
	else
		m_iDiplomacyStatus[race] = newDiplomacyStatus;	
	// Bei Krieg erlischt der Verteidigungspakt und bei einem Bündnis bekommen wir den automatisch
	if (newDiplomacyStatus == WAR || newDiplomacyStatus == AFFILIATION)
	{
		SetDefencePact(race, FALSE);
		m_iDurationOfDefencePact[race] = 0;
	}
	// Bei Krieg werden wird die Dauer eines alten Vertrages auf NULL gesetzt
	if (newDiplomacyStatus == WAR)
		m_iDurationOfAgreement[race] = 0;
}

/// Funktion setzt die Nummer der Rasse (HUMAN == 1 usw.) und ihre Haupteigenschaft z.B. WARLIKE bei Klingonen.
void CMajorRace::SetRaceNumber(BYTE RaceNumber)
{
	m_iRaceNumber = RaceNumber;	
	CString read[5];
	this->ReadDataFromFile(read);
	m_iRaceProberty = atoi(read[3]);
	m_iDiplomacyBonus = atoi(read[4]);
	SetKnownMajorRace(RaceNumber);	
}

/// Diese Funktion berechnet, wie eine computergesteuerte Majorrace, auf ein Angebot reagiert
/// Das ist hier sozusagen die komplette KI der Reaktion auf irgendwelche Angebote.
/// Rückgabewert ist einfach <code>TRUE</code> für Annahme bzw. <code>FALSE</code> bei Ablehnung.
USHORT CMajorRace::CalculateDiplomaticOffer(USHORT offerFrom, short type, USHORT warpactEnemy, const UINT* shipPowers)
{
	// Hier werden auch die Verbesserungen der Beziehung durch Geschenke bzw. deren Verschlechterung
	// wird hier auch behandelt
	
	// berechnen ob es klappt
	USHORT neededRelation;
	// Als erstes wird der Wert festgelegt, den wir später im Algorithmus zufällig erreichen müssen. Umso höher dieser
	// Wert ist, umso unwahrscheinlicher wird es, das wir das Angebot annehmen. Dieser zu erreichende Wert ist abhängig
	// vom Angebot an uns und von unserer Rasseneigenschaft. Z.B. sind wir die Föderation, somit sind wir Pazifisten,
	// und nehmen Angebote gegen den Krieg eher an als andere Rassen. Kriegsangebote nehmen wir so gut wie gar nicht an.
	if (type == NON_AGGRESSION_PACT)
	{
		switch (m_iRaceProberty)
		{
		case WARLIKE: {neededRelation = 55; break;}
		case SECRET: {neededRelation = 45; break;}
		case RESEARCHER: {neededRelation = 25; break;}
		case PACIFIST: {neededRelation = 10; break;}
		default: neededRelation = 35;
		}
	}
	else if (type == TRADE_AGREEMENT)
	{
		switch (m_iRaceProberty)
		{
		case SECRET: {neededRelation = 60; break;}
		case PRODUCER: {neededRelation = 45; break;}
		case FINANCIAL: {neededRelation = 35; break;}
		default: neededRelation = 55;
		}
	}
	else if (type == FRIENDSHIP_AGREEMENT)
	{
		switch (m_iRaceProberty)
		{
		case SECRET: {neededRelation = 75; break;}
		case WARLIKE: {neededRelation = 75; break;}
		case FINANCIAL: {neededRelation = 60; break;}
		case RESEARCHER: {neededRelation = 50; break;}
		case PACIFIST: {neededRelation = 50; break;}
		default: neededRelation = 70;
		}
	}
	else if (type == DEFENCE_PACT)
	{
		switch (m_iRaceProberty)
		{
		case FARMER: {neededRelation = 60; break;}
		case FINANCIAL: {neededRelation = 60; break;}
		case RESEARCHER: {neededRelation = 60; break;}
		default: neededRelation = 70;
		}
	}
	else if (type == COOPERATION)
	{
		switch (m_iRaceProberty)
		{
		case SECRET: {neededRelation = 90; break;}
		case RESEARCHER: {neededRelation = 75; break;}
		case FINANCIAL: {neededRelation = 75; break;}
		case PACIFIST: {neededRelation = 70; break;}
		default: neededRelation = 80;
		}
	}
	else if (type == AFFILIATION)
	{
		switch (m_iRaceProberty)
		{
		case SECRET: {neededRelation = 100; break;}
		case PACIFIST: {neededRelation = 85; break;}
		case RESEARCHER: {neededRelation = 85; break;}
		case FINANCIAL: {neededRelation = 80; break;}
		default: neededRelation = 90;
		}
	}
	else if (type == WAR_PACT)
	{
		switch (m_iRaceProberty)
		{
		case PACIFIST: {neededRelation = 70 + m_iRelationshipOtherMajor[warpactEnemy]; break;}
		case FARMER: {neededRelation = 60 + m_iRelationshipOtherMajor[warpactEnemy]; break;}
		case SECRET: {neededRelation = 50 + m_iRelationshipOtherMajor[warpactEnemy]; break;}
		case FINANCIAL: {neededRelation = 30 + m_iRelationshipOtherMajor[warpactEnemy]; break;}
		case SNEAKY: {neededRelation = 30 + (USHORT)(m_iRelationshipOtherMajor[warpactEnemy]/3); break;} // Hier gilt die Beziehung nur zu einem Drittel
		case WARLIKE: {neededRelation = 15 + m_iRelationshipOtherMajor[warpactEnemy]; break;}
		default: neededRelation = 40 + m_iRelationshipOtherMajor[warpactEnemy];
		}
	}

	// Hier werden zusätzlich noch die Schiffsstärken der anderen Rassen mit der unseren verglichen. Ist die andere Rasse
	// stärker, so nehmen wir eher an.
	if (shipPowers[m_iRaceNumber] > 0)
	{
		float compare = 1.0f;
		if (shipPowers[m_iRaceNumber] > 0)
			compare = (float)(shipPowers[offerFrom]) / (float)(shipPowers[m_iRaceNumber]);
		if (compare > 1.0f)
		{
			// der Wert, der von neededRelation abgezogen wird ist der Vergleichswert * [5,10];
			short minus = (short)(compare * (rand()%6 + 5));
			if ((neededRelation - minus) < 0)
				neededRelation = 0;
			else
				neededRelation -= minus;
		}
	}
	
	double multi = m_iRelationshipOtherMajor[offerFrom]+100;
	multi = multi/100;
	double value = m_iRelationshipOtherMajor[offerFrom]*multi;
	USHORT forRandom = (USHORT)value;
	if (forRandom == 0)
		forRandom++;
	USHORT temp = 0;
	for (int i = 0; i < 5; i++)
		temp += rand()%forRandom+1;
	forRandom = (USHORT)(temp/5);
/*	CString s;
	s.Format("Beziehung: %d\nNeededRelation: %d\nMulti: %lf\nforRandom: %d",m_iRelationshipOtherMajor[offerFrom],neededRelation,multi,forRandom);
	AfxMessageBox(s);
*/	

	// Wenn wir ein Angebot von einer schwachen Rasse bekommen haben, und unsere Stärke auch ziemlich schwach ist,
	// so nehmen wir das Angebot mit ziemlicher Wahrscheinlichkeit an. Dies wird gemacht, damit sich schwächere
	// Rassen nicht untereinander noch mehr schwächen, sondern sich zusammentun und gemeinsam gegen eine stärkere Macht
	// kämpfen.
	if (rand()%2 == NULL)
	{
		UINT maxShipPower = 0;
		for (int i = HUMAN; i <= DOMINION; i++)
			if (shipPowers[i] > maxShipPower)
				maxShipPower = shipPowers[i];
		// Wenn unsere Schiffstärke kleiner einem Drittel der maximalen Schiffstärke ist und die maximale Schiffs-
		// stärke nicht zu der Rasse gehört, welcher wir hier ein Angebot machen wollen
		if (shipPowers[offerFrom] < maxShipPower * 0.5f)
			if (shipPowers[m_iRaceNumber] < maxShipPower * 0.5f)
				neededRelation = 0;
	}

	// Wenn der Randomwert höher größer gleich dem Wert ist, den wir erreichen mußten, dann wird angenommen
	if (forRandom >= neededRelation)
		return TRUE;
	else if (rand()%15 > 0)	// zu 14/15 wird dann abgelehnt, ansonsten wird einfach "nicht reagiert" zurückgegeben
		return DECLINED;
	else
		return NOT_REACTED;
}

/// Diese Funktion berechnet wie eine computergesteuerte Majorrace auf eine Forderung reagiert.
/// Rückgabewert ist einfach <code>TRUE</code> für Annahme bzw. <code>FALSE</code> bei Ablehnung.
USHORT CMajorRace::CalculateDiplomaticRequest(USHORT requestFrom, USHORT requestedLatinum, USHORT* requestedRessource, CEmpire* empire, UINT* averageRessourceStorages, CSystem systems[30][20], const UINT* shipPowers)
{
	/* Als erstes wird überprüft wie unsere Beziehung zu der Rasse ist und welchen aktuellen Vertrag wir mit ihr 
	   aufrechterhalten. Umso besser die Beziehung und umso höherwertiger der Vertrag ist, desto eher nehmen wir
	   den Vertrag auch an. Wann nehme ich den Vertrag an? -> wenn wir einen bestimmten Prozentsatz mehr als der
	   globale Durchschnitt von der Forderung besitzen. Dieser Prozentsatz ist wie oben beschrieben abhängig von
	   der Beziehung und der Wertigkeit des Vertrages. Wenn ich diesen Vertrag nun annehmen würde, wird überprüft,
	   auf welchem Planeten ich das meiste der geforderten Ressource habe. Wenn ich die Forderung damit erfüllen
	   könnte, wird angenommen. Bei gefordertem Latinum brauche ich nicht die Planeten durchgehen.
	*/
	// zu 10% reagieren wir gar nicht auf die Forderung
	if (rand()%10 == 0)
		return NOT_REACTED;
	
	BOOLEAN answer = TRUE;
	// (14-(unsere Beziehung / 10 + Vertrag)) / 10 , 14 ist das Maximum, was wir erreichen könnten
	double value = (double)(14 - ((double)m_iRelationshipOtherMajor[requestFrom] / 10 + m_iDiplomacyStatus[requestFrom])) / 10;
	
	// Hier werden zusätzlich noch die Schiffsstärken der anderen Rassen mit der unseren verglichen. Ist die andere Rasse
	// stärker, so nehmen wir eher an.
	if (shipPowers[m_iRaceNumber] > 0)
	{
		float compare = (float)(shipPowers[requestFrom]) / (float)(shipPowers[m_iRaceNumber]);
		if (compare > 1.0f)
			value -= compare;
		else
			value += compare;
	}

	if (value < 0)
		value = 0;
	// umso niedriger Value ist, desto eher wird angenommen
	value += 1;
	
	// Jetzt haben wir den prozentulalen Wert (value) den wir davon brauchen
	// Wenn Latinum (mit) gefordert wird
	if (requestedLatinum > 0)
	{
		// Durchschnittslatinum berechnen
		long latinum = 0;
		USHORT counter = 0;
		for (int i = HUMAN; i <= DOMINION; i++)
			if (empire[i].GetNumberOfSystems() > 0)
			{
				latinum += empire[i].GetLatinum();
				counter++;
			}
		if (counter > 0)
			latinum /= counter;
		// schauen ob wir den prozentualen Mehrwert vom Durchschnittslatinum haben
		if (empire[m_iRaceNumber].GetLatinum() > (latinum * value))
			answer = TRUE;
		else
			answer = FALSE;
	}
	// Wenn Ressourcen (mit) gefordert werden
	if (answer == TRUE && (requestedRessource[TITAN] > 0 || requestedRessource[DEUTERIUM] > 0 || requestedRessource[DURANIUM] > 0 ||
		requestedRessource[CRYSTAL] > 0 || requestedRessource[IRIDIUM] > 0))
	{
		USHORT res = TITAN;
		if (requestedRessource[DEUTERIUM] > 0)
			res = DEUTERIUM;
		else if (requestedRessource[DURANIUM] > 0)
			res = DURANIUM;
		else if (requestedRessource[CRYSTAL] > 0)
			res = CRYSTAL;
		else if (requestedRessource[IRIDIUM] > 0)
			res = IRIDIUM;
		if (empire[m_iRaceNumber].GetStorage()[res] > (averageRessourceStorages[res] * value))
		{
			// Wenn jetzt noch "answer" auf TRUE steht, dann würden wir die Forderung annehmen. Jetzt muß ich noch
			// die Systeme durchgehen und schauen auf welchem ich das meiste dieser Ressource gelagert habe.
			// Dort ziehe ich es dann wenn möglich ab. Falls es nicht geht, dann stelle ich answer auf FALSE.
			answer = TRUE;
			ULONG mostRes = 0;
			CPoint system(0,0);
			for (int y = 0; y < 20; y++)
				for (int x = 0; x < 30; x++)
					if (systems[x][y].GetRessourceStore(res) > mostRes && systems[x][y].GetOwnerOfSystem() == m_iRaceNumber)
					{
						mostRes = systems[x][y].GetRessourceStore(res);
						system.x = x;
						system.y = y;
					}
			// Ressource wenn möglich abziehen
			if (mostRes >= requestedRessource[res])
			{
				/*CString s;
				s.Format("Request from: %d an %d\nunsere Beziehung zu denen: %d\nunser Status: %d\nvalue: %lf\ngeforderte Res: %d Menge: %d\naus System: %d/%d\ndavon vorhanden: %d",requestFrom,m_iRaceNumber,m_iRelationshipOtherMajor[requestFrom],m_iDiplomacyStatus[requestFrom],value,res,requestedRessource[res],system.x,system.y,systems[system.x][system.y].GetRessourceStore(res));
				AfxMessageBox(s);*/
				systems[system.x][system.y].SetRessourceStore(res, -requestedRessource[res]);
			}
			else answer = FALSE;
		}
		else
			answer = FALSE;
	}
	// Wenn ich, falls gefordert, die Ressourcen dazugebe und auch Latinum gefordert war und ich dieses auch geben
	// könnte, dann muß ich dieses noch von meinem Latinum abziehen
	if (answer == TRUE && requestedLatinum > 0)
	{
		/*CString s;
		s.Format("Request from: %d an %d\nunsere Beziehung zu denen: %d\nunser Status: %d\nvalue: %lf\ngefordertes Latinum: %d\ndavon bei uns vorhanden: %d\nForderer besitzt: %d",requestFrom,m_iRaceNumber,m_iRelationshipOtherMajor[requestFrom],m_iDiplomacyStatus[requestFrom],value,requestedLatinum,empire[m_iRaceNumber].GetLatinum(),empire[requestFrom].GetLatinum());
		AfxMessageBox(s);*/
		empire[m_iRaceNumber].SetLatinum(-requestedLatinum);
	}
	// Wenn hier noch answer auf TRUE steht, dann haben wir die Forderung angenommen, ansonsten haben wir sie abgelehnt
	if (answer == TRUE)
		return TRUE;
	else
		return DECLINED;
}

///	Funktion wird für jede computergesteuerte Majorrace in der <code>NextRound()</code>Funktion der Doc-Klasse aufgerufen.
///	Diese Funktion berechnet, ob die MajorRace einer anderen MajorRace ein Angebot aufgrund der 
/// Beziehung macht, oder ob sie ihr sogar den Krieg erklärt. Diese Funktion wird für jede einzelne andere Majorrace
///	aufgerufen, weil ich ja in einer Runde an mehrere Majorraces Angebote machen kann. Mit <code>race</code> wird immer
///	die andere Majorrace übergeben, der vielleicht an Angebot gemacht wird.
ComputerOfferStruct CMajorRace::PerhapsMakeOfferToMajorRace(USHORT race, USHORT relation, CEmpire* empire, CSystem systems[30][20],
															BYTE averageTechlevel, UINT* averageRessourceStorages, const UINT* shipPowers)
{
	ComputerOfferStruct theOffer;
	theOffer.KO.x = 0;
	theOffer.KO.y = 0;
	theOffer.latinum = 0;
	for (int l = TITAN; l <= IRIDIUM; l++)
		theOffer.ressource[l] = 0;
	theOffer.shownText = "";
	theOffer.type = 0;
	theOffer.duration = 0;	// Beziehungsweise die Dauer eines Vertrages wenns kein Kriegspakt ist
	// Können ja nur ein Angebot machen, wenn wir die Rasse kennen
	// Wir machen auch nur ein Angebot, wenn wir die Rasse gut leiden können und die Rasse, an die das Angebot geht
	// auch uns gut leiden kann. Die Differenz der beiden Werte darf nicht zu groß sein. Dafür wurde als Parameter
	// "relation" übergeben. Dies ist der Wert wie gut die andere Rasse uns leiden kann.
	// Außerdem muß später noch eingebaut werden, das wenn die Rasse "weg" ist, wir ihr auch kein Angebot mehr machen
	if (GetKnownMajorRace(race) == TRUE)
	{
		// jetzt zum Algorithmus:
		/*	Aufgrund der Beziehung zur Majorrace machen wir ihr vielleicht ein diplomatisches
			Angebot
		*/
		double multi = m_iRelationshipOtherMajor[race] + 100;
		multi = multi/100;
		double value = m_iRelationshipOtherMajor[race] * multi;
		USHORT forRandom = (USHORT)value;
		if (forRandom == 0)
			forRandom++;
		USHORT temp = 0;
		for (int i = 0; i < 3; i++)
			temp += rand()%forRandom+1;
		forRandom = (USHORT)(temp/3);

		// Hier werden zusätzlich noch die Schiffsstärken der anderen Rassen mit der unseren verglichen. Ist die andere Rasse
		// stärker, so nehmen wir eher an.
		float compare = 0.0f;
		if (shipPowers[m_iRaceNumber] > 0)
		{
			compare = (float)(shipPowers[race]) / (float)(shipPowers[m_iRaceNumber]);
			if (compare > 1.0f)
				forRandom += (USHORT)(compare * 5);
			else if ((forRandom - (USHORT)compare * 5) > 0)
				forRandom -= (USHORT)compare * 5;
			else
				forRandom = 0;
		}

		// Jetzt haben wir nen Randomwert, wenn dieser einen zufälligen Wert überschreitet, bietet die
		// Majorrace zu 33% etwas an, dieser zufällige Wert ist wiederrum von unserer Rasseneigenschaft abhängig
		short offer = 0;
		if (forRandom > this->CalculateValueByProberty(NON_AGGRESSION_PACT) && m_iDiplomacyStatus[race] < NON_AGGRESSION_PACT)
			offer = NON_AGGRESSION_PACT;
		if (forRandom > this->CalculateValueByProberty(TRADE_AGREEMENT) && m_iDiplomacyStatus[race] < TRADE_AGREEMENT)
			offer = TRADE_AGREEMENT;
		if (forRandom > this->CalculateValueByProberty(FRIENDSHIP_AGREEMENT) && m_iDiplomacyStatus[race] < FRIENDSHIP_AGREEMENT)
			offer = FRIENDSHIP_AGREEMENT;
		if (forRandom > this->CalculateValueByProberty(DEFENCE_PACT) && GetDefencePact(race) == FALSE
			&& m_iDiplomacyStatus[race] != WAR && m_iDiplomacyStatus[race] != AFFILIATION)
			offer = DEFENCE_PACT;
		if (forRandom > this->CalculateValueByProberty(COOPERATION) && m_iDiplomacyStatus[race] < COOPERATION)
			offer = COOPERATION;
		if (forRandom > this->CalculateValueByProberty(AFFILIATION) && m_iDiplomacyStatus[race] < AFFILIATION)
			offer = AFFILIATION;
		if (m_iDiplomacyStatus[race] >= offer && offer != DEFENCE_PACT) // weil DEFENCE_PACT negativ ist
			offer = 0;
			
		// Hier jetzt schauen, das die Beziehungen nicht so weit auseinanderliegen. Ein menschlicher Spieler würde
		// einem anderen ja auch nix anbieten, wenn er diesen leiden kann aber der andere ihn eh nicht leiden kann und
		// daher wahrscheinlich auch ablehnen wird
		// umso höher meine Beziehung ist, umso mehr darf seine abweichen. Umso höherwertig der Vertrag ist, umso weniger 
		// darf sie abweichen
		//(double)(m_iRelationshipOtherMajor[race]/100)	// Indikator für bessere Beziehung
		// abs(relation-m_iRelationshipOtherMajor[race]	// Abweichung
		// 40+mod-abs(offer*5)*Indikator				// soweit darf sie abweichen, 40 ist ein Erfahrungswert ;-)
		// mod											// wenn wir militärisch schwächer sind kann die Beziehung weiter abweichen
		short mod = 0;
		if (compare > 1.0f)
			mod = (short)(compare * 5);
		if (abs(relation-m_iRelationshipOtherMajor[race]) > (double)(40 + mod - abs(offer*5)) * (double)(m_iRelationshipOtherMajor[race])/100)
		{
		/*	if (offer != 0)
			{
				double temp = (double)(40 - abs(offer*5)) * (double)(m_iRelationshipOtherMajor[race])/100;
				CString h;
				h.Format("Abweichung: %d\nVergleichswert: %lf\nWir %d - an %d",abs(relation-m_iRelationshipOtherMajor[race]),
					temp,m_iRaceNumber,race);
				AfxMessageBox(h);
			}*/
			offer = 0;			
		}

		// Das könnte man noch von der Art der Rasse abhängig machen, kriegerische Rassen erklären eher Krieg,
		// pazifistische Rasse erklären gar keinen Krieg oder so ähnlich -> hier gemacht
		short minRel = 15;
		float modi = 0.0f;
		if (shipPowers[race] > 0)
			modi = (float)(shipPowers[m_iRaceNumber]) / (float)(shipPowers[race]);
		modi *= 2;
		if (m_iRaceProberty == WARLIKE)			// Wir lieben Krieg
			minRel = 25;
		else if (m_iRaceProberty == SNEAKY)		// Wir sind eh hinterhältig
			minRel = 20;
		else if (m_iRaceProberty == FINANCIAL)	// Krieg ist gut fürs Geschäft
			minRel = 15;
		else if (m_iRaceProberty == RESEARCHER)	// Krieg ist nicht immer gut für Forschung
			minRel = 5;
		else if (m_iRaceProberty == PACIFIST)	// Wir hassen Krieg
			minRel = 0;
		// wir sind stärker als der Gegner
		if (compare < 1.0f)
			minRel += (short)modi * 2;	// somit wird auch Krieg erklärt, wenn die Beziehung eigentlich noch besser ist.
		if (forRandom < minRel && m_iRelationshipOtherMajor[race] < minRel && m_iDiplomacyStatus[race] != WAR)
			offer = WAR;
		// Hier möglicherweise Angebot eines Kriegspaktes
		// das geht natürlich nur, wenn wir noch keinen Krieg mit der Rasse haben. Es ist aber möglich, wenn wir
		// eine Runde zuvor einen Kriegspakt angeboten haben, der später angenommen wird und wir auch so der Rasse 
		// den Krieg erklären, das wir ihr eigentlich 2x den Krieg erklären. Ist nicht so wichtig, sieht nur in der
		// Nachrichtenübersicht nicht so schön aus
		// wenn ich der Rasse den Krieg erklären würde, dann suche ich mir möglicherweise auch noch einen Kriegspaktpartner
		if (offer == WAR && m_iDiplomacyStatus[race] != WAR && rand()%4 > 0)	// zu 75% wird versucht ein Kriegspakt anzubieten
		{
			BYTE likeBest = NOBODY;		// beste Beziehung von uns zu
			BYTE likeSecond = NOBODY;	// zweitbeste Beziehung von uns zu
			USHORT tempRelation = 0;
			for (int i = HUMAN; i <= DOMINION; i++)
				if (i != m_iRaceNumber)	// nicht wir selber
					if (GetKnownMajorRace(i) == TRUE && m_iRelationshipOtherMajor[i] > tempRelation && m_iRelationshipOtherMajor[i] > (rand()%50)) // Beziehung muß größer rand()%50 sein
					{
						tempRelation = m_iRelationshipOtherMajor[i];
						if (likeBest != NOBODY)
							likeSecond = likeBest;
						likeBest = i;
					}
			BYTE warpactpartner = NOBODY;	// Kriegspaktpartner müssen wir noch einen finden, entweder nehmen wie den,
											// den wir am besten oder am zweitbesten leiden können und dessen Beziehung	
											// besser als rand()%50 ist.
			if (likeBest != NOBODY && likeSecond != NOBODY)
			{
				if (rand()%2 == 0 && likeBest != race)
					warpactpartner = likeBest;
				else if (likeSecond != race)
					warpactpartner = likeSecond;
			}
			else if (likeBest != NOBODY && likeBest != race)
				warpactpartner = likeBest;
			if (warpactpartner != NOBODY && empire[warpactpartner].GetNumberOfSystems() > 0)
			{
				// Kriegspaktpartner, wir müssen aber in NextRound() das Angebot dann an diesen Partner übergeben und
				// den Gegner über duration übergeben
				theOffer.duration = warpactpartner;	
				offer = WAR_PACT;
			}
		}
		// hier Algo um zu 33% was anzubieten
		forRandom = rand()%3;
		if (forRandom > 0)
			offer = 0;
		theOffer.type = offer;
/*		if (offer != 0)
		{
			CString s;
			s.Format("Angebot: %d\nvon: %s\nan: %s",offer,CMajorRace::GetRaceName(m_iRaceNumber),CMajorRace::GetRaceName(race));
			AfxMessageBox(s);
		}
*/		/* Ab hier kommt noch was großes dazu, nämlich ob und wann eine computergesteuerte Rasse auch Mitgifte macht.
		   Latinum kann sie ja immer dazugeben, aber Ressourcen geht nur, wenn sie mindst. einen Handelsvertrag hat.
		   Außerdem könnte die Rasse ja auch ein Geschenk geben oder eine Forderung stellen.
		*/
		if (offer != NO_AGREEMENT)
		{
			if (offer != WAR)
				this->PerhapsGiveDowry(&theOffer, race, empire, systems, averageTechlevel, averageRessourceStorages);
			// So, jetzt müssen wir noch die Dauer des Vertrages festlegen. Umso "höherwertiger" ein Vertrag ist, desto
			// wahrscheinlicher sollte eine längere Vertragsdauer auch sein. Das heißt, wir holen uns eine Zufallszahl
			// von 0 bis 10 und addieren darauf den Betrag des Wertes eines Vertrages. Alles was größer als 10 ist wird
			// auf 0 gesetzt. 0 bedeutet unbegrenzt.
			if (offer != WAR && offer != WAR_PACT)
			{
				USHORT duration = rand()%(11+abs(theOffer.type));
				if (duration > 10)
					duration = 0;
				theOffer.duration = duration*10;	// hier Dauer des Vertrages festlegen
			}
		//	AfxMessageBox("ANGEBOT");
			if (offer != WAR_PACT)
				theOffer.shownText = CGenDiploMessage::GenerateMajorOffer(m_iRaceNumber,theOffer.type,theOffer.latinum,theOffer.ressource,theOffer.duration);
			else
				theOffer.shownText = CGenDiploMessage::GenerateMajorOffer(m_iRaceNumber,theOffer.type,theOffer.latinum,theOffer.ressource,race);
			return theOffer;
		}
		// wenn kein Angebot gemacht werden würde, so gibt die KI mit sehr geringer Wahrscheinlichkeit auch Geschenke
		// Diese werden zur Zeit nicht durch einen bestimmten Grund gegeben, sondern nur sehr unwahrscheinlich
		else if (rand()%30 == 0) // zu 3.33% wird wenn nix angeboten wird möglicherweise ein Geschenk gemacht
		{
			theOffer.type = PRESENT;
			this->PerhapsGiveDowry(&theOffer, race, empire, systems, averageTechlevel, averageRessourceStorages);
			if (theOffer.latinum == 0 && (theOffer.ressource[TITAN] == 0 && theOffer.ressource[DEUTERIUM] == 0 &&
				theOffer.ressource[DURANIUM] == 0 && theOffer.ressource[CRYSTAL] == 0 && theOffer.ressource[IRIDIUM] == 0))
				theOffer.type = NO_AGREEMENT;
		//	else
		//		AfxMessageBox("GESCHENK");
		}
		// wenn wir kein Angebot machen würden und wir auch kein Geschenk geben, dann stellen wir vielleicht eine Forderung
		else
		{
			// Der Modifikator ausgehend von den Schiffsstärken verändert die Wahrscheinlichkeit für eine Forderung.
			// Umso stärker unsere Schiffe sind, desto eher wird auch eine Forderung gestellt. Hier werden zwei Zufalls-
			// variablen bestimmt, weil mit einer das Risiko trotz eines hohen modi-Werts eine Forderung zu stellen zu
			// groß ist.

			// hier ist der code so lang, da viele ZERO Überprüfungen dabei sind
			int temp = (int)(100 / (modi+1.0f));
			int a = 0;
			int b = 0;
			if (temp > 0)
			{
				a = rand()%temp;
				b = rand()%temp;
			}
			if (a < 2 && b < 8)
			{
#ifdef TRACE_DIPLOMATY
				MYTRACE(MT::LEVEL_DEBUG, "rand: %d - a = %d - b = %d - Modi = %lf (wir %d (%d) - Gegner %d (%d))\n",((int)(100 / (modi+1))), a, b, modi, m_iRaceNumber, shipPowers[m_iRaceNumber], race, shipPowers[race]);
#endif
				this->PerhapsHaveRequest(&theOffer, race, empire, systems, averageTechlevel, averageRessourceStorages, shipPowers);
			}

		}

		// Wenn bis jetzt noch kein Angebot gemacht wurde, dann wird überprüft, ob es Sinn macht, sich gegen ein stärkeres
		// fremdes Imperium mit einem anderen Partner zu verbünden bzw. dem anderen Partner eine Freundschaft oder
		// Kooperation anzubieten. Dies hat den Vorteil, dass sich relativ schwache Imperien nicht auch noch gegenseitig
		// schwächen, sondern sich eher verbünden. In einem Lategame könnte z.B. eine Rasse so stark wie alle anderen Rassen
		// zusammen sein, dann sollten sich die anderen Rassen verbünden und nicht selbst noch schwächen.
		if (theOffer.type == NO_AGREEMENT && m_iDiplomacyStatus[race] < FRIENDSHIP_AGREEMENT && rand()%5 == NULL)
		{
			UINT maxShipPower = 0;
			for (int i = HUMAN; i <= DOMINION; i++)
				if (shipPowers[i] > maxShipPower)
					maxShipPower = shipPowers[i];
			// Wenn unsere Schiffstärke kleiner einem Drittel der maximalen Schiffstärke ist und die maximale Schiffs-
			// stärke nicht zu der Rasse gehört, welcher wir hier ein Angebot machen wollen
			if (maxShipPower != shipPowers[race])
				if (shipPowers[m_iRaceNumber] < maxShipPower * 0.33f)
				{
					// Freundschaft oder Kooperation anbieten
					if (rand()%2 == NULL)
						theOffer.type = FRIENDSHIP_AGREEMENT;
					else
						theOffer.type = COOPERATION;
				}
		}
	}
	if (theOffer.type != NO_AGREEMENT)
		theOffer.shownText = CGenDiploMessage::GenerateMajorOffer(m_iRaceNumber,theOffer.type,theOffer.latinum,theOffer.ressource,theOffer.duration);
	return theOffer;
}

/// Funktion berechnet, ob und was für ein Angebot die Majorrace an die Minorrace macht.
ComputerOfferStruct CMajorRace::PerhapsMakeOfferToMinorRace(CMinorRace* minor, BOOLEAN isFavoritMinor, CEmpire* empire, CSystem systems[30][20], BYTE averageTechlevel, UINT* averageRessourceStorages)
{
	USHORT relation = minor->GetRelationshipToMajorRace(m_iRaceNumber);
	short status = minor->GetDiplomacyStatus(m_iRaceNumber);
	ComputerOfferStruct theOffer;
	theOffer.KO.x = 0;
	theOffer.KO.y = 0;
	theOffer.latinum = 0;
	for (int l = TITAN; l <= IRIDIUM; l++)
		theOffer.ressource[l] = 0;
	theOffer.shownText = "";
	theOffer.type = 0;
	theOffer.duration = 0;
	
	// Wenn unsere Beziehung hoch genug ist und wir den jeweiligen Vertrag noch nicht haben, so machen wir mit einer
	// bestimmten Wahrscheinlichkeit ein Angebot. Wenn wir ein Angebot machen könnten, aber dies nicht aufgrund von
	// Verträgen mit anderen Majorraces geht, dann versuchen wir die Minorrace zu bestechen, wenn es unsere Favorit
	// Minorrace ist und die Beziehung zu ihr auch hoch genug ist
	short othersStatus = NO_AGREEMENT;
	BYTE corruptedMajor = NOBODY;
	for (int i = HUMAN; i <= DOMINION; i++)
		if (i != m_iRaceNumber)
		{
			short temp = minor->GetDiplomacyStatus(i);
			if (temp > othersStatus)
			{
				othersStatus = temp;
				corruptedMajor = i;		// Bestochen werden sollte, dann is dies hier auch gleich die Majorrace
			}

		}
	if (relation > 90 && status < MEMBERSHIP && othersStatus < COOPERATION)
		theOffer.type = MEMBERSHIP;
	else if (relation > 85 && status < AFFILIATION && othersStatus < COOPERATION)
		theOffer.type = AFFILIATION;
	else if (relation > 80 && status < COOPERATION && othersStatus < COOPERATION)
		theOffer.type = COOPERATION;
	else if (relation > 70 && status < FRIENDSHIP_AGREEMENT && othersStatus < AFFILIATION)
		theOffer.type = FRIENDSHIP_AGREEMENT;
	else if (relation > 55 && status < TRADE_AGREEMENT && othersStatus < MEMBERSHIP)
		theOffer.type = TRADE_AGREEMENT;
	// Das könnte man noch von der Art der Rasse abhängig machen, kriegerische Rassen erklären eher Krieg,
	// pazifistische Rasse erklären gar keinen Krieg oder so ähnlich -> hier gemacht
	// Auch noch überprüfen ob die Minorrace ein Bündnis hat, weil wenn wir zu der verbündeten Majorrace
	// eine relativ gute Beziehung haben, dann erklären wir keinen Krieg. (noch machen!)
	if (othersStatus < MEMBERSHIP)
	{
		USHORT minRel = 10;
		if (m_iRaceProberty == WARLIKE)			// Wir lieben Krieg
			minRel = 25;
		else if (m_iRaceProberty == FINANCIAL)	// Krieg ist gut fürs Geschäft
			minRel = 20;
		else if (m_iRaceProberty == SNEAKY)		// Wir sind eh hinterhältig
			minRel = 15;
		else if (m_iRaceProberty == PACIFIST)	// Wir hassen Krieg
			minRel = 5;
		if (relation < minRel && status != WAR)
			theOffer.type = WAR;
		// Nun checken das ich durch irgendeinen Pakt nicht nem Freund den Krieg miterklären würde
		if (theOffer.type == WAR)
		{
			for (int i = HUMAN; i <= DOMINION; i++)
				if (i != m_iRaceNumber && minor->GetDiplomacyStatus(i) == AFFILIATION)	// Würden "i" auch den Krieg erklären
					if (m_iRelationshipOtherMajor[i] > 50)	// Hier einfach bei größer 50 wird kein Krieg erklärt
					{
						theOffer.type = NO_AGREEMENT;
						break;
					}
		}
	}
	// Nun haben wir den Vertrag, den wir anbieten möchten
	// Wenn es sich bei der Minorrace um unsere Favoritrace handelt, dann versuchen wir ihr auch Mitgifte zum Vertrag
	// zu geben
	// Haben wir mindst. einen Handelsvertrag -> Ressourcenmitgaben
	if (isFavoritMinor == TRUE && theOffer.type > NO_AGREEMENT && theOffer.type < PRESENT && theOffer.type >= TRADE_AGREEMENT)
		this->PerhapsGiveDowry(&theOffer,NOBODY,empire,systems,averageTechlevel,averageRessourceStorages,TRUE);
	// Ansonsten können wir nur Latinum mitgeben
	else if (isFavoritMinor == TRUE && theOffer.type > NO_AGREEMENT && theOffer.type < PRESENT)
		this->PerhapsGiveDowry(&theOffer,NOBODY,empire,systems,averageTechlevel,averageRessourceStorages);
	// Wenn wir kein Angebot an unsere Favoritrace gemacht haben bzw. wir konnten keins machen, dann geben wir
	// möglicherweise ein Geschenk oder wir Bestechen diese Rasse
	// Wenn wir mindst. einen Handelsvertrag mit unserer Favoritminor haben -> Ressourcengeschenk
	if (isFavoritMinor == TRUE && theOffer.type == NO_AGREEMENT && relation <= 91 && minor->GetDiplomacyStatus(m_iRaceNumber) >= TRADE_AGREEMENT)
		this->PerhapsGiveDowry(&theOffer, NOBODY, empire, systems, averageTechlevel, averageRessourceStorages, TRUE);
	// sonst gibts nur ein Ressourcengeschenk
	else if (isFavoritMinor == TRUE && theOffer.type == NO_AGREEMENT && relation <= 91)
		this->PerhapsGiveDowry(&theOffer,NOBODY,empire,systems,averageTechlevel,averageRessourceStorages);
	if ((theOffer.latinum > 0 || theOffer.ressource[TITAN] > 0 || theOffer.ressource[DEUTERIUM] > 0 || theOffer.ressource[DURANIUM] > 0
		 || theOffer.ressource[CRYSTAL] > 0  || theOffer.ressource[IRIDIUM] > 0) && theOffer.type == NO_AGREEMENT)
		 theOffer.type = PRESENT;
	// Wenn wir der Favoritminor kein Geschenk machen, dann Bestechen wir diese womöglich
	else if (isFavoritMinor == TRUE && theOffer.type == NO_AGREEMENT)
	{
		// Wir können nur bestechen, wenn wir mindst. Freundschaft mit der Minorrace haben oder diese Minorrace
		// hat mit irgendeiner anderen Rasse mindst. ein Bündnis
		if (othersStatus >= AFFILIATION)
		{
			if (minor->GetDiplomacyStatus(m_iRaceNumber) == TRADE_AGREEMENT)
				this->PerhapsGiveDowry(&theOffer, NOBODY, empire, systems, averageTechlevel, averageRessourceStorages, TRUE);
			else 
				this->PerhapsGiveDowry(&theOffer,NOBODY,empire,systems,averageTechlevel,averageRessourceStorages);
		}
		else if (minor->GetDiplomacyStatus(m_iRaceNumber) >= FRIENDSHIP_AGREEMENT && minor->GetDiplomacyStatus(m_iRaceNumber) != MEMBERSHIP && othersStatus > NO_AGREEMENT)
		{
			// hier müssen wir noch den Gegner der Bestechung wählen
			BOOLEAN pos[7] = {FALSE};
			USHORT counter = 0;
			for (int i = HUMAN; i <= DOMINION; i++)
				if (i != m_iRaceNumber && minor->GetDiplomacyStatus(i) > NO_AGREEMENT)
				{
					pos[i] = TRUE;
					counter++;
				}
			USHORT chosen = NOBODY;
			if (counter > 0)
				chosen = rand()%counter;
			counter = chosen;
			for (int i = HUMAN; i <= DOMINION; i++)
				if (i != m_iRaceNumber)
					if (pos[i] == TRUE)
					{
						if (counter == 0)
						{
							corruptedMajor = i;
							break;
						}
						counter--;
					}
			// Nun schauen ob ich die Bestechung durchführen kann
			this->PerhapsGiveDowry(&theOffer, NOBODY, empire, systems, averageTechlevel, averageRessourceStorages, TRUE);
		}
		if ((theOffer.latinum > 0 || theOffer.ressource[TITAN] > 0 || theOffer.ressource[DEUTERIUM] > 0 || theOffer.ressource[DURANIUM] > 0
			|| theOffer.ressource[CRYSTAL] > 0  || theOffer.ressource[IRIDIUM] > 0) && theOffer.type == NO_AGREEMENT)
		{	
			theOffer.type = CORRUPTION;
			theOffer.duration = corruptedMajor;
		}
	}
	theOffer.shownText = CGenDiploMessage::GenerateMajorOffer(m_iRaceNumber,theOffer.type,theOffer.latinum,theOffer.ressource);
/*	if (theOffer.latinum > 0)
	{
		CString k;
		k.Format("%s machen Angebot %d an %s\nLatinum: %d - Titan: %d aus %d/%d",CMajorRace::GetRaceName(m_iRaceNumber),theOffer.type,minor->GetRaceName(),theOffer.latinum,theOffer.ressource[TITAN],theOffer.KO.x,theOffer.KO.y);
		AfxMessageBox(k);
	}*/
	return theOffer;
}

/// Funktion berechnet die Beziehungsverbesserungen durch die Übergabe von Latinum. Das Latinum wird hier aber nicht
/// gutgeschrieben, sondern "nur" die Beziehung zur Majorrace verbessert. <code>race</code> ist die Rasse, die das
/// Geschenk übergegeben hat.
void CMajorRace::GiveLatinumPresent(USHORT latinum, CEmpire* empires, USHORT race)
{
	// Das ist hier alles wesentlich kürzer und nicht so komplex wie bei der Diplomatie mit
	// den Minorraces. Hier kommt es wesentlich mehr drauf an, wieviel Geld wir im Lager haben.
	// Nur an einer Stelle wird ein kleiner Unterschied bei der Art der Rasse gemacht.
	// Es soll relativ einfach sein, damit wir das Spiel nicht in eine Richtung lenken, jedes mal
	// kann und soll es ein bisschen anders sein.


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
	if (m_iRaceProberty == WARLIKE)
		neededValue += 50;		// bei ner kriegerischen Rasse ist der Wert höher
	else if (m_iRaceProberty == FINANCIAL)
		neededValue -= 25;		// bei ner finanziellen Rasse ist der Wert niedriger
	else if (m_iRaceProberty == SECRET)
		neededValue += 100;		// bei ner geheimdienst Rasse ist der Wert wesentlich höher

	/*	Jetzt berechnen, um wieviel sich die Beziehung verändern würde */
	double bonus = 0;
	if (randomMoney >= neededValue)
		bonus = randomMoney/neededValue;	// Wieviel Prozent bekommen wir als Bonus dazu

	// Jetzt die Abhängigkeit von dem schon vorhandenen Latinum im Lager
	/* Hier wird ein Wert berechnet, nämlich das gegebene Latinum/Latinumlager von uns
	   der erhaltene Wert wird mit dem bonus multipliziert und ergibt die Beziehungsverbesserung.
	   Der erhaltene Wert darf aber nicht größer als 1 sein */
	double value = ((double)latinum / (double)(empires[m_iRaceNumber].GetLatinum()+.00001));
	if (value > 1)
		value = 1;
	else if (value < 0)
		value = 0;
	bonus *= value;
/*	CString s;
	s.Format("Latinum: %d\nWert: %lf\nBeziehungsverbesserung: %lf\nLatinumlager: %i",latinum,value,bonus,empires[m_iRaceNumber].GetLatinum());
	AfxMessageBox(s);*/
	this->SetRelationshipToMajorRace(race, (short)bonus);
}

/// Funktion berechnet wie viel die Ressource in Latinum wert ist.
USHORT CMajorRace::GiveResourcePresent(USHORT* ressource, CEmpire* empires, USHORT race)
{
	/*	Titan wird getauscht im Verhältnis 1000 zu 400
		Deuterium wird getauscht im Verhältnis 1000 zu 364
		Duranium wird getauscht im Verhältnis 1000 zu 500
		Crystal wird getauscht im Verhältnis 1000 zu 600
		Iridium wird getauscht im Verhältnis 1000 zu 800	*/
	double value = 0;
	double div = 0;
	if (ressource[TITAN] != 0)		// Titan übergeben?
	{
		value = ressource[TITAN]/2.5;
		div = (double)ressource[TITAN] / (double)(empires[m_iRaceNumber].GetStorage()[TITAN]+.00001);
	}
	else if (ressource[DEUTERIUM] != 0)	// Deuterium übergeben?
	{
		value = ressource[DEUTERIUM]/2.75;
		div = (double)ressource[DEUTERIUM] / (double)(empires[m_iRaceNumber].GetStorage()[DEUTERIUM]+.00001);
	}
	else if (ressource[DURANIUM] != 0)	// Duranium übergeben?
	{
		value = ressource[DURANIUM]/2;
		div = (double)ressource[DURANIUM] / (double)(empires[m_iRaceNumber].GetStorage()[DURANIUM]+.00001);
	}
	else if (ressource[CRYSTAL] != 0)	// Kristalle übergeben?
	{
		value = ressource[CRYSTAL]/1.625;
		div = (double)ressource[CRYSTAL] / (double)(empires[m_iRaceNumber].GetStorage()[CRYSTAL]+.00001);
	}
	else if (ressource[IRIDIUM] != 0)	// Iridium übergeben?
	{
		value = ressource[IRIDIUM]/1.25;
		div = (double)ressource[IRIDIUM] / (double)(empires[m_iRaceNumber].GetStorage()[IRIDIUM]+.00001);
	}
	/*	So, nun gibts aber nicht immer diesen Betrag! Dafür fragen wir die
		kompletten Lager ab. Umso mehr wir haben, umso weniger ist der
		übergebene Rohstoff für uns wert.

		Dafür berechnen wir einen Teiler. Wir nehmen die aktuelle
		Menge des jeweiligen Rohstoffes, die in allen Systemen gelagert sind.
		Mit diesem Teiler wird der erhaltene Wert des Rohstoffes multipliziert.
		Dieser nun erhaltene Wert wird noch rassenabhängig modifiziert.
	*/

	if (div > 1)
		div = 1;
	else if (div < 0)
		div = 0;
	value *= div;
	
	// Wert nochmal modifizieren, aufgrund der Rassenart
	if (m_iRaceProberty == PRODUCER)
		value = value * 1.5;
	else if (m_iRaceProberty == INDUSTRIAL)
		value = value * 1.35;
	else if (m_iRaceProberty == WARLIKE)
		value = value * 1.2;
	else if (m_iRaceProberty == FINANCIAL)
		value = value * 1.1;
	else if (m_iRaceProberty == SECRET)
		value = value * 0.9;
	else if (m_iRaceProberty == FARMER)
		value = value * 0.8;

/*	CString s;
	s.Format("Ressourcenwert: %d",(USHORT)value);
	AfxMessageBox(s);*/
	if (value > 0)
		return (USHORT)value;
	return 0;
}

/// Funktion berechnet die Minorrace, der die Majorrace am liebsten Mitgifte geben würde. Also nicht die Minorrace,
/// die wir am besten leiden können, sondern die, denen wir ein Geschenk bzw. Mitgifte machen würden.
short CMajorRace::CalculateFavoritMinor(MinorRaceArray* minors)
{
	// Wir suchen hier nach der Minorrace, der ich ein Geschenk bzw. ein Angebot inkl. Mitgift machen würde
	// ich suche mir die 3 Rassen raus, zu denen ich die beste Beziehung habe. Von diesen 3 Rassen übergebe ich
	// danach eine zufällig, diese Rasse ist dann unser Favorit.
	// Außerdem gebe ich auch keiner Rasse Geld bzw. Mitgifte, wenn ich schon eine Mitgliedschaft mit dieser habe.
	short favorit[3];
	favorit[0] = favorit[1] = favorit[2] = -1;
	for (int i = 0; i < minors->GetSize(); i++)
		// kennen wir die Minorrace und wurde diese nicht unterworfen
		if (minors->GetAt(i).GetKnownByMajorRace(m_iRaceNumber) == TRUE && minors->GetAt(i).GetSubjugated() == FALSE)
			// ist die Beziehung noch kleiner 95
			if (minors->GetAt(i).GetRelationshipToMajorRace(m_iRaceNumber) < 95)
			{
				// ist die Akzeptanz bei einer anderen Major noch nicht zu hoch?
				BOOLEAN noFav = FALSE;
				for (int j = HUMAN; j <= DOMINION; j++)
					if (j != m_iRaceNumber)
						if ((minors->GetAt(i).GetRelationPoints(j) > 3000 && minors->GetAt(i).GetDiplomacyStatus(j) == MEMBERSHIP)
							|| minors->GetAt(i).GetRelationPoints(j) > 4000)
							noFav = TRUE;
				if (noFav)
					continue;
					
				short status = 0;
			/*	for (int j = HUMAN; j <= DOMINION; j++)
					if (j != m_iRaceNumber)
					{
						short temp = minors->GetAt(i).GetDiplomacyStatus(j);
						if (temp > status)
							status = temp;
					}*/
				status = minors->GetAt(i).GetDiplomacyStatus(m_iRaceNumber);
				if (status < MEMBERSHIP)
				{
					if (favorit[0] == -1) favorit[0] = i;
					else if (favorit[1] == -1) favorit[1] = i;
					else if (favorit[2] == -1) favorit[2] = i;
	
					if (minors->GetAt(i).GetRelationshipToMajorRace(m_iRaceNumber) >=
						minors->GetAt(favorit[0]).GetRelationshipToMajorRace(m_iRaceNumber))
					{
						if (favorit[2] != -1)
							favorit[2] = favorit[1];
						if (favorit[1] != -1)
							favorit[1] = favorit[0];
						favorit[0] = i;					
					}
					else if (minors->GetAt(i).GetRelationshipToMajorRace(m_iRaceNumber) >=
						minors->GetAt(favorit[1]).GetRelationshipToMajorRace(m_iRaceNumber))
					{
						if (favorit[2] != -1)
							favorit[2] = favorit[1];
						favorit[1] = i;
					}
					else if (minors->GetAt(i).GetRelationshipToMajorRace(m_iRaceNumber) >=
						minors->GetAt(favorit[2]).GetRelationshipToMajorRace(m_iRaceNumber))
					{
						// Wenn es genau gleich ist, dann nur zu 50% überschreiben
						if (minors->GetAt(i).GetRelationshipToMajorRace(m_iRaceNumber) ==
							minors->GetAt(favorit[2]).GetRelationshipToMajorRace(m_iRaceNumber))
						{
							if (rand()%2 == 0)
								favorit[2] = i;
						}
						else
							favorit[2] = i;
					}
				}
			}
/*	
	if (m_iRaceNumber == FERENGI)
	{
		CString h = "";
		if (favorit[0] != -1)
		{
			h += minors->GetAt(favorit[0]).GetRaceName();
			h += " - ";
		}
		if (favorit[1] != -1)
		{
			h += minors->GetAt(favorit[1]).GetRaceName();
			h += " - ";
		}
		if (favorit[2] != -1)
		{
			h += minors->GetAt(favorit[2]).GetRaceName();
			h += " - ";
		}
	AfxMessageBox(h);
	}
*/	
	// jetzt wird zufällig einer der favorits zurückgegeben
	short temp = rand()%3;
	return favorit[temp];
}

/// Die Resetfunktion der CMajorrace-Klasse.
void CMajorRace::Reset()
{
	m_Attributes = 0;
	m_iRaceNumber = NOBODY;
	m_iRaceProberty = NOTHING_SPECIAL;
	m_iDiplomacyBonus = NULL;
	for (int i = 0; i <= DOMINION; i++)
	{
		m_iRelationshipOtherMajor[i] = rand()%26+40;
		m_iDiplomacyStatus[i] = NO_AGREEMENT;
		m_iDurationOfAgreement[i] = 0;
		m_iDurationOfDefencePact[i] = 0;		
	}
}

//////////////////////////////////////////////////////////////////////
// private Funktionen
//////////////////////////////////////////////////////////////////////
/// Funktion die die Daten aus der Datei 'MajorRace.data' ausliest und diese in die
/// Variable read[3] schreibt.
void CMajorRace::ReadDataFromFile(CString* read)
{
	ASSERT(m_iRaceNumber);

	CString search;
	switch(m_iRaceNumber)
	{
		case HUMAN:		{search.Format("RACE1"); break;}
		case FERENGI:	{search.Format("RACE2"); break;}
		case KLINGON:	{search.Format("RACE3"); break;}
		case ROMULAN:	{search.Format("RACE4"); break;}
		case CARDASSIAN:{search.Format("RACE5"); break;}
		case DOMINION:	{search.Format("RACE6"); break;}
	}
	USHORT i = 0;
	CString csInput;												// auf csInput wird die jeweilige Zeile gespeichert
	CString fileName = *((CBotf2App*)AfxGetApp())->GetPath() + "Data\\Races\\MajorRaces.data";	// Name des zu Öffnenden Files 
	CStdioFile file;												// Varibale vom Typ CStdioFile
	if (file.Open(fileName, CFile::shareDenyNone | CFile::modeRead | CFile::typeText))		// Datei wird geöffnet
	{
		while (file.ReadString(csInput))
		{
			if (csInput == search || i > 0)	// Wenn wir in dem File die Rasse gefunden haben ("RASSENNAME:" gefunden!)
			{
				read[i] = csInput;
				i++;
			}
			if (i == 5)
				break;
		}
	}
	else
	{	
		AfxMessageBox("Error! Could not open \"MajorRaces.data\"...");
		exit(1);
	}
	if (read[0] == "")
	{
		CString s;
		s.Format("Error in file!\nCould not find tag for race %d in file MajorRaces.data", m_iRaceNumber);
		AfxMessageBox(s);
	}
	file.Close();							// Datei wird geschlossen
}

/// Funktion berechnet den nötigen Wert, ob wir ein Angebot machen. Der Wert ist abhängig von der Rasseneigenschaft.
USHORT CMajorRace::CalculateValueByProberty(short type)
{
	USHORT neededRelation = 200;
	if (type == NON_AGGRESSION_PACT)
	{
		switch (m_iRaceProberty)
		{
		case WARLIKE: {neededRelation = 70; break;}
		case SECRET: {neededRelation = 60; break;}
		case RESEARCHER: {neededRelation = 40; break;}
		case PACIFIST: {neededRelation = 30; break;}
		default: neededRelation = 50;
		}
	}
	else if (type == TRADE_AGREEMENT)
	{
		switch (m_iRaceProberty)
		{
		case SECRET: {neededRelation = 80; break;}
		case PRODUCER: {neededRelation = 60; break;}
		case FINANCIAL: {neededRelation = 50; break;}
		default: neededRelation = 70;
		}
	}
	else if (type == FRIENDSHIP_AGREEMENT)
	{
		switch (m_iRaceProberty)
		{
		case SECRET: {neededRelation = 90; break;}
		case WARLIKE: {neededRelation = 90; break;}
		case FINANCIAL: {neededRelation = 70; break;}
		case RESEARCHER: {neededRelation = 70; break;}
		case PACIFIST: {neededRelation = 65; break;}
		default: neededRelation = 80;
		}
	}
	else if (type == DEFENCE_PACT)
	{
		switch (m_iRaceProberty)
		{
		case FARMER: {neededRelation = 90; break;}
		case FINANCIAL: {neededRelation = 90; break;}
		case RESEARCHER: {neededRelation = 90; break;}
		case PACIFIST: {neededRelation = 80; break;}
		default: neededRelation = 100;
		}
	}
	else if (type == COOPERATION)
	{
		switch (m_iRaceProberty)
		{
		case SECRET: {neededRelation = 125; break;}
		case RESEARCHER: {neededRelation = 105; break;}
		case FINANCIAL: {neededRelation = 95; break;}
		case PACIFIST: {neededRelation = 90; break;}
		default: neededRelation = 115;
		}
	}
	else if (type == AFFILIATION)
	{
		switch (m_iRaceProberty)
		{
		case SECRET: {neededRelation = 155; break;}
		case PACIFIST: {neededRelation = 140; break;}
		case RESEARCHER: {neededRelation = 130; break;}
		case FINANCIAL: {neededRelation = 120; break;}
		default: neededRelation = 145;
		}
	}
	return neededRelation;
}

/// Funktion berechnet wieviel und welche Mitgifte die KI bei einem Angebot mitgbt.
void CMajorRace::PerhapsGiveDowry(ComputerOfferStruct* theOffer, USHORT race, CEmpire* empire, CSystem systems[30][20], BYTE averageTechlevel, UINT* averageRessourceStorages, BOOLEAN flag)
{
	// flag == FALSE -> nicht für Minorrace gedacht
	// flag == TRUE  -> für Minorrace gedacht, mit Handelsvertrag
	// race == NOBODY-> dann geht es an eine Minorrace 
	
	// Als erstes gibt die KI-gesteuerte Rasse nur Geld, wenn sie mindst. 10% mehr als von dem globalen
	// Durchschnittswert hat.
	// Dies könnte man später vielleicht nochmal ändern, aber erstmal fällt mir da kein besserer Algo ein.
	// Hier schauen ob ich auch noch Latinum mit dazugebe.
	// Schauen ob ich 20% mehr als den Durchschnittswert aller erreiche bzw. wenn ich 25000 Latinum habe,
	// dann gebe ich zu 50% auch Latinum dazu.
	BOOLEAN haveMost = FALSE;
	long latinum = 0;
	USHORT counter = 0;
	for (int i = HUMAN; i <= DOMINION; i++)
		if (empire[i].GetNumberOfSystems() > 0)
		{
			latinum += empire[i].GetLatinum();
			counter++;
		}
	if (counter > 0)
		latinum /= counter;
	if ((race == NOBODY && empire[m_iRaceNumber].GetLatinum() > 3000 && rand()%3 == 0) // geht es an eine Minorrace
		||	// oder an eine Majorrace, da brauchen wir viel mehr Geld
		((empire[m_iRaceNumber].GetLatinum() >= (long)(latinum * 1.2) && empire[m_iRaceNumber].GetLatinum() > 1000)
		|| (empire[m_iRaceNumber].GetLatinum() > 25000 && rand()%2 > 0)))
	{
		latinum = rand()%(empire[m_iRaceNumber].GetLatinum()+1);
		latinum = (long)(latinum/250)*250;
		if (latinum > 5000)
			latinum = 5000;
		if (latinum != 0)
		{
			// Jetzt das Latinum in das Offer schreiben
			theOffer->latinum = (USHORT)latinum;
			// Das Latinum abziehen
			empire[m_iRaceNumber].SetLatinum(-latinum);
		}
	}
	if ((flag == FALSE && m_iDiplomacyStatus[race] >= TRADE_AGREEMENT) || flag == TRUE)
	{
	//   Ressourcen gibt die Rasse nur, wenn sie 10% mehr als den globalen Durschschnitt dieser Ressource hat.
		
	// zuerst random-wert ermitteln, welche Ressource ich dazugeben würde
	// dafür benutze ich den durchschnittlichen Techlevel im Universum. Am Anfang wird deswegen nur Titan angeboten
	// später kommen dann auch die weiteren Ressourcen hinzu, wenn der Wert zu groß ist machen wir das solange,
	// bis der Wert max. Iridium erhält
		USHORT whichRes = TITAN;
		do {
			whichRes = rand()%(averageTechlevel+1);
		} while (whichRes > IRIDIUM);
		// Schauen ob wir mehr als 20% von der Ressource haben als der Durschnitt ist
		// nicht gleich in der do_while-Schleife, da ich nicht so oft dies als Mitgift geben möchte
		BOOLEAN canGive = FALSE;
		if (empire[m_iRaceNumber].GetStorage()[whichRes] >= (ULONG)(averageRessourceStorages[whichRes] * 1.2))
			canGive = TRUE;
		// wenn wir das meiste davon haben, dann suchen wir unser System, wo wir am meisten von der Ressource gelagert haben
		if (canGive == TRUE)
		{
			ULONG mostRes = 0;
			CPoint system(0,0);
			for (int y = 0; y < 20; y++)
				for (int x = 0; x < 30; x++)
					if (systems[x][y].GetRessourceStore(whichRes) > mostRes && systems[x][y].GetOwnerOfSystem() == m_iRaceNumber)
					{
						mostRes = systems[x][y].GetRessourceStore(whichRes);
						system.x = x;
						system.y = y;
					}
		/*	CString d;
			d.Format("Meiste Ressource (%d) liegt in System: %d:%d\nBesitzer %s\nMenge: %d\nglobaler Durchschnitt: %d",whichRes,system.x,system.y,this->GetRaceName(m_iRaceNumber),mostRes,averageRessourceStorages[whichRes]);
			AfxMessageBox(d);*/
			// Jetzt geben wir eine Menge zwischen 0 und 20000 in 1000er Schritten als Mitgift (max. 20000)
			ULONG giveRes = rand()%(mostRes+1); // Wert zwischen 0 und dem Lagerinhalt generieren
			giveRes = (ULONG)(giveRes/1000)*1000;
			if (giveRes > 20000)
				giveRes = 20000;
			if (giveRes != 0)
			{
				// Jetzt in die Ressource für das Offer schreiben
				theOffer->ressource[whichRes] = (USHORT)giveRes;
				theOffer->KO.x = system.x;
				theOffer->KO.y = system.y;
				// Die Ressource aus dem Lager des Systems abziehen
				systems[system.x][system.y].SetRessourceStore(whichRes,giveRes*(-1));
			}
		}
	}
}

/// Funktion berechnet ob und was für eine Forderung wir stellen.
void CMajorRace::PerhapsHaveRequest(ComputerOfferStruct* theOffer, USHORT race, CEmpire* empire, CSystem systems[30][20],
									BYTE averageTechlevel, UINT* averageRessourceStorages, const UINT* shipPowers)
{
	// Wir stellen nur eine Forderung, wenn wir zuwenig von einer Ressource oder von Latinum haben.
	// Zuwenig müssen wir anhand des globalen Durchschnitts berechnen
	// Erstmal schauen ob ich eine Forderung mit Latinum stelle, dafür globalen Durchschnitt des Latinum berechnen
	long latinum = 0;
	USHORT counter = 0;
	for (int i = HUMAN; i <= DOMINION; i++)
		if (empire[i].GetNumberOfSystems() > 0)
		{
			latinum += empire[i].GetLatinum();
			counter++;
		}
	if (counter > 0)
		latinum /= counter;

	// Hier werden zusätzlich noch die Schiffsstärken der anderen Rassen mit der unseren verglichen. Ist die andere Rasse
	// stärker, so nehmen wir eher an.
	BOOLEAN stronger = FALSE;
	if (shipPowers[m_iRaceNumber] > 0)
	{
		// wenn unsere Beziehung hoch genug ist, oder wir einen Freundschaftsvertrag haben, so werden die Schiffsstärken
		// nicht mit beachtet
		if (m_iRelationshipOtherMajor[race] < 75 || m_iDiplomacyStatus[race] < FRIENDSHIP_AGREEMENT)
		{
			float compare = (float)(shipPowers[race]) / (float)(shipPowers[m_iRaceNumber]);
			if (compare < 0.6f && rand()%2 == 0)
				stronger = TRUE;
		}
	}

	// Jetzt haben wir den Durchschnittswert des Latinum, wenn wir nur rand()%30Prozent von diesem Wert besitzen, dann
	// stellen wir eine Forderung an eine andere zufällig ausgewählte Rasse
	if (stronger == TRUE || empire[m_iRaceNumber].GetLatinum() <= (long)(latinum * (rand()%30+1) / 100))
	{
		// Dann wollen wir Latinum sehen, Menge ist abhängig vom Techlevel, umso höher, desto wahrscheinlicher ist
		// auch eine höhere Forderung
		latinum = rand()%(1000 + averageTechlevel * 750);
		latinum = (long)(latinum/250)*250;
		if (latinum > 5000)
			latinum = 5000;
		if (latinum != 0)
		{
			// Jetzt das Latinum in das Offer schreiben
			theOffer->latinum = (USHORT)latinum;
			theOffer->type = DIP_REQUEST;
		}
	}
	// Nun können wir auch noch Ressourcen fordern, auch zusätzlich zum Latinum und wir benötigen dafür auch keinen
	// aktuellen Handelsvertrag mit der Rasse ;-)
	// zuerst random-wert ermitteln, welche Ressource ich fordern würde
	// dafür benutze ich den durchschnittlichen Techlevel im Universum. Am Anfang wird deswegen nur Titan angeboten
	// später kommen dann auch die weiteren Ressourcen hinzu, wenn der Wert zu groß ist machen wir das solange,
	// bis der Wert max. Iridium erhält
	BOOLEAN canGive = FALSE;
	USHORT whichRes = TITAN;
	int count = 0;
	do {
		whichRes = rand()%(averageTechlevel/2 + 1);
		if (whichRes > IRIDIUM)
			whichRes = IRIDIUM;
		// Schauen ob wir weniger als rand()%50Prozent von der Ressource haben als der Durschnitt
		if (stronger == TRUE || (empire[m_iRaceNumber].GetStorage()[whichRes] <= (ULONG)(averageRessourceStorages[whichRes] * (rand()%50+1) / 100)
			&& empire[m_iRaceNumber].GetStorage()[whichRes] != 0))
			canGive = TRUE;
		count++;
	} while (count < 20 && canGive == FALSE);
	// wenn wir zuwenig davon haben, dann stellen wir die Forderung
	if (canGive == TRUE)
	{
		// Jetzt müssen wir eine Menge zwischen 0 und 20000 in 1000er Schritten ermitteln (max. 20000)
		ULONG giveRes = rand()%(1000 + averageTechlevel * 1500);
		giveRes = (ULONG)(giveRes/1000)*1000;
		if (giveRes > 20000)
			giveRes = 20000;
		if (giveRes != 0)
		{
			// Jetzt in die Ressource für das Offer schreiben
			theOffer->ressource[whichRes] = (USHORT)giveRes;
			theOffer->type = DIP_REQUEST;
		}
	}
//	if (theOffer->type == DIP_REQUEST)
//		AfxMessageBox("FORDERUNG");
}