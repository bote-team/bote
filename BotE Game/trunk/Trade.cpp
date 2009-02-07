#include "stdafx.h"
#include "Trade.h"

IMPLEMENT_SERIAL (CTrade, CObject, 1)
// statische Variable initialisieren
USHORT CTrade::m_iMonopolOwner[] = {0,0,0,0,0};	// überall NOBODY eintragen

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////
CTrade::CTrade(void)
{
	// Standardpreise/Startpreise für die Ressourcen festlegen (erstmal genauso, wie wieviel auch die Ressourcen kosten
	// würden, wenn wir einen Bauauftrag kaufen)
	for (int i = TITAN; i <= IRIDIUM; i++)
	{
		switch(i)
		{
		case TITAN:		m_iRessourcePrice[i] = 800; break;
		case DEUTERIUM: m_iRessourcePrice[i] = 500; break;
		case DURANIUM:	m_iRessourcePrice[i] = 1000;break;
		case CRYSTAL:	m_iRessourcePrice[i] = 2000;break;
		case IRIDIUM:	m_iRessourcePrice[i] = 3000;break;
		}
		m_iRessourcePriceAtRoundStart[i] = m_iRessourcePrice[i];
		m_iTaxes[i] = 0;
		m_dMonopolBuy[i] = 0.0f;
	}
	m_iQuantity = 100;
	m_iRace		= NOBODY;
	m_fTax		= 1.0f;
}

CTrade::~CTrade(void)
{
	for (int i = 0; i < m_TradeActions.GetSize(); )
		m_TradeActions.RemoveAt(i);
	m_TradeActions.RemoveAll();
}

///////////////////////////////////////////////////////////////////////
// Speichern / Laden
///////////////////////////////////////////////////////////////////////
void CTrade::Serialize(CArchive &ar)		
{
	CObject::Serialize(ar);
	// wenn gespeichert wird
	if (ar.IsStoring())
	{
		ar << m_iQuantity;
		ar << m_iRace;
		ar << m_fTax;
		for (int i = 0; i < 5; i++)
		{
			ar << m_iRessourcePrice[i];
			ar << m_iRessourcePriceAtRoundStart[i];
			ar << m_iTaxes[i];
			ar << m_dMonopolBuy[i];
		}
		ar << m_TradeActions.GetSize();
		for (int i = 0; i < m_TradeActions.GetSize(); i++)
		{
			ar << m_TradeActions.GetAt(i).number;
			ar << m_TradeActions.GetAt(i).price;
			ar << m_TradeActions.GetAt(i).res;
			ar << m_TradeActions.GetAt(i).system;
		}
	}
	// wenn geladen wird
	if (ar.IsLoading())
	{
		int number = 0;
		ar >> m_iQuantity;
		ar >> m_iRace;
		ar >> m_fTax;
		for (int i = 0; i < 5; i++)
		{
			ar >> m_iRessourcePrice[i];
			ar >> m_iRessourcePriceAtRoundStart[i];
			ar >> m_iTaxes[i];
			ar >> m_dMonopolBuy[i];
		}
		ar >> number;
		m_TradeStruct ts;
		m_TradeActions.RemoveAll();
		for (int i = 0; i < number; i++)
		{
			ar >> ts.number;
			ar >> ts.price;
			ar >> ts.res;
			ar >> ts.system;
			m_TradeActions.Add(ts);
		}
	}
}

//////////////////////////////////////////////////////////////////////
// sonstige Funktionen
//////////////////////////////////////////////////////////////////////

// Funktion kauft die Anzahl der jeweiligen Ressource für das System und fügt den Auftrag in das Array
// m_TradeActions ein. Danach berechnet sie den Preis der Ressource nach dem Kauf. Steuern
// werden hier noch nicht in den Preis mit einbezogen.
int CTrade::BuyRessource(USHORT res, ULONG number, CPoint system, long empires_latinum, BOOL flag)
{
	USHORT oldResPrice = m_iRessourcePrice[res];
	m_TradeStruct ts;
	ts.res		= res;
	ts.number	= number;
	ts.system   = system;
	ts.price	= 0;
	// Alle 100 gekauften Einheiten erhöht sich der Preis an unserer Börse
	for (int i = 0; i < ceil((float)(number / 100)); i++)
	{
		// Wenn flag == 1, dann den Ressourcenpreis von Beginn der Runde nehmen und verändern, weil wenn Bauaufträge
		// gekauft werden sollen diese Kosten nicht abhängig von den aktuellen Preisen an der Börse sein, sondern
		// abhängig von den Preisen, wie sie zu Rundenbeginn waren. Mehrfaches Kaufen erhöht aber trotzdem die Kaufkosten.
		if (flag == 1)
		{
			ts.price	+= (int)((m_iRessourcePriceAtRoundStart[res] * number) / 1000);
			m_iRessourcePriceAtRoundStart[res] += ((res+1)*5);
		}
		else
			ts.price	+= (int)((m_iRessourcePrice[res] * number) / 1000);
		// Preiserhöhung pro 100 Einheiten ist (1+Ressourcennummer) * 5
		m_iRessourcePrice[res] += ((res+1)*5);
	}
	if ((int)ceil((float)(number / 100)) != 0)
		ts.price /=  (int)ceil((float)(number / 100));
	// Nur bei flag == 0 wird die Aktion ins Feld geschrieben, sonst werden nur die neuen Preise an der Börse berechnet
	// und die Steuern in das Feld m_iTaxes geschrieben
	if (flag == 1)
	{
		m_iTaxes[res] += (int)(ts.price * m_fTax) - ts.price;
		return 0;
	}
	// Falls der Preis bei NULL liegt setzten wir den auf 1 (kostenlos gibts hier nix ;-) )
	if (ts.price == NULL)
		ts.price = 1;
	// Jetzt überprüfen ob wir das Latinum auch aufbringen können, wenn wir etwas kaufen, wenn es nicht klappt, dann
	// geben wir eine NULL zurück und setzen den Preis der Ressource wieder auf den alten
	if ((int)ceil(ts.price * m_fTax) > empires_latinum)
	{
		m_iRessourcePrice[res] = oldResPrice;
		return 0;
	}
	m_TradeActions.Add(ts);
	return (int)ceil(ts.price * m_fTax);
}

// Funktion verkauft die Anzahl der jeweiligen Ressource aus dem System und fügt den Auftrag in das Array
// m_TradeActions ein. Danach berechnet sie den Preis der Ressource nach dem Verkauf. Steuern
// werden hier noch nicht in den Preis mit einbezogen.
void CTrade::SellRessource(USHORT res, ULONG number, CPoint system, BOOL flag)
{
	m_TradeStruct ts;
	ts.res		= res;
	ts.number	= number;
	ts.system   = system;
	ts.price	= 0;
	// Alle 100 verkauften Einheiten verringert sich der Preis an unserer Börse
	for (int i = 0; i < ceil((float)(number / 100)); i++)
	{
		// Wenn flag == 1, dann den Ressourcenpreis von Beginn der Runde nehmen und verändern, weil wenn Bauaufträge
		// gekauft werden sollen diese Kosten nicht abhängig von den aktuellen Preisen an der Börse sein, sondern
		// abhängig von den Preisen, wie sie zu Rundenbeginn waren. Mehrfaches Kaufen erhöht aber trotzdem die Kaufkosten.
		if (flag == 1)
		{
			ts.price	-= (int)((m_iRessourcePriceAtRoundStart[res] * number) / 1000);
			// Preisverringerung pro 100 Einheiten ist (1+Ressourcennummer) * 10
			if ((res+1)*10 > m_iRessourcePriceAtRoundStart[res])
				m_iRessourcePriceAtRoundStart[res] = 1;
			else
				m_iRessourcePriceAtRoundStart[res] -= ((res+1)*10);
		}
		else
			ts.price	-= (int)((m_iRessourcePrice[res] * number) / 1000);
		// Preisverringerung pro 100 Einheiten ist (1+Ressourcennummer) * 10
		if ((res+1)*10 > m_iRessourcePrice[res])
			m_iRessourcePrice[res] = 1;
		else
			m_iRessourcePrice[res] -= ((res+1)*10);
	}
	if ((int)ceil((float)(number / 100)) != 0)
		ts.price /=  (int)ceil((float)(number / 100));
	// Nur bei flag == 0 wird die Aktion ins Feld geschrieben, sonst werden nur die neuen Preise an der Börse berechnet
	// und die Steuern in das Feld m_iTaxes geschrieben
	if (flag == 1)
	{
		m_iTaxes[res] += (int)(ts.price * m_fTax) - ts.price;
		return;
	}	
	// Falls der Preis bei NULL liegt setzten wir den auf 1 (kostenlos gibts hier nix ;-) )
	if (ts.price == NULL)
		ts.price = -1;
	m_TradeActions.Add(ts);
}

// Funktion berechnet die ganzen Handelsaktionen, lagert also Ressourcen ein oder gibt das Latinum, welches
// wir durch den Verkauf bekommen haben an das jeweilige Imperium
void CTrade::CalculateTradeActions(CEmpire* empires, CSystem systems[][STARMAP_SECTORS_VCOUNT], CSector sectors[][STARMAP_SECTORS_VCOUNT], CMajorRace* majors, USHORT* taxes)
{
	int sum[STARMAP_SECTORS_HCOUNT][STARMAP_SECTORS_VCOUNT][5] = {0};
	BOOLEAN didSome = FALSE;
	for (int i = 0; i < m_TradeActions.GetSize(); )
	{
		CPoint KO  = m_TradeActions.GetAt(i).system;
		USHORT res = m_TradeActions.GetAt(i).res;
		// Die Ressource in dem System lagern, wenn wir diese gekauft haben
		if (m_TradeActions.GetAt(i).price > 0)
		{
			systems[KO.x][KO.y].SetRessourceStore(res,m_TradeActions.GetAt(i).number);
			sum[KO.x][KO.y][res] += m_TradeActions.GetAt(i).number;
			didSome = TRUE;
		}
		// Das Latinum was wir bekommen dem Imperium geben
		else
			empires[m_iRace].SetLatinum(-m_TradeActions.GetAt(i).price);
		
		// Hier die Monopole beachten, wenn jemand ein Monopol auf die Ressource hat und dieser jemand mit an unserer
		// Handelsbörse aktiv ist, dann bekommt dieser die Steuern (auch wir selbst bekommen unsere Steuern zurück!)
		if (CTrade::m_iMonopolOwner[res] != NOBODY)
			if (CTrade::m_iMonopolOwner[res] == m_iRace || majors[m_iRace].GetKnownMajorRace((byte)CTrade::m_iMonopolOwner[res]) == TRUE)
				taxes[res] += (USHORT)ceil(abs((m_TradeActions.GetAt(i).price * m_fTax - m_TradeActions.GetAt(i).price)));
		// Auftrag nach Bearbeitung entfernen
		m_TradeActions.RemoveAt(i);
	}
	// Nachricht generieren, dass die entsprechende Menge der Ressourcen in dem System eingetroffen sind
	if (didSome)
		for (int x = 0; x < STARMAP_SECTORS_HCOUNT; x++)
			for (int y = 0; y < STARMAP_SECTORS_VCOUNT; y++)
				for (int i = TITAN; i <= IRIDIUM; i++)
					if (sum[x][y][i] > 0)
					{
						CString resName;
						switch(i)
						{
						case TITAN: resName = CResourceManager::GetString("TITAN"); break;
						case DEUTERIUM: resName = CResourceManager::GetString("DEUTERIUM"); break;
						case DURANIUM: resName = CResourceManager::GetString("DURANIUM"); break;
						case CRYSTAL: resName = CResourceManager::GetString("CRYSTAL"); break;
						case IRIDIUM: resName = CResourceManager::GetString("IRIDIUM"); break;
						}			
						CString s;
						s.Format("%d %s",sum[x][y][i],resName);
						CMessage message;
						message.GenerateMessage(CResourceManager::GetString("GET_RESOURCES",0,s,sectors[x][y].GetName()), m_iRace, ECONOMY, "", sectors[x][y].GetKO(), FALSE);
						empires[m_iRace].AddMessage(message);
					}
}

// Funktion berechnet den Preis der Ressourcen in Zusammenhang zu den anderen Börsen. 
// Übergeben wird eine Matrix mit allen Preisen sowie die Hauptrassen.
void CTrade::CalculatePrices(USHORT oldPrices[][5], CMajorRace* majors)
{
	// Der Preis einer anderen Börse kann natürlich nur miteingerechnet werden, wenn wir diese Hauptrasse
	// kennen. Deswegen haben wir das CMajorRace-Array übergeben.
	float newPrices[5];
	for (int j = TITAN; j <= IRIDIUM; j++)
	{
		newPrices[j] = oldPrices[m_iRace][j];	// mit eigenem Preis initialisieren
		USHORT count = 1;						// Zählvariable
		for (int i = HUMAN; i <= DOMINION; i++)	
			if (i != m_iRace && majors[m_iRace].GetKnownMajorRace(i) == TRUE)
			{
				newPrices[j] += oldPrices[i][j];
				count++;				
			}
		newPrices[j] /= count;
		m_iRessourcePrice[j] = (USHORT)ceil(newPrices[j]);
		m_iRessourcePriceAtRoundStart[j] = m_iRessourcePrice[j];
		m_iTaxes[j] = 0;
	}
}

// Resetfunktion für die Klasse CTrade
void CTrade::Reset(void)
{
	// Standardpreise/Startpreise für die Ressourcen festlegen (erstmal genauso, wie wieviel auch die Ressourcen kosten
	// würden, wenn wir einen Bauauftrag kaufen)
	for (int i = TITAN; i <= IRIDIUM; i++)
	{
		switch(i)
		{
		case TITAN:		m_iRessourcePrice[i] = 800; break;
		case DEUTERIUM: m_iRessourcePrice[i] = 500; break;
		case DURANIUM:	m_iRessourcePrice[i] = 1000;break;
		case CRYSTAL:	m_iRessourcePrice[i] = 2000;break;
		case IRIDIUM:	m_iRessourcePrice[i] = 3000;break;
		}
		m_iRessourcePriceAtRoundStart[i] = m_iRessourcePrice[i];
		m_iTaxes[i] = 0;
		m_dMonopolBuy[i] = 0.0f;
		// statische Variable reseten
		CTrade::m_iMonopolOwner[i] = 0;		
	}
	m_iQuantity = 100;
	m_iRace		= NOBODY;
	m_fTax		= 1.0f;
	for (int i = 0; i < m_TradeActions.GetSize(); )
		m_TradeActions.RemoveAt(i);
	m_TradeActions.RemoveAll();
}