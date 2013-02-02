/*
 *   Copyright (C)2004-2013 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de
 *
 */
#pragma once
#include "afx.h"
#include "IntelObject.h"
#include "array_sort.h"

/**
 * Diese Klasse beinhaltet Geheimdienstberichte eines Imperiums. Durch diese Geheimdienstberichte lassen sich
 * Geheimdienstaktionen nachvollziehen und manipulieren.
 *
 * @author Sir Pustekuchen
 * @version 0.1
 */
class CIntelReports : public CObject
{
	friend class CIntelligence;
public:
	DECLARE_SERIAL (CIntelReports)

	/// Konstruktor
	CIntelReports(void);

	/// Destruktor
	~CIntelReports(void);

	/// Serialisierungsfunktion
	void Serialize(CArchive &ar);

	// Zugriffsfunktionen
	/// Funktion gibt einen einzelnen Geheimdienstreport zurück.
	/// @param n Nummer des Reports im Feld
	CIntelObject* GetReport(int n) const {return dynamic_cast<CIntelObject*>(m_IntelObjects.GetAt(n));}

	/// Funktion gibt einen Zeiger auf das komplette Feld aller Geheimdienstreports zurück.
	CObArray* GetAllReports() {return &m_IntelObjects;}

	/// Funktion gibt die Anzahl aller Berichte zurück.
	int GetNumberOfReports() const {return m_IntelObjects.GetSize();}

	/// Funktion gibt die Nummer des aktiven Reports zurück.
	short GetActiveReport() const {return m_nActiveReport;}

	/// Funktion gibt einen Zeiger auf ein mögliches Anschlagsziel zurück. Ist kein Ziel vorhanden, so liefert
	/// die Funktion den Nullzeiger.
	CIntelObject* GetAttemptObject() const {return m_pAttemptObject;}

	/// Funktion legt ein neues Anschlagsobjekt an.
	/// @param spyReport Zeiger auf einen Spionagereport, von dessen aus das Anschlagsobjekt generiert werden soll
	void CreateAttemptObject(CIntelObject* spyReport);

	/// Funktion gibt einen Wahrheitswert zurück, der angibt ob in dieser Runde schon ein Geheimdienstreport
	/// hinzugefügt wurde.
	/// @return <code>TRUE</code> wenn in dieser Runde ein Report hinzugefügt wurde, ansonsten <code>FALSE</code>
	BOOLEAN IsReportAdded() const {return m_bAddedReport;}

	/// Funktion legt einen bestimmten Geheimdienstbericht als aktiven Bericht fest.
	/// @param n Nummer des Reports im Feld <code>m_IntelObjects</code>
	void SetActiveReport(short n);

	/// Funktion legt fast ob ein Report hinzugefügt wurde oder nicht
	/// @param isAdded <code>TRUE</code> wenn ja, ansonsten <code>FALSE</code>
	void SetIsReportAdded(BOOLEAN isAdded) {m_bAddedReport = isAdded;}

	// sonstige Funktionen
	/// Funktion fügt einen neuen Report dem Feld hinzu.
	/// @param report Zeiger auf einen Report, welcher dem Feld hinzugefügt werden soll
	void AddReport(CIntelObject* report) {m_IntelObjects.Add(report); SetIsReportAdded(TRUE);}

	/// Funktion löscht einen bestimmten Geheimdienstreport.
	/// @param n Stelle des Reports im Feld der ganzen Geheimdienstreports
	void RemoveReport(int n) {ASSERT(m_IntelObjects.GetAt(n)); delete m_IntelObjects.GetAt(n);	m_IntelObjects.RemoveAt(n);}

	/// Funktion löscht alle Geheimdienstreports, auch den für einen Anschlag
	void RemoveAllReports();

	/// Funktion sortiert die Geheimdienstberichte nach absteigender Rundenanzahl
	/// @param bDesc <code>true</code> wenn absteigend sortiert werden soll, sonst aufsteigend
	void SortAllReports(bool bDesc = true);

	/// Funktion löscht ein womöglich zuvor angelegtes Geheimdienstanschlagsobjekt.
	void RemoveAttemptObject() {if (m_pAttemptObject) {delete m_pAttemptObject; m_pAttemptObject = NULL;}}

private:
	// Attribute
	CObArray m_IntelObjects;				///< Feld aller Geheimdienstberichte

	CIntelObject* m_pAttemptObject;			///< Geheimdienstbericht, welcher für einen Anschlag genutzt werden soll

	short m_nActiveReport;					///< aktiver/angeklickter Geheimdienstbericht aus dem Feld

	BOOLEAN m_bAddedReport;					///< wurde in der aktuellen Runde ein neuer Report hinzugefügt.
};
