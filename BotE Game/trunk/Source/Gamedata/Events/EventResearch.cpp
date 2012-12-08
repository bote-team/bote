#include "stdafx.h"
#include "EventResearch.h"
#include "FontLoader.h"
#include "botf2.h"
#include "Botf2Doc.h"
#include "Races\RaceController.h"
#include "HTMLStringBuilder.h"
#include "General/ResourceManager.h"

IMPLEMENT_SERIAL (CEventResearch, CObject, 1)

CEventResearch::CEventResearch(const CString &sPlayersRaceID, const CString &headline, BYTE byTech) : m_byTech(byTech),
	CEventScreen(sPlayersRaceID, "Research", headline, ""),
	_graphic(),
	m_byTechlevel(0)
{
}

CEventResearch::CEventResearch(void) :
	m_byTech(0),
	_graphic(),
	m_byTechlevel(0)
{
}

CEventResearch::~CEventResearch(void)
{
}

///////////////////////////////////////////////////////////////////////
// Speichern / Laden
///////////////////////////////////////////////////////////////////////
void CEventResearch::Serialize(CArchive &ar)
{
	__super::Serialize(ar);

	// wenn gespeichert wird
	if (ar.IsStoring())
	{
		ar << m_byTech;
	}
	// wenn geladen wird
	else if (ar.IsLoading())
	{
		ar >> m_byTech;
	}
}

//////////////////////////////////////////////////////////////////////
// sonstige Funktionen
//////////////////////////////////////////////////////////////////////
void CEventResearch::Create(void)
{
	CEventScreen::Create();

	CBotf2Doc* pDoc = resources::pDoc;
	ASSERT(pDoc);

	CMajor* pMajor = dynamic_cast<CMajor*>(pDoc->GetRaceCtrl()->GetRace(m_sRace));
	ASSERT(pMajor);

	// Bild der Forschungstech laden
	_graphic = NULL;
	switch (m_byTech)
	{
	case 0:
		_graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Research\\biotech.bop");
		m_byTechlevel = pMajor->GetEmpire()->GetResearch()->GetBioTech();
		break;
	case 1:
		_graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Research\\energytech.bop");
		m_byTechlevel = pMajor->GetEmpire()->GetResearch()->GetEnergyTech();
		break;
	case 2:
		_graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Research\\computertech.bop");
		m_byTechlevel = pMajor->GetEmpire()->GetResearch()->GetCompTech();
		break;
	case 3:
		_graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Research\\propulsiontech.bop");
		m_byTechlevel = pMajor->GetEmpire()->GetResearch()->GetPropulsionTech();
		break;
	case 4:
		_graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Research\\constructiontech.bop");
		m_byTechlevel = pMajor->GetEmpire()->GetResearch()->GetConstructionTech();
		break;
	case 5:
		_graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Research\\weapontech.bop");
		m_byTechlevel = pMajor->GetEmpire()->GetResearch()->GetWeaponTech();
		break;
	}

	// mit dieser Forschung nun baubare Gebäude zeichnen
	BYTE researchLevels[6] =
		{
			pMajor->GetEmpire()->GetResearch()->GetBioTech(),
			pMajor->GetEmpire()->GetResearch()->GetEnergyTech(),
			pMajor->GetEmpire()->GetResearch()->GetCompTech(),
			pMajor->GetEmpire()->GetResearch()->GetPropulsionTech(),
			pMajor->GetEmpire()->GetResearch()->GetConstructionTech(),
			pMajor->GetEmpire()->GetResearch()->GetWeaponTech()
		};

	m_vNewBuildings.clear();
	m_vNewUpgrades.clear();
	m_vNewShips.clear();
	m_vNewTroops.clear();

	for (int i = 0; i < pDoc->BuildingInfo.GetSize(); i++)
		if (pDoc->BuildingInfo[i].GetOwnerOfBuilding() == pMajor->GetRaceBuildingNumber())
			if (pDoc->BuildingInfo[i].IsBuildingBuildableNow(researchLevels))
			{
				int nTechLevel = -1;
				switch (m_byTech)
				{
				case 0: nTechLevel = pDoc->BuildingInfo[i].GetBioTech();			break;
				case 1: nTechLevel = pDoc->BuildingInfo[i].GetEnergyTech();			break;
				case 2: nTechLevel = pDoc->BuildingInfo[i].GetCompTech();			break;
				case 3: nTechLevel = pDoc->BuildingInfo[i].GetPropulsionTech();		break;
				case 4: nTechLevel = pDoc->BuildingInfo[i].GetConstructionTech();	break;
				case 5: nTechLevel = pDoc->BuildingInfo[i].GetWeaponTech();			break;
				}
				if (nTechLevel != -1 && nTechLevel == m_byTechlevel)
				{
					if (pDoc->BuildingInfo[i].GetPredecessorID() == 0)
						m_vNewBuildings.push_back(&pDoc->BuildingInfo[i]);
					else
						m_vNewUpgrades.push_back(&pDoc->BuildingInfo[i]);
				}
			}
	for (int i = 0; i < pDoc->m_ShipInfoArray.GetSize(); i++)
		if (pDoc->m_ShipInfoArray[i].GetRace() == pMajor->GetRaceShipNumber())
			if (pDoc->m_ShipInfoArray[i].IsThisShipBuildableNow(researchLevels))
			{
				int nTechLevel = -1;
				switch (m_byTech)
				{
				case 0: nTechLevel = pDoc->m_ShipInfoArray[i].GetBioTech();				break;
				case 1: nTechLevel = pDoc->m_ShipInfoArray[i].GetEnergyTech();			break;
				case 2: nTechLevel = pDoc->m_ShipInfoArray[i].GetComputerTech();		break;
				case 3: nTechLevel = pDoc->m_ShipInfoArray[i].GetPropulsionTech();		break;
				case 4: nTechLevel = pDoc->m_ShipInfoArray[i].GetConstructionTech();	break;
				case 5: nTechLevel = pDoc->m_ShipInfoArray[i].GetWeaponTech();			break;
				}
				if (nTechLevel != -1 && nTechLevel == m_byTechlevel)
					m_vNewShips.push_back(&pDoc->m_ShipInfoArray[i]);
			}
	for (int i = 0; i < pDoc->m_TroopInfo.GetSize(); i++)
		if (pDoc->m_TroopInfo[i].GetOwner() == pMajor->GetRaceID())
			if (pDoc->m_TroopInfo[i].IsThisTroopBuildableNow(researchLevels))
			{
				int nTechLevel = pDoc->m_TroopInfo[i].GetNeededTechlevel(m_byTech);
				if (nTechLevel != -1 && nTechLevel == m_byTechlevel)
					m_vNewTroops.push_back(&pDoc->m_TroopInfo[i]);
			}

}

void CEventResearch::Draw(Graphics* g, CGraphicPool* graphicPool) const
{
	CEventScreen::Draw(g, graphicPool);

	CString fontName = "";
	Gdiplus::REAL fontSize = 0.0;
	StringFormat fontFormat;
	SolidBrush fontBrush(Color::White);

	CBotf2Doc* pDoc = resources::pDoc;
	ASSERT(pDoc);

	CMajor* pMajor = dynamic_cast<CMajor*>(pDoc->GetRaceCtrl()->GetRace(m_sRace));
	ASSERT(pMajor);

	CString s;
	CString sTechName;
	CString sTechDesc;
	CResearchInfo::GetTechInfos(m_byTech, m_byTechlevel, sTechName, sTechDesc);

	CFontLoader::CreateGDIFont(pMajor, 5, fontName, fontSize);
	fontFormat.SetAlignment(StringAlignmentCenter);
	fontFormat.SetLineAlignment(StringAlignmentCenter);
	fontFormat.SetFormatFlags(StringFormatFlagsNoWrap);
	Gdiplus::Color color;
	color.SetFromCOLORREF(pMajor->GetDesign()->m_clrNormalText);
	fontBrush.SetColor(color);

	// Überschrift zeichnen
	g->DrawString(CComBSTR(m_strHeadline), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(0,0,1280,48), &fontFormat, &fontBrush);

	CFontLoader::CreateGDIFont(pMajor, 3, fontName, fontSize);
	fontFormat.SetAlignment(StringAlignmentCenter);
	fontFormat.SetLineAlignment(StringAlignmentCenter);
	fontFormat.SetFormatFlags(!StringFormatFlagsNoWrap);
	// Forschungsbeschreibung zeichnen
	g->DrawString(CComBSTR(sTechDesc), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(185,275,910,125), &fontFormat, &fontBrush);

	Gdiplus::Color markColor;
	markColor.SetFromCOLORREF(pMajor->GetDesign()->m_clrListMarkTextColor);
	fontBrush.SetColor(markColor);
	fontFormat.SetFormatFlags(StringFormatFlagsNoWrap);
	// Forschungsname zeichnen
	g->DrawString(CComBSTR(sTechName), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(0,65,1280,35), &fontFormat, &fontBrush);

	// Bild der Forschungstech zeichnen
	if (_graphic)
		g->DrawImage(_graphic, 538,104,200,167);

	// neue baubare Sachen anzeigen
	CFontLoader::CreateGDIFont(pMajor, 1, fontName, fontSize);
	fontFormat.SetFormatFlags(!StringFormatFlagsNoWrap);
	fontFormat.SetLineAlignment(StringAlignmentNear);
	fontBrush.SetColor(color);
	Bitmap* graphic = NULL;
	int nCount = 0;
	// neue Gebäude zeichnen
	for (vector<CBuildingInfo*>::const_iterator it = m_vNewBuildings.begin(); it != m_vNewBuildings.end(); ++it)
	{
		graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Buildings\\" + (*it)->GetGraphikFileName());
		if (graphic == NULL)
			graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Buildings\\ImageMissing.bop");
		if (graphic)
			g->DrawImage(graphic, 15 + nCount * 165, 430, 150, 113);
		g->DrawString(CComBSTR((*it)->GetBuildingName()), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(nCount * 165, 555, 180, 50), &fontFormat, &fontBrush);
		nCount++;
	}
	nCount = 0;
	// neue Upgrades
	for (vector<CBuildingInfo*>::const_iterator it = m_vNewUpgrades.begin(); it != m_vNewUpgrades.end(); ++it)
	{
		graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Buildings\\" + (*it)->GetGraphikFileName());
		if (graphic == NULL)
			graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Buildings\\ImageMissing.bop");
		if (graphic)
			g->DrawImage(graphic, 15 + nCount * 165, 620, 150, 113);
		g->DrawString(CComBSTR((*it)->GetBuildingName()), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(nCount * 165, 745, 180, 50), &fontFormat, &fontBrush);
		nCount++;
	}

	// neue Schiffe und Truppen anzeigen
	nCount = 0;
	for (vector<CShipInfo*>::const_iterator it = m_vNewShips.begin(); it != m_vNewShips.end(); ++it)
	{
		graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Ships\\" + (*it)->GetShipClass() + ".bop");
		if (graphic == NULL)
			graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Ships\\ImageMissing.bop");
		if (graphic)
			g->DrawImage(graphic, 15 + nCount * 165, 815, 150, 113);
		s = (*it)->GetShipClass() + "-" + CResourceManager::GetString("CLASS") + " (" + (*it)->GetShipTypeAsString() + ")";
		g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(nCount * 165, 933, 180, 50), &fontFormat, &fontBrush);
		nCount++;
	}
	for (vector<CTroopInfo*>::const_iterator it = m_vNewTroops.begin(); it != m_vNewTroops.end(); ++it)
	{
		graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Troops\\" + (*it)->GetGraphicFileName());
		if (graphic == NULL)
			graphic = pDoc->GetGraphicPool()->GetGDIGraphic("Troops\\ImageMissing.bop");
		if (graphic)
			g->DrawImage(graphic, 15 + nCount * 165, 815, 150, 113);
		s = (*it)->GetName();
		g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(nCount * 165, 933, 180, 50), &fontFormat, &fontBrush);
		nCount++;
	}

	fontBrush.SetColor(markColor);
	s = CResourceManager::GetString("RESEARCHEVENT_NEWBUILDINGS");
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(0, 405, 1280, 20), &fontFormat, &fontBrush);
	s = CResourceManager::GetString("RESEARCHEVENT_NEWUPGRADES");
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(0, 595, 1280, 20), &fontFormat, &fontBrush);
	s = CResourceManager::GetString("RESEARCHEVENT_NEWSHIPS_AND_TROOPS");
	g->DrawString(CComBSTR(s), -1, &Gdiplus::Font(CComBSTR(fontName), fontSize), RectF(0, 790, 1280, 20), &fontFormat, &fontBrush);

	// Buttons zeichnen
	CFontLoader::CreateGDIFont(pMajor, 3, fontName, fontSize);
	CFontLoader::GetGDIFontColor(pMajor, 2, color);
	fontBrush.SetColor(color);
	for (int i = 0; i < m_Buttons.GetSize(); i++)
		m_Buttons.GetAt(i)->DrawButton(*g, graphicPool, Gdiplus::Font(CComBSTR(fontName), fontSize), fontBrush);
}

///	Funktion erstellt zur aktuellen Mouse-Position einen HTML Tooltip
/// @return	der erstellte Tooltip-Text
CString CEventResearch::GetTooltip(const CPoint &pt) const
{
	int nCount = 0;
	// neue Gebäude
	for (vector<CBuildingInfo*>::const_iterator it = m_vNewBuildings.begin(); it != m_vNewBuildings.end(); ++it)
	{
		if (CRect(15 + nCount * 165, 435, 15 + nCount * 165 + 150, 585).PtInRect(pt))
		{
			CString sName = (*it)->GetBuildingName();
			sName = CHTMLStringBuilder::GetHTMLColor(sName);
			sName = CHTMLStringBuilder::GetHTMLHeader(sName, _T("h3"));
			sName = CHTMLStringBuilder::GetHTMLCenter(sName);
			sName += CHTMLStringBuilder::GetHTMLStringNewLine();
			sName += CHTMLStringBuilder::GetHTMLStringNewLine();

			CString sProd = (*it)->GetProductionAsString();
			sProd = CHTMLStringBuilder::GetHTMLColor(sProd);
			sProd = CHTMLStringBuilder::GetHTMLHeader(sProd, _T("h5"));
			sProd += CHTMLStringBuilder::GetHTMLStringNewLine();
			sProd += CHTMLStringBuilder::GetHTMLStringHorzLine();
			sProd += CHTMLStringBuilder::GetHTMLStringNewLine();

			CString sDesc = (*it)->GetBuildingDescription();
			sDesc = CHTMLStringBuilder::GetHTMLColor(sDesc, _T("silver"));
			sDesc = CHTMLStringBuilder::GetHTMLHeader(sDesc, _T("h5"));

			return sName + sProd + sDesc;
		}
		nCount++;
	}

	nCount = 0;
	// neue Upgrades
	for (vector<CBuildingInfo*>::const_iterator it = m_vNewUpgrades.begin(); it != m_vNewUpgrades.end(); ++it)
	{
		if (CRect(15 + nCount * 165, 625, 15 + nCount * 165 + 150, 775).PtInRect(pt))
		{
			CString sName = (*it)->GetBuildingName();
			sName = CHTMLStringBuilder::GetHTMLColor(sName);
			sName = CHTMLStringBuilder::GetHTMLHeader(sName, _T("h3"));
			sName = CHTMLStringBuilder::GetHTMLCenter(sName);
			sName += CHTMLStringBuilder::GetHTMLStringNewLine();
			sName += CHTMLStringBuilder::GetHTMLStringNewLine();

			CString sProd = (*it)->GetProductionAsString();
			sProd = CHTMLStringBuilder::GetHTMLColor(sProd);
			sProd = CHTMLStringBuilder::GetHTMLHeader(sProd, _T("h5"));
			sProd += CHTMLStringBuilder::GetHTMLStringNewLine();
			sProd += CHTMLStringBuilder::GetHTMLStringHorzLine();
			sProd += CHTMLStringBuilder::GetHTMLStringNewLine();

			CString sDesc = (*it)->GetBuildingDescription();
			sDesc = CHTMLStringBuilder::GetHTMLColor(sDesc, _T("silver"));
			sDesc = CHTMLStringBuilder::GetHTMLHeader(sDesc, _T("h5"));

			return sName + sProd + sDesc;
		}
		nCount++;
	}

	// neue Schiffe und Truppen anzeigen
	nCount = 0;
	for (vector<CShipInfo*>::const_iterator it = m_vNewShips.begin(); it != m_vNewShips.end(); ++it)
	{
		if (CRect(15 + nCount * 165, 820, 15 + nCount * 165 + 150, 970).PtInRect(pt))
			return (*it)->GetTooltip();
		nCount++;
	}
	return "";
}
