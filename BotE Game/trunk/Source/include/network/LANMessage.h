#pragma once

#include "network.h"
#include "ReferenceCounter.h"
#include <map>

namespace network
{
	typedef std::map<CString, CString> PARAM_MAP;

	/**
	 * Nachricht, wie sie vom <code>CLANConnection</code>-Thread versendet oder empfangen wird.
	 * Eine Nachricht besteht aus einer <code>ID</code> und einer Liste von (Parameter, Wert)-Paaren,
	 * wobei Parametername und Wert <code>CString</code>-Objekte sind.
	 *
	 * Aufbau einer Nachricht:
	 * <pre>
	 * BYTE			id					ID der Nachricht
	 * CString		params				Liste von (Parameter, Wert)-Paaren der Form "param=value&..."; die
	 *									in param und value vorkommenden Zeichen '=', '&' und '%' werden beim
	 *									(de-)serialisieren entsprechend (de-)codiert
	 * </pre>
	 *
	 * @author CBot
	 * @version 0.0.1
	 */
	class CLANMessage : public CReferenceCounter
	{
	private:
		BYTE m_nID;						///< ID dieser Nachricht
		PARAM_MAP m_params;				///< Map, die die (Parameter, Wert)-Paare enthält

		DWORD m_dwSenderIP;				///< bei empfangener Nachricht: IP-Adresse des Absenders, sonst <code>0</code>
		UINT m_nSenderPort;				///< bei empfangener Nachricht: Port des Absenders, sonst <code>0</code>
		DWORD m_dwReceiverIP;			///< bei zu sendender Nachricht: IP-Adresse des Empfängers, sonst <code>0</code>
		UINT m_nReceiverPort;			///< bei zu sendender Nachricht: Port des Empfängers, sonst <code>0</code>

	public:
		CLANMessage();
		CLANMessage(BYTE nID);
		virtual ~CLANMessage();

		UINT GetId() const {return m_nID;}

		void SetValue(const CString &param, const CString &value);
		void SetValue(const CString &param, UINT value);
		
		void SetSenderIP(DWORD dwIP) {m_dwSenderIP = dwIP;}
		void SetSenderPort(UINT nPort) {m_nSenderPort = nPort;}
		void SetReceiverIP(DWORD dwIP) {m_dwReceiverIP = dwIP;}
		void SetReceiverPort(UINT nPort) {m_nReceiverPort = nPort;}

		void SetSender(DWORD dwIP, UINT nPort) {m_dwSenderIP = dwIP; m_nSenderPort = nPort;}
		void SetReceiver(DWORD dwIP, UINT nPort) {m_dwReceiverIP = dwIP; m_nReceiverPort = nPort;}

		CString GetStringValue(const CString &param) const;
		UINT GetUINTValue(const CString &param) const;

		DWORD GetSenderIP() const {return m_dwSenderIP;}
		UINT GetSenderPort() const {return m_nSenderPort;}
		DWORD GetReceiverIP() const {return m_dwReceiverIP;}
		UINT GetReceiverPort() const {return m_nReceiverPort;}
		
		void GetSender(DWORD &dwIP, UINT &nPort) const {dwIP = m_dwSenderIP; nPort = m_nSenderPort;}
		void GetReceiver(DWORD &dwIP, UINT &nPort) const {dwIP = m_dwReceiverIP; nPort = m_nReceiverPort;}

		UINT GetParamCount() const {return (UINT)m_params.size();}
		CString GetParamName(int idx) const;

		/**
		 * (De-)Serialisiert werden nur die ID und die Parameter der Nachricht. IP-Adresse und Port von
		 * Absender bzw. Empfänger dienen nur der Steuerung der Übertragung und werden durch die
		 * entsprechenden Setter- und Getter-Methoden geschrieben bzw. gelesen.
		 */
		virtual void Serialize(CArchive &ar);

	private:
		static CString Encode(const CString &str);
		static CString Decode(const CString &str);

	};

};
