#pragma once

#include "pch.h"
#include "chatClientDlg.h"
#include "Packet.h"
#include <vector>
//class CchatClientDlg;

enum e_Status
{
    E_LOGIN = 0,
    E_CHAT,
};

class CMySocket : public CAsyncSocket {

public:
    CMySocket(BOOL bChat, CchatClientDlg* pDlg, e_Status eStatus = E_LOGIN);
    ~CMySocket();

public:
    virtual void OnReceive(int nErrorCode) override ;
    virtual void OnConnect(int nErrorCode) override ;
    BOOL connect(const CString& strHost, const int nPort);

    std::string RecevieBuff();
    void Parse();

public:
    std::vector<char> m_recvBuff;
	CchatClientDlg* m_pDlg;

    BOOL m_bConnect;
	BOOL m_bChatSocket;
    BOOL m_bLoginPhase;
    BOOL m_bRegister;
};