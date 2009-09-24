#include "stdafx.h"
#include "Race.h"
#include "Botf2Doc.h"
#include "RaceController.h"
#include "AI\DiplomacyAI.h"
#include "AI\MajorAI.h"

IMPLEMENT_SERIAL (CRace, CObject, 1)

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////
CRace::CRace(void)
{
	m_pDiplomacyAI = NULL;
}

CRace::~CRace(void)
{
}

///////////////////////////////////////////////////////////////////////
// Speichern / Laden
///////////////////////////////////////////////////////////////////////
void CRace::Serialize(CArchive &ar)		
{
	CObject::Serialize(ar);
	
	// wenn gespeichert wird
	if (ar.IsStoring())
	{
		ar << m_sID;			// Rassen-ID
		ar << m_sHomeSystem;	// Name des Heimatsystems
		ar << m_sName;			// Rassenname
		ar << m_sNameArticle;	// Artikel f�r Rassenname
		ar << m_sDesc;			// Rassenbeschreibung
		ar << m_byType;			// Rassentyp (Major, Medior, Minor)
		ar << m_nProperty;		// Rasseneigenschaften
		ar << m_byShipNumber;	// zugewiesene Nummer welche Schiffe verwendet werden sollen
		ar << m_byBuildingNumber;	// zugewiesene Nummer welche Geb�ude verwendet werden sollen
		ar << m_byMoralNumber;	// zugewiesene Nummer welche Moralwerte verwendet werden sollen

		// Ingame-Attribute (Rassenwechselwirkung)
		// Beziehungsmap (Rassen-ID, Beziehungswert)
		ar << m_mRelations.size();
		for (map<CString, BYTE>::const_iterator it = m_mRelations.begin(); it != m_mRelations.end(); it++)
			ar << it->first << it->second;
		// Diplomatischer Status gegen�ber anderen Rassen (Rassen-ID, Status)
		ar << m_mAgreement.size();
		for (map<CString, short>::const_iterator it = m_mAgreement.begin(); it != m_mAgreement.end(); it++)
			ar << it->first << it->second;
		// kennt die Rasse eine andere Rasse (Rassen-ID, Wahrheitswert)
		ar << m_vInContact.size();
		for (vector<CString>::const_iterator it = m_vInContact.begin(); it != m_vInContact.end(); it++)
			ar << *it;
		// diplomatische Nachrichten
		ar << m_vDiplomacyNewsIn.size();
		for (vector<CDiplomacyInfo>::iterator it = m_vDiplomacyNewsIn.begin(); it != m_vDiplomacyNewsIn.end(); it++)
			it->Serialize(ar);
		ar << m_vDiplomacyNewsOut.size();
		for (vector<CDiplomacyInfo>::iterator it = m_vDiplomacyNewsOut.begin(); it != m_vDiplomacyNewsOut.end(); it++)
			it->Serialize(ar);

		// grafische Attribute
		ar << m_sGraphicFile;	// Name der zugeh�rigen Grafikdatei		
	}
	// wenn geladen wird
	else if (ar.IsLoading())
	{
		ar >> m_sID;			// Rassen-ID
		ar >> m_sHomeSystem;	// Name des Heimatsystems
		ar >> m_sName;			// Rassenname
		ar >> m_sNameArticle;	// Artikel f�r Rassenname
		ar >> m_sDesc;			// Rassenbeschreibung
		ar >> m_byType;			// Rassentyp (Major, Medior, Minor)
		ar >> m_nProperty;		// Rasseneigenschaften
		ar >> m_byShipNumber;	// zugewiesene Nummer welche Schiffe verwendet werden sollen
		ar >> m_byBuildingNumber;	// zugewiesene Nummer welche Geb�ude verwendet werden sollen
		ar >> m_byMoralNumber;	// zugewiesene Nummer welche Moralwerte verwendet werden sollen

		// Ingame-Attribute (Rassenwechselwirkung)
		// Beziehungsmap (Rassen-ID, Beziehungswert)
		m_mRelations.clear();
		size_t mapSize = 0;
		ar >> mapSize;
		for (size_t i = 0; i < mapSize; i++)
		{
			CString key;
			BYTE value;
			ar >> key;
			ar >> value;
			m_mRelations[key] = value;
		}
		// Diplomatischer Status gegen�ber anderen Rassen (Rassen-ID, Status)
		m_mAgreement.clear();
		mapSize = 0;
		ar >> mapSize;
		for (size_t i = 0; i < mapSize; i++)
		{
			CString key;
			short value;
			ar >> key;
			ar >> value;
			m_mAgreement[key] = value;
		}
		// kennt die Rasse eine andere Rasse (Rassen-ID, Wahrheitswert)
		m_vInContact.clear();
		size_t vectorSize = 0;
		ar >> vectorSize;
		for (size_t i = 0; i < vectorSize; i++)
		{
			CString sID;
			ar >> sID;
			m_vInContact.push_back(sID);
		}
		// diplomatische Nachrichten
		m_vDiplomacyNewsIn.clear();
		vectorSize = 0;
		ar >> vectorSize;
		for (size_t i = 0; i < vectorSize; i++)
		{
			CDiplomacyInfo info;
			info.Serialize(ar);
			m_vDiplomacyNewsIn.push_back(info);			
		}
		// diplomatische Nachrichten
		m_vDiplomacyNewsOut.clear();
		vectorSize = 0;
		ar >> vectorSize;
		for (size_t i = 0; i < vectorSize; i++)
		{
			CDiplomacyInfo info;
			info.Serialize(ar);
			m_vDiplomacyNewsOut.push_back(info);			
		}

		// grafische Attribute
		ar >> m_sGraphicFile;	// Name der zugeh�rigen Grafikdatei
	}
}

//////////////////////////////////////////////////////////////////////
// sonstige Funktionen
//////////////////////////////////////////////////////////////////////

/// Funktion zum setzen der Rasseneigenschaften
/// @param prop Rasseneigenschaft
/// @return <code>true</code>, wenn die Rasse die Eigenschaft besitzt, sonst <code>false</code>
bool CRace::IsRaceProperty(BYTE prop) const
{
	switch (prop)
	{
		case AGRARIAN:		return (m_nProperty & RACE_AGRARIAN)	== RACE_AGRARIAN;
		case FINANCIAL:		return (m_nProperty & RACE_FINANCIAL)	== RACE_FINANCIAL;
		case HOSTILE:		return (m_nProperty & RACE_HOSTILE)		== RACE_HOSTILE;
		case INDUSTRIAL:	return (m_nProperty & RACE_INDUSTRIAL)	== RACE_INDUSTRIAL;
		case PACIFIST:		return (m_nProperty & RACE_PACIFIST)	== RACE_PACIFIST;
		case PRODUCER:		return (m_nProperty & RACE_PRODUCER)	== RACE_PRODUCER;
		case SCIENTIFIC:	return (m_nProperty & RACE_SCIENTIFIC)	== RACE_SCIENTIFIC;
		case SECRET:		return (m_nProperty & RACE_SECRET)		== RACE_SECRET;
		case SNEAKY:		return (m_nProperty & RACE_SNEAKY)		== RACE_SNEAKY;
		case SOLOING:		return (m_nProperty & RACE_SOLOING)		== RACE_SOLOING;
		case WARLIKE:		return (m_nProperty & RACE_WARLIKE)		== RACE_WARLIKE;
		default:			return NOTHING_SPECIAL;
	}
}

/// Funktion zum Setzen von Rasseneigenschaften.
/// @param prop Rasseneigenschaft
/// @param is <code>true</code> oder <code>false</code>
void CRace::SetRaceProperty(BYTE prop, bool is)
{
	switch (prop)
	{
		case NOTHING_SPECIAL:	if (is) m_nProperty = 0;							break;
		case AGRARIAN:			SetAttributes(is, RACE_AGRARIAN, m_nProperty);		break;
		case FINANCIAL:			SetAttributes(is, RACE_FINANCIAL, m_nProperty);		break;
		case HOSTILE:			SetAttributes(is, RACE_HOSTILE, m_nProperty);		break;
		case INDUSTRIAL:		SetAttributes(is, RACE_INDUSTRIAL, m_nProperty);	break;
		case PACIFIST:			SetAttributes(is, RACE_PACIFIST, m_nProperty);		break;
		case PRODUCER:			SetAttributes(is, RACE_PRODUCER, m_nProperty);		break;
		case SCIENTIFIC:		SetAttributes(is, RACE_SCIENTIFIC, m_nProperty);	break;
		case SECRET:			SetAttributes(is, RACE_SECRET, m_nProperty);		break;
		case SNEAKY:			SetAttributes(is, RACE_SNEAKY, m_nProperty);		break;
		case SOLOING:			SetAttributes(is, RACE_SOLOING, m_nProperty);		break;
		case WARLIKE:			SetAttributes(is, RACE_WARLIKE, m_nProperty);		break;
	}
}

/// Funktion setzt die neue Beziehung zur Rasse.
/// @param sRaceID Rassen-ID zu der die Beziehung ge�ndert werden soll
/// @param nAdd Wert der zur alten Beziehung addiert werden soll
void CRace::SetRelation(const CString& sRaceID, short nAdd)
{
	short nRelation = m_mRelations[sRaceID] + nAdd;

	if (nRelation > 100)
		nRelation = 100;
	else if (nRelation < 0)
		nRelation = 0;

	m_mRelations[sRaceID] = (BYTE)nRelation;
	
	if (nRelation == 0)
		m_mRelations.erase(sRaceID);
}

/// Funktion legt den diplomatischen Status zu einer anderes Rasse fest.
/// @param sOtherRace andere Rasse
/// @param nNewAgreement neuer Vertrag
void CRace::SetAgreement(const CString& sOtherRace, short nNewAgreement)
{
	// wenn kein Vertrag besteht, so kann der Eintrag auch aus der Map entfernt werden
	if (nNewAgreement == NO_AGREEMENT)
		m_mAgreement.erase(sOtherRace);
	else
		m_mAgreement[sOtherRace] = nNewAgreement;
}

/// Funktion l�sst die Diplomatie-KI der Rasse Angebote an andere Rassen erstellen.
void CRace::MakeOffersAI(void)
{
	CBotf2Doc* pDoc = ((CBotf2App*)AfxGetApp())->GetDocument();
	ASSERT(pDoc);

	// wenn es sich um eine Majorrace handelt, dann die Lieblingsminorrace berechnen
	if (this->GetType() == MAJOR)
	{
		// bei einem menschlichen Spieler wird die KI nicht ausgef�hrt
		if (((CMajor*)this)->IsHumanPlayer())
			return;
		((CMajorAI*)m_pDiplomacyAI)->CalcFavoriteMinors();
	}

	// Angebote machen
	std::map<CString, CRace*>* races = pDoc->GetRaceCtrl()->GetRaces();
	ASSERT(races);

	for (map<CString, CRace*>::const_iterator it = races->begin(); it != races->end(); it++)
		if (m_sID != it->first)
		{
			// Minorangebot zu anderer Minor geht nicht!
			if (m_byType == MINOR && it->second->m_byType == MINOR)
				continue;

			// Nur wenn wir noch keine Angebote an diese Rasse gemacht haben
			bool bOffer = false;
			for (UINT i = 0; i < m_vDiplomacyNewsOut.size(); i++)
				if (m_vDiplomacyNewsOut[i].m_nFlag == DIPLOMACY_OFFER && m_vDiplomacyNewsOut[i].m_sToRace == it->first && m_vDiplomacyNewsOut[i].m_sFromRace == m_sID)
				{
					bOffer = true;
					break;
				}
			if (bOffer)
				continue;

			// Angebot erstellen
			CDiplomacyInfo info;
			CString sID = it->first;
			if (m_pDiplomacyAI->MakeOffer(sID, info))
				this->GetOutgoingDiplomacyNews()->push_back(info);			
		}
}

/// Funktion l�sst die KI auf diplomatische Angebote reagieren.
/// @param pOffer diplomatisches Angebot
void CRace::ReactOnOfferAI(CDiplomacyInfo* pOffer)
{
	// auf Angebot reagieren
	if (pOffer->m_nFlag == DIPLOMACY_OFFER)
	{
		short nResult = m_pDiplomacyAI->ReactOnOffer(*pOffer);
		pOffer->m_nAnswerStatus = nResult;
	}
	else
		AfxMessageBox("CRace::ReactOnOfferAI(): Could not react on non diplomacy offers!");
}

/// Funktion zum zur�cksetzen aller Werte auf Ausgangswerte.
void CRace::Reset(void)
{
	m_sID			= "";				// Rassen-ID
	m_sHomeSystem	= "";				// Name des Heimatsystems
	m_sName			= "";				// Rassenname
	m_sNameArticle	= "";				// Artikel f�r Rassenname
	m_sDesc			= "";				// Rassenbeschreibung
	m_byType		= MINOR;			// Rassentyp (Major, Medior, Minor)
	m_nProperty		= 0;				// Rasseneigenschaften
	m_byShipNumber	= 0;				// zugewiesene Nummer welche Schiffe verwendet werden sollen
	m_byBuildingNumber	= 0;			// zugewiesene Nummer welche Geb�ude verwendet werden sollen
	m_byMoralNumber	= 0;				// zugewiesene Nummer welche Moralwerte verwendet werden sollen

	// Ingame-Attribute (Rassenwechselwirkung)
	m_mRelations.clear();				// Beziehungsmap (Rassen-ID, Beziehungswert)
	m_mAgreement.clear();				// Diplomatischer Status gegen�ber anderen Rassen (Rassen-ID, Status)
	m_vInContact.clear();				// kennt die Rasse eine andere Rasse (Rassen-ID, Wahrheitswert)
	// diplomatische Nachrichten
	m_vDiplomacyNewsIn.clear();
	m_vDiplomacyNewsOut.clear();

	// grafische Attribute
	m_sGraphicFile	= "";				// Name der zugeh�rigen Grafikdatei
	
	// Diplomatie-KI nullen
	if (m_pDiplomacyAI)
	{
		delete m_pDiplomacyAI;
		m_pDiplomacyAI = NULL;
	}
}

/// Funktion gibt zur�ck, ob eine andere Rasse bekannt ist.
/// @param sRaceID ID der anderen Rasse
/// @return <code>true</code> wenn Kontakt zu der anderen Rasse besteht, ansonsten <code>false</code>
bool CRace::IsRaceContacted(const CString& sRaceID) const
{
	if (std::find(m_vInContact.begin(), m_vInContact.end(), sRaceID) != m_vInContact.end())
		return true;
	
	return false;
}

/// Funktion legt fest, ob die Rasse eine andere Rasse kennt.
/// @param sRace andere Rasse
/// @param bKnown <code>TRUE</code> wenn sie sie kennenlernt, ansonsten <code>FALSE</code>
void CRace::SetIsRaceContacted(const CString& sRace, bool bKnown)
{
	// nach der neuen Rasse im Vektor suchen
	vector<CString>::iterator it = std::find(m_vInContact.begin(), m_vInContact.end(), sRace);
	if (it != m_vInContact.end())
	{
		// soll sie kennengelernt werden, ist aber schon im Feld, dann aus der Funktion springen
		if (bKnown)
			return;
		// ansonsten aus dem Vektor l�schen
		else
		{
			m_vInContact.erase(it);
			return;
		}		
	}

	// soll die Rasse hinzugef�gt werden, ist aber noch nicht im Vektor. Dann einfach anh�ngen
	m_vInContact.push_back(sRace);
}
