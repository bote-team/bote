#pragma once

template <class T> class CReference;

#include "ReferenceCounter.h"
#include "ReferenceTransfer.h"

/**
 * Repräsentiert eine Referenz auf ein Objekt vom Typ <code>T</code>.
 *
 * @author CBot
 * @version 0.0.1
 */
template <class T> class CReference
{
	friend class CReferenceTransfer<T>;

private:
	T *m_pObject;					///< Zeiger auf das referenzierte Objekt oder <code>NULL</code>

public:
	/**
	 * Erzeugt eine neue Referenz auf das angegebene Objekt; inkrementiert dessen Referenzzähler.
	 */
	CReference(T *pObject = NULL) : m_pObject(pObject)
	{
		if (pObject) pObject->Pin();
	}

	/**
	 * Erzeugt eine neue Referenz auf das durch <code>other</code> referenzierte Objekt; inkrementiert
	 * dessen Referenzzähler.
	 */
	CReference(const CReference<T> &other) : m_pObject(other.m_pObject)
	{
		if (m_pObject) m_pObject->Pin();
	}

	/**
	 * Erzeugt eine neue Referenz auf das durch <code>other</code> referenzierte Objekt; inkrementiert
	 * dessen Referenzzähler.
	 */
	CReference(CReferenceTransfer<T> &other)
	{
		m_pObject = other.Detach();
	}

	/**
	 * Zerstört diese Referenz; dekrementiert den Zähler des referenzierten Objekts.
	 */
	virtual ~CReference()
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
	 * Gibt die Referenz auf das zuvor referenzierte Objekt frei, übernimmt die Referenz von <code>other</code>.
	 * <code>other</code> referenziert anschließend nichts mehr.
	 */
	inline bool operator=(CReferenceTransfer<T> &other)
	{
		if (m_pObject) m_pObject->Release();
		m_pObject = other.Detach();
		return (m_pObject != NULL);
	}

	/**
	 * Gibt die Referenz auf das zuvor referenzierte Objekt frei, erzeugt eine neue Referenz auf das gegebene
	 * referenzierte Objekt.
	 */
	inline bool operator=(const CReference<T> &other)
	{
		if (m_pObject) m_pObject->Release();
		m_pObject = other.m_pObject;
		if (m_pObject) m_pObject->Pin();
		return (m_pObject != NULL);
	}

	/**
	 * @return Zeiger auf das referenzierte Objekt
	 */
	inline T *operator->() const
	{
		return m_pObject;
	}

	/**
	 * @return Zeiger auf das referenzierte Objekt
	 */
	inline operator T *() const
	{
		return m_pObject;
	}

	/**
	 * Testet, ob ein Objekt referenziert wird.
	 * @return <code>true</code> gdw. dieses Objekt ein anderes referenziert
	 */
	inline operator bool() const
	{
		return (m_pObject != NULL);
	}

	/**
	 * @return Referenzzähler des referenzierten Objekts; <code>0</code>, wenn kein Objekt referenziert wird
	 */
	inline UINT GetReferenceCount() const
	{
		if (m_pObject)
			return m_pObject->GetReferenceCount();
		else
			return 0;
	}

	/**
	 * Gibt die Referenz auf das bisher referenzierte Objekt frei (dekrementiert dessen Referenzzähler) und
	 * ordnet diesem Objekt ein referenzgezähltes Objekt zu, ohne dessen Referenzzähler zu inkrementieren.
	 */
	inline void Attach(T *pObject)
	{
		if (m_pObject) m_pObject->Release();
		m_pObject = pObject; 
	}

	/**
	 * Trennt das referenzierte Objekt von dieser Referenz, ohne dessen Referenzzähler zu dekrementieren.
	 * Diese Referenz referenziert anschließend kein Objekt mehr.
	 *
	 * @return Zeiger auf das bisher referenzierte Objekt
	 */
	inline T *Detach()
	{
		T *result = m_pObject;
		m_pObject = NULL;
		return result;
	}

};
