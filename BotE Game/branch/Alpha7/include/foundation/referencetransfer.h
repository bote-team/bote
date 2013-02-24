#pragma once

template <class T> class CReferenceTransfer;

#include "Reference.h"

/**
 * Repr‰sentiert eine Referenz auf ein Objekt vom Typ <code>T</code>. Dient speziell der ‹bergabe
 * des Besitzes von Objekten, ohne den Referenzz‰hler zu in- und sofort wieder zu dekrementieren.
 *
 * @author CBot
 * @version 0.0.1
 */
template <class T> class CReferenceTransfer
{
	friend class CReference<T>;

private:
	T *m_pObject;					///< Zeiger auf das referenzierte Objekt oder <code>NULL</code>

public:
	/**
	 * Erzeugt eine neue Referenz auf das angegebene Objekt; inkrementiert dessen Referenzz‰hler.
	 */
	CReferenceTransfer(T *pObject = NULL) : m_pObject(pObject)
	{
		if (pObject) pObject->Pin();
	}

	/**
	 * ‹bernimmt die Referenz von <code>other</code>. <code>other</code> referenziert anschlieﬂend nichts mehr.
	 * Der Referenzz‰hler des referenzierten Objektes bleibt unver‰ndert.
	 */
	CReferenceTransfer(CReferenceTransfer<T> &other)
	{
		m_pObject = other.Detach();
	}

	/**
	 * ‹bernimmt die Referenz von <code>other</code>. <code>other</code> referenziert anschlieﬂend nichts mehr.
	 * Der Referenzz‰hler des referenzierten Objektes bleibt unver‰ndert.
	 */
	CReferenceTransfer(CReference<T> &other)
	{
		m_pObject = other.Detach();
	}

	/**
	 * Zerstˆrt diese Referenz; dekrementiert den Z‰hler des referenzierten Objekts.
	 */
	virtual ~CReferenceTransfer()
	{
		if (m_pObject) m_pObject->Release();
	}

	/**
	 * Gibt die Referenz auf das zuvor referenzierte Objekt frei, erzeugt eine Referenz auf das zugewiesene
	 * Objekt.
	 */
	inline void operator=(T *pObject)
	{
		if (m_pObject) m_pObject->Release();
		m_pObject = pObject;
		if (m_pObject) m_pObject->Pin();
	}

	/**
	 * Trennt das referenzierte Objekt von dieser Referenz, ohne dessen Referenzz‰hler zu dekrementieren.
	 * Diese Referenz referenziert anschlieﬂend kein Objekt mehr.
	 *
	 * @return Zeiger auf das bisher referenzierte Objekt
	 */
	T *Detach()
	{
		T *result = m_pObject;
		m_pObject = NULL;
		return result;
	}

};
