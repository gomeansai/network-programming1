
// PRJ3_0613Dlg.h : ͷ�ļ�
//
#include "user.h"
#define SOCK_LIST_SIZE 8

// CPRJ3_0613Dlg �Ի���
#include "afxwin.h"
class CPRJ3_0613Dlg : public CDialogEx
{
// ����
public:
	CPRJ3_0613Dlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_PRJ3_0613_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
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
	LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	afx_msg void OnBnClickedDakaianniu();
	

	///////////////////////////////////////////////////////////////////
	//�����������ӵĽ���
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
	int last_len[SOCK_LIST_SIZE];	//Ϊ�ط���׼������Ҫָ�����

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
	///////////////////////////////////////////////////////////////////


	CListBox m_ShangChuanLuJing;	
	
	CString strPath;
	afx_msg void OnBnClickedShangchuananniu();
	afx_msg void OnBnClickedGengxinliebiao();
	afx_msg void OnBnClickedXiazaianniu();
	CListBox m_XiaZaiLieBiao;
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
