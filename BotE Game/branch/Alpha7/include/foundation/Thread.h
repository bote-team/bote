#pragma once

#include "afx.h"

/**
 * Java-ähnliche Thread-Kapselung für MFC.
 *
 * Zur Implementierung eines eigenen Threads wird diese Klasse abgeleitet und es werden die Methoden
 * <code>InitInstance()</code>, <code>Run()</code> und <code>ExitInstance()</code> implementiert.
 *
 * Die Hauptschleife in <code>Run()</code> sollte mittels <code>IsInterrupted()</code> prüfen, ob dieser Thread
 * per <code>Interrupt()</code> unterbrochen wurde und beendet ggf. ihre Arbeit. <code>Interrupt()</code>
 * und <code>IsInterrupted()</code> können überschrieben werden, um dieses Verhalten anzupassen.
 *
 * Zum Starten eines Threads wird ein Objekt der abgeleiteten Klasse erzeugt und <code>Start()</code> aufgerufen.
 * Ein unterbrochener bzw. beendeter Thread kann nicht erneut gestartet werden.
 *
 * @author CBot
 * @version 0.0.6
 */
class CThread : public CObject
{
	DECLARE_DYNAMIC(CThread)

private:
	CWinThread *m_pWinThread;				///< eigentliches Thread-Objekt
	bool m_bInterrupted;					///< <code>true</code> gdw. dieser Thread unterbrochen wurde
	HANDLE m_hThreadStoppedEvent;			///< Handle eines Events, das gesetzt wird, wenn dieser Thread beendet ist
	UINT m_nExitCode;						///< Exit-Code dieses Threads
	bool m_bStarted;						///< <code>true</code> gdw. <code>Start()</code> aufgerufen wurde
	BOOL m_bAutoDelete;						///< <code>TRUE</code> gdw. dieses Objekt automatisch zerstört werden soll

public:
	/**
	 * Erzeugt diesen Thread. Der Thread wird erst mit dem Aufruf von <code>Start()</code> gestartet.
	 *
	 * Wird für <code>bAutoDelete</code> der Wert <code>TRUE</code> angegeben, sollten nach <code>Start()</code>
	 * ohne entsprechende Vorkehrungen keine Methoden dieses Objekts mehr aufgerufen werden!
	 *
	 * @param bAutoDelete <code>TRUE</code> gdw. dieses Objekt nach Beendigung des Threads zerstört werden soll
	 */
	CThread(BOOL bAutoDelete = FALSE);

	/**
	 * Zerstört dieses Objekt, ohne den Thread zu unterbrechen. Der Thread sollte bereits vor Aufruf des Destruktors
	 * seine Arbeit beendet haben oder unterbrochen worden sein.
	 */
	virtual ~CThread();

	/// @return den per <code>SetExitCode()</code> gesetzten Wert; nach Beendigung dieses Threads der von
	/// <code>ExitInstance()</code> gelieferte Wert
	inline UINT GetExitCode() const {return m_nExitCode;}

	/**
	 * Startet diesen Thread. Ein Thread kann nach seiner Beendigung nicht erneut gestartet werden; es muss
	 * ein neues Objekt der Thread-Klasse angelegt werden, welches dann gestartet werden kann.
	 *
	 * @return <code>TRUE</code> gdw. dieser Thread gestartet wurde
	 */
	BOOL Start();

	/**
	 * Unterbricht diesen Thread.
	 */
	virtual void Interrupt() {m_bInterrupted = true;}

	/**
	 * Wartet, bis dieser Thread beendet ist oder der angegebene Timeout abläuft.
	 *
	 * Wurde <code>Start()</code> bisher nicht erfolgreich aufgerufen, kehrt diese Methode nicht zurück bzw.
	 * bricht nach einem Timeout immer mit <code>FALSE</code> ab.
	 *
	 * @param dwTimeout Anzahl der Millisekunden, die maximal auf die Beendigung des Threads gewartet
	 * werden soll; der Timeout läuft bei Angabe von <code>INFINITE</code> nie ab; bei Angabe von
	 * <code>0</code> prüft die Methode, ob dieser Thread beendet ist und kehrt sofort zum Aufrufer zurück
	 * @return <code>TRUE</code>, wenn dieser Thread beendet ist; <code>FALSE</code>, wenn der angegebene
	 * Timeout abgelaufen ist
	 */
	BOOL Join(DWORD dwTimeout = INFINITE);

protected:
	/**
	 * Initialisiert diesen Thread. Bei Erfolg wird die <code>Run()</code>-Methode des Threads aufgerufen.
	 *
	 * @return <code>TRUE</code> gdw. die Initialisierung erfolgreich war
	 */
	virtual BOOL InitInstance();

	/**
	 * Wird bei Beendigung des Threads aufgerufen. Wird auch aufgerufen, wenn <code>InitInstance()</code>
	 * fehlgeschlagen ist.
	 *
	 * Die Standardimplementierung liefert den per <code>SetExitCode()</code> gesetzten Wert. Wurde
	 * <code>SetExitCode()</code> nie aufgerufen, liefert die Methode <code>0</code>.
	 *
	 * @return Exit-Code des Threads
	 */
	virtual UINT ExitInstance();

	/**
	 * Methode, die die Funktion dieses Threads implementiert. Diese Methode muss abbrechen, wenn
	 * <code>IsInterrupted()</code> <code>TRUE</code> liefert.
	 *
	 * Kann <code>SetExitCode()</code> verwenden, um den Wert festzulegen, den die Standardimplementierung
	 * von <code>ExitInstance()</code> liefert.
	 */
	virtual void Run() =0;

protected:
	/// @return <code>TRUE</code> gdw. dieser Thread unterbrochen wurde
	inline virtual BOOL IsInterrupted() const {return m_bInterrupted;}

	/**
	 * Setzt den Wert, den die Standardimplementierung von <code>ExitInstance()</code> liefern soll.
	 */
	inline void SetExitCode(UINT nExitCode) {m_nExitCode = nExitCode;}

private:
	/**
	 * Eigentliche Thread-Funktion. Ruft bei Erfolg von <code>InitInstance()</code> die Methode
	 * <code>Run()</code> auf; gibt den von <code>ExitInstance()</code> gelieferten Wert zurück.
	 * Setzt nach dem Ende von <code>ExitInstance()</code> das Event, das angibt, dass der Thread
	 * beendet ist.
	 *
	 * @param pObject ein <code>CThread</code>-Objekt, dessen Methoden aufgerufen werden sollen
	 */
	static UINT ThreadProc(LPVOID pObject);

};
