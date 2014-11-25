#pragma once

#include <vector>

class CStatistics;
class CRaceController;
class CShipMap;
class CSystem;

class CMajorJoining
{
private:
	/// Konstruktor private, damit man sich keine Instanzen holen kann.
	CMajorJoining(void);

	/// Den Kopierkonstruktor schützen um zu vermeiden, dass das Objekt unbeabsichtigt kopiert wird.
	CMajorJoining(const CMajorJoining& cc);


public:
	/// Standarddestruktor
	~CMajorJoining(void);

	/// Funktion liefert die einzige Instanz dieser Klasse (Singleton).
	/// @return Instanz dieser Klasse
	static CMajorJoining* GetInstance(void);

	void InitFromIni();

	void Calculate(int turn, const CStatistics& stats, CRaceController& race_ctrl,
		CShipMap& ships, std::vector<CSystem>& systems, float difficulty);

	void Serialize(CArchive& ar);

private:

	void CalcStartTurnChangeDueToHumansStrength(float human_average_mark, int count_of_majors, float difficulty);
	bool ShouldHappenNow(int turn) const;

	int m_TimesOccured;
	int m_TimesShouldOccur;
	int m_StartTurn;
	const int m_RisingTurns;
	const int m_Pause;
	const int m_Randomize_by;
};
