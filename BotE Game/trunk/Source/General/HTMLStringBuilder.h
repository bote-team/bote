/*
 *   Copyright (C)2004-2011 Sir Pustekuchen
 *
 *   Author   :  Sir Pustekuchen
 *   Home     :  http://birth-of-the-empires.de
 *
 */

#pragma once

/// Klasse bietet funktionen an, durch welche HTML String zusammengebaut werden können
class CHTMLStringBuilder
{
private:
	/// Standardkonstruktor
	CHTMLStringBuilder(void) {}

	/// Standarddestruktor
	~CHTMLStringBuilder(void) {}

public:

	/// Funktion zentriert den übergebenen String.
	/// @param str zu zentrierender Text
	/// @return zentrierter Text
	static inline CString GetHTMLCenter(const CString& str)
	{
		CString msg = "";
	
		msg = _T("<center>");
		msg += str;
		msg += _T("</center>");
	
		return msg;
	};

	/// Funktion macht den übergebenen String fett.
	/// @param str fettzumachender Text
	/// @return fetter Text
	static inline CString GetHTMLStringBold(const CString& str)
	{
		CString msg = "";
	
		msg = _T("<b>");
		msg += str;
		msg += _T("</b>");
		
		return msg;
	}

	/// Funktion gibt ein NewLine HTML Kommando zurück.
	/// @return NewLine HTML Kommando als String
	static inline CString GetHTMLStringNewLine(void)
	{
		CString msg = "";
		msg = _T("<br>");

		return msg;
	}

	/// Funktion gibt ein Zeichne-Horizontale-Linie-HTML-Kommando zurück.
	/// @return Zeichne-Horizontale-Linie-HTML-Kommando
	static inline CString GetHTMLStringHorzLine(void)
	{
		CString msg = "";
		msg = _T("<font color=white><hr></font>");

		return msg;
	}

	/// Funktion macht aus dem übergebenen String eine HTML Überschrift
	/// @param str Text für Überschrift
	/// @param head HTML Überschriftsart
	/// @return Text als Überschrift
	static inline CString GetHTMLHeader(const CString& str, const CString& head = "h1")
	{
		CString msg = "";
		msg = _T("<" + head + ">");
		msg += str;
		msg += _T("</" + head + ">");

		return msg;
	}

	/// Funktion weißt dem übergebenen String eine HTML-Farbe zu.
	/// @param str Text für Farbe
	/// @param clr HTML-Farbe
	/// @return Text mit HTML-Farbkommando
	static inline CString GetHTMLColor(const CString& str, const CString& clr = "white")
	{
		CString msg = "";
		msg = _T("<font color=" + clr + ">");
		msg += str;
		msg += _T("</font>");

		return msg;
	}
};
