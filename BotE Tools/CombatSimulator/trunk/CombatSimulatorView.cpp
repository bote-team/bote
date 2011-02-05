// CombatSimulatorView.cpp : Implementierung der Klasse CCombatSimulatorView
//

#include "stdafx.h"
#include "CombatSimulator.h"

#include "CombatSimulatorDoc.h"
#include "CombatSimulatorView.h"

#include "memdc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CCombatSimulatorView

IMPLEMENT_DYNCREATE(CCombatSimulatorView, CView)

BEGIN_MESSAGE_MAP(CCombatSimulatorView, CView)
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

// CCombatSimulatorView-Erstellung/Zerstörung

CCombatSimulatorView::CCombatSimulatorView()
{
	// TODO: Hier Code zur Konstruktion einfügen

}

CCombatSimulatorView::~CCombatSimulatorView()
{
}

BOOL CCombatSimulatorView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Ändern Sie hier die Fensterklasse oder die Darstellung, indem Sie
	//  CREATESTRUCT cs modifizieren.

	return CView::PreCreateWindow(cs);
}

// CCombatSimulatorView-Zeichnung
void CCombatSimulatorView::OnDraw(CDC* dc)
{
	CCombatSimulatorDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;
	// TODO: Code zum Zeichnen der systemeigenen Daten hinzufügen
	CMemDC pDC(dc);
	
	CFont font;
	LOGFONT lf;
	memset(&lf, 0, sizeof(LOGFONT)); 
	
	strcpy_s(lf.lfFaceName, "Arial");
	
	lf.lfHeight = 15;
	
	lf.lfQuality = CLEARTYPE_QUALITY;
	lf.lfPitchAndFamily = FF_SWISS;
		
	font.CreateFontIndirect(&lf);
	pDC->SelectObject(&font);
	pDC->FillRect(r,&CBrush(RGB(0,0,0)));

	bool bExit = true;

	if (pDoc->repeat == 0)
	{
	// ***************************** TEST DES KAMPFMODUSES ZEICHNEN **********************************
		SetFocus();
		GetClientRect(r);
		pDC->SetBkMode(TRANSPARENT);
		// Die Mitte ist (0,0,0)
		pDC->SetTextColor(RGB(250,250,250));
		
		CPen phaser(PS_SOLID,0,RGB(255,0,0));
		CPen pulse(PS_DOT,0,RGB(255,0,0));

		if (pDoc->combat.m_bReady)
		{
			bExit = false;

			pDoc->combat.CalculateCombat(winner);
			CString s;
			s.Format("TICK: %d", pDoc->combat.m_iTime);
			pDC->DrawText(s, r, DT_RIGHT);
			if (xy_ebene)
				s.Format("current level: XY");
			else
				s.Format("current level: XZ");
			pDC->DrawText(s, r, DT_TOP | DT_CENTER);
			for (int q = 0; q < pDoc->combat.m_CS.GetSize(); q++)
			{
				//CBrush nb = CBrush(RGB(0,0,0));
				CBrush nb = CBrush(color[pDoc->combat.m_CS.GetAt(q)->m_pShip->GetOwnerOfShip()]);
				pDC->SelectObject(&nb);
				//CPen black(PS_SOLID,0,RGB(0,0,0));
				CPen black(PS_SOLID,0,color[pDoc->combat.m_CS.GetAt(q)->m_pShip->GetOwnerOfShip()]);
				if (xy_ebene == TRUE)
				{
					pDC->SelectObject(&black);					
					pDC->SetTextColor(color[pDoc->combat.m_CS.GetAt(q)->m_pShip->GetOwnerOfShip()]);
					s.Format("%s", pDoc->combat.m_CS.GetAt(q)->m_pShip->GetShipClass());
					if (pDoc->combat.m_CS.GetAt(q)->m_byCloak > 0)
						s += "(c)";
					CString life;
					life.Format(" %.0lf%%/%.0lf%%", pDoc->combat.m_CS.GetAt(q)->SHIELDS, pDoc->combat.m_CS.GetAt(q)->LIFE);
					s += life;
					pDC->TextOut(pDoc->combat.m_CS.GetAt(q)->m_KO.x+r.right/2 - 20, pDoc->combat.m_CS.GetAt(q)->m_KO.y+r.bottom/2 - 15, s);
					
					s.Format("%.0lf°", pDoc->combat.m_CS.GetAt(q)->SCAL);
					pDC->TextOut(pDoc->combat.m_CS.GetAt(q)->m_KO.x+r.right/2 - 5, pDoc->combat.m_CS.GetAt(q)->m_KO.y+r.bottom/2 + 5, s);
					
					pDC->Ellipse(pDoc->combat.m_CS.GetAt(q)->m_KO.x+r.right/2,pDoc->combat.m_CS.GetAt(q)->m_KO.y+r.bottom/2,
						pDoc->combat.m_CS.GetAt(q)->m_KO.x+6+r.right/2,pDoc->combat.m_CS.GetAt(q)->m_KO.y+6+r.bottom/2);
					// Phaserfeuer zeichnen
					pDC->SelectObject(&phaser);
					if (pDoc->combat.m_CS.GetAt(q)->m_Fire.phaserIsShooting == TRUE)
					{
						// bei Pulsebeams die gestrichelte Linie zeichnen
						if (pDoc->combat.m_CS.GetAt(q)->m_bPulseFire)
							pDC->SelectObject(&pulse);
						pDC->MoveTo(pDoc->combat.m_CS.GetAt(q)->m_KO.x+r.right/2+3,pDoc->combat.m_CS.GetAt(q)->m_KO.y+r.bottom/2+3);
						pDC->LineTo(pDoc->combat.m_CS.GetAt(q)->m_pTarget->m_KO.x+r.right/2+3,pDoc->combat.m_CS.GetAt(q)->m_pTarget->m_KO.y+r.bottom/2+3);
					}
					black.DeleteObject();
				}
				else
				{
					pDC->SelectObject(&black);
					pDC->SetTextColor(color[pDoc->combat.m_CS.GetAt(q)->m_pShip->GetOwnerOfShip()]);
					s.Format("%s", pDoc->combat.m_CS.GetAt(q)->m_pShip->GetShipClass());
					if (pDoc->combat.m_CS.GetAt(q)->m_byCloak > 0)
						s += "(c)";
					CString life;
					life.Format(" %.0lf%%/%.0lf%%", pDoc->combat.m_CS.GetAt(q)->SHIELDS, pDoc->combat.m_CS.GetAt(q)->LIFE);
					s += life;
					pDC->TextOut(pDoc->combat.m_CS.GetAt(q)->m_KO.x+r.right/2 - 20, pDoc->combat.m_CS.GetAt(q)->m_KO.z+r.bottom/2 - 15, s);

					s.Format("%.0lf°", pDoc->combat.m_CS.GetAt(q)->SCAL);
					pDC->TextOut(pDoc->combat.m_CS.GetAt(q)->m_KO.x+r.right/2 - 5, pDoc->combat.m_CS.GetAt(q)->m_KO.z+r.bottom/2 + 5, s);					

					pDC->Ellipse(pDoc->combat.m_CS.GetAt(q)->m_KO.x+r.right/2,pDoc->combat.m_CS.GetAt(q)->m_KO.z+r.bottom/2,
								pDoc->combat.m_CS.GetAt(q)->m_KO.x+4+r.right/2,pDoc->combat.m_CS.GetAt(q)->m_KO.z+4+r.bottom/2);
					// Phaserfeuer zeichnen
					pDC->SelectObject(&phaser);
					if (pDoc->combat.m_CS.GetAt(q)->m_Fire.phaserIsShooting == TRUE)
					{
						// bei Pulsebeams die gestrichelte Linie zeichnen
						if (pDoc->combat.m_CS.GetAt(q)->m_bPulseFire)
							pDC->SelectObject(&pulse);
						pDC->MoveTo(pDoc->combat.m_CS.GetAt(q)->m_KO.x+r.right/2+3,pDoc->combat.m_CS.GetAt(q)->m_KO.z+r.bottom/2+3);
						pDC->LineTo(pDoc->combat.m_CS.GetAt(q)->m_pTarget->m_KO.x+r.right/2+3,pDoc->combat.m_CS.GetAt(q)->m_pTarget->m_KO.z+r.bottom/2+3);
					}
					black.DeleteObject();
				}
				nb.DeleteObject();
			}
			
			// Torpedos zeichnen
			for (list<CTorpedo*>::const_iterator it = pDoc->combat.m_CT.begin(); it != pDoc->combat.m_CT.end(); ++it)
			{
				pDC->SelectObject(&phaser);
				if (xy_ebene == TRUE)
					pDC->Rectangle((*it)->m_KO.x+r.right/2, (*it)->m_KO.y+r.bottom/2,(*it)->m_KO.x+4+r.right/2,(*it)->m_KO.y+4+r.bottom/2);
				else
					pDC->Rectangle((*it)->m_KO.x+r.right/2,(*it)->m_KO.z+r.bottom/2,(*it)->m_KO.x+4+r.right/2,(*it)->m_KO.z+4+r.bottom/2);
				phaser.DeleteObject();
			}
			
			counter++;
			Sleep(pDoc->SlowingSpeed);
			if (counter%200 == 0)
				xy_ebene = !xy_ebene;
		};
		// ************** TEST DES KAMPFMODUSES ZEICHNEN ist hier zu Ende **************
		if (bExit)
			return;

		if (!pDoc->combat.m_bReady)
		{
			bExit = true;
			for (map<CString, BYTE>::const_iterator it = winner.begin(); it != winner.end(); ++it)
				if (it->second == 1)
					AfxMessageBox("Race " + it->first + " wins the combat");

			// Das Log-File über die vernichteten Schiffe anlegen
			CString fileName="Combat.log";														// Name des zu Öffnenden Files 
			CStdioFile file;																	// Varibale vom Typ CStdioFile
			if (file.Open(fileName, CFile::modeCreate | CFile::modeWrite | CFile::typeText))	// Datei wird geöffnet
			{
				file.WriteString("Birth of the Empires - Combat Log file\n\n");
				for (set<CString>::const_iterator it = pDoc->combat.m_mInvolvedRaces.begin(); it != pDoc->combat.m_mInvolvedRaces.end(); ++it)
				{
					CString s;
					s.Format("----------------------------------------\nlost ships of race: %s\n", *it);
					file.WriteString(s);
					for (int j = 0; j < pDoc->m_ShipArray.GetSize(); j++)
						if (pDoc->m_ShipArray.GetAt(j).GetOwnerOfShip() == *it)
							if (pDoc->m_ShipArray.GetAt(j).GetHull()->GetCurrentHull() == 0)
								file.WriteString(pDoc->m_ShipArray.GetAt(j).GetShipClass()+"\n");
				}
			}
			file.Close();
			return;
		}
		else
			Invalidate(FALSE);
	}
	else
	{
		GetClientRect(r);
		pDC->FillRect(r,&CBrush(RGB(0,0,0)));
		// Das Log-File über die Statistiken anlegen
		CString fileName="Stats.log";														// Name des zu Öffnenden Files 
		CStdioFile file;																	// Varibale vom Typ CStdioFile
		if (file.Open(fileName, CFile::modeCreate | CFile::modeWrite | CFile::typeText))	// Datei wird geöffnet
		{
			CString s;
			file.WriteString("Birth of the Empires - Stats Log file\n\n");
			s.Format("overall combats: %d\n",pDoc->repeat);
			file.WriteString(s);
			for (map<CString, int>::const_iterator it = pDoc->wins.begin(); it != pDoc->wins.end(); ++it)
			{
				s.Format("----------------------------------------\nrace: %s wins %d combats", it->first, it->second);
				file.WriteString(s);
				s.Format(" -> %d%%\n", it->second * 100 / pDoc->repeat);
				file.WriteString(s);				
			}
		}
		file.Close();
		AfxMessageBox("Generating statsfile... ready\n\nClose this application or run a new one");
	}

	font.DeleteObject();
// CCombatSimulatorView-Diagnose
}

#ifdef _DEBUG
void CCombatSimulatorView::AssertValid() const
{
	CView::AssertValid();
}

void CCombatSimulatorView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CCombatSimulatorDoc* CCombatSimulatorView::GetDocument() const // Nicht-Debugversion ist inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CCombatSimulatorDoc)));
	return (CCombatSimulatorDoc*)m_pDocument;
}
#endif //_DEBUG


// CCombatSimulatorView-Meldungshandler

void CCombatSimulatorView::OnInitialUpdate()
{
//	CCombatSimulatorView* pDoc = (CCombatSimulatorView*)GetDocument();
	CView::OnInitialUpdate();

	// TODO: Fügen Sie hier Ihren spezialisierten Code ein, und/oder rufen Sie die Basisklasse auf.
	counter = 0;
	xy_ebene = TRUE;

	color[""]		= RGB(123,123,123);
	color["MAJOR1"]	= RGB(0,0,255);
	color["MAJOR2"]	= RGB(248,211,5);
	color["MAJOR3"]	= RGB(255,0,0);
	color["MAJOR4"]	= RGB(0,140,0);
	color["MAJOR5"] = RGB(125,0,125);
	color["MAJOR6"]	= RGB(73,240,240);
}

BOOL CCombatSimulatorView::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default

	return FALSE;
}
