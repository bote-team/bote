#pragma once

#include "afxmt.h"

/**
 * Basisklasse für Klassen, deren Objekte mit Hilfe von <code>CReference</code> und
 * <code>CReferenceTransfer</code> referenzgezählt werden sollen.
 *
 * @author CBot
 * @version 0.0.1
 */
class CReferenceCounter
{
private:
	UINT m_nCounter;							///< Referenzzähler
	CCriticalSection m_CriticalSection;

public:
	/**
	 * Erzeugt ein Objekt. Initialisiert den Referenzzähler mit <code>0</code>. Anschließend
	 * wird mit der Konstruktion von <code>CReference</code> oder <code>CReferenceTransfer</code>
	 * <code>Pin()</code> aufgerufen, so dass ein späterer Aufruf von <code>Release()</code>
	 * dieses Objekt zerstört.
	 */
	CReferenceCounter();
	virtual ~CReferenceCounter();

	/**
	 * @return aktueller Referenzzähler, nach der Konstruktion <code>0</code>, nach dem ersten
	 * Aufruf von <code>Pin()</code> immer <code>&gt;= 1</code>
	 */
	inline UINT GetReferenceCount() const {return m_nCounter;}

	/**
	 * Erhöht den Referenzzähler. Diese Methode ist synchronisiert.
	 */
	void Pin();
	/**
	 * Dekrementiert den Referenzzähler. Zerstört dieses Objekt, wenn der Zähler mit dem Aufruf
	 * dieser Methode auf <code>0</code> fällt. Diese Methode ist synchronisiert.
	 */
	void Release();

};
