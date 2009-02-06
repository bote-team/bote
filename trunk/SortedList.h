/*
 *   Copyright (C)2004-2008 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de.vu
 *
 */
#pragma once

/**
 * Repräsentiert einen Eintrag in einer sortierten Liste vom Typ
 * CSortedList.
 */
template<class T> class CSortedListEntry
{
public:
	CSortedListEntry() : left(NULL), right(NULL) {}

	T *data;
	CSortedListEntry<T> *left, *right; // linker und rechter Nachfolger
};

/**
 * Sortierte Liste, die Zeiger auf Objekte vom Typ T aufnimmt. Die Objekte werden
 * mit Hilfe des Komparators C, dessen operator()-Methode zwei Parameter vom
 * Typ T* entgegen nimmt, verglichen. Beim Zerstören der Liste werden die übergebenen
 * T-Objekte nicht zerstört.
 *
 * Die Liste wird mittels Binärbaum implementiert.
 */
template<class T, class C> class CSortedList
{
public:
	CSortedList() : root(NULL) {}
	virtual ~CSortedList(); ///< ruft Clear() auf

	/// Fügt ein Objekt zur Liste hinzu.
	void Add(T *data);
	/// Liefert das kleinste Objekt und entfernt den Eintrag aus der Liste.
	T *PopFirst();

	/// Löscht alle Einträge der Liste (ohne die eingetragenen
	/// T-Objekte zu zerstören)
	void Clear();

protected:
	CSortedListEntry<T> *root; ///< Wurzel
};

// -----

template<class T, class C> CSortedList<T, C>::~CSortedList()
{
	Clear();
}

template<class T, class C> void CSortedList<T, C>::Clear()
{
	while (PopFirst());
}

template<class T, class C> void CSortedList<T, C>::Add(T *data)
{
	if (!data) return;

	// Eintrag erzeugen
	CSortedListEntry<T> *entry = new CSortedListEntry<T>();
	entry->data = data;

	// in Baum einsortieren
	CSortedListEntry<T> **tmp = &root;
	C cmp;

	while (*tmp)
	{
		if (cmp(entry->data, (*tmp)->data) < 0)
			tmp = &(*tmp)->left;	// <, nach links absteigen
		else
			tmp = &(*tmp)->right;	// >=, nach rechts absteigen
	}

	*tmp = entry;
}

template<class T, class C> T *CSortedList<T, C>::PopFirst()
{
	if (!root) return NULL;

	CSortedListEntry<T> *result;

	// so weit wie möglich nach links absteigen
	CSortedListEntry<T> **tmp = &root;
	while ((*tmp)->left) tmp = &(*tmp)->left;
	result = *tmp;

	// rechten Teilbaum des zu entfernenden Eintrags links an den
	// Elternknoten anhängen
	*tmp = (*tmp)->right;

	// Zeiger auf Inhalt merken, Listeneintrag zerstören, Zeiger zurückgeben
	T *data = result->data;
	delete result;
	return data;
}
