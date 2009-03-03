#pragma once
#include "afx.h"

class CFireArc : public CObject
{
public:
	DECLARE_SERIAL (CFireArc)

	/// Standardkonstruktir
	CFireArc(void);

	/// Konstruktor mit Parameterübergabe
	/// @param mountPos Position der Waffe auf dem Schiff (zwischen 0° und 360°)
	/// @param angle Öffnungswinkel (zwischen 0° und 360°)
	CFireArc(USHORT mountPos, USHORT angle);

	/// Standarddestruktor
	~CFireArc(void);

	/// Kopierkonstruktor
	CFireArc(const CFireArc & rhs);

	/// Zuweisungsoperator
	CFireArc & operator=(const CFireArc &);

	/// Serialisierungsfunktion
	void Serialize(CArchive &ar);

	// Zugriffsfunktionen

	/// Funktion gibt die Position der Waffe auf dem Schiff zurück.
	/// @return Position der Waffe auf dem Schiff
	USHORT GetPosition(void) const {return m_nMountPos;}

	/// Funktion gibt den Öffnungswinkel der Waffe zurück.
	/// @return Öffnungswinkel in Grad
	USHORT GetAngle(void) const {return m_nAngle;}
	
private:
	/// Anbringung der Waffe auf dem Schiff <code>FRONT, RIGHT, BACK, LEFT</code>
	USHORT m_nMountPos;

	/// Schusswinkel der Waffe (zwischen 0° und 360°) kegelförmig
	USHORT m_nAngle;
};
