
// UDP_MFC_ClientDlg.h : 头文件
//

#pragma once
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include "afxwin.h"
#include "afxcmn.h"
#pragma comment(lib,"ws2_32.lib")

#define DEFAULT_SERVER_IP "127.0.0.1"
#define DEFAULT_SERVER_PORT 0x1234

#define ONLINE 1
#define OFFLINE 0

#define LISTMAXLEN 10
#define MSG_BUF_SIZE 1024
#define CHAR_BUF_SIZE MSG_BUF_SIZE+5

#define WM_SOCKET WM_USER+101


// CUDP_MFC_ClientDlg 对话框
class CUDP_MFC_ClientDlg : public CDialogEx
{
// 构造
public:
	CUDP_MFC_ClientDlg(CWnd* pParent = NULL);	// 标准构造函数

	SOCKET sock;

	struct msgstructure{
		union{
			struct{ SOCKET ID; char command; char msg_buf[MSG_BUF_SIZE]; }msg;
			char char_buf[CHAR_BUF_SIZE];
		};
	};
	msgstructure msgpack;

	typedef struct infomanagestruct{
		int num;
		SOCKET id;
		char name[20];
	} infomanagestruct;
	infomanagestruct infomanage[LISTMAXLEN];
	int listlen = 0;
	char state = OFFLINE;
// 对话框数据
	enum { IDD = IDD_UDP_MFC_CLIENT_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedExit();

	LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

	afx_msg void OnBnClickedStart();
	afx_msg void OnBnClickedStop();
	CEdit m_Edit_Port;
	CEdit m_Edit_Name;
	afx_msg void OnEnChangeIpAddr();
	CEdit m_IP_Addr;
	CComboBox m_Combo_Bind;
	afx_msg void OnCbnSelchangeBind();
	CEdit m_Edit_IP_Addr;

	void UserInit();
	void Modify_Friend_List();
	afx_msg void OnBnClickedSend();
	CComboBox m_Combo_Friend_List;
	afx_msg void OnLvnItemchangedList3(NMHDR *pNMHDR, LRESULT *pResult);
	CListCtrl m_List_Msg;
	CButton m_Button_Send;
};
