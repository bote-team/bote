/*
 *   Copyright (C)2004-2009 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de.vu
 *
 */
#pragma once
#include "afx.h"
#include "Options.h"

// forward declaration
class CSystem;

class CGlobalStorage :	public CObject
{
public:
	DECLARE_SERIAL (CGlobalStorage)

	struct StorageStruct
	{
		USHORT resTransfer;
		BYTE res;
		CPoint ko;
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
	UINT GetRessourceStorage(BYTE res) const {return m_iRessourceStorages[res];}

	/**
	 * Diese Funktion gibt einen Zeiger auf das Feld mit dem Lagerinhalt aller Ressourcen zurück.
	 */
	UINT* GetRessourceStorages() {return m_iRessourceStorages;}

	/**
	 * Diese Funktion gibt die Menge der Ressource <code>res</code> zurück, die von dem System mit der Koordinate
	 * <code>ko</code> in das globale Lager kommen sollen.
	 */
	USHORT GetAddedResource(BYTE res, CPoint ko) const;

	/**
	 * Diese Funktion gibt die Menge der Ressource <code>res</code> zurück, die in das System mit der Koordinate
	 * <code>ko</code> aus dem globale Lager kommen sollen.
	 */
	USHORT GetSubResource(BYTE res, CPoint ko) const;

	/**
	 * Diese Funktion gibt die gesamte Menge der Ressource <code>res</code> zurück, die in das globale Lager kommen soll.
	 */
	USHORT GetAllAddedResource(BYTE res) const;

	/**
	 * Diese Funktion gibt die gesamte Menge der Ressource <code>res</code> zurück, die aus dem globalen Lager entfernt
	 * werden soll.
	 */
	USHORT GetAllSubResource(BYTE res) const;

	/**
	 * Diese Funktion gibt den Prozentsatz zurück, mit dem man jede Runde Ressourcen aus dem Lager verliert.
	 */
	BYTE GetLosing() const {return m_byPercentLosing;}

	/**
	 * Diese Funktion gibt die Menge an Ressourcen zurück, die man in dieser Runde schon aus dem Lager genommen hat.
	 */
	USHORT GetTakenRessources() const {return m_iTakeFromStorage;}

	/**
	 * Diese Funktion gibt den Wert zurück, welche Menge wir maximal aus dem Lager nehmen können.
	 */
	USHORT GetMaxTakenRessources() const {return m_iMaxTakeFromStorage;}

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
	void SetMaxTakenRessources(USHORT max) {if (max > 20000) max = 20000; m_iMaxTakeFromStorage = max;}
		
	// sonstige Funktionen
	/**
	 * Diese Funktion addiert die mit dem Parameter <code>add</code> übergebende Anzahl der Ressource
	 * <code>res</code> zum Lagerinhalt. Im Parameter <code>ko</code> wird die Systemkoordinate übergeben,
	 * von wo aus man die Transaktion führt. Der Rückgabewert der Funktion ist eine Menge, einer
	 * Ressource, die in der selben Runde schon aus dem globalen Lager in das System kommen soll.
	 */
	USHORT AddRessource(USHORT add, BYTE res, CPoint ko);

	/**
	 * Diese Funktion subtrahiert die mit dem Parameter <code>sub</code> übergebende Anzahl der Ressource
	 * <code>res</code> vom Lagerinhalt. Im Parameter <code>ko</code> wird die Systemkoordinate übergeben, von wo aus
	 * man die Transaktion führt. Dabei beachtet die Funktion, dass nicht schon zuviel aus dem Lager genommen
	 * wurde und das der Lagerinhalt nicht negativ werden kann. Der Rückgabewert der Funktion ist eine Menge, einer
	 * Ressource, die in der selben Runde schon aus dem gleichen System addiert wurde. Somit kann man die Waren
	 * auch sofort wieder herausnehmen.
	 */
	USHORT SubRessource(USHORT sub, BYTE res, CPoint ko);

	/**
	 * Diese Funktion führt am Lagerinhalt alle möglichen Änderungen durch, die bei jeder neuen Runde eintreten
	 * können. Dabei füllt sie auch die Lager der entsprechenden Systeme.
	 */
	void Calculate(CSystem systems[STARMAP_SECTORS_HCOUNT][STARMAP_SECTORS_VCOUNT]);

	/**
	 * Resetfunktion für die Klasse CGlobalStorage, welche alle Werte wieder auf Ausgangswerte setzt.
	 */
	void Reset();


private:
	/// In diesem Feld werden die einzelnen Ressourcen gespeichert
	UINT m_iRessourceStorages[5];

	/// Wieviel Prozent gehen pro Runde aus dem Lager verloren
	BYTE m_byPercentLosing;

	/// Welche Menge an Ressourcen wurden in dieser Runde aus dem Lager genommen
	USHORT m_iTakeFromStorage;

	/// Welche Menge darf maximal aus dem Lager genommen werden. Obergrenze wurde
	/// hier auf 20k festgelegt.
	USHORT m_iMaxTakeFromStorage;

	/// In diesem Feld werden alle Systeme gespeichert, an die in der neuen Runde
	/// Ressourcen gehen.
	CArray<StorageStruct> m_ResOut;

	/// In diesem Feld werden alle Systeme gespeichert, aus denen in der neuen
	/// Runde Ressourcen ins globale Lager gehen.
	CArray<StorageStruct> m_ResIn;
};
