// Diplomacy.cpp: Implementierung der Klasse CDiplomacy.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "stdafx.h"
#include "Diplomacy.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_SERIAL (CDiplomacy, CObject, 1)

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

CDiplomacy::CDiplomacy()
{
	m_MadeOfferArray.RemoveAll();
	m_AnswerArray.RemoveAll();
	m_iDiplomacy = 0;
}

CDiplomacy::~CDiplomacy()
{
	m_MadeOfferArray.RemoveAll();
	m_AnswerArray.RemoveAll();
}

//////////////////////////////////////////////////////////////////////
// Kopierkonstruktor
//////////////////////////////////////////////////////////////////////
CDiplomacy::CDiplomacy(const CDiplomacy & rhs)
{
	m_iDiplomacy = rhs.m_iDiplomacy;
	for (int i = 0; i < rhs.m_MadeOfferArray.GetSize(); i++)
		m_MadeOfferArray.Add(rhs.m_MadeOfferArray.GetAt(i));
	for (int i = 0; i < rhs.m_AnswerArray.GetSize(); i++)
		m_AnswerArray.Add(rhs.m_AnswerArray.GetAt(i));
}

//////////////////////////////////////////////////////////////////////
// Zuweisungsoperator
//////////////////////////////////////////////////////////////////////
CDiplomacy & CDiplomacy::operator=(const CDiplomacy & rhs)
{
	if (this == &rhs)
		return *this;
	m_iDiplomacy = rhs.m_iDiplomacy;
	for (int i = 0; i < rhs.m_MadeOfferArray.GetSize(); i++)
		m_MadeOfferArray.Add(rhs.m_MadeOfferArray.GetAt(i));
	for (int i = 0; i < rhs.m_AnswerArray.GetSize(); i++)
		m_AnswerArray.Add(rhs.m_AnswerArray.GetAt(i));
	return *this;
}

///////////////////////////////////////////////////////////////////////
// Speichern / Laden
///////////////////////////////////////////////////////////////////////
void CDiplomacy::Serialize(CArchive &ar)		
{
	CObject::Serialize(ar);
	// wenn gespeichert wird
	if (ar.IsStoring())
	{		
		ar << m_iDiplomacy;
		ar << m_MadeOfferArray.GetSize();
		for (int i = 0; i < m_MadeOfferArray.GetSize(); i++)
		{
			ar << m_MadeOfferArray.GetAt(i).corruptedMajor;
			ar << m_MadeOfferArray.GetAt(i).KO;
			ar << m_MadeOfferArray.GetAt(i).latinum;
			ar << m_MadeOfferArray.GetAt(i).majorRace;
			ar << m_MadeOfferArray.GetAt(i).minorRace;
			ar << m_MadeOfferArray.GetAt(i).offerFromRace;
			for (int j = 0; j < 5; j++)
				ar << m_MadeOfferArray.GetAt(i).ressource[j];
			ar << m_MadeOfferArray.GetAt(i).round;
			ar << m_MadeOfferArray.GetAt(i).shownText;
			ar << m_MadeOfferArray.GetAt(i).type;
		}
		ar << m_AnswerArray.GetSize();
		for (int i = 0; i < m_AnswerArray.GetSize(); i++)
		{
			ar << m_AnswerArray.GetAt(i).headline;
			ar << m_AnswerArray.GetAt(i).majorRace;
			ar << m_AnswerArray.GetAt(i).message;
			ar << m_AnswerArray.GetAt(i).minorRace;			
		}
	}
	// wenn geladen wird
	if (ar.IsLoading())
	{
		int number = 0;
		ar >> m_iDiplomacy;		
		ar >> number;
		DiplomaticActionStruct das;
		m_MadeOfferArray.RemoveAll();
		for (int i = 0; i < number; i++)
		{
			ar >> das.corruptedMajor;
			ar >> das.KO;
			ar >> das.latinum;
			ar >> das.majorRace;
			ar >> das.minorRace;
			ar >> das.offerFromRace;
			for (int j = 0; j < 5; j++)
				ar >> das.ressource[j];
			ar >> das.round;
			ar >> das.shownText;
			ar >> das.type;
			m_MadeOfferArray.Add(das);
		}
		ar >> number;
		DiplomaticAnswersStruct da;
		m_AnswerArray.RemoveAll();
		for (int i = 0; i < number; i++)
		{
			ar >> da.headline;
			ar >> da.majorRace;
			ar >> da.message;
			ar >> da.minorRace;
			m_AnswerArray.Add(da);
		}
	}
}

//////////////////////////////////////////////////////////////////////
// sonstige Funktionen
//////////////////////////////////////////////////////////////////////

/// Funktion legt ein neues Strukturobjekt in dem Feld <code>m_MadeOfferArray</code> an. Dieses Feld wird bei
/// jeder neuen Runde durchlaufen. Wenn eine neue Runde erreicht wurde, welche in der jeweiligen Struktur steht,
/// dann wird die Aktion ausgeführt.
void CDiplomacy::MakeOfferToMinorRace(BYTE majorRace, USHORT minorRace, USHORT latinum, USHORT* ressource, USHORT currentRound, const CString &ShownText, short type, CPoint KO, BYTE corruptedMajor)
{
	// Werte an die Struktur übergeben
	DiplomaticActionStruct tempAction;
	tempAction.majorRace = majorRace;
	tempAction.minorRace = minorRace;
	tempAction.latinum = latinum;
	for (int i = 0; i < 5; i++)
		tempAction.ressource[i] = ressource[i];
	tempAction.round = currentRound;
	tempAction.shownText = ShownText;
	tempAction.type = type;
	tempAction.KO = KO;
	tempAction.corruptedMajor = corruptedMajor;
	tempAction.offerFromRace = FALSE;
	// Struktur in das dynamische Feld legen
	m_MadeOfferArray.Add(tempAction);
}

/// Wenn man ein Angebot an eine andere Majorrace machen will, dann muss diese Funktion aufgerufen werden, welche
/// einen Eintrag in <code>m_MadeOfferArray</code> macht. Dadurch hat man einer anderen Majorrace <code>toMajorRace
/// </code> ein Angebot gemacht. Diese Funktion wird für das Diplomatieobjekt aufgerufen, für die Rasse
/// welche das Angebot macht, während <code>GetOfferFromMajorRace()</code> für das Diplomatieobjekt aufgerufen wird,
/// an die das Angebot geht. <code>duration</code> hat hier mehrere Funktionen. Einerseits steht in der Variable
/// die Dauer eines Vertrags, natürlich	nur dort, wo man auch eine Dauer angeben kann. Bei einem Kriegspakt steht
/// in <code>duration</code> die Rasse, welcher der Krieg erklärt werden soll. Ansonsten wird diese Variable nicht benötigt.
void CDiplomacy::MakeOfferToMajorRace(BYTE toMajorRace, USHORT latinum, USHORT* ressource, USHORT currentRound, const CString &ShownText, short type, CPoint KO, BYTE duration)
{
	// Werte an die Struktur übergeben
	DiplomaticActionStruct tempAction;
	tempAction.majorRace = toMajorRace;
	tempAction.minorRace = 0;
	tempAction.latinum = latinum;
	for (int i = 0; i < 5; i++)
		tempAction.ressource[i] = ressource[i];
	tempAction.round = currentRound;
	tempAction.shownText = ShownText;
	tempAction.type = type;
	tempAction.KO = KO;
	tempAction.corruptedMajor = duration;	// Dauer des Vertrags
	tempAction.offerFromRace = FALSE;
	// Struktur in das dynamische Feld legen
	m_MadeOfferArray.Add(tempAction);
}

/// Funktion legt ein neues Strukturobjekt im Feld <code>m_MadeOfferArray</code> an. Dieses Feld wird bei jeder
/// neuen Runde durchlaufen. Wenn eine Runde erreicht wurde, welche in der jeweiligen Struktur steht, dann würde
/// die Aktion ausgeführt werden. <code>duration</code> hat hier mehrere Funktionen. Einerseits steht in der Variable
/// die Dauer eines Vertrags, natürlich	nur dort, wo man auch eine Dauer angeben kann. Bei einem Kriegspakt steht
/// in <code>duration</code> die Rasse, welcher der Krieg erklärt werden soll. Ansonsten wird diese Variable nicht benötigt.
void CDiplomacy::GetOfferFromMajorRace(BYTE myRace, USHORT latinum, USHORT* ressource, USHORT currentRound, const CString &ShownText, short type, CPoint KO, BYTE duration)
{
	// Werte an die Struktur übergeben
	DiplomaticActionStruct tempAction;
	tempAction.majorRace = myRace;
	tempAction.minorRace = 0;
	tempAction.latinum = latinum;
	for (int i = 0; i < 5; i++)
		tempAction.ressource[i] = ressource[i];
	tempAction.round = currentRound;
	tempAction.shownText = ShownText;
	tempAction.type = type;
	tempAction.KO = KO;
	tempAction.corruptedMajor = duration;	// Dauer des Vertrags
	tempAction.offerFromRace = NOT_REACTED;
	// Struktur in das dynamische Feld legen
	m_MadeOfferArray.Add(tempAction);
}

/// Funktion legt ein neues Strukturobjekt im Feld <code>m_MadeOfferArray</code> an. Nur das diesmal der Parameter
/// <code>offerFromRace</code> auf <code>TRUE</code> gesetzt wird.	Wenn dieses Feld durchlaufen wird und der Nutzer
/// dieses Objekt nicht durch eine Ablehnung des Angebots gelöscht hat, dann wird der Status der kleinen Rasse auf
/// ihr Angebot automatisch verändert.
void CDiplomacy::MinorMakesOfferToMajor(USHORT minorRace, BYTE majorRace, USHORT currentRound, short type,
										MinorRaceArray* minors, CMajorRace* majors, CEmpire* empire)
{
	// Werte der Struktur übergeben
	DiplomaticActionStruct tempAction;
	tempAction.minorRace = minorRace;
	tempAction.majorRace = majorRace;
	tempAction.latinum = 0;
	for (int i = 0; i < 5; i++)
		tempAction.ressource[i] = 0;
	tempAction.round = currentRound;
	tempAction.shownText = CGenDiploMessage::GenerateMinorOffer(minors->GetAt(minorRace).GetRaceName(),
		minors->GetAt(minorRace).GetKind(),type);
	tempAction.type = type;
	tempAction.corruptedMajor = NOBODY;
	tempAction.KO = 0;
	tempAction.offerFromRace = TRUE;
	// Struktur in das dynamische Feld legen
	// hier einfach noch überprüfen, das kein Angebot der Minor schon in dem Feld ist, weil
	// sonst kann es vorkommen, das sie uns z.B. ein Kooperationsangebot macht und in der nächsten
	// Runde einen Handelsvertrag anbietet, weil die Berechnung, ob wir angenommen haben erst
	// nach der Berechnung ob die Minorrace ein Angebot macht durchgeführt werden kann
	BOOL okay = TRUE;
	for (int i = 0; i < m_MadeOfferArray.GetSize(); i++)
		if (m_MadeOfferArray.GetAt(i).minorRace == minorRace)
			okay = FALSE;
	if (okay == TRUE)
	{
		m_MadeOfferArray.Add(tempAction);
		// jetzt das Angebote der kleinen Rasse durchgehen, die jeweilige Nachricht generieren und auf NOT_REACTED setzten
		// können gleich das letzte nehmen, weil wir es ja gerade hinzugefügt haben
		int i = m_MadeOfferArray.GetUpperBound();
		CString s;
		if (m_MadeOfferArray[i].type == TRADE_AGREEMENT)
		{
			s = CResourceManager::GetString("MIN_OFFER_TRADE",FALSE,minors->GetAt(m_MadeOfferArray[i].minorRace).GetRaceName());
			m_MadeOfferArray[i].offerFromRace = NOT_REACTED;	// NOT_REACTED bedeutet, Angebot der Minor auf nicht beachtet gesetzt
			CMessage message;
			message.GenerateMessage(s,DIPLOMACY,"",0,0,2);
			empire->AddMessage(message);
		}
		else if (m_MadeOfferArray[i].type == FRIENDSHIP_AGREEMENT)
		{
			s = CResourceManager::GetString("MIN_OFFER_FRIEND",FALSE,minors->GetAt(m_MadeOfferArray[i].minorRace).GetRaceName());
			m_MadeOfferArray[i].offerFromRace = NOT_REACTED;	// NOT_REACTED bedeutet, Angebot der Minor auf nicht beachtet gesetzt
			CMessage message;
			message.GenerateMessage(s,DIPLOMACY,"",0,0,2);
			empire->AddMessage(message);
		}
		else if (m_MadeOfferArray[i].type == COOPERATION)
		{
			s = CResourceManager::GetString("MIN_OFFER_COOP",FALSE,minors->GetAt(m_MadeOfferArray[i].minorRace).GetRaceName());
			m_MadeOfferArray[i].offerFromRace = NOT_REACTED;	// NOT_REACTED bedeutet, Angebot der Minor auf nicht beachtet gesetzt
			CMessage message;
			message.GenerateMessage(s,DIPLOMACY,"",0,0,2);
			empire->AddMessage(message);
		}
		else if (m_MadeOfferArray[i].type == AFFILIATION)
		{
			s = CResourceManager::GetString("MIN_OFFER_AFFI",FALSE,minors->GetAt(m_MadeOfferArray[i].minorRace).GetRaceName());
			m_MadeOfferArray[i].offerFromRace = NOT_REACTED;	// NOT_REACTED bedeutet, Angebot der Minor auf nicht beachtet gesetzt
			CMessage message;
			message.GenerateMessage(s,DIPLOMACY,"",0,0,2);
			empire->AddMessage(message);
		}
		else if (m_MadeOfferArray[i].type == MEMBERSHIP)
		{
			s = CResourceManager::GetString("MIN_OFFER_MEMBER",FALSE,minors->GetAt(m_MadeOfferArray[i].minorRace).GetRaceName());
			m_MadeOfferArray[i].offerFromRace = NOT_REACTED;	// NOT_REACTED bedeutet, Angebot der Minor auf nicht beachtet gesetzt
			CMessage message;
			message.GenerateMessage(s,DIPLOMACY,"",0,0,2);
			empire->AddMessage(message);
		}
		else if (m_MadeOfferArray[i].type == WAR)
		{
			s = CResourceManager::GetString("MIN_OFFER_WAR",FALSE,minors->GetAt(m_MadeOfferArray[i].minorRace).GetRaceName());
			CMessage message;
			message.GenerateMessage(s,DIPLOMACY,"",0,0,2);
			empire->AddMessage(message);
			
			// zusätzliche Eventnachricht wegen der Moral an das Imperium
			CString param = CResourceManager::GetString("FEMALE_ARTICLE")+" "+minors->GetAt(m_MadeOfferArray[i].minorRace).GetRaceName();
			CString eventText = "";
			// Other Empire Declares War when Neutral #31
			if (minors->GetAt(m_MadeOfferArray[i].minorRace).GetDiplomacyStatus(m_MadeOfferArray[i].majorRace) == NO_AGREEMENT)
				eventText = majors[m_MadeOfferArray[i].majorRace].GetMoralObserver()->AddEvent(31, m_MadeOfferArray[i].majorRace, param);
			// Other Empire Declares War with an Affiliation (or Membership) #33
			else if (minors->GetAt(m_MadeOfferArray[i].minorRace).GetDiplomacyStatus(m_MadeOfferArray[i].majorRace) >= AFFILIATION)
				eventText = majors[m_MadeOfferArray[i].majorRace].GetMoralObserver()->AddEvent(33, m_MadeOfferArray[i].majorRace, param);
			// Other Empire Declares War with Treaty #32
			else
				eventText = majors[m_MadeOfferArray[i].majorRace].GetMoralObserver()->AddEvent(32, m_MadeOfferArray[i].majorRace, param);			
			if (!eventText.IsEmpty())
			{
				message.GenerateMessage(eventText, SOMETHING, "", 0, 0);
				empire->AddMessage(message);
			}
			// Bei Krieg sofort berechnen, soll ja noch in der gleichen Runde geschehen und wir müssen nicht auf eine Reaktion warten
			minors->ElementAt(m_MadeOfferArray[i].minorRace).CalculateDiplomaticOffer(m_MadeOfferArray[i].majorRace,m_MadeOfferArray[i].type,m_MadeOfferArray[i].offerFromRace);
		}
	}
}

///	Diese Funktion wird bei jeder neuen Rundenberechnung aufgerufen und berechnet wann eine Aktion feuert
/// und generiert selbst neue diplomatische Nachrichten.
void CDiplomacy::CalculateDiplomaticFallouts(MinorRaceArray* minors, CMajorRace* majors, CEmpire* empire,
		USHORT currentRound, CSystem systems[30][20], UINT* averageRessourceStorages, CGlobalStorage* globalStorages, const UINT* shipPowers)
{
	// erstmal alle antworten der letzten Runde löschen
	m_AnswerArray.RemoveAll();
	for (int i = 0; i < m_MadeOfferArray.GetSize(); i++)
	{
		// Ist das Angebot von einer Minorrace?
		if (m_MadeOfferArray[i].majorRace == m_iDiplomacy)
		{
			// Achtung, falls diese Minorrace mittlerweile vernichtet wurde, könnte es zum Absturz kommen
			// deswegen die Struktur jedesmal überprüfen, wenn eine Minorrace vernichtet wurde und gegebenenfalls den
			// Eintrag dieser Rasse entfernen -> wird in der Funktion ClearArraysFromMinor() gemacht
			if (m_MadeOfferArray[i].round == (currentRound-1))
			{
				CString race;
				switch (m_MadeOfferArray[i].majorRace)
				{
				case 1:	{race = CResourceManager::GetString("ARTICLE_RACE1_EMPIRE", TRUE); break;}
				case 2:	{race = CResourceManager::GetString("ARTICLE_RACE2_EMPIRE", TRUE); break;}
				case 3:	{race = CResourceManager::GetString("ARTICLE_RACE3_EMPIRE", TRUE); break;}
				case 4:	{race = CResourceManager::GetString("ARTICLE_RACE4_EMPIRE", TRUE); break;}
				case 5:	{race = CResourceManager::GetString("ARTICLE_RACE5_EMPIRE", TRUE); break;}
				case 6:	{race = CResourceManager::GetString("ARTICLE_RACE6_EMPIRE", TRUE); break;}
				}
				CString race2;
				switch (m_MadeOfferArray[i].majorRace)
				{
				case 1:	{race2 = CResourceManager::GetString("ARTICLE_RACE1"); break;}
				case 2:	{race2 = CResourceManager::GetString("ARTICLE_RACE2"); break;}
				case 3:	{race2 = CResourceManager::GetString("ARTICLE_RACE3"); break;}
				case 4:	{race2 = CResourceManager::GetString("ARTICLE_RACE4"); break;}
				case 5:	{race2 = CResourceManager::GetString("ARTICLE_RACE5"); break;}
				case 6:	{race2 = CResourceManager::GetString("ARTICLE_RACE6"); break;}
				}
				// Wenn wir sozusagen eine Runde gewartet haben, dann Nachricht machen, das wir ein Geschenk gemacht haben
				if (m_MadeOfferArray[i].type == PRESENT)
				{
					CString s;
					CMessage message;
					if (m_MadeOfferArray[i].latinum > 0)
					{
						CString number;
						number.Format("%d",m_MadeOfferArray[i].latinum);
						s = race+" "+CResourceManager::GetString("LATINUM_PRESENT", FALSE, number, minors->GetAt(m_MadeOfferArray[i].minorRace).GetRaceName());
						message.GenerateMessage(s,DIPLOMACY,"",0,0);
						empire[m_MadeOfferArray[i].majorRace].AddMessage(message);
					}
					if (m_MadeOfferArray[i].ressource[TITAN] > 0)
					{
						CString number;
						number.Format("%d",m_MadeOfferArray[i].ressource[TITAN]);
						s = race+" "+CResourceManager::GetString("TITAN_PRESENT", FALSE, number, minors->GetAt(m_MadeOfferArray[i].minorRace).GetRaceName());
						message.GenerateMessage(s,DIPLOMACY,"",0,0);
						empire[m_MadeOfferArray[i].majorRace].AddMessage(message);
					}
					else if (m_MadeOfferArray[i].ressource[DEUTERIUM] > 0)
					{
						CString number;
						number.Format("%d",m_MadeOfferArray[i].ressource[DEUTERIUM]);
						s = race+" "+CResourceManager::GetString("DEUTERIUM_PRESENT", FALSE, number, minors->GetAt(m_MadeOfferArray[i].minorRace).GetRaceName());
						message.GenerateMessage(s,DIPLOMACY,"",0,0);
						empire[m_MadeOfferArray[i].majorRace].AddMessage(message);
					}
					else if (m_MadeOfferArray[i].ressource[DURANIUM] > 0)
					{
						CString number;
						number.Format("%d",m_MadeOfferArray[i].ressource[DURANIUM]);
						s = race+" "+CResourceManager::GetString("DURANIUM_PRESENT", FALSE, number, minors->GetAt(m_MadeOfferArray[i].minorRace).GetRaceName());
						message.GenerateMessage(s,DIPLOMACY,"",0,0);
						empire[m_MadeOfferArray[i].majorRace].AddMessage(message);
					}
					else if (m_MadeOfferArray[i].ressource[CRYSTAL] > 0)
					{
						CString number;
						number.Format("%d",m_MadeOfferArray[i].ressource[CRYSTAL]);
						s = race+" "+CResourceManager::GetString("CRYSTAL_PRESENT", FALSE, number, minors->GetAt(m_MadeOfferArray[i].minorRace).GetRaceName());
						message.GenerateMessage(s,DIPLOMACY,"",0,0);
						empire[m_MadeOfferArray[i].majorRace].AddMessage(message);
					}
					else if (m_MadeOfferArray[i].ressource[IRIDIUM] > 0)
					{
						CString number;
						number.Format("%d",m_MadeOfferArray[i].ressource[IRIDIUM]);
						s = race+" "+CResourceManager::GetString("IRIDIUM_PRESENT", FALSE, number, minors->GetAt(m_MadeOfferArray[i].minorRace).GetRaceName());
						message.GenerateMessage(s,DIPLOMACY,"",0,0);
						empire[m_MadeOfferArray[i].majorRace].AddMessage(message);
					}
				}
				else if (m_MadeOfferArray[i].type == WAR)
				{
					CString s;
					// Wenn wir der Minorrace den Krieg erklärt haben
					if (m_MadeOfferArray[i].offerFromRace == FALSE)
					{
						s = race+" "+CResourceManager::GetString("OUR_MIN_OFFER_WAR", FALSE, minors->GetAt(m_MadeOfferArray[i].minorRace).GetRaceName());
						m_AnswerArray.Add(this->GenerateMinorAnswer(m_MadeOfferArray[i].majorRace,m_MadeOfferArray[i].minorRace,WAR,
							minors->GetAt(m_MadeOfferArray[i].minorRace).GetRaceName(),minors->GetAt(m_MadeOfferArray[i].minorRace).GetKind(),FALSE));
					}
					CMessage message;
					message.GenerateMessage(s,DIPLOMACY,"",0,0);
					empire[m_MadeOfferArray[i].majorRace].AddMessage(message);
					// zusätzliche Eventnachricht wegen der Moral an das Imperium
					CString param = CResourceManager::GetString("FEMALE_ARTICLE")+" "+minors->GetAt(m_MadeOfferArray[i].minorRace).GetRaceName();
					CString eventText = "";
					// Declare War on an Empire when Neutral #24
					if (minors->GetAt(m_MadeOfferArray[i].minorRace).GetDiplomacyStatus(m_MadeOfferArray[i].majorRace) == NO_AGREEMENT)
						eventText = majors[m_MadeOfferArray[i].majorRace].GetMoralObserver()->AddEvent(24, m_MadeOfferArray[i].majorRace, param);
					// Declare War on an Empire with Trade Treaty #26
					else if (minors->GetAt(m_MadeOfferArray[i].minorRace).GetDiplomacyStatus(m_MadeOfferArray[i].majorRace) == TRADE_AGREEMENT)
						eventText = majors[m_MadeOfferArray[i].majorRace].GetMoralObserver()->AddEvent(26, m_MadeOfferArray[i].majorRace, param);
					// Declare War on an Empire with Friendship Treaty #27
					else if (minors->GetAt(m_MadeOfferArray[i].minorRace).GetDiplomacyStatus(m_MadeOfferArray[i].majorRace) == FRIENDSHIP_AGREEMENT)
						eventText = majors[m_MadeOfferArray[i].majorRace].GetMoralObserver()->AddEvent(27, m_MadeOfferArray[i].majorRace, param);
					// Declare War on an Empire with CooperationTreaty #29
					else if (minors->GetAt(m_MadeOfferArray[i].minorRace).GetDiplomacyStatus(m_MadeOfferArray[i].majorRace) == COOPERATION)
						eventText = majors[m_MadeOfferArray[i].majorRace].GetMoralObserver()->AddEvent(29, m_MadeOfferArray[i].majorRace, param);
					// Declare War on an Empire with Affiliation #30 (auch bei Mitgliedschaft wird)
					else if (minors->GetAt(m_MadeOfferArray[i].minorRace).GetDiplomacyStatus(m_MadeOfferArray[i].majorRace) >= AFFILIATION)
						eventText = majors[m_MadeOfferArray[i].majorRace].GetMoralObserver()->AddEvent(30, m_MadeOfferArray[i].majorRace, param);
					if (!eventText.IsEmpty())
					{
						message.GenerateMessage(eventText, SOMETHING, "", 0, 0);
						empire[m_MadeOfferArray[i].majorRace].AddMessage(message);
					}
					minors->ElementAt(m_MadeOfferArray[i].minorRace).CalculateDiplomaticOffer(m_MadeOfferArray[i].majorRace,m_MadeOfferArray[i].type,m_MadeOfferArray[i].offerFromRace);
					// Hier überprüfen, ob diese Minorrace mit einer anderen Majorrace (nicht wir) ein Bündnis hat
					// dann erklären wir dieser Majorrace auch den Krieg
					for (int j = HUMAN; j <= DOMINION; j++)
						if (minors->GetAt(m_MadeOfferArray[i].minorRace).GetDiplomacyStatus(j) == AFFILIATION && j != m_iDiplomacy) 
						{
							majors[m_iDiplomacy].SetDiplomaticStatus(j,WAR);	// ich hab Krieg mit dem Bündnispartner
							majors[j].SetDiplomaticStatus(m_iDiplomacy,WAR);	// Bündnispartner hat Krieg mit mir
							CString partner;
							switch (j)
							{
								case HUMAN: {partner = CResourceManager::GetString("TO_RACE1_EMPIRE"); break;}
								case FERENGI: {partner = CResourceManager::GetString("TO_RACE2_EMPIRE"); break;}
								case KLINGON: {partner = CResourceManager::GetString("TO_RACE3_EMPIRE"); break;}
								case ROMULAN: {partner = CResourceManager::GetString("TO_RACE4_EMPIRE"); break;}
								case CARDASSIAN: {partner = CResourceManager::GetString("TO_RACE5_EMPIRE"); break;}
								case DOMINION: {partner = CResourceManager::GetString("TO_RACE6_EMPIRE"); break;}
							}
							// Nachricht für uns
							s = CResourceManager::GetString("WE_DECLARE_WAR",FALSE,partner);
							message.GenerateMessage(s,DIPLOMACY,"",0,0);
							empire[m_iDiplomacy].AddMessage(message);
							// Nachricht und Kriegserklärung an den Bündnispartner
							s = race+" "+CResourceManager::GetString("WAR_TO_PARTNER", FALSE, minors->GetAt(i).GetRaceName());
							message.GenerateMessage(s,DIPLOMACY,"",0,0);
							empire[j].AddMessage(message);
							s = race+" "+CResourceManager::GetString("WAR_TO_US_AS_PARTNER");
							message.GenerateMessage(s,DIPLOMACY,"",0,0);
							empire[j].AddMessage(message);
							// Beziehung zu der verbündeten Majorrace verschlechtert sich
							majors[m_iDiplomacy].SetRelationshipToMajorRace(j,-(rand()%25+25)); // ich zu ihnen
							majors[j].SetRelationshipToMajorRace(m_iDiplomacy,-(rand()%50+25)); // Bündnispartner zu mir
						}
				}
				// Wenn wir sozusagen eine Runde gewartet haben, dann Nachricht machen, das wir eine Bestechung versuchen
				else if (m_MadeOfferArray[i].type == CORRUPTION)
				{
					CString s;
					if (m_MadeOfferArray[i].offerFromRace == FALSE)
						s = race+" "+CResourceManager::GetString("TRY_CORRUPTION", FALSE, minors->GetAt(m_MadeOfferArray[i].minorRace).GetRaceName());
					CMessage message;
					message.GenerateMessage(s,DIPLOMACY,"",0,0);
					empire[m_MadeOfferArray[i].majorRace].AddMessage(message);
				}
				// Wenn wir sozusagen eine Runde gewartet haben, dann Nachricht machen, das wir Vertrag aufgehoben haben
				else if (m_MadeOfferArray[i].type == NO_AGREEMENT)
				{
					CString s;
					if (m_MadeOfferArray[i].offerFromRace == FALSE)
						s = race+" "+CResourceManager::GetString("CANCEL_CONTRACT", FALSE, minors->GetAt(m_MadeOfferArray[i].minorRace).GetRaceName());
					CMessage message;
					message.GenerateMessage(s,DIPLOMACY,"",0,0);
					empire[m_MadeOfferArray[i].majorRace].AddMessage(message);
					minors->ElementAt(m_MadeOfferArray[i].minorRace).CalculateDiplomaticOffer(m_MadeOfferArray[i].majorRace,m_MadeOfferArray[i].type,m_MadeOfferArray[i].offerFromRace);
				}
				// Wenn wir sozusagen eine Runde gewartet haben, dann Nachricht machen, das wir einen Vertrag angeboten haben
				else
				{
					CString s;
					CString agreement;
					switch(m_MadeOfferArray[i].type)
					{
					case TRADE_AGREEMENT: {agreement = CResourceManager::GetString("TRADE_AGREEMENT_WITH_ARTICLE"); break;}
					case FRIENDSHIP_AGREEMENT: {agreement = CResourceManager::GetString("FRIENDSHIP_WITH_ARTICLE"); break;}
					case COOPERATION: {agreement = CResourceManager::GetString("COOPERATION_WITH_ARTICLE"); break;}
					case AFFILIATION: {agreement = CResourceManager::GetString("AFFILIATION_WITH_ARTICLE"); break;}
					case MEMBERSHIP: {agreement = CResourceManager::GetString("MEMBERSHIP_WITH_ARTICLE"); break;}
					}
					if (m_MadeOfferArray[i].offerFromRace == FALSE)
						s = race+" "+CResourceManager::GetString("OUR_MIN_OFFER", FALSE, minors->GetAt(m_MadeOfferArray[i].minorRace).GetRaceName(), agreement);
					CMessage message;
					message.GenerateMessage(s,DIPLOMACY,"",0,0);
					empire[m_MadeOfferArray[i].majorRace].AddMessage(message);
				}
			}
			// Nach 2 Runden fangen wir an, der Rasse wirklich das Geld zu geben, wodurch sich ihre Beziehung ändern könnte
			/* Latinum und Ressourcen werden bei Geschenk und Bestechung auf jeden Fall übergeben. Bei den anderen
			   Verträgen wird das nur übergeben, wenn die kleine Rasse auch annimmt. Wenn sie ablehnt, dann bekommen
			   wir das Latinum und die Ressourcen zurück. Die beziehungstechnischen Auswirkungen dieser Zugaben wird
			   trotzdem zuvor berechnet. Wenn die Rasse dann ablehnt, wird die Auswirkung auf die Beziehung wieder
			   rückgängig gemacht
			*/			
			if (m_MadeOfferArray[i].round == currentRound-2 && m_MadeOfferArray[i].type != WAR)
			{
				// aktuelle Beziehungen der Minorrace zu allen anderen Majors holen, weil wenn die Minorrace
				// ablehnt, dann müssen wir die Beziehungen wieder auf diese Werte hier zurücksetzen
				short relations[7];
				// aktuelle (alte) Beziehung holen
				for (int j = HUMAN; j <= DOMINION; j++)
					relations[j] = minors->GetAt(m_MadeOfferArray[i].minorRace).GetRelationshipToMajorRace(j);
				CString race;
				switch (m_MadeOfferArray[i].majorRace)
				{
					case HUMAN: {race = CResourceManager::GetString("TO_RACE1_EMPIRE"); break;}
					case FERENGI: {race = CResourceManager::GetString("TO_RACE2_EMPIRE"); break;}
					case KLINGON: {race = CResourceManager::GetString("TO_RACE3_EMPIRE"); break;}
					case ROMULAN: {race = CResourceManager::GetString("TO_RACE4_EMPIRE"); break;}
					case CARDASSIAN: {race = CResourceManager::GetString("TO_RACE5_EMPIRE"); break;}
					case DOMINION: {race = CResourceManager::GetString("TO_RACE6_EMPIRE"); break;}
				}
				CString agreement;
				USHORT LatFromRes = 0;	// Latinum, welches unsere gegeben Ressourcen wert sind
				// Hier wird der Minorrace die Ressource übergeben, außer bei Bestechung
				if ((m_MadeOfferArray[i].ressource[0] > 0 || m_MadeOfferArray[i].ressource[1] > 0 || m_MadeOfferArray[i].ressource[2] > 0 ||
					m_MadeOfferArray[i].ressource[3] > 0 || m_MadeOfferArray[i].ressource[4] > 0) && m_MadeOfferArray[i].type != CORRUPTION)
				LatFromRes = minors->ElementAt(m_MadeOfferArray[i].minorRace).GiveResourcePresent(m_MadeOfferArray[i].majorRace,m_MadeOfferArray[i].ressource,FALSE);
				// Hier wird der Minorrace das Geld übergeben, außer bei Bestechung
				if (m_MadeOfferArray[i].latinum + LatFromRes > 0 && m_MadeOfferArray[i].type != CORRUPTION)
					minors->ElementAt(m_MadeOfferArray[i].minorRace).GiveLatinumPresent(m_MadeOfferArray[i].majorRace,
						majors[m_MadeOfferArray[i].majorRace].GetDiplomacyBonus(), m_MadeOfferArray[i].latinum + LatFromRes);
				// aktuelle Beziehung (nach Übergabe der Ressourcen und des Latinums holen und von der alten Beziehung subtrahieren
				for (int j = HUMAN; j <= DOMINION; j++)
					relations[j] -= minors->GetAt(m_MadeOfferArray[i].minorRace).GetRelationshipToMajorRace(j);
				
				// Wenn die Majorrace der Computer spielt, dann entscheiden, ob das Angebot der Minorrace
				// angenommen wird. Angenommen wird einfach zu 95%, zu 3% wird nicht reagiert und zu 2% abgelehnt
				if (empire[m_iDiplomacy].GetPlayerOfEmpire() == COMPUTER && (m_MadeOfferArray[i].offerFromRace == TRUE || m_MadeOfferArray[i].offerFromRace == NOT_REACTED))
				{
					USHORT result = rand()%100;
					if (result > 10)		
						result = TRUE;
					else if (result > 1)	
						result = NOT_REACTED;
					else
						result = DECLINED;
					m_MadeOfferArray[i].offerFromRace = result;
				}

				// Wenn wir der Minorrace ein Vertragsangebot gemacht haben
				if (m_MadeOfferArray[i].type != CORRUPTION && m_MadeOfferArray[i].type != PRESENT
					&& m_MadeOfferArray[i].type != NO_AGREEMENT && m_MadeOfferArray[i].offerFromRace != NOT_REACTED)
				{
					BOOL result = minors->ElementAt(m_MadeOfferArray[i].minorRace).CalculateDiplomaticOffer(m_MadeOfferArray[i].majorRace,m_MadeOfferArray[i].type,m_MadeOfferArray[i].offerFromRace);
					CString s;
					CString eventText = "";
					if (result == TRUE)
					{
						switch(m_MadeOfferArray[i].type)
						{
						case TRADE_AGREEMENT: {agreement = CResourceManager::GetString("TRADE_AGREEMENT_WITH_ARTICLE"); break;}
						case FRIENDSHIP_AGREEMENT: {agreement = CResourceManager::GetString("FRIENDSHIP_WITH_ARTICLE"); break;}
						case COOPERATION: {agreement = CResourceManager::GetString("COOPERATION_WITH_ARTICLE"); break;}
						case AFFILIATION: {agreement = CResourceManager::GetString("AFFILIATION_WITH_ARTICLE"); break;}
						case MEMBERSHIP: {agreement = CResourceManager::GetString("MEMBERSHIP_WITH_ARTICLE");
								eventText = majors[m_MadeOfferArray[i].majorRace].GetMoralObserver()->AddEvent(10,
									m_MadeOfferArray[i].majorRace,minors->GetAt(m_MadeOfferArray[i].minorRace).GetRaceName());
								break;}
						}
						s = CResourceManager::GetString("FEMALE_ARTICLE",TRUE)+" "+minors->GetAt(m_MadeOfferArray[i].minorRace).GetRaceName()
							+" "+CResourceManager::GetString("MIN_ACCEPT_OFFER", FALSE, agreement, race);
						//s.Format("Die %s schliessen %s mit %s ab",minors->GetAt(m_MadeOfferArray[i].minorRace).GetRaceName(),agreement,race);
						m_AnswerArray.Add(this->GenerateMinorAnswer(m_MadeOfferArray[i].majorRace,m_MadeOfferArray[i].minorRace,
							m_MadeOfferArray[i].type,minors->GetAt(m_MadeOfferArray[i].minorRace).GetRaceName(),minors->GetAt(m_MadeOfferArray[i].minorRace).GetKind(),TRUE));
					}
					else if (m_MadeOfferArray[i].offerFromRace != DECLINED)	// Wir haben es nicht abgelehnt, sondern die Minorrace selbst
					{
						switch(m_MadeOfferArray[i].type)
						{
						case TRADE_AGREEMENT: {agreement = CResourceManager::GetString("TRADE_AGREEMENT"); break;}
						case FRIENDSHIP_AGREEMENT: {agreement = CResourceManager::GetString("FRIENDSHIP"); break;}
						case COOPERATION: {agreement = CResourceManager::GetString("COOPERATION"); break;}
						case AFFILIATION: {agreement = CResourceManager::GetString("AFFILIATION"); break;}
						case MEMBERSHIP: {agreement = CResourceManager::GetString("MEMBERSHIP"); break;}
						}
						s = CResourceManager::GetString("MIN_DECLINE_OFFER", FALSE, minors->GetAt(m_MadeOfferArray[i].minorRace).GetRaceName(),agreement);
						m_AnswerArray.Add(this->GenerateMinorAnswer(m_MadeOfferArray[i].majorRace,m_MadeOfferArray[i].minorRace,
							m_MadeOfferArray[i].type,minors->GetAt(m_MadeOfferArray[i].minorRace).GetRaceName(),minors->GetAt(m_MadeOfferArray[i].minorRace).GetKind(),FALSE));
						/* Wurde unser Angebot abgelehnt, dann Beziehungsauswirkung durch Latinum und Ressourcen, welche
						   vorher berechnet wurde wieder rückgängig machen. Mögliche Beziehungsverschlechterung durch
						   Ablehnung aber beibehalten. Latinum und Ressourcen der Majorrace wieder gutschreiben.
						   (Algo: neue Beziehung = Beziehung nach Ablehnung + alte Beziehung - Beziehung nach Latinum- und Resübergabe)
						*/
						for (int j = HUMAN; j <= DOMINION; j++)
							minors->ElementAt(m_MadeOfferArray[i].minorRace).SetRelationshipToMajorRace(j,relations[j]);
						// jetzt mir noch das Latinum und die Ressourcen zurückgeben
						empire[m_MadeOfferArray[i].majorRace].SetLatinum(m_MadeOfferArray[i].latinum);
						systems[m_MadeOfferArray[i].KO.x][m_MadeOfferArray[i].KO.y].SetTitanStore(m_MadeOfferArray[i].ressource[0]);
						systems[m_MadeOfferArray[i].KO.x][m_MadeOfferArray[i].KO.y].SetDeuteriumStore(m_MadeOfferArray[i].ressource[1]);
						systems[m_MadeOfferArray[i].KO.x][m_MadeOfferArray[i].KO.y].SetDuraniumStore(m_MadeOfferArray[i].ressource[2]);
						systems[m_MadeOfferArray[i].KO.x][m_MadeOfferArray[i].KO.y].SetCrystalStore(m_MadeOfferArray[i].ressource[3]);
						systems[m_MadeOfferArray[i].KO.x][m_MadeOfferArray[i].KO.y].SetIridiumStore(m_MadeOfferArray[i].ressource[4]);
					}
					else
					{
						switch(m_MadeOfferArray[i].type)
						{
							case TRADE_AGREEMENT: {agreement = CResourceManager::GetString("TRADE_AGREEMENT"); break;}
							case FRIENDSHIP_AGREEMENT: {agreement = CResourceManager::GetString("FRIENDSHIP"); break;}
							case COOPERATION: {agreement = CResourceManager::GetString("COOPERATION"); break;}
							case AFFILIATION: {agreement = CResourceManager::GetString("AFFILIATION"); break;}
							case MEMBERSHIP: {agreement = CResourceManager::GetString("MEMBERSHIP"); break;}
						}
						s = CResourceManager::GetString("WE_DECLINE_MIN_OFFER", FALSE, agreement,minors->GetAt(m_MadeOfferArray[i].minorRace).GetRaceName());
					}
					CMessage message;
					message.GenerateMessage(s,DIPLOMACY,"",0,0);
					empire[m_MadeOfferArray[i].majorRace].AddMessage(message);
					// zusätzliche Eventnachricht, wegen der Moral an das Imperium
					if (!eventText.IsEmpty())
					{
						message.GenerateMessage(eventText,SOMETHING,"",0,0);
						empire[m_MadeOfferArray[i].majorRace].AddMessage(message);
					}
				}
				// Wenn wir einen Bestechungsversuch machen wollen
				else if (m_MadeOfferArray[i].type == CORRUPTION && m_MadeOfferArray[i].offerFromRace != NOT_REACTED)
				{
					USHORT latinumFromRessource = 0;
					// Schauen, ob wir Ressourcen dazugegeben haben, dann Wert der Ressourcen in "latinumFromRessource" speichern
					if (m_MadeOfferArray[i].ressource[0] > 0 || m_MadeOfferArray[i].ressource[1] > 0 || m_MadeOfferArray[i].ressource[2] > 0 ||
							m_MadeOfferArray[i].ressource[3] > 0 || m_MadeOfferArray[i].ressource[4] > 0)
						latinumFromRessource = minors->ElementAt(m_MadeOfferArray[i].minorRace).GiveResourcePresent(m_MadeOfferArray[i].majorRace,m_MadeOfferArray[i].ressource,TRUE);
					// Jetzt Bestechungsfunktion aufrufen, die Messages werden in der Bestechungsfunktion generiert und abgeschickt
					BOOL result = minors->ElementAt(m_MadeOfferArray[i].minorRace).TryCorruption(m_MadeOfferArray[i].majorRace,m_MadeOfferArray[i].latinum + latinumFromRessource, empire, m_MadeOfferArray[i].corruptedMajor);
					CString s;
					if (result == TRUE)
						m_AnswerArray.Add(this->GenerateMinorAnswer(m_MadeOfferArray[i].majorRace,m_MadeOfferArray[i].minorRace,
							CORRUPTION,minors->GetAt(m_MadeOfferArray[i].minorRace).GetRaceName(),minors->GetAt(m_MadeOfferArray[i].minorRace).GetKind(),TRUE));
					else
						m_AnswerArray.Add(this->GenerateMinorAnswer(m_MadeOfferArray[i].majorRace,m_MadeOfferArray[i].minorRace,
							CORRUPTION,minors->GetAt(m_MadeOfferArray[i].minorRace).GetRaceName(),minors->GetAt(m_MadeOfferArray[i].minorRace).GetKind(),FALSE));
				}
				// Eintrag entfernen
				m_MadeOfferArray.RemoveAt(i);
				// i runterzählen, weil wir ja das Feld verkleinern und sich damit auch GetSize() um eins verkleinert
				i--;
			}
			// Wenn die Minorrace uns den Krieg erklärt hat, dann das hier separat behandeln, weil das oben nicht
			// behandelt wird
			else if (m_MadeOfferArray[i].round == currentRound-2 && m_MadeOfferArray[i].type == WAR && m_MadeOfferArray[i].offerFromRace == TRUE)
			{
				// Eintrag entfernen
				m_MadeOfferArray.RemoveAt(i);
				// i runterzählen, weil wir ja das Feld verkleinern und sich damit auch GetSize() um eins verkleinert
				i--;
			}

		}
		//
		//
		// Wenn das Angebot von einer anderen Majorrace stammt
		//
		//
		else
		{
			CString race;
			switch (m_MadeOfferArray[i].majorRace)
			{
				case 1:	{race = CResourceManager::GetString("ARTICLE_RACE1_EMPIRE", TRUE); break;}
				case 2:	{race = CResourceManager::GetString("ARTICLE_RACE2_EMPIRE", TRUE); break;}
				case 3:	{race = CResourceManager::GetString("ARTICLE_RACE3_EMPIRE", TRUE); break;}
				case 4:	{race = CResourceManager::GetString("ARTICLE_RACE4_EMPIRE", TRUE); break;}
				case 5:	{race = CResourceManager::GetString("ARTICLE_RACE5_EMPIRE", TRUE); break;}
				case 6:	{race = CResourceManager::GetString("ARTICLE_RACE6_EMPIRE", TRUE); break;}
			}
			CString race2;
			switch (m_MadeOfferArray[i].majorRace)
			{
				case HUMAN: {race2 = CResourceManager::GetString("TO_RACE1_EMPIRE"); break;}
				case FERENGI: {race2 = CResourceManager::GetString("TO_RACE2_EMPIRE"); break;}
				case KLINGON: {race2 = CResourceManager::GetString("TO_RACE3_EMPIRE"); break;}
				case ROMULAN: {race2 = CResourceManager::GetString("TO_RACE4_EMPIRE"); break;}
				case CARDASSIAN: {race2 = CResourceManager::GetString("TO_RACE5_EMPIRE"); break;}
				case DOMINION: {race2 = CResourceManager::GetString("TO_RACE6_EMPIRE"); break;}
			}
			CString race3;
			switch (m_iDiplomacy)
			{
				case 1:	{race3 = CResourceManager::GetString("ARTICLE_RACE1_EMPIRE", TRUE); break;}
				case 2:	{race3 = CResourceManager::GetString("ARTICLE_RACE2_EMPIRE", TRUE); break;}
				case 3:	{race3 = CResourceManager::GetString("ARTICLE_RACE3_EMPIRE", TRUE); break;}
				case 4:	{race3 = CResourceManager::GetString("ARTICLE_RACE4_EMPIRE", TRUE); break;}
				case 5:	{race3 = CResourceManager::GetString("ARTICLE_RACE5_EMPIRE", TRUE); break;}
				case 6:	{race3 = CResourceManager::GetString("ARTICLE_RACE6_EMPIRE", TRUE); break;}
			}
			CString race4;
			switch (m_iDiplomacy)
			{
				case HUMAN: {race4 = CResourceManager::GetString("TO_RACE1_EMPIRE"); break;}
				case FERENGI: {race4 = CResourceManager::GetString("TO_RACE2_EMPIRE"); break;}
				case KLINGON: {race4 = CResourceManager::GetString("TO_RACE3_EMPIRE"); break;}
				case ROMULAN: {race4 = CResourceManager::GetString("TO_RACE4_EMPIRE"); break;}
				case CARDASSIAN: {race4 = CResourceManager::GetString("TO_RACE5_EMPIRE"); break;}
				case DOMINION: {race4 = CResourceManager::GetString("TO_RACE6_EMPIRE"); break;}
			}
			CString race5;
			switch (m_MadeOfferArray[i].majorRace)
			{
				case 1:	{race5 = CResourceManager::GetString("ARTICLE_RACE1"); break;}
				case 2:	{race5 = CResourceManager::GetString("ARTICLE_RACE2"); break;}
				case 3:	{race5 = CResourceManager::GetString("ARTICLE_RACE3"); break;}
				case 4:	{race5 = CResourceManager::GetString("ARTICLE_RACE4"); break;}
				case 5:	{race5 = CResourceManager::GetString("ARTICLE_RACE5"); break;}
				case 6:	{race5 = CResourceManager::GetString("ARTICLE_RACE6"); break;}
			}
			// Nach einer Runden die Nachrichten an die jeweiligen Imperien verschicken
			if (m_MadeOfferArray[i].round == (currentRound-1))
			{
				CString agreement;
				switch(m_MadeOfferArray[i].type)
				{
				case TRADE_AGREEMENT: {agreement = CResourceManager::GetString("TRADE_AGREEMENT_WITH_ARTICLE"); break;}
				case FRIENDSHIP_AGREEMENT: {agreement = CResourceManager::GetString("FRIENDSHIP_WITH_ARTICLE"); break;}
				case COOPERATION: {agreement = CResourceManager::GetString("COOPERATION_WITH_ARTICLE"); break;}
				case AFFILIATION: {agreement = CResourceManager::GetString("AFFILIATION_WITH_ARTICLE"); break;}
				case NON_AGGRESSION_PACT: {agreement = CResourceManager::GetString("NON_AGGRESSION_WITH_ARTICLE"); break;}
				case DEFENCE_PACT: {agreement = CResourceManager::GetString("DEFENCE_PACT_WITH_ARTICLE"); break;}
				case WAR_PACT: {agreement = CResourceManager::GetString("WAR_PACT_WITH_ARTICLE", FALSE, CMajorRace::GetRaceName(m_MadeOfferArray[i].corruptedMajor)); break;}
				case DIP_REQUEST: {agreement = CResourceManager::GetString("REQUEST_WITH_ARTICLE"); break;}
				}
				// Wenn wir sozusagen eine Runde gewartet haben, dann Nachricht machen, das wir ein Geschenk gemacht haben
				// dieses Geschenk der jeweiligen Rasse gutschreiben, so dass sie es sofort hat
				if (m_MadeOfferArray[i].type == PRESENT)
				{
					CString s;
					CMessage message;
					if (m_MadeOfferArray[i].latinum > 0 || (m_MadeOfferArray[i].ressource[0] > 0 || m_MadeOfferArray[i].ressource[1] > 0 || m_MadeOfferArray[i].ressource[2] > 0 ||
						m_MadeOfferArray[i].ressource[3] > 0 || m_MadeOfferArray[i].ressource[4] > 0) )
					{
						if (m_MadeOfferArray[i].offerFromRace == NOT_REACTED)	// Nachricht für mich, das wir ein Angebot bekommen haben
						{
							s = CResourceManager::GetString("WE_GET_PRESENT", FALSE, race);
							message.GenerateMessage(s,DIPLOMACY,"",0,0,2);
							empire[m_iDiplomacy].AddMessage(message);
							// die Beziehungsverbesserung zur Geberrasse berechnen
							USHORT LatFromRes = majors[m_iDiplomacy].GiveResourcePresent(m_MadeOfferArray[i].ressource, empire, m_MadeOfferArray[i].majorRace);
							majors[m_iDiplomacy].GiveLatinumPresent(m_MadeOfferArray[i].latinum + LatFromRes, empire, m_MadeOfferArray[i].majorRace);
							// Das Latinum des Geschenkes mir gutschreiben
							empire[m_iDiplomacy].SetLatinum(m_MadeOfferArray[i].latinum);
							// Die Rohstoffe des Geschenkes mir gutschreiben
							// HIER INS GLOBALE LAGER
							for (int k = TITAN; k <= IRIDIUM; k++)
								if (m_MadeOfferArray[i].ressource[k] > 0)
									globalStorages[m_iDiplomacy].AddRessource(m_MadeOfferArray[i].ressource[k],k,CPoint(-1,-1));
						}
						else if (m_MadeOfferArray[i].offerFromRace == FALSE)	// Nachricht für mich, das wir ein Angebot gemacht haben
						{
							s = CResourceManager::GetString("WE_GIVE_PRESENT", FALSE, race2);
							message.GenerateMessage(s,DIPLOMACY,"",0,0);
							empire[m_iDiplomacy].AddMessage(message);
						}
					}
				}
				// Wenn wir sozusagen eine Runde gewartet haben, dann Nachricht machen, das wir einen Vertrag angeboten bekommen haben
				else if (m_MadeOfferArray[i].type == DIP_REQUEST)
				{
					CString s;
					CMessage message;
					if (m_MadeOfferArray[i].offerFromRace == NOT_REACTED)	// Nachricht für mich, das wir ein Angebot bekommen haben
					{
						s = CResourceManager::GetString("WE_GET_REQUEST", FALSE, race, agreement);
						message.GenerateMessage(s,DIPLOMACY,"",0,0,2);
						empire[m_iDiplomacy].AddMessage(message);
					}
					else if (m_MadeOfferArray[i].offerFromRace == FALSE)	// Nachricht für mich, das wir ein Angebot gemacht haben
					{
						s = CResourceManager::GetString("WE_HAVE_REQUEST", FALSE, race2, agreement);
						message.GenerateMessage(s,DIPLOMACY,"",0,0);
						empire[m_iDiplomacy].AddMessage(message);
					}
				}
				// Wenn wir sozusagen eine Runde gewartet haben, dann Nachricht machen, das wir einen Vertrag angeboten bekommen haben
				else if (m_MadeOfferArray[i].type != WAR)
				{
					CString s;
					CMessage message;
					if (m_MadeOfferArray[i].offerFromRace == NOT_REACTED)	// Nachricht für mich, das wir ein Angebot bekommen haben
					{
						s = CResourceManager::GetString("WE_GET_MAJ_OFFER", FALSE, race, agreement);
						message.GenerateMessage(s,DIPLOMACY,"",0,0,2);
						empire[m_iDiplomacy].AddMessage(message);
					}
					else if (m_MadeOfferArray[i].offerFromRace == FALSE)	// Nachricht für mich, das wir ein Angebot gemacht haben
					{
						s = CResourceManager::GetString("WE_MAKE_MAJ_OFFER", FALSE, race2, agreement);
						message.GenerateMessage(s,DIPLOMACY,"",0,0);
						empire[m_iDiplomacy].AddMessage(message);
					}
				}
				// Wenn wir sozusagen eine Runde gewartet haben, dann Nachricht machen, das wir Krieg erklärt haben, dieser gilt sofort
				else if (m_MadeOfferArray[i].type == WAR)
				{
					CString s;
					CMessage message;
					// Nachricht für mich, das uns der Krieg von einer anderen Majorrace erklärt wurde
					if (m_MadeOfferArray[i].offerFromRace == NOT_REACTED)
					{
						s = CResourceManager::GetString("WE_GET_WAR", FALSE, race);
						message.GenerateMessage(s,DIPLOMACY,"",0,0,2);
						empire[m_iDiplomacy].AddMessage(message);
						empire[m_MadeOfferArray[i].majorRace].AddIncomingMajorAnswer(this->GenerateMajorAnswer(WAR,TRUE,m_iDiplomacy));
						// Die Beziehung zu der Kriegserklärenden Majorrace verschlechtert sich enorm
						majors[m_iDiplomacy].SetRelationshipToMajorRace(m_MadeOfferArray[i].majorRace,-100);
						
						// zusätzliche Eventnachricht wegen der Moral an das Imperium
						CString eventText = "";
						// Other Empire Declares War when Neutral #31
						if (majors[m_iDiplomacy].GetDiplomacyStatus(m_MadeOfferArray[i].majorRace) == NO_AGREEMENT)
							eventText = majors[m_iDiplomacy].GetMoralObserver()->AddEvent(31, m_iDiplomacy, race5);
						// Other Empire Declares War with an Affiliation #33
						else if (majors[m_iDiplomacy].GetDiplomacyStatus(m_MadeOfferArray[i].majorRace) == AFFILIATION)
							eventText = majors[m_iDiplomacy].GetMoralObserver()->AddEvent(33, m_iDiplomacy, race5);
						// Other Empire Declares War with Treaty #32
						else
							eventText = majors[m_iDiplomacy].GetMoralObserver()->AddEvent(32, m_iDiplomacy, race5);
						if (!eventText.IsEmpty())
						{
							message.GenerateMessage(eventText, SOMETHING, "", 0, 0);
							empire[m_iDiplomacy].AddMessage(message);
						}
					}
					// Nachricht für mich, dass wir den Krieg einer anderen Majorrace erklärt haben
					if (m_MadeOfferArray[i].offerFromRace == FALSE)
					{
						s = CResourceManager::GetString("WE_DECLARE_WAR", FALSE, race2);
						message.GenerateMessage(s,DIPLOMACY,"",0,0);
						empire[m_iDiplomacy].AddMessage(message);
						// zusätzliche Eventnachricht wegen der Moral an das Imperium
						CString eventText = "";
						// Declare War on an Empire with Defense Pact #28 (nur, wenn wir einen Vertrag kleiner als den der
						// Kooperation haben und dazu auch noch einen Verteidigungspakt)
						if (majors[m_iDiplomacy].GetDiplomacyStatus(m_MadeOfferArray[i].majorRace) < COOPERATION
							&& majors[m_iDiplomacy].GetDefencePact(m_MadeOfferArray[i].majorRace) == TRUE)
							eventText = majors[m_iDiplomacy].GetMoralObserver()->AddEvent(28, m_iDiplomacy, race5);
						// Declare War on an Empire when Neutral #24
						else if (majors[m_iDiplomacy].GetDiplomacyStatus(m_MadeOfferArray[i].majorRace) == NO_AGREEMENT)
							eventText = majors[m_iDiplomacy].GetMoralObserver()->AddEvent(24, m_iDiplomacy, race5);
						// Declare War on an Empire when Non-Aggression #25
						else if (majors[m_iDiplomacy].GetDiplomacyStatus(m_MadeOfferArray[i].majorRace) == NON_AGGRESSION_PACT)
							eventText = majors[m_iDiplomacy].GetMoralObserver()->AddEvent(25, m_iDiplomacy, race5);
						// Declare War on an Empire with Trade Treaty #26
						else if (majors[m_iDiplomacy].GetDiplomacyStatus(m_MadeOfferArray[i].majorRace) == TRADE_AGREEMENT)
							eventText = majors[m_iDiplomacy].GetMoralObserver()->AddEvent(26, m_iDiplomacy, race5);
						// Declare War on an Empire with Friendship Treaty #27
						else if (majors[m_iDiplomacy].GetDiplomacyStatus(m_MadeOfferArray[i].majorRace) == FRIENDSHIP_AGREEMENT)
							eventText = majors[m_iDiplomacy].GetMoralObserver()->AddEvent(27, m_iDiplomacy, race5);
						// Declare War on an Empire with CooperationTreaty #29
						else if (majors[m_iDiplomacy].GetDiplomacyStatus(m_MadeOfferArray[i].majorRace) == COOPERATION)
							eventText = majors[m_iDiplomacy].GetMoralObserver()->AddEvent(29, m_iDiplomacy, race5);
						// Declare War on an Empire with Affiliation #30
						else if (majors[m_iDiplomacy].GetDiplomacyStatus(m_MadeOfferArray[i].majorRace) == AFFILIATION)
							eventText = majors[m_iDiplomacy].GetMoralObserver()->AddEvent(30, m_iDiplomacy, race5);
						if (!eventText.IsEmpty())
						{
							message.GenerateMessage(eventText, SOMETHING, "", 0, 0);
							empire[m_iDiplomacy].AddMessage(message);
						}
						// Die Beziehung zu der Majorrace, welcher wir den Krieg erklärt haben, verschlechtert sich enorm
						majors[m_iDiplomacy].SetRelationshipToMajorRace(m_MadeOfferArray[i].majorRace,-100);
						majors[m_iDiplomacy].SetDiplomaticStatus(m_MadeOfferArray[i].majorRace,WAR);
						majors[m_MadeOfferArray[i].majorRace].SetDiplomaticStatus(m_iDiplomacy,WAR);
						/* Wenn wir jemanden den Krieg erklären und dieser Jemand ein Bündnis oder einen
						   Verteidigungspakt mit einer anderen Majorrace hat, so erklären wir der anderen Majorrace
						   auch den Krieg. Außerdem erklären wir auch jeder Minorrace den Krieg, mit der der Jemand 
						   mindst. ein Bündnis hatte.
						*/
						for (int j = HUMAN; j <= DOMINION; j++) //erstmal die Majorraces
							if ((majors[m_MadeOfferArray[i].majorRace].GetDiplomacyStatus(j) == AFFILIATION 
								|| majors[m_MadeOfferArray[i].majorRace].GetDefencePact(j) == TRUE)
								&& j != m_MadeOfferArray[i].majorRace && j != m_iDiplomacy)
							{
								majors[m_iDiplomacy].SetDiplomaticStatus(j,WAR);	// ich hab Krieg mit dem Bündnispartner
								majors[j].SetDiplomaticStatus(m_iDiplomacy,WAR);	// Bündnispartner hat Krieg mit mir
								CString partner;
								switch (j)
								{
								case HUMAN: {partner = CResourceManager::GetString("TO_RACE1_EMPIRE"); break;}
								case FERENGI: {partner = CResourceManager::GetString("TO_RACE2_EMPIRE"); break;}
								case KLINGON: {partner = CResourceManager::GetString("TO_RACE3_EMPIRE"); break;}
								case ROMULAN: {partner = CResourceManager::GetString("TO_RACE4_EMPIRE"); break;}
								case CARDASSIAN: {partner = CResourceManager::GetString("TO_RACE5_EMPIRE"); break;}
								case DOMINION: {partner = CResourceManager::GetString("TO_RACE6_EMPIRE"); break;}
								}
								// Nachricht für uns
								s = CResourceManager::GetString("WE_DECLARE_WAR", FALSE, partner);
								message.GenerateMessage(s,DIPLOMACY,"",0,0);
								empire[m_iDiplomacy].AddMessage(message);
								empire[m_iDiplomacy].AddIncomingMajorAnswer(this->GenerateMajorAnswer(WAR,TRUE,j));
								// Nachricht und Kriegserklärung an den Bündnispartner
								s = race3+" "+CResourceManager::GetString("WAR_TO_PARTNER", FALSE, CMajorRace::GetRaceName(m_MadeOfferArray[i].majorRace));
								message.GenerateMessage(s,DIPLOMACY,"",0,0);
								empire[j].AddMessage(message);
								s = race3+" "+CResourceManager::GetString("WAR_TO_US_AS_PARTNER");
								message.GenerateMessage(s,DIPLOMACY,"",0,0);
								empire[j].AddMessage(message);
								// Beziehung zu der verbündeten Majorrace verschlechtert sich
								majors[m_iDiplomacy].SetRelationshipToMajorRace(j,-(rand()%25+25)); // ich zu ihnen
								majors[j].SetRelationshipToMajorRace(m_iDiplomacy,-(rand()%50+25)); // Bündnispartner zu mir
							}
						for (int j = 0; j < minors->GetSize(); j++) // bei den Minorraces nach suchen
						{
							if (minors->GetAt(j).GetDiplomacyStatus(m_MadeOfferArray[i].majorRace) >= AFFILIATION)
							{
								minors->ElementAt(j).SetDiplomacyStatus(m_iDiplomacy,WAR);	// Status auf Krieg setzen
								// unsere Beziehung zur Minorrace verschlechtern (zwischen 25 und 49 Punkten)
								minors->ElementAt(j).SetRelationshipToMajorRace(m_iDiplomacy,-(rand()%25+25)); // Beziehung für uns auf 0 setzen
								// Beziehung zu verbündeter Majorrace verbessern (zwischen 0 und 15 Punkten)
								minors->ElementAt(j).SetRelationshipToMajorRace(m_MadeOfferArray[i].majorRace,rand()%16);
								// Nachricht für uns
								s = CResourceManager::GetString("WE_DECLARE_WAR_TO_MIN", FALSE, minors->GetAt(j).GetRaceName());
								message.GenerateMessage(s,DIPLOMACY,"",0,0);
								empire[m_iDiplomacy].AddMessage(message);
								m_AnswerArray.Add(this->GenerateMinorAnswer(m_iDiplomacy,j,WAR,minors->GetAt(j).GetRaceName(),minors->GetAt(j).GetKind(),TRUE));
							}
						}
					}
					///////////////
				}
			}
			// Nach 2 Runden wird das Angebot "ausgeführt" und das Ergebnis als Nachricht an die jeweiligen Imperien versandt
			if (m_MadeOfferArray[i].round == currentRound-2)	// Nach 2 Runden fangen wir an, der Rasse wirklich das Geld zu geben, wodurch sich ihre Beziehung ändern könnte
			{
				// Vertragsannahme oder Ablehnung
				if (m_MadeOfferArray[i].type != WAR && m_MadeOfferArray[i].type != PRESENT
					&& m_MadeOfferArray[i].type != DIP_REQUEST && m_MadeOfferArray[i].type != MEMBERSHIP
					&& m_MadeOfferArray[i].type != WAR_PACT	&& m_MadeOfferArray[i].offerFromRace != FALSE)
				{
					CString s;
					CMessage message;
					CString agreement;
					switch(m_MadeOfferArray[i].type)
					{
						case TRADE_AGREEMENT: {agreement = CResourceManager::GetString("TRADE_AGREEMENT_WITH_ARTICLE"); break;}
						case FRIENDSHIP_AGREEMENT: {agreement = CResourceManager::GetString("FRIENDSHIP_WITH_ARTICLE"); break;}
						case COOPERATION: {agreement = CResourceManager::GetString("COOPERATION_WITH_ARTICLE"); break;}
						case AFFILIATION: {agreement = CResourceManager::GetString("AFFILIATION_WITH_ARTICLE"); break;}
						case NON_AGGRESSION_PACT: {agreement = CResourceManager::GetString("NON_AGGRESSION_WITH_ARTICLE"); break;}
						case DEFENCE_PACT: {agreement = CResourceManager::GetString("DEFENCE_PACT_WITH_ARTICLE"); break;}
					}
					CString agreement2;
					switch(m_MadeOfferArray[i].type)
					{
						case TRADE_AGREEMENT: {agreement2 = CResourceManager::GetString("TRADE_AGREEMENT"); break;}
						case FRIENDSHIP_AGREEMENT: {agreement2 = CResourceManager::GetString("FRIENDSHIP"); break;}
						case COOPERATION: {agreement2 = CResourceManager::GetString("COOPERATION"); break;}
						case AFFILIATION: {agreement2 = CResourceManager::GetString("AFFILIATION"); break;}
						case NON_AGGRESSION_PACT: {agreement2 = CResourceManager::GetString("NON_AGGRESSION"); break;}
						case DEFENCE_PACT: {agreement2 = CResourceManager::GetString("DEFENCE_PACT"); break;}
					}
					if (empire[m_iDiplomacy].GetPlayerOfEmpire() == COMPUTER)
					{
						// Hier die Funktion aufrufen wenn die Majorrace der Computer spielt
						/* zuvor müssen wir aber die möglicherweise verbesserte Beziehung durch Mitgifte berechnen
						   wenn die Rasse das Angebot annimmt geht alles klar, ansonsten müssen wir den Beziehungswert
						   wieder auf den ursprünglichen Wert setzen. Ist ja auch logisch, denn die Rasse bekommt dann
						   ja auch keine Mitgift
						*/
						// alte Beziehung vom Vertragsanbieter holen
						USHORT relation = majors[m_iDiplomacy].GetRelationshipToMajorRace(m_MadeOfferArray[i].majorRace);
						// Beziehungsverbesserung durch Mitgifte des Vertragsangebotes berechnen
						if (m_MadeOfferArray[i].latinum > 0 || (m_MadeOfferArray[i].ressource[TITAN] > 0 || m_MadeOfferArray[i].ressource[DEUTERIUM] > 0 || m_MadeOfferArray[i].ressource[DURANIUM] > 0 ||
							m_MadeOfferArray[i].ressource[CRYSTAL] > 0 || m_MadeOfferArray[i].ressource[IRIDIUM] > 0))
						{
							USHORT LatFromRes = majors[m_iDiplomacy].GiveResourcePresent(m_MadeOfferArray[i].ressource, empire, m_MadeOfferArray[i].majorRace);
							majors[m_iDiplomacy].GiveLatinumPresent(m_MadeOfferArray[i].latinum + LatFromRes, empire, m_MadeOfferArray[i].majorRace);
						}
						// Berechnen ob Annahme oder Ablehnung
						m_MadeOfferArray[i].offerFromRace = majors[m_iDiplomacy].CalculateDiplomaticOffer(m_MadeOfferArray[i].majorRace,m_MadeOfferArray[i].type,NOBODY,shipPowers);
						// bei Ablehnung alte Beziehung wiederherstellen
						if (m_MadeOfferArray[i].offerFromRace != TRUE)
							majors[m_iDiplomacy].SetRelationshipToMajorRace(m_MadeOfferArray[i].majorRace, relation-majors[m_iDiplomacy].GetRelationshipToMajorRace(m_MadeOfferArray[i].majorRace));
					}
					if (m_MadeOfferArray[i].offerFromRace == TRUE)	// Wir haben das Angebot angenommen
					{
						s = CResourceManager::GetString("WE_ACCEPT_MAJ_OFFER", FALSE, agreement, race2);
						majors[m_iDiplomacy].SetDiplomaticStatus(m_MadeOfferArray[i].majorRace,m_MadeOfferArray[i].type);
						message.GenerateMessage(s,DIPLOMACY,"",0,0);
						empire[m_iDiplomacy].AddMessage(message);
						s = CResourceManager::GetString("MAJ_ACCEPT_OFFER", FALSE, race3, agreement2);
						majors[m_MadeOfferArray[i].majorRace].SetDiplomaticStatus(m_iDiplomacy,m_MadeOfferArray[i].type);
						message.GenerateMessage(s,DIPLOMACY,"",0,0,2);
						empire[m_MadeOfferArray[i].majorRace].AddMessage(message);
						empire[m_MadeOfferArray[i].majorRace].AddIncomingMajorAnswer(this->GenerateMajorAnswer(m_MadeOfferArray[i].type,TRUE,m_iDiplomacy));
						// Das möglicherweise dazugegebene Latinum und die Ressourcen auch gutschreiben
						// Bis jetzt wird nur Latinum gutgeschrieben, Ressourcen werden genau wie bei einem
						// Geschenk noch nicht übergeben, die sollen ja später in ein "globales Lager"
						empire[m_iDiplomacy].SetLatinum(m_MadeOfferArray[i].latinum);
						// GLOABLES LAGER
						for (int k = TITAN; k <= IRIDIUM; k++)
							if (m_MadeOfferArray[i].ressource[k] > 0)
								globalStorages[m_iDiplomacy].AddRessource(m_MadeOfferArray[i].ressource[k],k,CPoint(-1,-1));
						
						// Dauer des Angebots an die CMajorRace Klasse beider Partein übergeben und dort festhalten
						if (m_MadeOfferArray[i].type != DEFENCE_PACT)
						{
							majors[m_iDiplomacy].SetDurationOfAgreement(m_MadeOfferArray[i].majorRace,m_MadeOfferArray[i].corruptedMajor);
							majors[m_MadeOfferArray[i].majorRace].SetDurationOfAgreement(m_iDiplomacy,m_MadeOfferArray[i].corruptedMajor);
						}
						else // seperate Funktion für Defencepact, weil wir den zusätzlich zu den anderen Verträgen haben können
						{
							majors[m_iDiplomacy].SetDurationOfDefencePact(m_MadeOfferArray[i].majorRace,m_MadeOfferArray[i].corruptedMajor);
							majors[m_MadeOfferArray[i].majorRace].SetDurationOfDefencePact(m_iDiplomacy,m_MadeOfferArray[i].corruptedMajor);
						}

						// zusätzliche Eventnachricht wegen der Moral an das Imperium
						CString eventText1 = "";
						CString eventText2 = "";
						switch (m_MadeOfferArray[i].type)
						{
						// Sign Trade Treaty #34
						case TRADE_AGREEMENT:
							eventText1 = majors[m_iDiplomacy].GetMoralObserver()->AddEvent(34, m_iDiplomacy, race2);
							eventText2 = majors[m_MadeOfferArray[i].majorRace].GetMoralObserver()->AddEvent(34, m_MadeOfferArray[i].majorRace, race4);
							break;
						// Sign Friendship/Cooperation Treaty #35
						case FRIENDSHIP_AGREEMENT:
							eventText1 = majors[m_iDiplomacy].GetMoralObserver()->AddEvent(35, m_iDiplomacy, race2);
							eventText2 = majors[m_MadeOfferArray[i].majorRace].GetMoralObserver()->AddEvent(35, m_MadeOfferArray[i].majorRace, race4);
							break;
						// Sign Friendship/Cooperation Treaty #35
						case COOPERATION:
							eventText1 = majors[m_iDiplomacy].GetMoralObserver()->AddEvent(35, m_iDiplomacy, race2);
							eventText2 = majors[m_MadeOfferArray[i].majorRace].GetMoralObserver()->AddEvent(35, m_MadeOfferArray[i].majorRace, race4);
							break;
						// Sign an Affiliation Treaty #36
						case AFFILIATION:
							eventText1 = majors[m_iDiplomacy].GetMoralObserver()->AddEvent(36, m_iDiplomacy, race2);
							eventText2 = majors[m_MadeOfferArray[i].majorRace].GetMoralObserver()->AddEvent(36, m_MadeOfferArray[i].majorRace, race4);
							break;
						}
						if (!eventText1.IsEmpty())
						{
							// Bei Annahme an beide Imperien dieses Event versenden
							message.GenerateMessage(eventText1, SOMETHING, "", 0, 0);
							empire[m_iDiplomacy].AddMessage(message);
							message.GenerateMessage(eventText2, SOMETHING, "", 0, 0);
							empire[m_MadeOfferArray[i].majorRace].AddMessage(message);
						}

						// Beziehungsverbesserung auf beiden Seiten
						majors[m_iDiplomacy].SetRelationshipToMajorRace(m_MadeOfferArray[i].majorRace,abs(m_MadeOfferArray[i].type)*2);
						majors[m_MadeOfferArray[i].majorRace].SetRelationshipToMajorRace(m_iDiplomacy,abs(m_MadeOfferArray[i].type)*2);
						// Wenn die Beziehung zu schlecht ist, z.B. nachdem ein Krieg war und ein Nichtangriffspakt nachher
						// angenommen wurde, so die Beziehung auf ein Minimum setzen.
						if (majors[m_iDiplomacy].GetRelationshipToMajorRace(m_MadeOfferArray[i].majorRace) < ((m_MadeOfferArray[i].type+3)*10))
						{
							int add = ((m_MadeOfferArray[i].type+3)*10) - majors[m_iDiplomacy].GetRelationshipToMajorRace(m_MadeOfferArray[i].majorRace);
							majors[m_iDiplomacy].SetRelationshipToMajorRace(m_MadeOfferArray[i].majorRace, add);
						}
						if (majors[m_MadeOfferArray[i].majorRace].GetRelationshipToMajorRace(m_iDiplomacy) < ((m_MadeOfferArray[i].type+3)*10))
						{
							int add = ((m_MadeOfferArray[i].type+3)*10) - majors[m_MadeOfferArray[i].majorRace].GetRelationshipToMajorRace(m_iDiplomacy);
							majors[m_MadeOfferArray[i].majorRace].SetRelationshipToMajorRace(m_iDiplomacy, add);
						}
					}
					else if (m_MadeOfferArray[i].offerFromRace == DECLINED)	// Wir haben das Angebot abgelehnt
					{
						s = CResourceManager::GetString("WE_DECLINE_MAJ_OFFER", FALSE, agreement2,CMajorRace::GetRaceName(m_MadeOfferArray[i].majorRace));
						message.GenerateMessage(s,DIPLOMACY,"",0,0);
						empire[m_iDiplomacy].AddMessage(message);
						s = CResourceManager::GetString("MAJ_DECLINE_OFFER", FALSE, race3, agreement2);
						message.GenerateMessage(s,DIPLOMACY,"",0,0,2);
						empire[m_MadeOfferArray[i].majorRace].AddMessage(message);				
						empire[m_MadeOfferArray[i].majorRace].AddIncomingMajorAnswer(this->GenerateMajorAnswer(m_MadeOfferArray[i].type,FALSE,m_iDiplomacy));
						// Wenn wir das Angebot abgelehnt haben, dann bekommt die Majorrace, die es mir gemacht hat
						// ihre Ressourcen und ihr Latinum wieder zurück, sofern sie es mir als Anreiz mit zum Vertrags-
						// angebot gemacht haben
						empire[m_MadeOfferArray[i].majorRace].SetLatinum(m_MadeOfferArray[i].latinum);
						systems[m_MadeOfferArray[i].KO.x][m_MadeOfferArray[i].KO.y].SetTitanStore(m_MadeOfferArray[i].ressource[0]);
						systems[m_MadeOfferArray[i].KO.x][m_MadeOfferArray[i].KO.y].SetDeuteriumStore(m_MadeOfferArray[i].ressource[1]);
						systems[m_MadeOfferArray[i].KO.x][m_MadeOfferArray[i].KO.y].SetDuraniumStore(m_MadeOfferArray[i].ressource[2]);
						systems[m_MadeOfferArray[i].KO.x][m_MadeOfferArray[i].KO.y].SetCrystalStore(m_MadeOfferArray[i].ressource[3]);
						systems[m_MadeOfferArray[i].KO.x][m_MadeOfferArray[i].KO.y].SetIridiumStore(m_MadeOfferArray[i].ressource[4]);
						// Beziehungsverschlechterung auf beiden Seiten, wenn unser Angebot abgelehnt wird, so geht die
						// Beziehung von uns zum Ablehner stärker runter, als die vom Ablehner zu uns
						majors[m_iDiplomacy].SetRelationshipToMajorRace(m_MadeOfferArray[i].majorRace,-(USHORT)(rand()%(abs(m_MadeOfferArray[i].type)))/2);
						majors[m_MadeOfferArray[i].majorRace].SetRelationshipToMajorRace(m_iDiplomacy,-(USHORT)(rand()%(abs(m_MadeOfferArray[i].type))));
					}
					else // wir haben nicht auf deren Angebot reagiert
					{
						s = CResourceManager::GetString("NOT_REACTED", FALSE, race3);
						message.GenerateMessage(s,DIPLOMACY,"",0,0);
						empire[m_MadeOfferArray[i].majorRace].AddMessage(message);
						// Wenn wir nicht auf das Angebot reagiert haben, dann bekommt die Majorrace, die es mir gemacht hat
						// ihre Ressourcen und ihr Latinum wieder zurück, sofern sie es mir als Anreiz mit zum Vertrags-
						// angebot gemacht haben
						empire[m_MadeOfferArray[i].majorRace].SetLatinum(m_MadeOfferArray[i].latinum);
						systems[m_MadeOfferArray[i].KO.x][m_MadeOfferArray[i].KO.y].SetTitanStore(m_MadeOfferArray[i].ressource[0]);
						systems[m_MadeOfferArray[i].KO.x][m_MadeOfferArray[i].KO.y].SetDeuteriumStore(m_MadeOfferArray[i].ressource[1]);
						systems[m_MadeOfferArray[i].KO.x][m_MadeOfferArray[i].KO.y].SetDuraniumStore(m_MadeOfferArray[i].ressource[2]);
						systems[m_MadeOfferArray[i].KO.x][m_MadeOfferArray[i].KO.y].SetCrystalStore(m_MadeOfferArray[i].ressource[3]);
						systems[m_MadeOfferArray[i].KO.x][m_MadeOfferArray[i].KO.y].SetIridiumStore(m_MadeOfferArray[i].ressource[4]);
					}
				}
				// Forderungsannahme oder Ablehnung
				else if (m_MadeOfferArray[i].type == DIP_REQUEST && m_MadeOfferArray[i].offerFromRace != FALSE)
				{
					CString s;
					CMessage message;
					if (empire[m_iDiplomacy].GetPlayerOfEmpire() == COMPUTER)
					{
						// Hier die Funktion aufrufen wenn die Majorrace der Computer spielt
						m_MadeOfferArray[i].offerFromRace = majors[m_iDiplomacy].CalculateDiplomaticRequest(m_MadeOfferArray[i].majorRace,m_MadeOfferArray[i].latinum,m_MadeOfferArray[i].ressource,empire,averageRessourceStorages,systems,shipPowers);
					}
					if (m_MadeOfferArray[i].offerFromRace == TRUE) // Wir haben die Forderung angenommen
					{
						s = CResourceManager::GetString("WE_ACCEPT_REQUEST", FALSE, race2);
						message.GenerateMessage(s,DIPLOMACY,"",0,0);
						empire[m_iDiplomacy].AddMessage(message);
						s = CResourceManager::GetString("OUR_REQUEST_ACCEPT", FALSE, race3);
						message.GenerateMessage(s,DIPLOMACY,"",0,0,2);
						empire[m_MadeOfferArray[i].majorRace].AddMessage(message);
						empire[m_MadeOfferArray[i].majorRace].AddIncomingMajorAnswer(this->GenerateMajorAnswer(m_MadeOfferArray[i].type,TRUE,m_iDiplomacy));
						// Die Forderung auch gutschreiben, wenn sie angenommen wurde
						// Bis jetzt wird nur Latinum gutgeschrieben, Ressourcen werden genau wie bei einem
						// Geschenk noch nicht übergeben, die sollen ja später in ein "globales Lager"
						empire[m_MadeOfferArray[i].majorRace].SetLatinum(m_MadeOfferArray[i].latinum);
						// GLOBALES LAGER
						for (int k = TITAN; k <= IRIDIUM; k++)
							if (m_MadeOfferArray[i].ressource[k] > 0)
								globalStorages[m_MadeOfferArray[i].majorRace].AddRessource(m_MadeOfferArray[i].ressource[k],k,CPoint(-1,-1));
						// Beziehungsverbesserung bei Annahme auf der fordernden Seite (0 bis 10 Punkte)
						majors[m_MadeOfferArray[i].majorRace].SetRelationshipToMajorRace(m_iDiplomacy, rand()%DIP_REQUEST);
					}
					else if (m_MadeOfferArray[i].offerFromRace == DECLINED) // Wir haben die Forderung abgelehnt
					{
						s = CResourceManager::GetString("WE_DECLINE_REQUEST", FALSE, race2);
						message.GenerateMessage(s,DIPLOMACY,"",0,0);
						empire[m_iDiplomacy].AddMessage(message);
						s = CResourceManager::GetString("OUR_REQUEST_DECLINE", FALSE, race3);
						message.GenerateMessage(s,DIPLOMACY,"",0,0,2);
						empire[m_MadeOfferArray[i].majorRace].AddMessage(message);
						empire[m_MadeOfferArray[i].majorRace].AddIncomingMajorAnswer(this->GenerateMajorAnswer(m_MadeOfferArray[i].type,FALSE,m_iDiplomacy));
						// Beziehungsverschlechterung bei Ablehnung, hier wird ein bissl vom Computer gecheated.
						// Wenn ein Computer von einem anderen Computer die Forderung ablehnt, dann wird die Beziehung
						// nicht ganz so stark verschlechtert als wenn ein Mensch die Forderung des Computers ablehnt
						if (empire[m_iDiplomacy].GetPlayerOfEmpire() == COMPUTER)
							majors[m_MadeOfferArray[i].majorRace].SetRelationshipToMajorRace(m_iDiplomacy, (short)((-rand()%DIP_REQUEST)/5));
						else
							majors[m_MadeOfferArray[i].majorRace].SetRelationshipToMajorRace(m_iDiplomacy, -rand()%DIP_REQUEST);
					}
					else // wir haben nicht auf deren Angebot reagiert
					{
						s = CResourceManager::GetString("NOT_REACTED_REQUEST", FALSE, race3);
						message.GenerateMessage(s,DIPLOMACY,"",0,0);
						empire[m_MadeOfferArray[i].majorRace].AddMessage(message);
						// Beziehungsverschlechterung bei Nichtbeachtung, hier wird ein bissl vom Computer gecheated.
						// Wenn ein Computer von einem anderen Computer die Forderung nicht beachtet, dann wird die Beziehung
						// nicht ganz so stark verschlechtert als wenn ein Mensch die Forderung des Computers nicht beachtet.
						if (empire[m_iDiplomacy].GetPlayerOfEmpire() == COMPUTER)
							majors[m_MadeOfferArray[i].majorRace].SetRelationshipToMajorRace(m_iDiplomacy, (short)((-rand()%DIP_REQUEST)/10));
						else
							majors[m_MadeOfferArray[i].majorRace].SetRelationshipToMajorRace(m_iDiplomacy, (short)((-rand()%DIP_REQUEST)/2));
					}
				}
				//Kriegspaktannahme oder Ablehnung (das ist hier ein bissl kompliziert, weil wir alle mgl. Bündnisse beachten müssen
				else if (m_MadeOfferArray[i].type == WAR_PACT && m_MadeOfferArray[i].offerFromRace != FALSE)
				{
					CString s;
					CMessage message;
					if (empire[m_iDiplomacy].GetPlayerOfEmpire() == COMPUTER)
					{
					// Hier die Funktion aufrufen wenn die Majorrace der Computer spielt	
						/* zuvor müssen wir aber die möglicherweise verbesserte Beziehung durch Mitgifte berechnen
						   wenn die Rasse das Angebot annimmt geht alles klar, ansonsten müssen wir den Beziehungswert
						   wieder auf den ursprünglichen Wert setzen. Ist ja auch logisch, denn die Rasse bekommt dann
						   ja auch keine Mitgift
						*/
						// alte Beziehung vom Vertragsanbieter holen
						USHORT relation = majors[m_iDiplomacy].GetRelationshipToMajorRace(m_MadeOfferArray[i].majorRace);
						// Beziehungsverbesserung durch Mitgifte des Vertragsangebotes berechnen
						if (m_MadeOfferArray[i].latinum > 0 || (m_MadeOfferArray[i].ressource[TITAN] > 0 || m_MadeOfferArray[i].ressource[DEUTERIUM] > 0 || m_MadeOfferArray[i].ressource[DURANIUM] > 0 ||
							m_MadeOfferArray[i].ressource[CRYSTAL] > 0 || m_MadeOfferArray[i].ressource[IRIDIUM] > 0) )
						{
							USHORT LatFromRes = majors[m_iDiplomacy].GiveResourcePresent(m_MadeOfferArray[i].ressource, empire, m_MadeOfferArray[i].majorRace);
							majors[m_iDiplomacy].GiveLatinumPresent(m_MadeOfferArray[i].latinum + LatFromRes, empire, m_MadeOfferArray[i].majorRace);
						}
						// Berechnen ob Annahme oder Ablehnung
						m_MadeOfferArray[i].offerFromRace = majors[m_iDiplomacy].CalculateDiplomaticOffer(m_MadeOfferArray[i].majorRace,WAR_PACT,m_MadeOfferArray[i].corruptedMajor,shipPowers);
						// bei Ablehnung alte Beziehung wiederherstellen
						if (m_MadeOfferArray[i].offerFromRace != TRUE)
							majors[m_iDiplomacy].SetRelationshipToMajorRace(m_MadeOfferArray[i].majorRace, relation-majors[m_iDiplomacy].GetRelationshipToMajorRace(m_MadeOfferArray[i].majorRace));
					}
					if (m_MadeOfferArray[i].offerFromRace == TRUE)	// Wir haben den Kriegspakt angenommen
					{
						CString race5;
						switch(m_MadeOfferArray[i].corruptedMajor)
						{
						case HUMAN: {race5 = CResourceManager::GetString("TO_RACE1_EMPIRE"); break;}
						case FERENGI: {race5 = CResourceManager::GetString("TO_RACE2_EMPIRE"); break;}
						case KLINGON: {race5 = CResourceManager::GetString("TO_RACE3_EMPIRE"); break;}
						case ROMULAN: {race5 = CResourceManager::GetString("TO_RACE4_EMPIRE"); break;}
						case CARDASSIAN: {race5 = CResourceManager::GetString("TO_RACE5_EMPIRE"); break;}
						case DOMINION: {race5 = CResourceManager::GetString("TO_RACE6_EMPIRE"); break;}
						}
						s = CResourceManager::GetString("WE_ACCEPT_WARPACT", FALSE, CMajorRace::GetRaceName(m_MadeOfferArray[i].corruptedMajor),race2);
						message.GenerateMessage(s,DIPLOMACY,"",0,0);
						empire[m_iDiplomacy].AddMessage(message);

						BOOLEAN weDeclareWar = false;
						BOOLEAN partnerDeclareWar = false;

						if (majors[m_iDiplomacy].GetDiplomacyStatus(m_MadeOfferArray[i].corruptedMajor) != WAR)
						{
							weDeclareWar = true;
							s = CResourceManager::GetString("WE_DECLARE_WAR", FALSE, race5);
							message.GenerateMessage(s,DIPLOMACY,"",0,0);
							empire[m_iDiplomacy].AddMessage(message);
							// Nachrichten an den Gegner, das die beiden Kriegspaktpartner ihm den Krieg erklärt haben
							s = CResourceManager::GetString("WE_GET_WAR", FALSE, race3);
							message.GenerateMessage(s, DIPLOMACY,"",0,0,2);
							empire[m_MadeOfferArray[i].corruptedMajor].AddMessage(message);
							empire[m_MadeOfferArray[i].corruptedMajor].AddIncomingMajorAnswer(this->GenerateMajorAnswer(WAR,TRUE,m_iDiplomacy));
							// Nachricht vom Gegner
							empire[m_iDiplomacy].AddIncomingMajorAnswer(this->GenerateMajorAnswer(WAR,TRUE,m_MadeOfferArray[i].corruptedMajor));
							// Wir setzen unseren Status auf Krieg und den unseres Gegner zu uns auch auf Krieg
							majors[m_iDiplomacy].SetDiplomaticStatus(m_MadeOfferArray[i].corruptedMajor,WAR);
							majors[m_MadeOfferArray[i].corruptedMajor].SetDiplomaticStatus(m_iDiplomacy,WAR);
							// Beziehungen auf 0 setzen (gegen den wir Krieg führen wollen)
							majors[m_iDiplomacy].SetRelationshipToMajorRace(m_MadeOfferArray[i].corruptedMajor,-100);
							majors[m_MadeOfferArray[i].corruptedMajor].SetRelationshipToMajorRace(m_iDiplomacy,-100);							
						}
						s = CResourceManager::GetString("OUR_WARPACT_ACCEPT", FALSE, race3,CMajorRace::GetRaceName(m_MadeOfferArray[i].corruptedMajor));
						message.GenerateMessage(s,DIPLOMACY,"",0,0,2);
						empire[m_MadeOfferArray[i].majorRace].AddMessage(message);
						// Beziehungsverbesserung auf beiden Seiten der Kriegspaktpartner
						majors[m_iDiplomacy].SetRelationshipToMajorRace(m_MadeOfferArray[i].majorRace,abs(m_MadeOfferArray[i].type));
						majors[m_MadeOfferArray[i].majorRace].SetRelationshipToMajorRace(m_iDiplomacy,abs(m_MadeOfferArray[i].type));
						empire[m_MadeOfferArray[i].majorRace].AddIncomingMajorAnswer(this->GenerateMajorAnswer(m_MadeOfferArray[i].type,TRUE,m_iDiplomacy));
						
						if (majors[m_MadeOfferArray[i].majorRace].GetDiplomacyStatus(m_MadeOfferArray[i].corruptedMajor) != WAR)
						{
							partnerDeclareWar = true;
							s = CResourceManager::GetString("WE_DECLARE_WAR", FALSE, race5);
							message.GenerateMessage(s,DIPLOMACY,"",0,0);
							empire[m_MadeOfferArray[i].majorRace].AddMessage(message);
							s = CResourceManager::GetString("WE_GET_WAR", FALSE, race);
							message.GenerateMessage(s,DIPLOMACY,"",0,0,2);
							empire[m_MadeOfferArray[i].corruptedMajor].AddMessage(message);
							empire[m_MadeOfferArray[i].corruptedMajor].AddIncomingMajorAnswer(this->GenerateMajorAnswer(WAR,TRUE,m_MadeOfferArray[i].majorRace));
							// Nachricht vom Gegner
							empire[m_iDiplomacy].AddIncomingMajorAnswer(this->GenerateMajorAnswer(WAR,TRUE,m_MadeOfferArray[i].corruptedMajor));
							empire[m_MadeOfferArray[i].majorRace].AddIncomingMajorAnswer(this->GenerateMajorAnswer(WAR,TRUE,m_MadeOfferArray[i].corruptedMajor));
							// Der Status unseres Kriegspaktpartners mit dem Gegner wird auf Krieg gesetzt und andersrum auch
							majors[m_MadeOfferArray[i].majorRace].SetDiplomaticStatus(m_MadeOfferArray[i].corruptedMajor,WAR);
							majors[m_MadeOfferArray[i].corruptedMajor].SetDiplomaticStatus(m_MadeOfferArray[i].majorRace,WAR);
							// Beziehungen auf 0 setzen (gegen den wir Krieg führen wollen)
							majors[m_MadeOfferArray[i].majorRace].SetRelationshipToMajorRace(m_MadeOfferArray[i].corruptedMajor,-100);
							majors[m_MadeOfferArray[i].corruptedMajor].SetRelationshipToMajorRace(m_MadeOfferArray[i].majorRace,-100);
						}						
						// Das möglicherweise dazugegebene Latinum und die Ressourcen auch gutschreiben
						// Bis jetzt wird nur Latinum gutgeschrieben, Ressourcen werden genau wie bei einem
						// Geschenk noch nicht übergeben, die sollen ja später in ein "globales Lager"
						empire[m_iDiplomacy].SetLatinum(m_MadeOfferArray[i].latinum);
						// GLOBALES LAGER
						for (int k = TITAN; k <= IRIDIUM; k++)
							if (m_MadeOfferArray[i].ressource[k] > 0)
								globalStorages[m_iDiplomacy].AddRessource(m_MadeOfferArray[i].ressource[k],k,CPoint(-1,-1));
						
				/*		Das hier überarbeiten!!! Eventnachrichten wegen Kriegserklärung
				
						// zusätzliche Eventnachricht wegen der Moral an das Imperium, dem der Krieg erklärt wurde
						CString eventText = "";
						// Other Empire Declares War when Neutral #31
						if (majors[m_MadeOfferArray[i].corruptedMajor].GetDiplomacyStatus(m_iDiplomacy) == NO_AGREEMENT)
							eventText = majors[m_MadeOfferArray[i].corruptedMajor].GetMoralObserver()->AddEvent(31, m_MadeOfferArray[i].corruptedMajor, race);
						// Other Empire Declares War with an Affiliation #33
						else if (majors[m_MadeOfferArray[i].corruptedMajor].GetDiplomacyStatus(m_iDiplomacy) == AFFILIATION)
							eventText = majors[m_iDiplomacy].GetMoralObserver()->AddEvent(33, m_MadeOfferArray[i].corruptedMajor, race);
						// Other Empire Declares War with Treaty #32
						else
							eventText = majors[m_MadeOfferArray[i].corruptedMajor].GetMoralObserver()->AddEvent(32, m_MadeOfferArray[i].corruptedMajor, race);
						if (!eventText.IsEmpty())
						{
							message.GenerateMessage(eventText, SOMETHING, "", 0, 0);
							empire[m_MadeOfferArray[i].corruptedMajor].AddMessage(message);
						}			*/						
						
						// Hier noch alle möglichen Bündnisse des Kriegsgegners überprüfen
						/* Wenn wir jemanden den Krieg erklären und dieser Jemand  ein Bündnis oder einen
						   Verteidigungspakt mit einer anderen Majorrace hat, so erklären wir der anderen Majorrace
						   auch den Krieg. Außerdem erklären wir auch jeder Minorrace den Krieg, mit der der Jemand 
						   mindst. ein Bündnis hatte.
						*/
						for (int j = HUMAN; j <= DOMINION; j++) //erstmal die Majorraces
							if ((majors[m_MadeOfferArray[i].corruptedMajor].GetDiplomacyStatus(j) == AFFILIATION 
								|| majors[m_MadeOfferArray[i].corruptedMajor].GetDefencePact(j) == TRUE)
								&& j != m_MadeOfferArray[i].corruptedMajor && j != m_iDiplomacy && j != m_MadeOfferArray[i].majorRace)
							{
								CString partner;
								switch (j)
								{
								case HUMAN: {partner = CResourceManager::GetString("TO_RACE1_EMPIRE"); break;}
								case FERENGI: {partner = CResourceManager::GetString("TO_RACE2_EMPIRE"); break;}
								case KLINGON: {partner = CResourceManager::GetString("TO_RACE3_EMPIRE"); break;}
								case ROMULAN: {partner = CResourceManager::GetString("TO_RACE4_EMPIRE"); break;}
								case CARDASSIAN: {partner = CResourceManager::GetString("TO_RACE5_EMPIRE"); break;}
								case DOMINION: {partner = CResourceManager::GetString("TO_RACE6_EMPIRE"); break;}
								}

								if (weDeclareWar)
								{
									majors[m_iDiplomacy].SetDiplomaticStatus(j,WAR);	// ich hab Krieg mit dem Bündnispartner
									majors[j].SetDiplomaticStatus(m_iDiplomacy,WAR);	// Bündnispartner hat Krieg mit mir
									// Nachricht für uns
									s = CResourceManager::GetString("WE_DECLARE_WAR", FALSE, partner);
									message.GenerateMessage(s,DIPLOMACY,"",0,0);
									empire[m_iDiplomacy].AddMessage(message);
									empire[m_iDiplomacy].AddIncomingMajorAnswer(this->GenerateMajorAnswer(WAR,TRUE,j));
									// Nachricht und Kriegserklärung an den Bündnispartner von mir
									s = race+" "+CResourceManager::GetString("WAR_TO_PARTNER", FALSE, CMajorRace::GetRaceName(m_MadeOfferArray[i].corruptedMajor));
									message.GenerateMessage(s,DIPLOMACY,"",0,0);
									empire[j].AddMessage(message);
									s = race+" "+CResourceManager::GetString("WAR_TO_US_AS_PARTNER");
									message.GenerateMessage(s,DIPLOMACY,"",0,0,2);
									empire[j].AddMessage(message);
									// Beziehung zu der verbündeten Majorrace verschlechtert sich
									majors[m_iDiplomacy].SetRelationshipToMajorRace(j,-(rand()%25+25)); // ich zu ihnen
									majors[j].SetRelationshipToMajorRace(m_iDiplomacy,-(rand()%50+25)); // Bündnispartner zu mir									
								}
								if (partnerDeclareWar)
								{
									majors[m_MadeOfferArray[i].majorRace].SetDiplomaticStatus(j,WAR);	// Kriegspaktpartner hat Krieg mit dem Bündnispartner
									majors[j].SetDiplomaticStatus(m_MadeOfferArray[i].majorRace,WAR);	// Bündnispartner hat Krieg mit meinem Kriegspaktpartner
									// Nachricht für unseren Kriegspaktpartner
									s = CResourceManager::GetString("WE_DECLARE_WAR", FALSE, partner);
									message.GenerateMessage(s,DIPLOMACY,"",0,0);
									empire[m_MadeOfferArray[i].majorRace].AddMessage(message);
									empire[m_MadeOfferArray[i].majorRace].AddIncomingMajorAnswer(this->GenerateMajorAnswer(WAR,TRUE,j));
									// Nachricht und Kriegserklärung an den Bündnispartner von meinem Kriegspaktpartner
									s = race3+" "+CResourceManager::GetString("WAR_TO_PARTNER", FALSE, CMajorRace::GetRaceName(m_MadeOfferArray[i].corruptedMajor));
									message.GenerateMessage(s,DIPLOMACY,"",0,0);
									empire[j].AddMessage(message);
									s = race3+" "+CResourceManager::GetString("WAR_TO_US_AS_PARTNER");
									message.GenerateMessage(s,DIPLOMACY,"",0,0,2);
									empire[j].AddMessage(message);
									// Beziehung zu der verbündeten Majorrace verschlechtert sich									
									majors[m_MadeOfferArray[i].majorRace].SetRelationshipToMajorRace(j,-(rand()%25+25)); // mein Kriegspaktpartner zu ihnen
									majors[j].SetRelationshipToMajorRace(m_MadeOfferArray[i].majorRace,-(rand()%50+25)); // Bündnispartner zu meinem Kriegspaktpartner
								}
							}
						for (int j = 0; j < minors->GetSize(); j++) // bei den Minorraces nach suchen
						{
							if (minors->GetAt(j).GetDiplomacyStatus(m_MadeOfferArray[i].corruptedMajor) >= AFFILIATION)
							{
								if (weDeclareWar)
								{
									minors->ElementAt(j).SetDiplomacyStatus(m_iDiplomacy,WAR);	// Status auf Krieg setzen
									minors->ElementAt(j).SetRelationshipToMajorRace(m_iDiplomacy,-(rand()%25+25)); // Beziehung für uns auf 0 setzen
									// Beziehung zu verbündeter Majorrace verbessern (zwischen 0 und 25 Punkten)
									minors->ElementAt(j).SetRelationshipToMajorRace(m_MadeOfferArray[i].corruptedMajor, rand()%26);
									// Nachricht für uns
									s = CResourceManager::GetString("WE_DECLARE_WAR_TO_MIN", FALSE, minors->GetAt(j).GetRaceName());
									message.GenerateMessage(s,DIPLOMACY,"",0,0);
									empire[m_iDiplomacy].AddMessage(message);
									m_AnswerArray.Add(this->GenerateMinorAnswer(m_iDiplomacy,j,WAR,minors->GetAt(j).GetRaceName(),minors->GetAt(j).GetKind(),TRUE));
								}
								if (partnerDeclareWar)
								{
									minors->ElementAt(j).SetDiplomacyStatus(m_MadeOfferArray[i].majorRace,WAR);	// Status meines Kriegspaktpartners auf Krieg setzen
									minors->ElementAt(j).SetRelationshipToMajorRace(m_MadeOfferArray[i].majorRace,-(rand()%25+25)); // Beziehung meines Kriegspaktpartners auf 0 setzen
									// Beziehung zu verbündeter Majorrace verbessern (zwischen 0 und 25 Punkten)
									minors->ElementAt(j).SetRelationshipToMajorRace(m_MadeOfferArray[i].corruptedMajor, rand()%26);
									// Nachricht für meinen Kriegspaktpartner
									s = CResourceManager::GetString("WE_DECLARE_WAR_TO_MIN", FALSE, minors->GetAt(j).GetRaceName());
									message.GenerateMessage(s,DIPLOMACY,"",0,0);
									empire[m_MadeOfferArray[i].majorRace].AddMessage(message);
									m_AnswerArray.Add(this->GenerateMinorAnswer(m_MadeOfferArray[i].majorRace,j,WAR,minors->GetAt(j).GetRaceName(),minors->GetAt(j).GetKind(),TRUE));
								}
							}
						}
					}
					else if (m_MadeOfferArray[i].offerFromRace == DECLINED) // Wir haben den Kriegspakt abgelehnt
					{
						s = CResourceManager::GetString("WE_DECLINE_WARPACT", FALSE, race2,CMajorRace::GetRaceName(m_MadeOfferArray[i].corruptedMajor));
						message.GenerateMessage(s,DIPLOMACY,"",0,0);
						empire[m_iDiplomacy].AddMessage(message);
						s = CResourceManager::GetString("OUR_WARPACT_DECLINE", FALSE, race3,CMajorRace::GetRaceName(m_MadeOfferArray[i].corruptedMajor));
						message.GenerateMessage(s,DIPLOMACY,"",0,0,2);
						empire[m_MadeOfferArray[i].majorRace].AddMessage(message);
						empire[m_MadeOfferArray[i].majorRace].AddIncomingMajorAnswer(this->GenerateMajorAnswer(m_MadeOfferArray[i].type,FALSE,m_iDiplomacy));
						// Beziehungsverschlechterung auf beiden Seiten, wenn unser Angebot abgelehnt wird, so geht die
						// Beziehung von uns zum Ablehner stärker runter, als die vom Ablehner zu uns
						majors[m_iDiplomacy].SetRelationshipToMajorRace(m_MadeOfferArray[i].majorRace,-(USHORT)(rand()%(abs(m_MadeOfferArray[i].type)))/2);
						majors[m_MadeOfferArray[i].majorRace].SetRelationshipToMajorRace(m_iDiplomacy,-(USHORT)(rand()%(abs(m_MadeOfferArray[i].type))));
					}
					else // wir haben nicht auf deren Angebot reagiert
					{
						s = CResourceManager::GetString("NOT_REACTED_WARPACT", FALSE, race3,CMajorRace::GetRaceName(m_MadeOfferArray[i].corruptedMajor));
						message.GenerateMessage(s,DIPLOMACY,"",0,0);
						empire[m_MadeOfferArray[i].majorRace].AddMessage(message);				
					}
				}
				// Eintrag entfernen
				m_MadeOfferArray.RemoveAt(i);
				// i runterzählen, weil wir ja das Feld verkleinern und sich damit auch GetSize() um eins verkleinert
				i--;
			}
		}
	}
}

/// Diese Funktion löscht alle Einträge aus den Variablen in denen auf die Minorrace <code>minor</code> verwiesen wurde.
void CDiplomacy::ClearArraysFromMinor(USHORT minor, BOOLEAN minorDeleted)
{
	for (int i = 0; i < m_MadeOfferArray.GetSize(); i++)
	{
		// Alle Einträge die nach der Minor kamen, auf die sich bezogen wurde müssen um eins verringert werden,
		// da später die Minor aus dem Feld gelöscht wird
		if (m_MadeOfferArray[i].majorRace == m_iDiplomacy && m_MadeOfferArray[i].minorRace > minor && minorDeleted == TRUE)
			m_MadeOfferArray[i].minorRace--;
		// Ist das Angebot von dieser Minorrace?
		if (m_MadeOfferArray[i].majorRace == m_iDiplomacy && m_MadeOfferArray[i].minorRace == minor)
			m_MadeOfferArray.RemoveAt(i--);
	}
}

/// Diese Funktion generiert eine Antwort einer Minorrace auf ein diplomatisches Angebot.
DiplomaticAnswersStruct CDiplomacy::GenerateMinorAnswer(BYTE majorRace, USHORT minorRace, short type, CString minorRaceName, USHORT minorType, BOOLEAN accepted)
{
	DiplomaticAnswersStruct tempAnswer;
	tempAnswer.majorRace = majorRace;
	tempAnswer.minorRace = minorRace;
	switch (type)
	{
	case TRADE_AGREEMENT:
		{
			if (accepted == TRUE)
				tempAnswer.headline = CResourceManager::GetString("ACCEPT_TRADE_AGREEMENT");
			else
				tempAnswer.headline = CResourceManager::GetString("DECLINE_TRADE_AGREEMENT");
			break;
		}
	case FRIENDSHIP_AGREEMENT:
		{
			if (accepted == TRUE)
				tempAnswer.headline = CResourceManager::GetString("ACCEPT_FRIENDSHIP");
			else
				tempAnswer.headline = CResourceManager::GetString("DECLINE_FRIENDSHIP");
			break;
		}
	case COOPERATION:
		{
			if (accepted == TRUE)
				tempAnswer.headline = CResourceManager::GetString("ACCEPT_COOPERATION");
			else
				tempAnswer.headline = CResourceManager::GetString("DECLINE_COOPERATION");
			break;
		}
	case AFFILIATION:
		{
			if (accepted == TRUE)
				tempAnswer.headline = CResourceManager::GetString("ACCEPT_AFFILIATION");
			else
				tempAnswer.headline = CResourceManager::GetString("DECLINE_AFFILIATION");
			break;
		}
	case MEMBERSHIP:
		{
			if (accepted == TRUE)
				tempAnswer.headline = CResourceManager::GetString("ACCEPT_MEMBERSHIP");
			else
				tempAnswer.headline = CResourceManager::GetString("DECLINE_MEMBERSHIP");
			break;
		}
	case CORRUPTION:
		{
			if (accepted == TRUE)
				tempAnswer.headline = CResourceManager::GetString("ACCEPT_CORRUPTION");
			else
				tempAnswer.headline = CResourceManager::GetString("DECLINE_CORRUPTION");
			break;
		}
	case WAR:
		{
			tempAnswer.headline = CResourceManager::GetString("WAR");
			break;
		}
	}
	tempAnswer.message = CGenDiploMessage::GenerateMinorAnswer(minorRaceName,minorType,type,accepted);
	return tempAnswer;
}

/// Diese Funktion generiert eine Antwort einer Majorrace auf ein diplomatisches Angebot.
DiplomacyMajorAnswersStruct CDiplomacy::GenerateMajorAnswer(short type, BOOLEAN accepted, BYTE answerFrom)
{
	DiplomacyMajorAnswersStruct tempAnswer;
	tempAnswer.AnswerFromMajorRace = answerFrom;
	switch (type)
	{
	case TRADE_AGREEMENT:
		{
			if (accepted == TRUE)
				tempAnswer.headline = CResourceManager::GetString("ACCEPT_TRADE_AGREEMENT");
			else
				tempAnswer.headline = CResourceManager::GetString("DECLINE_TRADE_AGREEMENT");
			break;
		}
	case FRIENDSHIP_AGREEMENT:
		{
			if (accepted == TRUE)
				tempAnswer.headline = CResourceManager::GetString("ACCEPT_FRIENDSHIP");
			else
				tempAnswer.headline = CResourceManager::GetString("DECLINE_FRIENDSHIP");
			break;
		}
	case COOPERATION:
		{
			if (accepted == TRUE)
				tempAnswer.headline = CResourceManager::GetString("ACCEPT_COOPERATION");
			else
				tempAnswer.headline = CResourceManager::GetString("DECLINE_COOPERATION");
			break;
		}
	case AFFILIATION:
		{
			if (accepted == TRUE)
				tempAnswer.headline = CResourceManager::GetString("ACCEPT_AFFILIATION");
			else
				tempAnswer.headline = CResourceManager::GetString("DECLINE_AFFILIATION");
			break;
		}
	case NON_AGGRESSION_PACT:
		{
			if (accepted == TRUE)
				tempAnswer.headline = CResourceManager::GetString("ACCEPT_NON_AGGRESSION");
			else
				tempAnswer.headline = CResourceManager::GetString("DECLINE_NON_AGGRESSION");
			break;
		}
	case DEFENCE_PACT:
		{
			if (accepted == TRUE)
				tempAnswer.headline = CResourceManager::GetString("ACCEPT_DEFENCE_PACT");
			else
				tempAnswer.headline = CResourceManager::GetString("DECLINE_DEFENCE_PACT");
			break;
		}
	case WAR_PACT:
		{
			if (accepted == TRUE)
				tempAnswer.headline = CResourceManager::GetString("ACCEPT_WARPACT");
			else
				tempAnswer.headline = CResourceManager::GetString("DECLINE_WARPACT");
			break;
		}
	case DIP_REQUEST:
		{
			if (accepted == TRUE)
				tempAnswer.headline = CResourceManager::GetString("ACCEPT_REQUEST");
			else
				tempAnswer.headline = CResourceManager::GetString("DECLINE_REQUEST");
			break;
		}
	case WAR:
		{
			tempAnswer.headline = CResourceManager::GetString("WAR");
			break;
		}
	}
	tempAnswer.message = CGenDiploMessage::GenerateMajorAnswer(answerFrom,type,accepted);
	return tempAnswer;
}

/// Resetfunktion für die Klasse CDiplomacy.
void CDiplomacy::Reset()
{
	m_iDiplomacy = 0;
	m_MadeOfferArray.RemoveAll();
	m_AnswerArray.RemoveAll();
}



