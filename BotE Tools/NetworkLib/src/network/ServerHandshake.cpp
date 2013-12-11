
#include "StdAfx.h"
#include "ServerHandshake.h"

#include "BotEServer.h"

#pragma warning (disable: 4267 4018)

#ifdef _DEBUG
#define DEBUG_MSG(msg) \
	{CString ip; UINT port; \
	socket.GetPeerName(ip, port); \
	TRACE("server: [%s:%u] %s\n", ip, port, (msg));}
#else
#define DEBUG_MSG(msg)
#endif

namespace network
{
	IMPLEMENT_DYNAMIC(CServerHandshake, CThread)

	CServerHandshake::CServerHandshake(SOCKET hSocket, UINT nServerVersion)
		: CThread(TRUE), m_hSocket(hSocket), m_nServerVersion(nServerVersion)
	{
		ASSERT(hSocket != INVALID_SOCKET);
	}

	CServerHandshake::~CServerHandshake()
	{
	}

	void CServerHandshake::Run()
	{
		VERIFY(AfxSocketInit());

		CSocket socket; // führen Handshake innerhalb des Threads synchron aus
		socket.Attach(m_hSocket);

		clock_t end = clock() + HANDSHAKE_TIMEOUT * CLOCKS_PER_SEC;

		// (1.) auf eingehenden Identifizierungs-String (HANDSHAKE_CLIENTID) + Versionsnummer
		// vom Client warten
		UINT nCount = strlen(HANDSHAKE_CLIENTID) + 1 + sizeof(UINT) + 1;
		if (!RequestData(socket, nCount, end))
		{
			DEBUG_MSG("client timed out, disconnected");
			SetExitCode(1);
			return;
		}

		// Daten in Puffer übertragen
		BYTE *lpBuf = new BYTE[nCount];
		int nDone = socket.Receive(lpBuf, nCount);
		if (nDone == SOCKET_ERROR) goto sockerror;
		else if (nDone < nCount) goto error;

		// Pufferinhalt prüfen
		if (memcmp(lpBuf, HANDSHAKE_CLIENTID, strlen(HANDSHAKE_CLIENTID)) != 0
			|| lpBuf[strlen(HANDSHAKE_CLIENTID)] != 0
			|| lpBuf[nCount - 1] != 0)
		{
			DEBUG_MSG("client is not a BotE client, disconnected");
			delete[] lpBuf;
			SetExitCode(1);
			return;
		}

		// Puffer erfolgreich geprüft, Versionsnummer extrahieren
		UINT nClientVersion;
		memcpy(&nClientVersion, lpBuf + (strlen(HANDSHAKE_CLIENTID) + 1), sizeof(UINT));
		TRACE("server: client has version %u\n", nClientVersion);
		delete[] lpBuf;
		lpBuf = NULL;

		// abbrechen, wenn andere Versionsnummer
		if (nClientVersion != m_nServerVersion)
		{
			DEBUG_MSG("client has incompatible version, disconnected");
			SetExitCode(1);
			return;
		}

		// (2.) Server-Identifizierung + Versionsnummer senden
		nDone = socket.Send(HANDSHAKE_SERVERID, strlen(HANDSHAKE_SERVERID));
		if (nDone == SOCKET_ERROR) goto sockerror;
		else if (nDone < strlen(HANDSHAKE_SERVERID)) goto error;

		lpBuf = new BYTE[nCount = 1 + sizeof(UINT) + 1];
		lpBuf[0] = lpBuf[nCount - 1] = 0;
		memcpy(&lpBuf[1], &m_nServerVersion, sizeof(UINT));

		nDone = socket.Send(lpBuf, nCount);
		if (nDone == SOCKET_ERROR) goto sockerror;
		else if (nDone < nCount) goto error;

		delete[] lpBuf;
		lpBuf = NULL;

		// (3.a) Bestätigung vom Client abwarten
		int nLen = 3 + sizeof(UINT);
		if (!RequestData(socket, nLen, end))
		{
			DEBUG_MSG("client timed out, disconnected");
			SetExitCode(1);
			return;
		}

		// Bestätigung prüfen
		lpBuf = new BYTE[3];
		nDone = socket.Receive(lpBuf, 3);
		if (nDone == SOCKET_ERROR) goto sockerror;
		else if (nDone < 3) goto error;

		if (memcmp(lpBuf, "OK\0", 3) != 0)
		{
			DEBUG_MSG("client did not confirm, disconnected");
			delete[] lpBuf;
			SetExitCode(1);
			return;
		}

		delete[] lpBuf;

		// Länge des Benutzernamens ermitteln
		lpBuf = new BYTE[sizeof(UINT)];
		nDone = socket.Receive(lpBuf, sizeof(UINT));
		if (nDone == SOCKET_ERROR) goto sockerror;
		else if (nDone < sizeof(UINT)) goto error;

		UINT nStrLen = 0;
		memcpy(&nStrLen, lpBuf, sizeof(UINT));

		delete[] lpBuf;

		// (3.b) Benutzername empfangen
		if (!RequestData(socket, nStrLen, end))
		{
			DEBUG_MSG("client timed out, disconnected");
			SetExitCode(1);
			return;
		}

		// Benutzername lesen
		lpBuf = new BYTE[nStrLen + 1];
		nDone = socket.Receive(lpBuf, nStrLen);
		if (nDone == SOCKET_ERROR) goto sockerror;
		else if (nDone < nStrLen) goto error;

		{
			lpBuf[nStrLen] = 0;
			CString strUserName((char *)lpBuf);
			strUserName.Trim();

			// dem Server einen neuen Client melden; verwenden kleinere Versionsnummer zur
			// Kommunikation
			server.OnNewClient(socket.Detach(), min(m_nServerVersion, nClientVersion), strUserName);
		}

		delete[] lpBuf;
		return;

	sockerror:
		server.OnSocketError(socket.GetLastError());
	error:
		if (lpBuf) delete[] lpBuf;
		SetExitCode(1);
	}

	BOOL CServerHandshake::RequestData(CAsyncSocket &socket, UINT nCount, clock_t clkDeadline)
	{
		while (true)
		{
			DWORD dwAvailable = 0;
			if (!socket.IOCtl(FIONREAD, &dwAvailable))
			{
				server.OnSocketError(socket.GetLastError());
				return FALSE;
			}
			if (dwAvailable >= nCount) return TRUE;
			if (clock() > clkDeadline) return FALSE;
		}
	}

};
