#include "StdAfx.h"
#include "GraphicPool.h"
#include "Options.h"
#include "ImageStone/ImageStone.h"

#define TRACE_GRAPHICLOAD MYTRACE_DOMAIN("graphicload")

//////////////////////////////////////////////////////////////////////
// Konstruktion und Destruktion
//////////////////////////////////////////////////////////////////////
CGraphicPool::CGraphicPool(const CString& path) : m_strPath(path)
{
}

CGraphicPool::~CGraphicPool(void)
{
	// Zeiger auf die Grafiken in der Map zerstören
	POSITION pos = m_Graphics.GetStartPosition();
	CString key;
	CBitmap* bm = NULL;
	while (pos != NULL)
	{
		m_Graphics.GetNextAssoc(pos, key, bm);
		if (bm)
		{
			delete bm;
			bm = NULL;
		}
	}

	// Zeiger auf die Grafiken in der Map zerstören
	pos = m_GDIGraphics.GetStartPosition();
	Bitmap* gdibm = NULL;
	while (pos != NULL)
	{
		m_GDIGraphics.GetNextAssoc(pos, key, gdibm);
		if (gdibm)
		{
			delete gdibm;
			gdibm = NULL;
		}
	}
}

//////////////////////////////////////////////////////////////////////
// sonstige Funktionen
//////////////////////////////////////////////////////////////////////
/// Funktion liefert einen Zeiger auf eine Grafik.
/// @param name Name der Grafik.
CBitmap* CGraphicPool::GetGraphic(const CString &name)
{
	// Funktion sucht in der Map nach der Grafik. Konnte sie nicht gefunden werden, so wird die
	// Grafik in die Map geladen. Danach wird die Grafik zurückgegeben.
	// Bei einem Fehler gibt die Funktion NULL zurück.

	if (name.IsEmpty())
		return NULL;

	CBitmap* g = NULL;
	// nach der Grafik in der Map suchen, wenn sie gefunden werden konnte, so wird sie zurückgegeben
	if (m_Graphics.Lookup(name, g))
		return g;

	// ansonsten muss die Grafik geladen werden und in die Map gesteckt werden
	FCObjImage* img = new FCObjImage();
	// kompletten Pfad inkl. relativen Pfadnamen
	CString fileName(m_strPath + name);
	TRACE_GRAPHICLOAD(MT::LEVEL_DEBUG, "graphic: %s not found in map ... loading\n", fileName);
	// Grafik laden
	if (!img->Load(fileName))
	{
		TRACE_GRAPHICLOAD(MT::LEVEL_WARNING, "Could not load graphic: %s\n", fileName);
		delete img;
		img = NULL;
		return NULL;
	}
	// Grafik in die Map stecken und zurückgeben
	g = new CBitmap();
	g->Attach(FCWin32::CreateDDBHandle(*img));
	m_Graphics.SetAt(name, g);
	img->Destroy();
	delete img;
	img = NULL;
	return g;
}

/// Funktion liefert einen Zeiger auf eine Grafik.
/// @param name Name der Grafik.
Bitmap* CGraphicPool::GetGDIGraphic(const CString &name, const bool require_existence)
{
	// Funktion sucht in der Map nach der Grafik. Konnte sie nicht gefunden werden, so wird die
	// Grafik in die Map geladen. Danach wird die Grafik zurückgegeben.
	// Bei einem Fehler gibt die Funktion NULL zurück.

	if (name.IsEmpty())
		return NULL;

	Bitmap* img = NULL;
	// nach der Grafik in der Map suchen, wenn sie gefunden werden konnte, so wird sie zurückgegeben
	if (m_GDIGraphics.Lookup(name, img))
		return img;

	// ansonsten muss die Grafik geladen werden und in die Map gesteckt werden
	CString fileName(m_strPath + name);
	img = NULL;
	img = Bitmap::FromFile(CComBSTR(fileName));

	TRACE_GRAPHICLOAD(MT::LEVEL_DEBUG, "graphic: %s not found in map ... loading\n", fileName);
	// Grafik laden
	if (img->GetLastStatus() != Ok)
	{
		if(require_existence) {
			TRACE_GRAPHICLOAD(MT::LEVEL_WARNING, "Could not load graphic: %s\n", fileName);
		}
		delete img;
		img = NULL;
		return NULL;
	}
	Bitmap* tmp = img->Clone(0, 0, img->GetWidth(), img->GetHeight(), PixelFormat32bppPARGB);
	delete img;
	img = tmp;

	// Grafik in die Map stecken und zurückgeben
	m_GDIGraphics.SetAt(name, img);
	return img;
}
