
// chatClientDlg.h: 헤더 파일
//

#pragma once
#include "pch.h"

class CLogin;
class CMySocket;

// CchatClientDlg 대화 상자
class CchatClientDlg : public CDialogEx
{
// 생성입니다.
public:
	CchatClientDlg(CWnd* pParent = nullptr);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CHATCLIENT_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:
	void AppendLog(CString msg);
	CAsyncSocket m_sockTest;
	CEdit m_editInput;
	CListBox m_listChat;
	afx_msg void OnBnClickedButtonSend();
	void OnSocketConnect(BOOL bConnect);
	void OnLogin();

public:
	CMySocket* m_sockLogin;
	CMySocket* m_sockChat;
	CLogin* m_dlgLogin;
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
