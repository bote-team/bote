#include "stdafx.h"
#include "CombatAI.h"
#include "Botf2Doc.h"
#include "Races/RaceController.h"
#include "Ships/Combat.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////
CCombatAI::CCombatAI(void)
{
}

CCombatAI::~CCombatAI(void)
{
}

//////////////////////////////////////////////////////////////////////
// sonstige Funktionen
//////////////////////////////////////////////////////////////////////
bool CCombatAI::CalcCombatTactics(const CArray<CShip*>& vInvolvedShips, const map<CString, CRace*>* pmRaces, map<CString, int>& mCombatOrders) const
{
	// allgemeinen Kampfbefehl für alle beteiligten KI Rassen einstellen
	ApplyCombatOrders(vInvolvedShips, pmRaces, mCombatOrders);

	// Haben alle beteiligten Rassen Grußfrequenzen geöffnet, so findet kein Kampf statt
	bool bAllHailingFrequencies = true;
	for (map<CString, int>::const_iterator it = mCombatOrders.begin(); it != mCombatOrders.end(); ++it)
		if (it->second != COMBAT_HAILING)
		{
			bAllHailingFrequencies = false;
			break;
		}
	if (bAllHailingFrequencies)
		return false;



	// genaue Befehle an alle beteiligten Schiffe übergeben
	ApplyShipTactics(vInvolvedShips, mCombatOrders);
	
	return true;
}

//////////////////////////////////////////////////////////////////////
// private Funktionen
//////////////////////////////////////////////////////////////////////
void CCombatAI::ApplyCombatOrders(const CArray<CShip*>& vInvolvedShips, const map<CString, CRace*>* pmRaces, map<CString, int>& mCombatOrders) const
{
	// beteiligte Rassen
	set<CString> sInvolvedRaces;
	for (int i = 0; i < vInvolvedShips.GetSize(); i++)
		sInvolvedRaces.insert(vInvolvedShips[i]->GetOwnerOfShip());
	
	// Kampfbefehl berechnen
	for (set<CString>::const_iterator it = sInvolvedRaces.begin(); it != sInvolvedRaces.end(); ++it)
	{
		// KI-Rassen und Minors müssen noch ihre Befehle einstellen
		// Alle Clients haben ihre Befehle schon eingestellt
		if (mCombatOrders.find(*it) != mCombatOrders.end())
			continue;
		
		// Beziehung zu anderen Rassen beachten, dafür Rassen aus Map holen
		map<CString, CRace*>::const_iterator iter = pmRaces->find(*it);
		if (iter == pmRaces->end())
			continue;
		
		CRace* pRace1 = iter->second;
		int nMinRelation = 100;
		for (set<CString>::const_iterator it2 = sInvolvedRaces.begin(); it2 != sInvolvedRaces.end(); ++it2)
		{
			if (*it != *it2)
			{
				iter = pmRaces->find(*it2);
				if (iter == pmRaces->end())
					continue;
				
				CRace* pRace2 = iter->second;
				// Beziehung der anderes Rasse zu uns
				if (pRace2->GetType() != MAJOR || (pRace2->GetType() == MAJOR && ((CMajor*)pRace2)->IsHumanPlayer() == false))
					nMinRelation = min(nMinRelation, pRace2->GetRelation(pRace1->GetRaceID()));
				else if (pRace1->GetType() != MAJOR || (pRace1->GetType() == MAJOR && ((CMajor*)pRace1)->IsHumanPlayer() == false))
					nMinRelation = min(nMinRelation, pRace2->GetRelation(pRace1->GetRaceID()));
			}
		}

		int nRaceMod = 0;
		if (pRace1->IsRaceProperty(HOSTILE))
			nRaceMod -= 50;
		if (pRace1->IsRaceProperty(WARLIKE))
			nRaceMod -= 25;
		if (pRace1->IsRaceProperty(SOLOING))
			nRaceMod -= 10;
		if (pRace1->IsRaceProperty(SNEAKY))
			nRaceMod -= 10;
		if (pRace1->IsRaceProperty(FINANCIAL))
			nRaceMod += 10;
		if (pRace1->IsRaceProperty(SCIENTIFIC))
			nRaceMod += 10;
		if (pRace1->IsRaceProperty(AGRARIAN))
			nRaceMod += 15;
		if (pRace1->IsRaceProperty(PACIFIST))
			nRaceMod += 25;

		// bei gute Beziehungen wird sehr wahrscheinlich die Grußfrequenzen geöffnet
		if (rand()%100 < nMinRelation + nRaceMod)
		{
			mCombatOrders[*it] = COMBAT_HAILING;
			continue;
		}

		// Ist die Beziehung nicht ausreichend, dann die Schiffstärken im Sektor beachten und Gewinnchance ermitteln
		set<const CRace*> sFriends;
		set<const CRace*> sEnemies;
		double dWinningChance = CCombat::GetWinningChance(pRace1, vInvolvedShips, pmRaces, sFriends, sEnemies);
	
		if (dWinningChance > 0.75)
			mCombatOrders[*it] = COMBAT_AUTO;
		else if (dWinningChance < 0.25)
			mCombatOrders[*it] = COMBAT_RETREAT;
		else
		{
			if (rand()%100 < dWinningChance * 100 - nRaceMod / 2)
				mCombatOrders[*it] = COMBAT_AUTO;
			else
				mCombatOrders[*it] = COMBAT_RETREAT;
		}
	}

	// Logging
	for (set<CString>::const_iterator it = sInvolvedRaces.begin(); it != sInvolvedRaces.end(); ++it)
	{
		CString sTactic;
		switch (mCombatOrders[*it])
		{
		case COMBAT_NON:		sTactic = "- (error)"; break;
		case COMBAT_USER:		sTactic = "User"; break;
		case COMBAT_HAILING:	sTactic = "Haling Frequencies"; break;
		case COMBAT_RETREAT:	sTactic = "Retreat"; break;
		case COMBAT_AUTO:		sTactic = "Auto"; break;
		}
		MYTRACE(MT::LEVEL_INFO, "Race %s is involved in combat. Tactic: %s\n", *it, sTactic);
	}
}

void CCombatAI::ApplyShipTactics(const CArray<CShip*>& vInvolvedShips, map<CString, int>& mCombatOrders) const
{
	// eingestellte Befehle an die Schiffe übergeben	
	for (int i = 0; i < vInvolvedShips.GetSize(); i++)
	{
		CShip* pShip = vInvolvedShips[i];
		CString sOwner = pShip->GetOwnerOfShip();
		if (mCombatOrders.find(sOwner) != mCombatOrders.end())
		{
			int nOrder = mCombatOrders[sOwner];
			if (nOrder == COMBAT_AUTO)
			{
				// Kampfschiffe bekommen den Angreifen-Befehl
				pShip->SetCombatTactic(COMBAT_TACTIC_ATTACK);
				// Non-Combats bekommen den Meiden-Befehl
				if (IS_NONCOMBATSHIP(pShip->GetShipType()))
				{
					// Hat das Schiff Offensivpower, so kann es doch angreifen
					if (pShip->GetCompleteOffensivePower() <= 1)
						pShip->SetCombatTactic(COMBAT_TACTIC_AVOID);
				}
				// Bei einem Autokampf wird sich kein Schiff automatisch zurückziehen!
			}
			else if (nOrder == COMBAT_HAILING)
			{
				// Hier gilt das gleiche wie beim Autokampf
				// Außer wenn alle anderen Rassen Grüßen oder einen Rückzug machen,
				// dann lassen wir sie entkommen und zerschießen nicht ihre Schiffe.

				// Prüfen ob alle anderen Kampfbeteiligten Grüßen
				bool bAvoid = true;
				for (map<CString, int>::const_iterator it = mCombatOrders.begin(); it != mCombatOrders.end(); ++it)
					if (it->first != sOwner)
						if (it->second != COMBAT_HAILING && it->second != COMBAT_RETREAT)
						{
							bAvoid = false;
							break;
						}
				if (bAvoid)
				{
					pShip->SetCombatTactic(COMBAT_TACTIC_AVOID);
				}
				else
				{
					// gleiche Zuweisung wie beim Autokampf
					// Kampfschiffe bekommen den Angreifen-Befehl
					pShip->SetCombatTactic(COMBAT_TACTIC_ATTACK);
					// Non-Combats bekommen den Meiden-Befehl
					if (pShip->IsNonCombat())
					{
						// Hat das Schiff Offensivpower, so kann es doch angreifen
						if (pShip->GetCompleteOffensivePower() <= 1)
							pShip->SetCombatTactic(COMBAT_TACTIC_AVOID);
					}
				}
			}
			else if (nOrder == COMBAT_RETREAT)
			{
				// alle Schiffe bekommen den Rückzugsbefehl, außer sie haben Manövrierbarkeit 0
				if (pShip->GetManeuverability() == 0)
					pShip->SetCombatTactic(COMBAT_TACTIC_AVOID);
				else
					pShip->SetCombatTactic(COMBAT_TACTIC_RETREAT);
			}
			else if (nOrder == COMBAT_USER)
			{
				// nichts umstellen, der Spieler hat die Befehle manuell eingestellt
			}
			else
				ASSERT(FALSE);
		}
	}
}
