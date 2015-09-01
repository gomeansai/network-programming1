
// PRJ3_0613Dlg.h : 头文件
//
#include "user.h"
#define SOCK_LIST_SIZE 8

// CPRJ3_0613Dlg 对话框
#include "afxwin.h"
#include "afxcmn.h"
class CPRJ3_0613Dlg : public CDialogEx
{
// 构造
public:
	CPRJ3_0613Dlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_PRJ3_0613_DIALOG };

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
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedAnniu();
	afx_msg void OnBnClickedShangxiananniu();
	afx_msg void OnBnClickedXiaxiananniu();
	afx_msg void OnBnClickedTuichuanniu();

	void OffLine();
	void GUIOffline();
	void GUIOnline();
	void ChatGUIInit();
	void Modify_Friend_List();
	LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	afx_msg void OnBnClickedDakaianniu();
	

	///////////////////////////////////////////////////////////////////
	//处理数据连接的建立
	SOCKET CtlSock;
	SOCKET DatSock;
	UCHAR Is_On;

	//void Recurse(LPCTSTR pstr);
	void AcceptDataSocket(SOCKET s);

	SOCKET filelist_sock;
	UCHAR fl_sock_status;
	int sock_num;
	SOCKET sock_list[SOCK_LIST_SIZE];
	UCHAR sock_status[SOCK_LIST_SIZE];
	CString FileName[SOCK_LIST_SIZE];
	CString FilePath[SOCK_LIST_SIZE];
	FILE* file_pointer[SOCK_LIST_SIZE];
	int last_len[SOCK_LIST_SIZE];	//为重发做准备，需要指针回退

	short CPRJ3_0613Dlg::SendFileAgain(int i);

	short StartSendFile(SOCKET s, int i);
	short SendFile(SOCKET s);
	short StartRecvFile(SOCKET s, int i);
	short RecvFile(SOCKET sock);
	short StartRecvFileList(SOCKET s, FilePDU *file_pdu);
	short RecvFileList(SOCKET s);

	void Re_Account(SOCKET sock);
	void Re_Inquiry(SOCKET sock, char *tempcmdbuff);


	void AddToFileList(CString filename);
	void ClearFileList();

	#define LISTMAXLEN 16
	typedef struct infomanagestruct{			//好友列表结构
		int num;
		int id;
		char name[255];
	} infomanagestruct;
	infomanagestruct infomanage[LISTMAXLEN];
	///////////////////////////////////////////////////////////////////


	CListBox m_ShangChuanLuJing;	
	
	CString strPath;
	afx_msg void OnBnClickedShangchuananniu();
	afx_msg void OnBnClickedGengxinliebiao();
	afx_msg void OnBnClickedXiazaianniu();
	CListBox m_XiaZaiLieBiao;
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	CListCtrl m_LiaoTianJiLuXianShi;
	CComboBox m_HaoYouLieBiao;
	afx_msg void OnBnClickedFasongxiaoxianniu();
};
