/**
 * Die folgenden Funktionen stammen ursprünglich aus LzmaRam und LzmaRamDecode des Projektes
 * LZMA_Alone. Der Quellcode wurde übernommen und für unsere Zwecke angepasst. Die notwendigen
 * übersetzten Objekte wurden statisch zur Bibliothek BotE_LZMA.lib gelinkt. Sämtliche
 * Headerdateien wurden mit ihrer ursprünglichen Ordnerstruktur unverändert übernommen.
 *
 * @author CBot
 * @version 0.0.1
 */
#pragma once

#include "stdafx.h"

#define DICT_SIZE	(512 * 1024) 	///< Größe des Wörterbuchs, 512 KB

/**
 * Werte für <code>filterMode</code> in <code>BotE_LzmaCompress()</code>.
 */
typedef enum ESZ_FILTER_MODE
{
	SZ_FILTER_NO,
	SZ_FILTER_YES,
	SZ_FILTER_AUTO
}
ESZ_FILTER_MODE;

/**
 * Liefert die unkomprimierte Größe der in <code>inBuffer</code> vorliegenden komprimierten Daten.
 * @return <code>TRUE</code> bei Erfolg, sonst <code>FALSE</code>
 */
BOOL BotE_LzmaGetUncompressedSize(const BYTE *inBuffer, size_t inSize, size_t *outSize);

/**
 * Dekomprimiert die in <code>inBuffer</code> vorliegenden Daten, schreibt das Ergebnis nach
 * <code>outBuffer</code>.
 * @return <code>TRUE</code> bei Erfolg, sonst <code>FALSE</code>
 */
BOOL BotE_LzmaDecompress(const BYTE *inBuffer, size_t inSize, CFile &out,
	void *(*allocFunc)(size_t size) = malloc, void (*freeFunc)(void *) = free);

/**
 * Komprimiert die in <code>inBuffer</code> vorliegenden Daten, schreibt das Ergebnis nach
 * <code>pOut</code>.
 * @return <code>TRUE</code> bei Erfolg, sonst <code>FALSE</code>
 */
BOOL BotE_LzmaCompress(const BYTE *inBuffer, size_t inSize, CFile &out, UINT dictionarySize = DICT_SIZE,
	ESZ_FILTER_MODE filterMode = SZ_FILTER_AUTO);
