#include "stdafx.h"
#include "EventFirstContact.h"
#include "FontLoader.h"
#include "Botf2Doc.h"
#include "Races\RaceController.h"

IMPLEMENT_SERIAL (CEventFirstContact, CObject, 1)

CEventFirstContact::CEventFirstContact(const CString &sPlayersRaceID, const CString &sRaceID) : m_sRaceID(sRaceID),
	CEventScreen(sPlayersRaceID, "FirstContact", "", "")
{	
}

CEventFirstContact::CEventFirstContact(void) : m_sRaceID("")
{
}

CEventFirstContact::~CEventFirstContact(void)
{
}

///////////////////////////////////////////////////////////////////////
// Speichern / Laden
///////////////////////////////////////////////////////////////////////
void CEventFirstContact::Serialize(CArchive &ar)		
{
	__super::Serialize(ar);
	
	// wenn gespeichert wird
	if (ar.IsStoring())
	{
		ar << m_sRaceID;
	}
	// wenn geladen wird
	else if (ar.IsLoading())
	{
		ar >> m_sRaceID;
	}
}

//////////////////////////////////////////////////////////////////////
// sonstige Funktionen
//////////////////////////////////////////////////////////////////////
void CEventFirstContact::Create(void)
{
	CEventScreen::Create();	
}

void CEventFirstContact::Draw(Graphics* g, CGraphicPool* graphicPool) const
{
	CEventScreen::Draw(g, graphicPool);

	if (m_sRaceID == "")
		return;

	CBotf2Doc* pDoc = ((CBotf2App*)AfxGetApp())->GetDocument();
	ASSERT(pDoc);
	CRace* pContactedRace = pDoc->GetRaceCtrl()->GetRace(m_sRaceID);
	if (!pContactedRace)
		return;

	CMajor* pMajor = dynamic_cast<CMajor*>(pDoc->GetRaceCtrl()->GetRace(m_sRace));
	ASSERT(pMajor);

	CString s;

	CString fontName = "";
	Gdiplus::REAL fontSize = 0.0;
	StringFormat fontFormat;
	SolidBrush fontBrush(Color::White);
	
	CFontLoader::CreateGDIFont(pMajor, 5, fontName, fontSize);
	fontFormat.SetAlignment(StringAlignmentCenter);
	fontFormat.SetLineAlignment(StringAlignmentCenter);
	fontFormat.SetFormatFlags(StringFormatFlagsNoWrap);
	Gdiplus::Color color;
	color.SetFromCOLORREF(pMajor->GetDesign()->m_clrNormalText);
	fontBrush.SetColor(color);

	// Überschrift zeichnen
	s = CResourceManager::GetString("FIRSTCONTACTEVENT_HEADLINE");
	g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(730,500,540,75), &fontFormat, &fontBrush);

	CFontLoader::CreateGDIFont(pMajor, 4, fontName, fontSize);
	fontFormat.SetAlignment(StringAlignmentCenter);
	fontFormat.SetLineAlignment(StringAlignmentNear);
	fontFormat.SetFormatFlags(!StringFormatFlagsNoWrap);
	s = CResourceManager::GetString("FIRSTCONTACTEVENT_TEXT", FALSE, pContactedRace->GetRaceName());
	g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(800,610,470,75), &fontFormat, &fontBrush);
	
	// Bild der Rasse zeichnen
	Bitmap* graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Races\\" + pContactedRace->GetGraphicFileName());
	if (graphic == NULL)
		graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Races\\ImageMissing.bop");
	if (graphic)
		g->DrawImage(graphic, 6, 399, 301, 300);
	// Name der Rasse zeichnen
	fontFormat.SetLineAlignment(StringAlignmentCenter);
	fontFormat.SetFormatFlags(StringFormatFlagsNoWrap);
	s = pContactedRace->GetRaceName();
	g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(5,717,304,50), &fontFormat, &fontBrush);

	// Eigenschaften zeichnen
	Gdiplus::Color markColor;
	markColor.SetFromCOLORREF(pMajor->GetDesign()->m_clrListMarkTextColor);
	fontBrush.SetColor(markColor);
	s = CResourceManager::GetString("PROPERTIES");
	g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(730,140,540,75), &fontFormat, &fontBrush);
	
	CFontLoader::CreateGDIFont(pMajor, 3, fontName, fontSize);
	fontBrush.SetColor(color);
	vector<CString> sProperties;
	if (pContactedRace->IsRaceProperty(FINANCIAL))
		sProperties.push_back(CResourceManager::GetString("FINANCIAL"));
	if (pContactedRace->IsRaceProperty(WARLIKE))
		sProperties.push_back(CResourceManager::GetString("WARLIKE"));
	if (pContactedRace->IsRaceProperty(AGRARIAN))
		sProperties.push_back(CResourceManager::GetString("AGRARIAN"));
	if (pContactedRace->IsRaceProperty(INDUSTRIAL))
		sProperties.push_back(CResourceManager::GetString("INDUSTRIAL"));
	if (pContactedRace->IsRaceProperty(SECRET))
		sProperties.push_back(CResourceManager::GetString("SECRET"));
	if (pContactedRace->IsRaceProperty(SCIENTIFIC))
		sProperties.push_back(CResourceManager::GetString("SCIENTIFIC"));
	if (pContactedRace->IsRaceProperty(PRODUCER))
		sProperties.push_back(CResourceManager::GetString("PRODUCER"));
	if (pContactedRace->IsRaceProperty(PACIFIST))
		sProperties.push_back(CResourceManager::GetString("PACIFIST"));
	if (pContactedRace->IsRaceProperty(SNEAKY))
		sProperties.push_back(CResourceManager::GetString("SNEAKY"));
	if (pContactedRace->IsRaceProperty(SOLOING))
		sProperties.push_back(CResourceManager::GetString("SOLOING"));
	if (pContactedRace->IsRaceProperty(HOSTILE))
		sProperties.push_back(CResourceManager::GetString("HOSTILE"));
	if (sProperties.size() == 0)
		sProperties.push_back(CResourceManager::GetString("NONE"));

	for (UINT i = 0; i < sProperties.size(); i++)
		g->DrawString(sProperties[i].AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(730,210 + i*30,540,30), &fontFormat, &fontBrush);

	// Technischen Fortschritt zeichnen
	if (pContactedRace->GetType() == MINOR)
	{
		CString sProgress;
		switch (((CMinor*)pContactedRace)->GetTechnologicalProgress())
		{
		case 0: sProgress = CResourceManager::GetString("VERY_UNDERDEVELOPED");	break;
		case 1: sProgress = CResourceManager::GetString("UNDERDEVELOPED");		break;
		case 2: sProgress = CResourceManager::GetString("NORMAL_DEVELOPED");	break;
		case 3: sProgress = CResourceManager::GetString("DEVELOPED");			break;
		case 4: sProgress = CResourceManager::GetString("VERY_DEVELOPED");		break;
		}
		fontFormat.SetFormatFlags(!StringFormatFlagsNoWrap);
		g->DrawString(sProgress.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(730,100,540,40), &fontFormat, &fontBrush);
		fontFormat.SetFormatFlags(StringFormatFlagsNoWrap);
	}

	// Spezialgebäude und -schiffe der Rasse zeichnen, sofern es sich um eine Minorrace handelt
	if (pContactedRace->GetType() == MINOR)
	{
		fontFormat.SetFormatFlags(!StringFormatFlagsNoWrap);
		fontFormat.SetLineAlignment(StringAlignmentNear);
		CFontLoader::CreateGDIFont(pMajor, 1, fontName, fontSize);
		graphic = NULL;
		int nCount = 0;
		// Spezialgebäude zeichnen
		for (int i = 0; i < pDoc->BuildingInfo.GetSize(); i++)
			if (pDoc->BuildingInfo[i].GetOwnerOfBuilding() == NOBODY)
				if (pDoc->BuildingInfo[i].GetOnlyMinorRace() == TRUE)
					if (pDoc->BuildingInfo[i].GetOnlyInSystemWithName() == pContactedRace->GetHomesystemName())
					{
						graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Buildings\\" + pDoc->BuildingInfo[i].GetGraphikFileName());
						if (graphic == NULL)
							graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Buildings\\ImageMissing.bop");
						if (graphic)
							g->DrawImage(graphic, 15 + nCount * 165, 810, 150, 113);
						s = pDoc->BuildingInfo[i].GetBuildingName();
						g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(nCount * 165, 935, 180, 50), &fontFormat, &fontBrush);
						nCount++;
					}
		// Schiffe des Minors zeichnen
		for (int i = 0; i < pDoc->m_ShipInfoArray.GetSize(); i++)
			if (pDoc->m_ShipInfoArray[i].GetRace() == MINORNUMBER)
				if (pDoc->m_ShipInfoArray[i].GetOnlyInSystem() == pContactedRace->GetHomesystemName())
				{
					graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Ships\\" + pDoc->m_ShipInfoArray[i].GetShipClass() + ".bop");
					if (graphic == NULL)
						graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Ships\\ImageMissing.bop");
					if (graphic)
						g->DrawImage(graphic, 15 + nCount * 165, 810, 150, 113);
					s = pDoc->m_ShipInfoArray[i].GetShipClass() + "-" + CResourceManager::GetString("CLASS") + " (" + pDoc->m_ShipInfoArray[i].GetShipTypeAsString() + ")";
					g->DrawString(s.AllocSysString(), -1, &Gdiplus::Font(fontName.AllocSysString(), fontSize), RectF(nCount * 165, 935, 180, 50), &fontFormat, &fontBrush);
					nCount++;
				}		
	}

	// Buttons zeichnen
	CFontLoader::CreateGDIFont(pMajor, 3, fontName, fontSize);
	CFontLoader::GetGDIFontColor(pMajor, 2, color);
	fontBrush.SetColor(color);
	for (int i = 0; i < m_Buttons.GetSize(); i++)
		m_Buttons.GetAt(i)->DrawButton(*g, graphicPool, Gdiplus::Font(fontName.AllocSysString(), fontSize), fontBrush);
}