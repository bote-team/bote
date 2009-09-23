#include "stdafx.h"
#include "AttackSystem.h"
#include "Fleet.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////
CAttackSystem::CAttackSystem(void)
{	
}


CAttackSystem::~CAttackSystem(void)
{
	m_pShips.RemoveAll();
	m_pTroops.RemoveAll();	
}

//////////////////////////////////////////////////////////////////////
// sonstige Funktionen
//////////////////////////////////////////////////////////////////////
/// Diese Funktion initiiert das CAttackSystem Objekt mit den entsprechenden Variablen. Dabei übernimmt sie als
/// Parameter einen Zeiger auf die verteidigende Rasse <code>pDefender</code>, einen Zeiger auf das System <code>system</code>,
/// welches angegriffen wird, einen Zeiger auf das komplette Feld aller Schiffe <code>ships</code>, einen Zeiger auf den
/// zum System gehörenden Sektor <code>sector</code>, einen Zeiger auf die Gebäudeinformationen <code>buildingInfos</code>
/// und das Feld mit den Monopolbesitzern <code>monopolOwner</code>.
void CAttackSystem::Init(CRace* pDefender, CSystem* system, ShipArray* ships, CSector* sector, BuildingInfoArray* buildingInfos, const CString* monopolOwner)
{
	m_pDefender = pDefender;
	m_pSystem = system;
	m_pSector = sector;
	m_pBuildingInfos = buildingInfos;
	m_KO = sector->GetKO();;
	m_sMonopolOwner = monopolOwner;
	
	m_bTroopsInvolved = FALSE;
	m_bAssultShipInvolved = FALSE;
	m_fKilledPop = 0.0;
	m_iDestroyedBuildings = 0;
	
	for (int i = 0; i < ships->GetSize(); i++)
		if (ships->GetAt(i).GetKO() == m_KO && ships->GetAt(i).GetCurrentOrder() == ATTACK_SYSTEM)
		{
			m_pShips.Add(&ships->GetAt(i));
			// Wenn das Schiff eine Flotte besitzt, diese Schiffe auch dem Feld hinzufügen
			if (ships->GetAt(i).GetFleet() != 0)
				for (int j = 0; j < ships->GetAt(i).GetFleet()->GetFleetSize(); j++)
					m_pShips.Add(ships->GetAt(i).GetFleet()->GetPointerOfShipFromFleet(j));
		}
}

/// Diese Funktion führt den Angriff durch. Außerdem werden alle Berechnungen der Auswirkungen des Angriffs 
/// durchgeführt. Der Rückgabewert ist <code>TRUE</code>, wenn der Angriff erfolgreich war, bei Misserfolg
/// ist der Rückgabewert <code>FALSE</code>.
BOOLEAN CAttackSystem::Calculate()
{
	m_fKilledPop = (float)m_pSystem->GetHabitants();
	USHORT killedTroopsInSystem = m_pSystem->GetTroops()->GetSize();

	int shipDefence = 0;
	if (m_pSystem->GetOwnerOfSystem() != "" && m_pDefender != NULL && m_pDefender->GetType() == MAJOR)
		shipDefence = m_pSystem->GetProduction()->GetShipDefend();

	// Zuerst wird die Schiffsabwehr des Systems beachtet. Dadurch können schon einige Schiffe zerstört werden.
	this->CalculateShipDefence();
	// Danach wird das System bombardiert.
	this->CalculateBombAttack();
	// Jetzt kommt es zum Bodenangriff in dem System durch die Truppen.
	this->CalculateTroopAttack();
	
	// abschließende Berechnungen für den Systemangriff
	// Getötete Bevölkerung berechnen
	m_fKilledPop -= (float)m_pSystem->GetHabitants();
	VERIFY(m_fKilledPop >= 0);

	if (m_fKilledPop != 0.0f)
	{
		CString n; n.Format("%.3lf", m_fKilledPop);
		m_strNews.Add(CResourceManager::GetString("KILLED_POP_BY_SYSTEMATTACK",0,n,m_pSector->GetName()));
	}
	// verlorene Truppen berechnen
	USHORT killedTroopsInTransport = 0;
	killedTroopsInSystem -= m_pSystem->GetTroops()->GetSize();
	if (killedTroopsInSystem > 0)
	{
		CString n; n.Format("%d",killedTroopsInSystem);
		m_strNews.Add(CResourceManager::GetString("KILLED_TROOPS_IN_SYSTEM",0,n,m_pSector->GetName()));
	}
	// Erfahrung der Schiffe berechnen
	// (Bevölkerungsverlust in Mrd.) * 100 + aktive shipdefence EP.
	int XP = (int)(m_fKilledPop * 100 + shipDefence);
	// den XP-Wert gleichverteilt auf alle noch lebenden Schiffe anrechnen
	if (m_pShips.GetSize() > 0)
	{
		XP /= m_pShips.GetSize();
		for (int i = 0; i < m_pShips.GetSize(); i++)
			m_pShips.GetAt(i)->SetCrewExperiance(XP);
	}


	// Das System wurde erfolgreich erobert
	if (m_pTroops.GetSize() > 0 && m_pSystem->GetHabitants() > 0.0f)
	{
		// Das System wurde erobert. Truppen aus dem Transporter werden in dem System stationiert
		for (int i = 0; i < m_pTroops.GetSize(); i++)
			m_pSystem->AddTroop(m_pTroops.GetAt(i));
		m_pSystem->SetOwnerOfSystem(m_pTroops.GetAt(0)->GetOwner());
		m_pSector->SetOwned(TRUE);
		// Alle Truppen von den Transportern löschen, da diese jetzt im System sind
		for (int i = 0; i < m_pShips.GetSize(); i++)
			for (int j = 0; j < m_pShips.GetAt(i)->GetTransportedTroops()->GetSize();)
			{
				if (m_pShips.GetAt(i)->GetTransportedTroops()->GetAt(j).GetPower() == NULL)
					killedTroopsInTransport++;
				m_pShips.GetAt(i)->GetTransportedTroops()->RemoveAt(j);
			}
		if (killedTroopsInTransport > 0)
		{
			CString n; n.Format("%d",killedTroopsInTransport);
			m_strNews.Add(CResourceManager::GetString("KILLED_TROOPS_IN_TRANSPORTS",0,n,m_pSector->GetName()));
		}
		if (m_bTroopsInvolved)
			m_strNews.InsertAt(0,CResourceManager::GetString("INVASION_SUCCESSFUL",0,m_pSector->GetName()));
		return TRUE;
	}
	// das System wurde bombardiert bzw. die ganze Bevölkerung vernichtet
	else
	{
		// Alle Truppen mit Stärke == NULL von den Transportern löschen, da diese im Kampf gefallen sind und
		// nicht wieder mitgenommen werden können
		for (int i = 0; i < m_pShips.GetSize(); i++)
			for (int j = 0; j < m_pShips.GetAt(i)->GetTransportedTroops()->GetSize(); j++)
				if (m_pShips.GetAt(i)->GetTransportedTroops()->GetAt(j).GetPower() == NULL)
				{
					m_pShips.GetAt(i)->GetTransportedTroops()->RemoveAt(j--);
					killedTroopsInTransport++;
				}
		if (killedTroopsInTransport > 0)
		{
			CString n; n.Format("%d",killedTroopsInTransport);
			m_strNews.Add(CResourceManager::GetString("KILLED_TROOPS_IN_TRANSPORTS",0,n,m_pSector->GetName()));
		}
		if (m_bTroopsInvolved == TRUE && m_pSystem->GetHabitants() > 0.0f)
			m_strNews.InsertAt(0,CResourceManager::GetString("INVASION_FAILED",0,m_pSector->GetName()));
		else if (m_bTroopsInvolved == FALSE && m_pSystem->GetHabitants() > 0.0f)
			m_strNews.InsertAt(0,CResourceManager::GetString("SYSTEM_BOMBED",0,m_pSector->GetName()));
		else
			m_strNews.InsertAt(0,CResourceManager::GetString("ALL_LIFE_DIED",0,m_pSector->GetName()));
		return FALSE;
	}
}

/// Diese Funktion gibt zurück, ob der Verteidiger ungleich dem/den Angreifer/n ist.
BOOLEAN CAttackSystem::IsDefenderNotAttacker(CString sDefender, const map<CString, bool>* attacker)
{
	if (attacker->find(sDefender) == attacker->end())
		return true;
	else
		return false;
}

//////////////////////////////////////////////////////////////////////
// private Funktionen
//////////////////////////////////////////////////////////////////////

/// Private Funktion, die allein die Schiffsabwehr berechnet
void CAttackSystem::CalculateShipDefence()
{
	MYTRACE(MT::LEVEL_INFO, "CAttackSystem::CalculateShipDefence() begin...\n");
	int defence = 0;
	USHORT killedShips = 0;
	if (m_pSystem->GetOwnerOfSystem() != "" && m_pDefender != NULL && m_pDefender->GetType() == MAJOR)
		defence = m_pSystem->GetProduction()->GetShipDefend();
	// einfacher Algorithmus:
	//		Der Defencewert wird durch die Anzahl der angreifenden Schiffe geteilt. Dann wird der Anzahl der
	//		angreifenden Schiffe zufällig auf ein Schiff geschossen, welches jedesmal defence / Anzahl (+-20%)
	//		Schaden verursacht.
	for (int i = 0; i < m_pShips.GetSize(); i++)
	{
		if (m_pShips.GetAt(i)->GetTransportedTroops()->GetSize() > 0)
			m_bTroopsInvolved = TRUE;
		USHORT hit = rand()%m_pShips.GetSize();
		int dam = (defence * (rand()%41+80))/100 / m_pShips.GetSize();
		// Wenn es sich um ein Assaultship handelt, so werden 20% des Schadens vermieden
		if (m_pShips[hit]->HasSpecial(ASSULTSHIP))
			dam = (int)(dam * 0.8f);
		m_pShips[hit]->GetHull()->SetCurrentHull(-dam);
	}
	// Alle Schiffe, die mittlerweile keine Hülle mehr haben sind zerstört!
	for (int i = 0; i < m_pShips.GetSize(); i++)
	{
		if (m_pShips.GetAt(i)->GetHull()->GetCurrentHull() == NULL)
		{
			m_pShips.RemoveAt(i--);
			killedShips++;
		}
		else
		{
			// Wenn das	Schiff nicht zerstört wurde und Truppen an Bord hat, so wird das Truppenfeld gefüllt
			for (int j = 0; j < m_pShips.GetAt(i)->GetTransportedTroops()->GetSize(); j++)
			{
				m_pTroops.Add(&m_pShips.GetAt(i)->GetTransportedTroops()->GetAt(j));
				//m_pShips.GetAt(i)->GetTransportedTroops()->RemoveAt(j);
			}
		}
	}
	if (killedShips == 0)
		m_strNews.Add(CResourceManager::GetString("NO_ATTACKING_SHIPS_KILLED",0,m_pSector->GetName()));
	else
	{
		CString n; n.Format("%d",killedShips);
		m_strNews.Add(CResourceManager::GetString("ATTACKING_SHIPS_KILLED",0,n,m_pSector->GetName()));
	}
/*	CString s;
	s.Format("Anzahl der angreifenden Truppen: %d",m_pTroops.GetSize());
	AfxMessageBox(s);
*/
}

/// Private Funktion, die allein die Systembombardierung beachtet
void CAttackSystem::CalculateBombAttack()
{
	MYTRACE(MT::LEVEL_INFO, "CAttackSystem::CalculateBombAttack() begin...\n");
	// Bei der Bombardierung werden nur die Torpedos der Schiffe beachtet.
	// Bei der Bombardierung können Gebäude, Truppen und Bevölkerung zerstört werden bzw. sterben. Aktivierte Schilde
	// in dem System können dies reduzieren bzw. auch komplett verhindern.
	int shield = m_pSystem->GetProduction()->GetShieldPower();
	int torpedoDamage = 0;
	for (int i = 0; i < m_pShips.GetSize(); i++)
		for (int j = 0; j < m_pShips.GetAt(i)->GetTorpedoWeapons()->GetSize(); j++)
		{
			int dmg = m_pShips.GetAt(i)->GetTorpedoWeapons()->GetAt(j).GetTorpedoPower() *
				m_pShips.GetAt(i)->GetTorpedoWeapons()->GetAt(j).GetNumber() *
				m_pShips.GetAt(i)->GetTorpedoWeapons()->GetAt(j).GetNumberOfTupes();
			// Wenn es sich um ein Assultship handelt, so wird der Torpedoschaden um 20% erhöht
			if (m_pShips.GetAt(i)->HasSpecial(ASSULTSHIP))
			{
				dmg = (int)(dmg * 1.2f);
				m_bAssultShipInvolved = TRUE;
			}
			torpedoDamage += dmg;
		}
	// Die Schilde werden vom Torpedoschaden abgezogen
	if ((torpedoDamage - shield) < 0)
		torpedoDamage = 0;
	else
		torpedoDamage -= shield;
	
	// Nachricht das Schilde den Schaden im System verringert haben
	if (shield > 0)
		m_strNews.Add(CResourceManager::GetString("SHIELDS_SAVED_LIFE",0,m_pSector->GetName()));

/*	CString s;
	s.Format("Torpedoschaden im System: %i",torpedoDamage);
	AfxMessageBox(s);
*/
	m_iDestroyedBuildings = 0;
	// Durch den Torpedoschaden kann sich die Bevölkerung in dem System verringern.
	if (torpedoDamage > 0)
	{
		float killedPop = (float)((rand()%torpedoDamage)*0.00075f);
		m_pSector->LetPlanetsShrink(-killedPop);
		m_pSystem->SetHabitants(m_pSector->GetCurrentHabitants());

		// Dann werden zufällig Gebäude zerstört. Umso mehr Gebäude vorhanden sind, desto wahrscheinlicher werden diese
		// zerstört. Auch stationierte Truppen können bei einem Angriff vernichtet werden.
		for (int i = 0; i < torpedoDamage; i++)
		{
			if (rand()%50 < m_pSystem->GetAllBuildings()->GetSize())
			{
				m_pSystem->GetAllBuildings()->RemoveAt(rand()%m_pSystem->GetAllBuildings()->GetSize());
				m_iDestroyedBuildings++;
			}
			if (rand()%25 < m_pSystem->GetTroops()->GetSize())
				m_pSystem->GetTroops()->RemoveAt(rand()%m_pSystem->GetTroops()->GetSize());
			torpedoDamage /= 4;
		}
		// Wenn der Torpedoangriff Gebäude zerstört hat, dann dies bei dem System beachten. Deshalb werden die Werte des
		// Systems neu berechnet. Speziel muss man hier die Gebäude beachten, die durch einen Energiemangel hätten ausfallen
		// können. Diese auch offline schalten und dann die Sache nochmal berechnen.
		if (m_pSystem->GetOwnerOfSystem() != "" && m_pDefender != NULL && m_pDefender->GetType() == MAJOR)
		{
			CResearchInfo* pInfo = ((CMajor*)m_pDefender)->GetEmpire()->GetResearch()->GetResearchInfo();
			m_pSystem->CalculateVariables(m_pBuildingInfos, pInfo, m_pSector->GetPlanets(), ((CMajor*)m_pDefender), m_sMonopolOwner);
			m_pSystem->CheckEnergyBuildings(m_pBuildingInfos);
			m_pSystem->CalculateVariables(m_pBuildingInfos, pInfo, m_pSector->GetPlanets(), ((CMajor*)m_pDefender), m_sMonopolOwner);
		}
		
		if (m_iDestroyedBuildings != 0)
		{
			CString n;
			n.Format("%d", m_iDestroyedBuildings);
			m_strNews.Add(CResourceManager::GetString("DESTROYED_BUILDINGS_BY_SYSTEMATTACK",0,n,m_pSector->GetName()));
		}
	}
}

/// Private Funktion, die allein den Angriff durch Truppen berechnet
void CAttackSystem::CalculateTroopAttack()
{
	MYTRACE(MT::LEVEL_INFO, "CAttackSystem::CalculateTroopAttack() begin...\n");
	// Wenn ein Assaultship am Angriff beteiligt ist, so bekommen die angreifenden Truppen einen 20% Stärkebonus
	BYTE offenceBoni = 0;
	if (m_bAssultShipInvolved)
		offenceBoni = 20;

	// Zuerst kämpfen die angreifenden Truppen gegen
	// die Bodenabwehr in dem System. Danach werden die stationierten Truppen gegen die Angreifer kämpfen. Wenn diese
	// vernichtet wurden, dann kämpft noch der Rest der Bevölkerung gegen die angreifenden Truppen. Wenn ein Großteil
	// dieser vernichtet wurde, dann gilt das System als erobert.
	int groundDefence = m_pSystem->GetProduction()->GetGroundDefend();	// <- beinhaltet schon GroundDefendBoni!
	
	// Dieser Verteidigungswert addiert sich zum Verteidigungswert durch die Bevölkerung. Aller 5Mrd. Bevölkerung bekommt
	// man eine Verteidigungseinheit. Das würde bedeuten, dass man mit 50Mrd. Bevölkerung 10 Einheiten mit der
	// Angriffsstärke von 10 bekommt. Ein GroundDefence-Wert von z.B. 30 wird behandelt, als wären zusätzliche
	// 30Mrd. Bevölkerung in diesem System. Somit bekommt man zusätzliche 6 Einheiten und jeder Angriff dieser Verteidigung
	// wird mit einer 10er Stärke durchgeführt. Also genauso stark wie die Bevölkerung.
	for (int i = 0; i < groundDefence; i += 5)
	{
		if (m_pTroops.GetSize() == 0)
			break;
		CTroopInfo* ti = new CTroopInfo();
		ti->SetPower(10);
		int number = rand()%m_pTroops.GetSize();
		BYTE result = m_pTroops.GetAt(number)->Attack((CTroop*)ti, offenceBoni, 0);
		// Die angreifende Einheit hat verloren
		if (result > 0)
		{
			// Alle Truppen mit Stärke NULL werden am Ende von den Schiffen gelöscht
			m_pTroops.GetAt(number)->SetPower(0);
			m_pTroops.RemoveAt(number);			
		}
		delete ti;
	}

	// Hier der Angriff gegen die stationierten Truppen
	while (m_pSystem->GetTroops()->GetSize() > 0)
	{
		if (m_pTroops.GetSize() == 0)
			break;
		int numberAtt = rand()%m_pTroops.GetSize();
		int numberDef = rand()%m_pSystem->GetTroops()->GetSize();
		BYTE result = m_pTroops.GetAt(numberAtt)->Attack(&m_pSystem->GetTroops()->GetAt(numberDef), offenceBoni,
			m_pSystem->GetProduction()->GetGroundDefendBoni());
		if (result == 0)
			m_pSystem->GetTroops()->RemoveAt(numberDef);
		else if (result == 1)
		{
			// Alle Truppen mit Stärke NULL werden am Ende von den Schiffen gelöscht
			m_pTroops.GetAt(numberAtt)->SetPower(0);
			m_pTroops.RemoveAt(numberAtt);
		}
		else
		{
			m_pSystem->GetTroops()->RemoveAt(numberDef);
			// Alle Truppen mit Stärke NULL werden am Ende von den Schiffen gelöscht
			m_pTroops.GetAt(numberAtt)->SetPower(0);
			m_pTroops.RemoveAt(numberAtt);
		}
	}

	BOOLEAN fighted = FALSE;
	USHORT maxFightsFromPop = (USHORT)ceil(m_pSystem->GetHabitants() / 5) * m_pSystem->GetMoral() / 100;
	// zu allerletzt der Angriff gegen die verteidigende Bevölkerung
	while (m_pTroops.GetSize() > 0 && maxFightsFromPop > 0)
	{
		CTroopInfo* ti = new CTroopInfo();
		// Hier wird die Rasseneigenschaft der verteidigenden Bevölkerung beachtet.
		int nPower = 10;
		if (m_pDefender != NULL)
		{
			if (m_pDefender->IsRaceProperty(FINANCIAL))
				nPower -= 2;
			if (m_pDefender->IsRaceProperty(WARLIKE))
				nPower += 10;
			if (m_pDefender->IsRaceProperty(AGRARIAN))
				nPower -= 3;
			if (m_pDefender->IsRaceProperty(PACIFIST))
				nPower -= 5;
			if (m_pDefender->IsRaceProperty(SNEAKY))
				nPower += 5;
			if (m_pDefender->IsRaceProperty(SOLOING))
				nPower += 2;
			if (m_pDefender->IsRaceProperty(HOSTILE))
				nPower += 7;

			if (nPower > MAXBYTE)
				nPower = MAXBYTE;
			else if (nPower < 0)
				nPower = 0;
		}
		ti->SetPower((BYTE)nPower);

		// wie stark sich die Bevölkerung verteidigt hängt vom Moralwert derer ab
		ti->SetPower(ti->GetPower() * m_pSystem->GetMoral() / 100);
		int number = rand()%m_pTroops.GetSize();
		BYTE result = m_pTroops.GetAt(number)->Attack((CTroop*)ti, offenceBoni, m_pSystem->GetProduction()->GetGroundDefendBoni());
		// Die angreifende Einheit hat verloren
		if (result != 0)
		{
			// Wenn die Angriffstärke dieser Einheit NULL beträgt, ist sie vernichtet. Am Ende des Kampfes können so die
			// einzelnen Truppen aus den Transportern gelöscht werden
			m_pTroops.GetAt(number)->SetPower(0);
			m_pTroops.RemoveAt(number);
		}
		if (result != 1)
		{
			// Durch den Angriff der Truppen kann sich die Bevölkerung in dem System verringern.
			m_pSector->LetPlanetsShrink(-(float)(rand()%5));
			fighted = TRUE;
			maxFightsFromPop--;
		}
		delete ti;
	}
	if (fighted)
	{
		m_pSystem->SetHabitants(m_pSector->GetCurrentHabitants());
		if (m_pSystem->GetOwnerOfSystem() != "" && m_pDefender != NULL && m_pDefender->GetType() == MAJOR)
		{
			CResearchInfo* pInfo = ((CMajor*)m_pDefender)->GetEmpire()->GetResearch()->GetResearchInfo();
			m_pSystem->CalculateVariables(m_pBuildingInfos, pInfo, m_pSector->GetPlanets(), ((CMajor*)m_pDefender), m_sMonopolOwner);
			m_pSystem->CheckEnergyBuildings(m_pBuildingInfos);
			m_pSystem->CalculateVariables(m_pBuildingInfos, pInfo, m_pSector->GetPlanets(), ((CMajor*)m_pDefender), m_sMonopolOwner);
		}
	}
	
	// Wenn Truppen verschiedener Imperien angegriffen haben, so müssen diese noch gegeneinander antreten. Es kann immer
	// nur ein Imperium dieses System erobern.
	if (m_pTroops.GetSize() > 0)
		for (int i = 0; i < m_pTroops.GetSize(); i++)
			if (m_pTroops.GetSize() > (i+1))
				if (m_pTroops.GetAt(i)->GetOwner() != m_pTroops.GetAt(i+1)->GetOwner())
				{
					BYTE result = m_pTroops.GetAt(i)->Attack(m_pTroops.GetAt(i+1), 0, 0);
					if (result == 0)
					{
						// Alle Truppen mit Stärke NULL werden am Ende von den Schiffen gelöscht
						m_pTroops.GetAt(i+1)->SetPower(0);
						m_pTroops.RemoveAt(i+1);
					}
					else if (result == 1)
					{
						// Alle Truppen mit Stärke NULL werden am Ende von den Schiffen gelöscht
						m_pTroops.GetAt(i)->SetPower(0);
						m_pTroops.RemoveAt(i);
					}
					else
					{
						m_pTroops.GetAt(i)->SetPower(0);
						m_pTroops.RemoveAt(i);
						// hier gleich nochmal an der selben Stelle, da der Nachfolger ja um eins nach vorn gerutscht ist
						m_pTroops.GetAt(i)->SetPower(0);
						m_pTroops.RemoveAt(i);
					}
					i = 0;
				}	
}