#include "pch.h"

#include "MySocket.h"
#include "CLogin.h"
#include "UTIL.h"

CMySocket::CMySocket(BOOL bChat, CchatClientDlg* pDlg, e_Status eStatus) : m_bChatSocket(bChat), m_pDlg(pDlg)
{
    m_bConnect = FALSE;
    m_bLoginPhase = eStatus == E_LOGIN ? TRUE : FALSE;
    m_bRegister = FALSE;
}
CMySocket::~CMySocket() {}
 
void CMySocket::OnReceive(int nErrorCode)
{
        std::string buf = RecevieBuff();
        int len = buf.size();

        { char szTmp[2058]; sprintf_s(szTmp, sizeof(szTmp), "[%s][%d]gunoo22_TEST m_bLoginPhase[%d]", __FUNCTION__, __LINE__, m_bLoginPhase); OutputDebugStringA(szTmp); }

        //회원가입
        if (m_bRegister)
        {
            m_pDlg->m_dlgLogin->m_pRegDlg->OnRegister(buf.c_str(), len);
            return;
        }
        
        //로그인
        if (m_bLoginPhase)
        {
            m_bLoginPhase = FALSE;
            m_pDlg->m_dlgLogin->OnLogin(buf.c_str(), len);
            return;
        }

        //채팅
        if (len > 0) {
            m_recvBuff.insert(m_recvBuff.end(), buf.begin(), buf.end());
            Parse();
            //buf[len] = 0;

            //char* pTmp = UTIL::UTF8ToANSI(buf);

            //CString str;
            ////str.Format(_T("%S"), buf);
            //str.Format(_T("%S"), pTmp);
            //m_pDlg->AppendLog(str);

            //delete[] pTmp;
        }

        { char szTmp[2058]; sprintf_s(szTmp, sizeof(szTmp), "[%s][%d] gunoo22_TEST recvBuff[%s]", __FUNCTION__, __LINE__, buf); OutputDebugStringA(szTmp); }

        CAsyncSocket::OnReceive(nErrorCode);
}

void CMySocket::OnConnect(int nErrorCode)
{
        if (nErrorCode == 0) {
            AfxMessageBox(_T("서버 연결 성공!"));
            m_bConnect = TRUE;
        }
        else {
            CString msg;
            msg.Format(_T("서버 연결 실패: %d"), nErrorCode);
            AfxMessageBox(msg);
        }

        if (m_bLoginPhase)
            m_pDlg->m_dlgLogin->OnSocketConnect(m_bConnect);
        CAsyncSocket::OnConnect(nErrorCode);
}

BOOL CMySocket::connect(const CString &strHost, const int nPort)
{
    BOOL bRet;
    
    if ((bRet = this->Create()) != TRUE)
        goto err;
    if ((bRet = this->Connect(strHost, nPort)) != TRUE)
        goto err;

   err:
    return bRet;
}

std::string CMySocket::RecevieBuff()
{
    char temp[BUFFER_SIZE];
    int  tempLen = 0;
    std::string buf;

    do {
        memset(temp, 0x00, sizeof(temp));
        tempLen = Receive(temp, sizeof(temp) - 1);
        if (tempLen <= 0)
        {
            AfxMessageBox(_T("Server Down.."));
            return;
        }
        buf.append(temp, tempLen);
    } while (tempLen == BUFFER_SIZE);
    
    return buf;
}

void CMySocket::Parse()
{
    auto& buf = m_recvBuff;

    while (buf.size())
    {
        if (buf.size() < sizeof(PacketHeader))
        {
            //K_slog_trace(K_SLOG_ERROR, "[%s][%d] buf.size() < sizeof(PacketHeader)", __FUNCTION__, __LINE__);
      /*      K_slog_trace(K_SLOG_ERROR, "[%s][%d] sizeof(PacketHeader)[%d]", __FUNCTION__, __LINE__, sizeof(PacketHeader));
            K_slog_trace(K_SLOG_ERROR, "[%s][%d] buf.size[%d]", __FUNCTION__, __LINE__, buf.size());*/
            return;
        }

        PacketHeader* hdr = reinterpret_cast<PacketHeader*>(buf.data());
        uint16_t pktLen = hdr->length;

        if (buf.size() < pktLen)
        {
            //K_slog_trace(K_SLOG_ERROR, "[%s] buf.size small pktLen", __FUNCTION__);
            return;
        }
        uint16_t type = hdr->type;

        const char* payload = reinterpret_cast<const char*>(buf.data() + sizeof(PacketHeader));
        int payloadLen = pktLen - sizeof(PacketHeader);
        //K_slog_trace(K_SLOG_TRACE, "[%s] type=%x", __FUNCTION__, type);

        auto handler = PacketFactory::Create(type);
        if (handler)
            handler->Execute(cli, payload, payloadLen, clients, broadcast);

        buf.erase(buf.begin(), buf.begin() + pktLen);
    }
}