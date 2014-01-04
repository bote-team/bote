#include "stdafx.h"
#include "Anomaly.h"
#include "IOData.h"
#include "BotE.h"
#include "BotEDoc.h"
#include "Races/RaceController.h"

#include "Ships/Ships.h"
#include "General/Loc.h"
#include "Galaxy/Sector.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////
CAnomaly::CAnomaly(void) :
	m_bFlipHorz(false)
{
	m_byType = (ANOMALYTYP)(rand()%15);
	// Wurmlöcher sind sehr selten
	if (m_byType == WORMHOLE)
		m_byType = (ANOMALYTYP)(rand()%15);


	// Nebel
	if (m_byType == RADIONEBULA)
		m_sImageFile = "RadioNebula.bop";
	else if (m_byType == METNEBULA)
		m_sImageFile = "MetNebula.bop";
	else if (m_byType == DEUTNEBULA)
		m_sImageFile = "DeutNebula.bop";
	else if (m_byType == IONSTORM)
		m_sImageFile = "Ionstorm.bop";
	else if (m_byType == BINEBULA)
		m_sImageFile = "BiNebula.bop";
	else if (m_byType == TORIONGASNEBULA)
		m_sImageFile = "ToriongasNebula.bop";
	// Neutronensterne
	else if (m_byType == NEUTRONSTAR)
		m_sImageFile = "NeutronStar.bop";
	else if (m_byType == RADIOPULSAR)
		m_sImageFile = "RadioPulsar.bop";
	else if (m_byType == XRAYPULSAR)
		m_sImageFile = "XRayPulsar.bop";
	else if (m_byType == MAGNETAR)
		m_sImageFile = "Magnetar.bop";
	// Verzerrungen
	else if (m_byType == GRAVDISTORTION)
		m_sImageFile = "GravDistortion.bop";
	else if (m_byType == CONTINUUMRIP)
		m_sImageFile = "ContinuumRip.bop";
	else if (m_byType == BLACKHOLE)
		m_sImageFile = "BlackHole.bop";
	// sonstiges
	else if (m_byType == QUASAR)
		m_sImageFile = "Quasar.bop";
	else
		m_sImageFile = "Wormhole.bop";

	rand()%2 == 1 ? m_bFlipHorz = true : m_bFlipHorz = false;
}

CAnomaly::CAnomaly(const CAnomaly& other) :
	m_sImageFile(other.m_sImageFile),
	m_bFlipHorz(other.m_bFlipHorz),
	m_byType(other.m_byType)
{
}

CAnomaly& CAnomaly::operator=(const CAnomaly& other)
{
	m_sImageFile = other.m_sImageFile;
	m_bFlipHorz = other.m_bFlipHorz;
	m_byType = other.m_byType;

	return *this;
}

CAnomaly::~CAnomaly(void)
{
}

///////////////////////////////////////////////////////////////////////
// Speichern / Laden
///////////////////////////////////////////////////////////////////////
void CAnomaly::Serialize(CArchive &ar)
{
	// wenn gespeichert wird
	if (ar.IsStoring())
	{
		int nType = (int)m_byType;
		ar << nType;
		ar << m_sImageFile;
		ar << m_bFlipHorz;
	}
	// wenn geladen wird
	if (ar.IsLoading())
	{
		int nType;
		ar >> nType;
		m_byType = (ANOMALYTYP)nType;
		ar >> m_sImageFile;
		ar >> m_bFlipHorz;
	}
}

//////////////////////////////////////////////////////////////////////
// sonstige Funktionen
//////////////////////////////////////////////////////////////////////
void CAnomaly::Draw(Graphics* g, const CPoint& ptSector) const
{
	Gdiplus::Image* pBGImage;

	CString sFile = CIOData::GetInstance()->GetAppPath() + "Graphics\\MapStars\\" + m_sImageFile;
	pBGImage = Bitmap::FromFile(CComBSTR(sFile));
	AssertBotE(pBGImage);

	if (m_bFlipHorz)
		pBGImage->RotateFlip(Gdiplus::RotateNoneFlipX);

	if (pBGImage->GetLastStatus() == Ok)
		g->DrawImage(pBGImage, ptSector.x, ptSector.y, STARMAP_SECTOR_WIDTH, STARMAP_SECTOR_HEIGHT);

	delete pBGImage;
	pBGImage = NULL;
}

CString CAnomaly::GetMapName(const CPoint &pt) const
{
	CString sName;
	switch (m_byType)
	{
	// Nebel
	case RADIONEBULA:		sName = CLoc::GetString("RADIONEBULA");		break;
	case METNEBULA:			sName = CLoc::GetString("METNEBULA");		break;
	case DEUTNEBULA:		sName = CLoc::GetString("DEUTNEBULA");		break;
	case IONSTORM:			sName = CLoc::GetString("IONSTORM");		break;
	case BINEBULA:			sName = CLoc::GetString("BINEBULA");		break;
	case TORIONGASNEBULA:	sName = CLoc::GetString("TORIONGASNEBULA");break;
	// Neutronensterne
	case NEUTRONSTAR:		sName = CLoc::GetString("NEUTRONSTAR");		break;
	case RADIOPULSAR:		sName = CLoc::GetString("RADIOPULSAR");		break;
	case XRAYPULSAR:		sName = CLoc::GetString("XRAYPULSAR");		break;
	case MAGNETAR:			sName = CLoc::GetString("MAGNETAR");		break;
	// Verzerrungen
	case GRAVDISTORTION:	sName = CLoc::GetString("GRAVDISTORTION");	break;
	case CONTINUUMRIP:		sName = CLoc::GetString("CONTINUUMRIP");	break;
	case BLACKHOLE:			sName = CLoc::GetString("BLACKHOLE");		break;
	// sonstiges
	case QUASAR:			sName = CLoc::GetString("QUASAR");			break;
	case WORMHOLE:			sName = CLoc::GetString("WORMHOLE");		break;
	}

	CString s;
	s.Format("%s %c%i", sName, (char)(pt.y + 65), pt.x + 1);
	return s;
}

CString CAnomaly::GetPhysicalDescription() const
{
	switch (m_byType)
	{
	// Nebel
	case RADIONEBULA:		return CLoc::GetString("RADIONEBULA_DESC");
	case METNEBULA:			return CLoc::GetString("METNEBULA_DESC");
	case DEUTNEBULA:		return CLoc::GetString("DEUTNEBULA_DESC");
	case IONSTORM:			return CLoc::GetString("IONSTORM_DESC");
	case BINEBULA:			return CLoc::GetString("BINEBULA_DESC");
	case TORIONGASNEBULA:	return CLoc::GetString("TORIONGASNEBULA_DESC");
	// Neutronensterne
	case NEUTRONSTAR:		return CLoc::GetString("NEUTRONSTAR_DESC");
	case RADIOPULSAR:		return CLoc::GetString("RADIOPULSAR_DESC");
	case XRAYPULSAR:		return CLoc::GetString("XRAYPULSAR_DESC");
	case MAGNETAR:			return CLoc::GetString("MAGNETAR_DESC");
	// Verzerrungen
	case GRAVDISTORTION:	return CLoc::GetString("GRAVDISTORTION_DESC");
	case CONTINUUMRIP:		return CLoc::GetString("CONTINUUMRIP_DESC");
	case BLACKHOLE:			return CLoc::GetString("BLACKHOLE_DESC");
	// sonstiges
	case QUASAR:			return CLoc::GetString("QUASAR_DESC");
	case WORMHOLE:			return CLoc::GetString("WORMHOLE_DESC");
	}

	return _T("");
}

CString CAnomaly::GetGameplayDescription(void) const
{
	switch (m_byType)
	{
	// Nebel
	case RADIONEBULA:		return CLoc::GetString("RADIONEBULA_GAME");
	case METNEBULA:			return CLoc::GetString("METNEBULA_GAME");
	case DEUTNEBULA:		return CLoc::GetString("DEUTNEBULA_GAME");
	case IONSTORM:			return CLoc::GetString("IONSTORM_GAME");
	case BINEBULA:			return CLoc::GetString("BINEBULA_GAME");
	case TORIONGASNEBULA:	return CLoc::GetString("TORIONGASNEBULA_GAME");
	// Neutronensterne
	case NEUTRONSTAR:		return CLoc::GetString("NEUTRONSTAR_GAME");
	case RADIOPULSAR:		return CLoc::GetString("RADIOPULSAR_GAME");
	case XRAYPULSAR:		return CLoc::GetString("XRAYPULSAR_GAME");
	case MAGNETAR:			return CLoc::GetString("MAGNETAR_GAME");
	// Verzerrungen
	case GRAVDISTORTION:	return CLoc::GetString("GRAVDISTORTION_GAME");
	case CONTINUUMRIP:		return CLoc::GetString("CONTINUUMRIP_GAME");
	case BLACKHOLE:			return CLoc::GetString("BLACKHOLE_GAME");
	// sonstiges
	case QUASAR:			return CLoc::GetString("QUASAR_GAME");
	case WORMHOLE:			return CLoc::GetString("WORMHOLE_GAME");
	}

	return _T("");
}

double CAnomaly::GetWaySearchWeight(void) const
{
	switch (m_byType)
	{
	// Nebel
	case RADIONEBULA:		return 5.0;
	case METNEBULA:			return 0.0;
	case DEUTNEBULA:		return 5.0;
	case IONSTORM:			return 5.0;
	case BINEBULA:			return 0.0;
	case TORIONGASNEBULA:	return 0.0;
	// Neutronensterne
	case NEUTRONSTAR:		return 10.0;
	case RADIOPULSAR:		return 25.0;
	case XRAYPULSAR:		return 50.0;
	case MAGNETAR:			return 100.0;
	// Verzerrungen
	case GRAVDISTORTION:	return 10.0;
	case CONTINUUMRIP:		return 100.0;
	case BLACKHOLE:			return 0.0;
	// sonstigees
	case QUASAR:			return 0.0;
	case WORMHOLE:			return 0.0;
	}

	return 0.0;
}

bool CAnomaly::IsShipScannerDeactivated(void) const
{
	return (m_byType == NEUTRONSTAR || m_byType == RADIOPULSAR || m_byType == XRAYPULSAR || m_byType == MAGNETAR || m_byType == DEUTNEBULA || m_byType == IONSTORM);
}

void CAnomaly::CalcShipEffects(CShips* pShip, CShipMap& ships_from_fleets) const
{
	if (m_byType == NEUTRONSTAR || m_byType == RADIOPULSAR || m_byType == XRAYPULSAR || m_byType == MAGNETAR)
	{
		// teilweise Schildschaden machen
		// hat das Schiff eine Flotte, so jedes Schiff in der Flotte beachten

		for (CShips::iterator i = pShip->begin(); i != pShip->end(); ++i)
		{
			MakeShieldDmg(500, 75, i->second.get());
			// sind die Schilde runter, so wird das Schiff zerstört
			if (i->second->GetShield()->GetCurrentShield() == 0)
				i->second->GetHull()->SetCurrentHull(i->second->GetHull()->GetCurrentHull() * (-1));

			// Verlust aller Crewerfahrung bei Röntgenpulsar und Magnetar
			if (m_byType == XRAYPULSAR || m_byType == MAGNETAR)
				i->second->SetCrewExperiance(i->second->GetCrewExperience() * (-1));
			if (m_byType == MAGNETAR)
				PerhabsStrand(i->second.get());
		}

		// Schiff selbst
		MakeShieldDmg(500, 75, pShip);
		// sind die Schilde runter, so wird das Schiff zerstört
		if (pShip->GetShield()->GetCurrentShield() == 0)
			pShip->GetHull()->SetCurrentHull(pShip->GetHull()->GetCurrentHull() * (-1));

		// Verlust aller Crewerfahrung bei Röntgenpulsar und Magnetar
		if (m_byType == XRAYPULSAR || m_byType == MAGNETAR)
			pShip->SetCrewExperiance(pShip->GetCrewExperience() * (-1));
		if (m_byType == MAGNETAR)
			PerhabsStrand(pShip);
	}
	else if (m_byType == DEUTNEBULA)
	{
		// Schilde komplett weg
		// hat das Schiff eine Flotte, so jedes Schiff in der Flotte beachten
		for (CShips::iterator i = pShip->begin(); i != pShip->end(); ++i) {
			i->second->GetShield()->SetCurrentShield(i->second->GetShield()->GetCurrentShield() * (-1));
		}
		// Schiff selbst
		pShip->GetShield()->SetCurrentShield(pShip->GetShield()->GetCurrentShield() * (-1));
		if(pShip->GetCurrentOrder() == SHIP_ORDER::EXTRACT_DEUTERIUM)
			pShip->ExtractDeuterium(ships_from_fleets);
	}
	else if (m_byType == RADIONEBULA)
	{
		// Verlust aller Crewerfahrung bei radioaktiven Nebel
		for (CShips::iterator i = pShip->begin(); i != pShip->end(); ++i) {
			i->second->SetCrewExperiance(i->second->GetCrewExperience() * (-1));
		}
		// Schiff selbst
		pShip->SetCrewExperiance(pShip->GetCrewExperience() * (-1));
	}
	else if (m_byType == CONTINUUMRIP)
	{
		// Schiff zerstört
		// hat das Schiff eine Flotte, so jedes Schiff in der Flotte beachten
		for (CShips::iterator i = pShip->begin(); i != pShip->end(); ++i)
				i->second->GetHull()->SetCurrentHull(i->second->GetHull()->GetCurrentHull() * (-1));
		// Schiff selbst
		pShip->GetHull()->SetCurrentHull(pShip->GetHull()->GetCurrentHull() * (-1), true);
	}
	else if (m_byType == GRAVDISTORTION)
	{
		// teilweise Hüllenschaden machen (unabhängig von Schilden)
		// hat das Schiff eine Flotte, so jedes Schiff in der Flotte beachten
		for (CShips::iterator i = pShip->begin(); i != pShip->end(); ++i)
				MakeHullDmg(50, 50, i->second.get());
		// Schiff selbst
		MakeHullDmg(50, 50, pShip);
	}
	else if (m_byType == IONSTORM)
		pShip->ApplyIonstormEffects(ships_from_fleets);
}

void CAnomaly::MakeShieldDmg(int nMinDmgValue, int nMaxDmgPercent, CShips* pShip)
{
	int nMax = (int)pShip->GetShield()->GetCurrentShield() * (rand()%(nMaxDmgPercent + 1)) / 100;
	int nShieldDmg = max(nMinDmgValue, nMax);
	pShip->GetShield()->SetCurrentShield(pShip->GetShield()->GetCurrentShield() - nShieldDmg);
}

void CAnomaly::MakeHullDmg(int nMinDmgValue, int nMaxDmgPercent, CShips* pShip)
{
	int nMax = (int)pShip->GetHull()->GetCurrentHull() * (rand()%(nMaxDmgPercent + 1)) / 100;
	int nHullDmg = max(nMinDmgValue, nMax);
	pShip->GetHull()->SetCurrentHull(-nHullDmg, true);
}

void CAnomaly::ReduceScanPower(const CPoint &pt) const
{
	if (m_byType == BLACKHOLE || m_byType == RADIOPULSAR || m_byType == XRAYPULSAR || m_byType == MAGNETAR)
	{
		CBotEDoc* pDoc = resources::pDoc;
		AssertBotE(pDoc);

		// Scanstärke verringern
		map<CString, CMajor*>* pmMajors = pDoc->GetRaceCtrl()->GetMajors();
		for (map<CString, CMajor*>::const_iterator it = pmMajors->begin(); it != pmMajors->end(); ++it)
		{
			int nRange = 1;	// Reichweite der Scannerbeeinträchtigung
			pDoc->GetSystem(pt.x, pt.y).PutScannedSquare(nRange, -50,
				*static_cast<CRace*>(it->second), false, false, true);
		}
	}
}

void CAnomaly::PerhabsStrand(CShips* pShip)
{
	CBotEDoc* pDoc = resources::pDoc;
	AssertBotE(pDoc);

	// Antriebstech der Schiffes ermitteln
	int nPropTech = 0;
	for (int i = 0; i < pDoc->GetShipInfos()->GetSize(); i++)
		if (pDoc->GetShipInfos()->GetAt(i).GetShipClass() == pShip->GetShipClass())
		{
			nPropTech = pDoc->GetShipInfos()->GetAt(i).GetPropulsionTech();
			if (nPropTech >= 255)
				nPropTech = 0;
			break;
		}

	// umso größer die Antriebstech, desto niedriger ist die Wahrscheinlichkeit das das Schiff strandet
	int nRandom = rand()%100;
	if (nRandom > nPropTech * 10)
	{
		pShip->SetSpeed(0);
		pShip->SetRange(SHIP_RANGE::SHORT);
	}
}
