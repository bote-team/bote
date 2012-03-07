#pragma once
#include "Gamedata/Events/EventScreen.h"

class CEventRandom : public CEventScreen
{
public:
	DECLARE_SERIAL (CEventRandom)

	/// Standardkonstruktor
	CEventRandom(void) {};

	/// Standardkonstruktor mit Parameterübergabe
	CEventRandom(const CString& sPlayersRace, const CString &imageName, const CString &headline = "", const CString &text = "");

	/// Standarddestruktor
	~CEventRandom(void);

	/// Serialisierungsfunktion
	virtual void Serialize(CArchive &ar);

	virtual void Create(void);

	// Zeichenfunktionen
	virtual void Draw(Graphics* g, CGraphicPool* graphicPool) const;
};
