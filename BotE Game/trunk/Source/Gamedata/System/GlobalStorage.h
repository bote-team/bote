/*
 *   Copyright (C)2004-2011 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de
 *
 */
#pragma once
#include "afx.h"
#include "Options.h"
#include <vector>

// forward declaration
class CSystem;

class CGlobalStorage :	public CObject
{
public:
	DECLARE_SERIAL (CGlobalStorage)

	struct StorageStruct
	{
		StorageStruct() : nResTransfer(0), nRes(0), ptKO(-1, -1) {};

		UINT nResTransfer;
		BYTE nRes;
		CPoint ptKO;
	};

	/// Konstruktor
	CGlobalStorage(void);

	/// Destruktor
	~CGlobalStorage(void);

	/// Serialisierungsfunktion
	virtual void Serialize(CArchive &ar);

	// Zugriffsfunktionen zum Lesen der Membervariablen
	/**
	 * Diese Funktion gibt den Inhalt des globalen Lagers wieder. Der Parameter <code>res</code> der dabei
	 * übergeben wird, ist die Ressource, von der man den Lagerinhalt wissen will.
	 */
	UINT GetResourceStorage(BYTE res) const {return m_nResourceStorages[res];}

	/**
	 * Diese Funktion gibt einen Zeiger auf das Feld mit dem Lagerinhalt aller Ressourcen zurück.
	 */
	UINT* GetResourceStorages() {return m_nResourceStorages;}

	/**
	 * Diese Funktion gibt die Menge der Ressource <code>res</code> zurück, die von dem System mit der Koordinate
	 * <code>ko</code> in das globale Lager kommen sollen.
	 */
	UINT GetAddedResource(BYTE res, const CPoint& ko) const;

	/**
	 * Diese Funktion gibt die Menge der Ressource <code>res</code> zurück, die in das System mit der Koordinate
	 * <code>ko</code> aus dem globale Lager kommen sollen.
	 */
	UINT GetSubResource(BYTE res, const CPoint& ko) const;

	/**
	 * Diese Funktion gibt die gesamte Menge der Ressource <code>res</code> zurück, die in das globale Lager kommen soll.
	 */
	UINT GetAllAddedResource(BYTE res) const;

	/**
	 * Diese Funktion gibt die gesamte Menge der Ressource <code>res</code> zurück, die aus dem globalen Lager entfernt
	 * werden soll.
	 */
	UINT GetAllSubResource(BYTE res) const;

	/**
	 * Diese Funktion gibt den Prozentsatz zurück, mit dem man jede Runde Ressourcen aus dem Lager verliert.
	 */
	BYTE GetLosing() const {return m_byPercentLosing;}

	/**
	 * Diese Funktion gibt die Menge an Ressourcen zurück, die man in dieser Runde schon aus dem Lager genommen hat.
	 */
	UINT GetTakenRessources() const {return m_iTakeFromStorage;}

	/**
	 * Diese Funktion gibt den Wert zurück, welche Menge wir maximal aus dem Lager nehmen können.
	 */
	UINT GetMaxTakenRessources() const {return m_iMaxTakeFromStorage;}

	/**
	 * Diese Funktion gibt <code>TRUE</code> zurück, wenn sich irgendeine Ressource im globalen Lager befindet.
	 */
	BOOLEAN IsFilled() const;

	// Zugriffsfunktionen zum Schreiben der Membervariablen
	/**
	 * Diese Funktion setzt den Prozentsatz, der jede Runde vom Lagerinhalt abgezogen wird. Dieser Prozentsatz
	 * muss positiv und maximal 100% sein.
	 */
	void SetLosing(BYTE percent) {m_byPercentLosing = percent;}

	/**
	 * Diese Funktion setzt den maximalen Wert, den man pro Runde aus den Lagern entfernen kann. Die Obergrenze
	 * liegt dabei bei 20k.
	 */
	void SetMaxTakenRessources(int nMax) {m_iMaxTakeFromStorage = min(20000, nMax);}

	// sonstige Funktionen
	/**
	 * Diese Funktion addiert die mit dem Parameter <code>add</code> übergebende Anzahl der Ressource
	 * <code>res</code> zum Lagerinhalt. Im Parameter <code>ko</code> wird die Systemkoordinate übergeben,
	 * von wo aus man die Transaktion führt. Der Rückgabewert der Funktion ist eine Menge, einer
	 * Ressource, die in der selben Runde schon aus dem globalen Lager in das System kommen soll.
	 */
	UINT AddRessource(UINT add, BYTE res, const CPoint& ko);

	/**
	 * Diese Funktion subtrahiert die mit dem Parameter <code>sub</code> übergebende Anzahl der Ressource
	 * <code>res</code> vom Lagerinhalt. Im Parameter <code>ko</code> wird die Systemkoordinate übergeben, von wo aus
	 * man die Transaktion führt. Dabei beachtet die Funktion, dass nicht schon zuviel aus dem Lager genommen
	 * wurde und das der Lagerinhalt nicht negativ werden kann. Der Rückgabewert der Funktion ist eine Menge, einer
	 * Ressource, die in der selben Runde schon aus dem gleichen System addiert wurde. Somit kann man die Waren
	 * auch sofort wieder herausnehmen.
	 */
	UINT SubRessource(UINT sub, BYTE res, const CPoint& ko);

	/**
	 * Diese Funktion führt am Lagerinhalt alle möglichen Änderungen durch, die bei jeder neuen Runde eintreten
	 * können. Dabei füllt sie auch die Lager der entsprechenden Systeme.
	 */
	void Calculate(std::vector<CSystem>& systems);

	/**
	 * Resetfunktion für die Klasse CGlobalStorage, welche alle Werte wieder auf Ausgangswerte setzt.
	 */
	void Reset();


private:
	/// In diesem Feld werden die einzelnen Ressourcen gespeichert
	UINT m_nResourceStorages[5];

	/// Wieviel Prozent gehen pro Runde aus dem Lager verloren
	BYTE m_byPercentLosing;

	/// Welche Menge an Ressourcen wurden in dieser Runde aus dem Lager genommen
	UINT m_iTakeFromStorage;

	/// Welche Menge darf maximal aus dem Lager genommen werden. Obergrenze wurde hier auf 20k festgelegt.
	UINT m_iMaxTakeFromStorage;

	/// In diesem Feld werden alle Systeme gespeichert, an die in der neuen Runde Ressourcen gehen.
	CArray<StorageStruct> m_ResOut;

	/// In diesem Feld werden alle Systeme gespeichert, aus denen in der neuen
	/// Runde Ressourcen ins globale Lager gehen.
	CArray<StorageStruct> m_ResIn;
};
