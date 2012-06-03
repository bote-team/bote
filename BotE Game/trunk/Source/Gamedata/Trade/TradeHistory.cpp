#include "stdafx.h"
#include "TradeHistory.h"
#include <math.h>

IMPLEMENT_SERIAL (CTradeHistory, CObject, 1)
//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////
CTradeHistory::CTradeHistory(void)
{
}

CTradeHistory::~CTradeHistory(void)
{
	for (int i = TITAN; i <= IRIDIUM; i++)
	{
		for (int j = 0; j < m_Prices[i].GetSize(); )
			m_Prices[i].RemoveAt(j);
		m_Prices[i].RemoveAll();
	}
}

///////////////////////////////////////////////////////////////////////
// Speichern / Laden
///////////////////////////////////////////////////////////////////////
void CTradeHistory::Serialize(CArchive &ar)
{
	CObject::Serialize(ar);
	// wenn gespeichert wird
	if (ar.IsStoring())
	{
		for (int i = 0; i < 5; i++)
			m_Prices[i].Serialize(ar);
	}
	// wenn geladen wird
	if (ar.IsLoading())
	{
		for (int i = 0; i < 5; i++)
		{
			m_Prices[i].RemoveAll();
			m_Prices[i].Serialize(ar);
		}
	}
}

//////////////////////////////////////////////////////////////////////
// sonstige Funktionen
//////////////////////////////////////////////////////////////////////
// Funktion fügt die aktuelle Preisliste in das Feld ein
void CTradeHistory::SaveCurrentPrices(USHORT* resPrices, float currentTax)
{
	for (int i = TITAN; i <= IRIDIUM; i++)
	{
		//resPrices[i] = (USHORT)ceil(resPrices[i] * currentTax);
		m_Prices[i].Add((USHORT)ceil(resPrices[i] * currentTax));
	}
}

// Funktion gibt den Minimumpreis einer bestimmten Ressource zurück
USHORT CTradeHistory::GetMinPrice(USHORT res)
{
	UINT min = 65535;
	for (int i = 0; i < m_Prices[res].GetSize(); i++)
		if (m_Prices[res].GetAt(i) < min)
			min = m_Prices[res].GetAt(i);
	return min;
}

// Funktion gibt den Maximumpreis einer bestimmten Ressource zurück. Übergeben wird die Art der Ressource
// und optional der Bereich, aus dem das Maximum ermittelt werden soll
USHORT CTradeHistory::GetMaxPrice(USHORT res, USHORT start, USHORT end)
{
	UINT max = 0;
	if (start == 0 && end == 0)
	{
		for (int i = 0; i < m_Prices[res].GetSize(); i++)
			if (m_Prices[res].GetAt(i) > max)
				max = m_Prices[res].GetAt(i);
	}
	else
	{
		for (int i = start; i <= end; i++)
			if (m_Prices[res].GetAt(i) > max)
				max = m_Prices[res].GetAt(i);
	}
	return max;
}

// Funktion gibt den Durchschnittspreis einer bestimmten Ressource zurück
USHORT CTradeHistory::GetAveragePrice(USHORT res)
{
	double price = 0.0;
	for (int i = 0; i < m_Prices[res].GetSize(); i++)
		price += m_Prices[res].GetAt(i);
	if (m_Prices[res].GetSize() > 0)
		price /= m_Prices[res].GetSize();
	return (USHORT)price;
}

// Resetfunktion für die Klasse CTradeHistory
void CTradeHistory::Reset()
{
	for (int i = TITAN; i <= IRIDIUM; i++)
	{
		for (int j = 0; j < m_Prices[i].GetSize(); )
			m_Prices[i].RemoveAt(j);
		m_Prices[i].RemoveAll();
	}
}
