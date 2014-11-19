/*
 *   Copyright (C)2004-2013 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de
 *
 */
// Statistcs.h: Schnittstelle für die Klasse CStatistcs.
//
//////////////////////////////////////////////////////////////////////
#pragma once

#include <map>
#include <list>
#include "Constants.h"
#include <vector>

// forward declaration
class CBotEDoc;

class CStatistics : public CObject
{
public:
	DECLARE_SERIAL (CStatistics)

	/// Standardkonstruktor
	CStatistics(void);
	/// Standarddestruktor
	virtual ~CStatistics(void);
	/// Serialisierungsfunktion
	virtual void Serialize(CArchive &ar);

	// Zugriffsfunktionen
	// zum Lesen der Membervariablen

	/// Funktion gibt das universumweite Durchschnittstechlevel zurück.
	/// @return durchschnittliches Techlevel aller Hauptrassen
	BYTE GetAverageTechLevel(void) const {return m_byAverageTechLevel;}

	/// Funktion gibt Feld mit durchschnittlichen Ressourcenlagern zurück.
	/// @return Feld der durchschnittliches Menge im Ressourcenlager
	UINT* GetAverageResourceStorages(void) {return m_nAverageResourceStorages;}

	/// Funktion gibt Map mit den Schiffsstärken aller Rassen zurück.
	/// @param sRaceID Rassen-ID für die die Schiffsstärke erfragt werden soll
	/// @return Schiffsstärke der Kriegsschiffe
	UINT GetShipPower(const CString& sRaceID) const;

	// zum Schreiben der Membervariablen
	/// Funktion zum Berechnen aller Statistiken.
	/// @param pDoc Zeiger auf das Dokument
	void CalcStats(const CBotEDoc* const pDoc);

	/// Funktion ermittelt die Demographiewerte einer bestimmten Rasse.
	/// @param sRaceID gewünschte Rasse
	/// @param [out] nPlace Platzierung
	/// @param [out] fValue eigener Wert
	/// @param [out] fAverage Durchschnittswert
	/// @param [out] fFirst bester Wert
	/// @param [out] fLast schlechtester Wert
	void GetDemographicsBSP(const CString& sRaceID, int& nPlace, float& fValue, float& fAverage, float& fFirst, float& fLast);

	/// Funktion ermittelt die Demographiewerte einer bestimmten Rasse.
	/// @param sRaceID gewünschte Rasse
	/// @param [out] nPlace Platzierung
	/// @param [out] fValue eigener Wert
	/// @param [out] fAverage Durchschnittswert
	/// @param [out] fFirst bester Wert
	/// @param [out] fLast schlechtester Wert
	void GetDemographicsProductivity(const CString& sRaceID, int& nPlace, float& fValue, float& fAverage, float& fFirst, float& fLast);

	/// Funktion ermittelt die Demographiewerte einer bestimmten Rasse.
	/// @param sRaceID gewünschte Rasse
	/// @param [out] nPlace Platzierung
	/// @param [out] fValue eigener Wert
	/// @param [out] fAverage Durchschnittswert
	/// @param [out] fFirst bester Wert
	/// @param [out] fLast schlechtester Wert
	void GetDemographicsMilitary(const CString& sRaceID, int& nPlace, float& fValue, float& fAverage, float& fFirst, float& fLast);

	/// Funktion ermittelt die Demographiewerte einer bestimmten Rasse.
	/// @param sRaceID gewünschte Rasse
	/// @param [out] nPlace Platzierung
	/// @param [out] fValue eigener Wert
	/// @param [out] fAverage Durchschnittswert
	/// @param [out] fFirst bester Wert
	/// @param [out] fLast schlechtester Wert
	void GetDemographicsResearch(const CString& sRaceID, int& nPlace, float& fValue, float& fAverage, float& fFirst, float& fLast);

	/// Funktion ermittelt die Demographiewerte einer bestimmten Rasse.
	/// @param sRaceID gewünschte Rasse
	/// @param [out] nPlace Platzierung
	/// @param [out] fValue eigener Wert
	/// @param [out] fAverage Durchschnittswert
	/// @param [out] fFirst bester Wert
	/// @param [out] fLast schlechtester Wert
	void GetDemographicsMoral(const CString& sRaceID, int& nPlace, float& fValue, float& fAverage, float& fFirst, float& fLast);

	/// Funktion gibt die aktuellen Spielpunkte einer Rasse zurück.
	/// @param sRace ID gewünschte Rasse
	/// @param sCurrentRound aktuelle Runde
	/// @param fDifficultyLevel Schwierigkeitsgradswert
	/// @return Punktzahl
	int GetGamePoints(const CString& sRaceID, int nCurrentRound, float fDifficultyLevel);

	/// Funktion errechnet eine sortierte Liste der aktuellen Topsysteme.
	/// @param nLimit Anzahl der zu errechnenden Topsystem (z.B. Top 5)
	/// @param [out] lSystems Liste in welche die Koordinaten der Topsysteme abgelegt werden.
	void GetTopSystems(int nLimit, std::list<CPoint>& lSystems) const;

	float GetMark(const CString& race) const;
	std::vector<std::pair<CString, float>> GetSortedMarks() const;

	/// Funktion zum zurücksetzen aller Werte auf Ausgangswerte.
	void Reset(void);

private:
	// private Funktionen

	/// Funktion zum Berechnen des universumweiten Techdurchschnittlevels.
	/// @param pDoc Zeiger auf das Dokument
	void CalcAverageTechLevel(const CBotEDoc* const pDoc);

	/// Funktion zum Berechnen der durchschnittlichen Befüllung der Ressourcenlager.
	/// @param pDoc Zeiger auf das Dokument
	void CalcAverageResourceStorages(const CBotEDoc* const pDoc);

	/// Funktion zum Berechnen der gesamten militärischen Schiffsstärken aller Rassen.
	/// @param pDoc Zeiger auf das Dokument
	void CalcShipPowers(const CBotEDoc* const pDoc);

	void CalcDemographicsSystem(const CBotEDoc& doc);
	void CalcDemographicsMilitary(const CBotEDoc& doc);

	void CalcMarks();

	// Attribute
	BYTE m_byAverageTechLevel;						///< Durchschnittliches Techlevel aller Rassen

	UINT m_nAverageResourceStorages[RESOURCES::DERITIUM + 1];	///< Durschschnittlicher Inhalt der Ressourcenlager

	std::map<CString, UINT> m_mShipPowers;			///< Schiffsstärken aller Rassen

	enum DEMO_TYPE
	{
		BSP,
		PRODUCTIVITY,
		MILITARY,
		RESEARCH,
		MORAL
	};

	struct DEMOGRAPHICS_STORAGE
	{
		DEMOGRAPHICS_STORAGE() :
			average(0),
			first(0),
			last(0)
		{}

		std::map<CString, int> places;
		std::map<CString, float> values;
		float average;
		float first;
		float last;

		void clear()
		{
			places.clear();
			values.clear();
		}

		void Serialize(CArchive &ar)
		{
			if (ar.IsStoring())
			{
				ar << places.size();
				for(std::map<CString, int>::const_iterator it = places.begin(); it != places.end(); ++it)
					ar << it->first << it->second;

				ar << values.size();
				for(std::map<CString, float>::const_iterator it = values.begin(); it != values.end(); ++it)
					ar << it->first << it->second;

				ar << average;
				ar << first;
				ar << last;
			}
			else
			{
				unsigned size;

				places.clear();
				ar >> size;
				for(unsigned i = 0; i < size; ++i)
				{
					CString key;
					ar >> key;
					int value;
					ar >> value;
					places.insert(std::pair<CString, int>(key, value));
				}

				values.clear();
				ar >> size;
				for(unsigned i = 0; i < size; ++i)
				{
					CString key;
					ar >> key;
					float value;
					ar >> value;
					values.insert(std::pair<CString, float>(key, value));
				}

				ar >> average;
				ar >> first;
				ar >> last;
			}
		}
	};

	void CalcMarksForDemoType(const DEMOGRAPHICS_STORAGE& store, std::map<CString, float>& marks,
		bool do_average);

	void InternalCalcDemographics(DEMOGRAPHICS_STORAGE& store, const std::map<CString, float>& m);

	std::map<CString, float> m_Marks;

	DEMOGRAPHICS_STORAGE m_Bsp;
	DEMOGRAPHICS_STORAGE m_Productivity;
	DEMOGRAPHICS_STORAGE m_Military;
	DEMOGRAPHICS_STORAGE m_Research;
	DEMOGRAPHICS_STORAGE m_Moral;
};
