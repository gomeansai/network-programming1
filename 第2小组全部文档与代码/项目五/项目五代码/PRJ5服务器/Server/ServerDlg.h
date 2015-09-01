
// ServerDlg.h : header file
//

#pragma once
#include "afxwin.h"

#include "UserUnit.h"
#include "Header.h"


#define USER_LIST_SIZE 64

// CServerDlg dialog
class CServerDlg : public CDialogEx
{
// Construction
public:
	CServerDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_SERVER_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	void ShowInfo(CString text);

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	LPCTSTR pstr;
	BOOL bWorking;

	SOCKET main_sock;
	short user_num;
	class UserUnit *user_list[USER_LIST_SIZE];


	short CServerDlg::Receive(SOCKET sock, char* recv_buf, short buf_size);
	short CServerDlg::CloseCtlSock(SOCKET sock);
	//short CServerDlg::CloseFilelistSock(SOCKET sock);
	//short CServerDlg::CloseFileSock(int i, int j);
	//short CServerDlg::SendACKFilePDU(SOCKET sock, char No);
	short CServerDlg::SendFileAgain(int i, int j);
	void CServerDlg::Find_ij(SOCKET sock, int &i, int &j);

	void AcceptNewUser(SOCKET sock);
	void RecvCtl(SOCKET sock, struct CtlPDU *ctl_pdu);
	int RequstAccount(SOCKET sock);
	short RecvAccount(SOCKET s, struct CtlPDU *ctl_pdu);
	
	short StartSendFilelist(SOCKET s);
	short SendFilelist(SOCKET s);
	short RecvInquiry(SOCKET s, struct InquiryPDU *inq_pdu);
	int SendCtlPDU(SOCKET sock, char cmd);
	short StartSendFile(SOCKET s, char *filename);
	short SendFile(SOCKET sock);
	
	short StartRecvFile(SOCKET s, char *filename);
	short RecvFile(SOCKET sock);
	void ChatProcess(SOCKET sock, ChatPDU *chat_buff);
	void Frd_Online_Notice(SOCKET sock);
	//void BayesCategoryTest(CString tp);			//(BayesTest* bt, CString tp);

	UINT m_Port;
	afx_msg void OnClickedStartServer();
	CButton m_StartServer;
	CButton m_CloseServer;
	afx_msg void OnClickedCloseServer();
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	afx_msg void OnCbnSelchangeCombo1();
	afx_msg void OnEnChangeEdit1();
	CString m_Output;
	CListBox m_InfoList;
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	CListBox m_FileList;
	CListBox m_UserList;


	CString m_ServerDataRoot;
	void CServerDlg::ShowUser(ULONG ID, CString nickname);
	void CServerDlg::ShowFile(CString filename);
	void CServerDlg::ShowFileList(CString pstr);
	void CServerDlg::ClearList();
	void CServerDlg::MakeSerDataDir(ULONG id);

	void CServerDlg::RequestTranslateFile(SOCKET sock, struct CtlPDU *ctl_pdu);
	short CServerDlg::StartTranslateFile(SOCKET sock, int i, int j);
	short CServerDlg::QuitTranslateFile(SOCKET sock, int i, int j);
	short CServerDlg::TranslateFile(SOCKET sock);
};
