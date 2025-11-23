
// chatClientDlg.cpp: 구현 파일
//

#include "pch.h"
#include "framework.h"
#include "chatClient.h"
#include "chatClientDlg.h"
#include "afxdialogex.h"
#include "MySocket.h"
#include "CLogin.h"
#include "PacketUtility.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CchatClientDlg 대화 상자



CchatClientDlg::CchatClientDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_CHATCLIENT_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	//m_sockChat(TRUE, this)
	m_sockLogin = new CMySocket(TRUE, this, E_LOGIN);
	m_sockChat = new CMySocket(TRUE, this, E_CHAT);
	m_dlgLogin = new CLogin(m_sockLogin);
}

void CchatClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_INPUT, m_editInput);
	DDX_Control(pDX, IDC_LIST_CHAT, m_listChat);
}

BEGIN_MESSAGE_MAP(CchatClientDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_SEND, &CchatClientDlg::OnBnClickedButtonSend)
END_MESSAGE_MAP()


// CchatClientDlg 메시지 처리기

BOOL CchatClientDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.

	

	if (m_dlgLogin->DoModal() != IDOK)
	{
		return FALSE;
	}

	//if (loginDlg.login() != TRUE)
	//{
	//	return FALSE;
	//}

	BOOL bRet;
	CString strHost, strPort;
	int nPort;

	/*m_dlgLogin->m_editHost.GetWindowText(strHost);
	m_dlgLogin->m_editPort.GetWindowText(strPort);*/
	strHost = m_dlgLogin->m_strHost;
	strPort = m_dlgLogin->m_strPort;
	nPort = atoi(CT2A(strPort)) + 200;
	m_sockChat->connect(strHost, nPort);

	AppendLog(_T("Connected to Chat Server"));
	
err: 
	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CchatClientDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 애플리케이션의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CchatClientDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CchatClientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

BOOL CchatClientDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN) // 엔터키 눌렀을 때
	{
		OnBnClickedButtonSend();
		return TRUE;
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}

void CchatClientDlg::AppendLog(CString msg)
{
//#if 1 /*UTF-8 -> ANSI 변경*/
//	CStringA utf8Str(msg);
//	CStringW uniStr(CA2W(utf8Str, CP_UTF8));
//	m_listChat.AddString(uniStr);
//#endif

	int i = m_listChat.GetCount();
	int idx = m_listChat.InsertString(i, msg);
	m_listChat.SetCurSel(idx);
}

//전송 버튼 클릭
void CchatClientDlg::OnBnClickedButtonSend()
{
	CString strMsg;
	CStringA msgA;

	m_editInput.GetWindowText(strMsg);
#if 1 //예외 처리
	if (strMsg.GetLength() == 0)
	{
		AfxMessageBox(_T("채팅 내용을 입력하세요."));
		return;
	}

	if (strMsg.GetLength() > 2000)
	{
		AfxMessageBox(_T("2000자 이내로 입력하세요."));
		return;
	}
#endif
	msgA = strMsg;

	PacketUtility::SendPacket(m_sockChat, PKT_CHAT, std::string(msgA));
	//m_sockChat->Send(msgA, msgA.GetLength());

	AppendLog(_T("[Me] ") + strMsg + _T("\n"));

	m_editInput.SetWindowTextW(_T("")); //채팅입력창 비우기
}

void CchatClientDlg::OnSocketConnect(BOOL bConnect)
{
	//m_dlgLogin->OnSocketConnect(bConnect);
}

void CchatClientDlg::OnLogin()
{
	//m_dlgLogin->OnLogin();
}
