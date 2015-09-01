
// ServerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Server.h"
#include "ServerDlg.h"
#include "afxdialogex.h"
#include "io.h"

#pragma warning(disable: 4996)

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


#define TIMEOUT 2000
//������������
#define P_AbandRand 0.9

//0, �����ͣ�1������
short AbandonRand(double p)
{
	int result;
	srand((int)time(0));
	result = rand() % 100;
	if (result <= 100 * p){
		return 0;
	}
	else{
		return 1;
	}
}



// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CServerDlg dialog



CServerDlg::CServerDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CServerDlg::IDD, pParent)
	, m_Port(12345)
	, m_Output(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_SERVER_PORT, m_Port);
	DDX_Control(pDX, IDC_START_SERVER, m_StartServer);
	DDX_Control(pDX, IDC_CLOSE_SERVER, m_CloseServer);
	DDX_Text(pDX, IDC_OUTPUT, m_Output);
	DDX_Control(pDX, IDC_INFO_LIST, m_InfoList);
}

BEGIN_MESSAGE_MAP(CServerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_START_SERVER, &CServerDlg::OnClickedStartServer)
	ON_BN_CLICKED(IDC_CLOSE_SERVER, &CServerDlg::OnClickedCloseServer)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CServerDlg message handlers

BOOL CServerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
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

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CServerDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CServerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CServerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CServerDlg::OnClickedStartServer()
{
	// TODO: Add your control notification handler code here
	m_StartServer.EnableWindow(FALSE);

	//�ӿؼ���ȡ����
	UpdateData(TRUE);
	//UpdateData(FALSE);

	//������׽��ֲ�Ϊ�㣬��������Ѿ������������������Ѿ��ڿ��������ð�ťΪ���ɰ����򲻻�����������
	if (main_sock != 0){
		MessageBox(TEXT("�������Ѿ�������"), TEXT("����������"), MB_OK);
		//m_InfoList.AddString(TEXT("�������󣺷������Ѿ�������"));
		ShowInfo(TEXT("�������󣺷������Ѿ�������"));
		return;
	}

	//����˿ںŲ�����Ҫ���򱨴�
	if (m_Port <= 0 || m_Port >= 65536){
		MessageBox(TEXT("��������ȷ�Ķ˿ںţ�0 < port < 65535����"), TEXT("�˿ںŴ���"), 0);
		//m_InfoList.AddString(TEXT("����������������ȷ�Ķ˿ںţ�0 < port < 65535����"));
		ShowInfo(TEXT("����������������ȷ�Ķ˿ںţ�0 < port < 65535����"));
		return;
	}

	//*********************
	//******����������******
	//*********************
	WSAData wsa;
	sockaddr_in ser_addr;
	int retval;

	WSAStartup(0x101, &wsa);

	//��ʼ�����׽���
	//main_sock = socket(AF_INET, SOCK_DGRAM, 0);
	main_sock = socket(AF_INET, SOCK_STREAM, 0);
	ser_addr.sin_family = AF_INET;
	ser_addr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	ser_addr.sin_port = htons(m_Port);

	//�󶨶˿�
	retval = bind(main_sock, (sockaddr *)&ser_addr, sizeof(ser_addr));
	if (retval == SOCKET_ERROR){
		ShowInfo(TEXT("main_sock��IP��ַ��˿ں�ʧ�ܣ�"));
		return;
	}

	//��������
	retval = listen(main_sock, 5);
	if (retval == -1){
		MessageBox(_T("main_sock��������ʧ�ܣ�"));
		return;
	}

	//ע���׽����첽��Ӧ
	retval = WSAAsyncSelect(main_sock, m_hWnd, UM_SOCK, FD_ACCEPT);

	ShowInfo(TEXT("�������ѿ�����"));

	//��ʼ���û�����
	int i;
	user_num = 0;
	for (i = 0; i < USER_LIST_SIZE; i++){
		user_list[i] = NULL;
	}

	//SetTimer(TIMER1, TIMEOUT, 0);
	//�ı䰴��״̬
	m_CloseServer.EnableWindow(TRUE);
}


void CServerDlg::OnClickedCloseServer()
{
	// TODO: Add your control notification handler code here
	m_CloseServer.EnableWindow(FALSE);

	//*******************
	//*****�رշ�����*****
	//*******************
	closesocket(main_sock);

	main_sock = 0;

	//����û�����
	int i, j;
	for (i = 0; i < USER_LIST_SIZE; i++){
		//����û�i���ڣ�������ر��׽���
		if (user_list[i] != NULL){
			//����׽��ֶ�������Ϊ�㣬������ر��׽���
			for (j = 0; (user_list[i]->sock_num != 0) && (j < SOCK_LIST_SIZE); j++){
				if (user_list[i]->sock_list[j] != 0){
					closesocket(user_list[i]->sock_list[j]);
					user_list[i]->sock_list[j] = 0;
					if (user_list[i]->file_pointer[j] != NULL){
						fclose(user_list[i]->file_pointer[j]);
					}
					user_list[i]->sock_num--;
				}
			}
			delete user_list[i];
			user_num--;
		}
	}

	ShowInfo(TEXT("�������ѹر�"));

	WSACleanup();
	//�ı䰴��״̬
	m_StartServer.EnableWindow(TRUE);
}


LRESULT CServerDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO: Add your specialized code here and/or call the base class
	SOCKET sock;
	int Event;
	int buf_len;
	int retval;
	char recv_buf[BUF_SIZE];

	switch (message){
	case UM_SOCK:
		sock = (SOCKET)LOWORD(wParam);
		Event = LOWORD(lParam);
		switch (Event){
		case FD_ACCEPT:
			AcceptNewUser(sock);
			break;
		case FD_READ:
			//�������ݣ������в���ж�
			buf_len = Receive(sock, recv_buf, BUF_SIZE);
			if (buf_len <= 0){
				if (buf_len == -1)
					retval = CloseCtlSock(sock);
				break;
			}

			char type;
			type = recv_buf[0];

			//�ж��յ���PDU�����ͣ����ദ��
			switch (type){
			case INQUIRY_PDU_TYPE:
				struct InquiryPDU *inq_pdu;
				inq_pdu = (struct InquiryPDU *)recv_buf;
				
				RecvInquiry(sock, inq_pdu);

				inq_pdu = NULL;
				break;
			case CTL_PDU_TYPE:
				struct CtlPDU *ctl_pdu;
				ctl_pdu = (struct CtlPDU *)recv_buf;
				
				RecvCtl(sock, ctl_pdu);

				ctl_pdu = NULL;
				break;
			}
			break;
		}
		break;

	case UM_FL_SOCK:
		sock = (SOCKET)LOWORD(wParam);
		Event = LOWORD(lParam);
		switch (Event){
		case FD_READ:
			SendFilelist(sock);
			break;
		case FD_CLOSE:
			ShowInfo(TEXT("�ļ��б������"));
			break;
		}
		break;
		
	case UM_SEND_FILE_SOCK:
		//�ļ�����
		sock = (SOCKET)LOWORD(wParam);
		Event = LOWORD(lParam);
		switch (Event){
		case FD_READ:
			SendFile(sock);
			break;
		case FD_CLOSE:
			ShowInfo(TEXT("�����ļ��������"));
			//���û��б����ҵ���Ӧ�û��׽���
			int i;
			int j;
			Find_ij(sock, i, j);

			UINT_PTR nIDEvent = (i + 1)*SOCK_LIST_SIZE + j;
			KillTimer(nIDEvent);

			user_list[i]->DeleteSockList(j);
			break;
		}
		break;

	case UM_RECV_FILE_SOCK:
		//�ļ�����
		sock = (SOCKET)LOWORD(wParam);
		Event = LOWORD(lParam);
		switch (Event){
		case FD_READ:
			//�����ļ������������û��ϴ����ļ�
			RecvFile(sock);
			break;
		case FD_CLOSE:
			ShowInfo(TEXT("�����ļ��������"));
			break;
		}
		break;
	}
	return CDialogEx::WindowProc(message, wParam, lParam);
}

//��������ʾ��Ϣ
//���ܣ�����Ϣ�������ΪINFO_LIST��listbox�ؼ���
//		��ʾһЩϵͳ������Ϣ�������û�������Ϣ
void CServerDlg::ShowInfo(CString text)
{
	m_InfoList.AddString(text);
}


short CServerDlg::Receive(SOCKET sock, char* recv_buf, short buf_size)
{
	int retval;
	short buf_len;

	buf_len = recv(sock, recv_buf, buf_size, 0);
	if (buf_len <= 0){
		retval = WSAGetLastError();
		if (retval != WSAEWOULDBLOCK){
			closesocket(sock);
			MessageBox(TEXT("���ӹرգ�"), TEXT("�׽��ִ���"), MB_OK);
			ShowInfo(TEXT("�׽��ִ������ӹرգ�"));
			return -1;
		}
		else{
			//���������µĴ��󣬼������м���
			return 0;
		}
	}
	recv_buf[buf_len] = 0;

	return buf_len;
}


//�����������ļ��׽���
//���ܣ������׽��֣����û������У��ҵ���Ӧ�û���Ӧ�׽���
//���룺�׽��� sock
//������û���־ i�� �׽��ֱ�־ j
void CServerDlg::Find_ij(SOCKET sock, int &i, int &j)
{
	for (i = 0; i < USER_LIST_SIZE; i++){
		for (j = 0; j < SOCK_LIST_SIZE; j++){
			if (user_list[i]->sock_list[j] == sock){
				return;
			}
		}
	}
}


//�������ر�ָ�������׽���
//���ܣ������û��б��ҵ�ָ���׽��֣��رղ�ɾ����Ӧ�û��Ļ�����Ϣ
//���룺�׽��� sock
//����� i (>=0) �ɹ��� -1 ʧ��
short CServerDlg::CloseCtlSock(SOCKET sock)
{
	int i;

	for (i = 0; i < USER_LIST_SIZE; i++){
		if (user_list[i] != NULL && user_list[i]->ctl_sock == sock){
			user_list[i]->DeleteUserUnit();
			delete user_list[i];
			user_list[i] = NULL;
			return i;
		}
	}
	return -1;
}





//�������������û�
//���ܣ������µ����ӽ�����������û�����ʼ��
void CServerDlg::AcceptNewUser(SOCKET sock)
{
	int i;
	int len;
	int retval;
	sockaddr_in remote;
	SOCKET new_sock;

	len = sizeof(remote);
	new_sock = accept(sock, (sockaddr*)&remote, &len);
	if (new_sock == SOCKET_ERROR){
		ShowInfo(TEXT("�׽��ִ�����������رգ�"));
		OnClickedCloseServer();
		return;
	}

	ShowInfo(TEXT("���յ�һ�������ӣ�"));
	WSAAsyncSelect(new_sock, m_hWnd, UM_SOCK, FD_READ | FD_CLOSE);

	retval = RequstAccount(new_sock);
	if (retval != 2){
		//����
		return;
	}
	
	for (i = 0; i < USER_LIST_SIZE; i++){
		if (user_list[i] != NULL){
			continue;
		}
		//�½��û���Ϣ��Ԫ����ʼ�������ڣ��ѿ����׽���״̬ΪS_AUTHSTART
		user_list[i] = new class UserUnit(0, new_sock, remote.sin_addr.S_un.S_addr, remote.sin_port);
		break;
	}
}

//����������CTL����
//���ܣ�����Ctl�����е�cmd�ֶ�ֵ���ַ���ͬ�Ĵ�����
//
void CServerDlg::RecvCtl(SOCKET sock, struct CtlPDU *ctl_pdu)
{
	//�ж��յ��ı����У����������
	switch (ctl_pdu->cmd){
	case R_ACCOUNT:
		//�ظ����ʺ�
		RecvAccount(sock, ctl_pdu);
		break;
	case R_FILELIST:
		//�����ļ��б�
		StartSendFilelist(sock);
		break;
	case R_DOWNFILE:
		//���������ļ�
		StartSendFile(sock, ctl_pdu->buf);
		break;
	case R_UPFILE:
		//�����ϴ��ļ�
		StartRecvFile(sock, ctl_pdu->buf);
		break;
	}
}


//���������͡�ѯ���˺š�����
//���ܣ��½�CtlPDU������type�ֶ�ΪCTL_PDU_TYPE��cmd�ֶ�ΪR_ACCOUNT������len�ֶ�Ϊ0��
//		���׽���sock��Ӧ�û�������2���ֽ�
int CServerDlg::RequstAccount(SOCKET sock)
{
	char* send_buf;
	struct CtlPDU ctl_pdu;
	int retval;

	ctl_pdu.type = CTL_PDU_TYPE;
	ctl_pdu.cmd = R_ACCOUNT;
	ctl_pdu.len = 0;
	send_buf = (char *)&ctl_pdu;
	retval = send(sock, send_buf, 2, 0);
	//delete ctl_pdu;
	send_buf = NULL;

	return retval;
}

//�����������û��ʺ���Ϣ
//���ܣ�����û��ʺ��Ƿ���ڣ������ڣ�������ѯ�����������ڣ����ʹ����ٴ������ʺ�
//���룺
//�����-1����ʧ��
short CServerDlg::RecvAccount(SOCKET s, struct CtlPDU *ctl_pdu)
{
	short retval = -1;
	long inquiry_num;
	unsigned long ip;
	FILE *fp;
	unsigned long ID, secret;
	char userlist_buf[CTL_BUF_SIZE];

	//char *tmp = (char *)malloc(CTL_BUF_SIZE);
	ctl_pdu->buf[ctl_pdu->len] = 0;		////////////////filename-->buf or name
	fopen_s(&fp, "C:\\UserList.txt", "rt");
	while (!feof(fp)){
		//��ȡ�û�ID����ת��Ϊlong����
		fgets(userlist_buf, CTL_BUF_SIZE, fp);
		ID = atol(userlist_buf);
		//��ȡ�û��ʺţ����Ƚ�
		fgets(userlist_buf, CTL_BUF_SIZE, fp);
		userlist_buf[strlen(userlist_buf) - 1] = 0;
		if (strcmp(userlist_buf, ctl_pdu->buf) == 0){
			//��ȡ���룬��ת��Ϊlong����
			fgets(userlist_buf, CTL_BUF_SIZE, fp);
			fclose(fp);
			secret = atol(userlist_buf);

			struct InquiryPDU *p_pdu;
			int i;
			unsigned char bufflen;
			p_pdu = new struct InquiryPDU;
			p_pdu->type = INQUIRY_PDU_TYPE;
			for (i = 0; i < USER_LIST_SIZE; i++){
				if (user_list[i]->ctl_sock == s){
					break;
				}
			}
			unsigned long mychap;
			mychap = user_list[i]->f_producenum(p_pdu, secret, &bufflen);
			//CString outprint;
			//outprint.Format(retval);
			//ShowInfo(outprint);

			char *send_buf;
			send_buf = (char*)p_pdu;
			retval = send(s, send_buf, sizeof(InquiryPDU), 0);
			if (retval){
				//�жϷ��ͺ����ķ���ֵ

			}
			delete p_pdu;
			send_buf = NULL;

			user_list[i]->user_name = ctl_pdu->buf;
			user_list[i]->ctl_sock_status = S_AUTHING;
			user_list[i]->user_id = ID;				//////////////////////////////////////////BUG�޸�

			break;
		}
		fgets(userlist_buf, CTL_BUF_SIZE, fp);
	}
}

//�����������ļ��б�������
//���ܣ��յ��ļ��б�������½��׽��֣����͵�һ���б����ļ���Ϣ�������׽���״̬ΪS_SNDFL0
//���룺
//�����
short CServerDlg::StartSendFilelist(SOCKET s)
{
	int i;
	//char root[] = "C://ServerData";

	long retval;
	SOCKET file_sock;
	int len;
	struct sockaddr_in cli_addr;
	struct FilePDU *file_pdu;

	for (i = 0; i < USER_LIST_SIZE; i++){
		if (user_list[i]->ctl_sock == s){
			break;
		}
	}
	
	pstr = _T("C:\\ServerData");

	// build a string with wildcards
	CString strWildcard(pstr);
	strWildcard += _T("\\*.*");

	CString filename;
	// start working for files
	bWorking = user_list[i]->fl_finder.FindFile(strWildcard);
	while (bWorking){
		bWorking = user_list[i]->fl_finder.FindNextFile();
		if (user_list[i]->fl_finder.IsDots()){
			continue;
		}
		else if (user_list[i]->fl_finder.IsDirectory()){
			filename = user_list[i]->fl_finder.GetFilePath();
			break;
		}
		else{
			filename = user_list[i]->fl_finder.GetFileName();
			break;
		}
	}
	
	//�༭PDU
	file_pdu = (struct FilePDU*)malloc(sizeof(struct FilePDU));
	file_pdu->type = FILE_PDU_TYPE;
	file_pdu->No = GET_FILELIST_START;

	ShowInfo(filename);

	strcpy(file_pdu->buf, filename);
	file_pdu->len = strlen(file_pdu->buf);
	

	//��������
	cli_addr.sin_family = AF_INET;
	cli_addr.sin_addr.S_un.S_addr = htonl(INADDR_LOOPBACK);		//
	cli_addr.sin_port = htons(CLIENT_PORT);

	file_sock = socket(AF_INET, SOCK_STREAM, 0);
	len = sizeof(cli_addr);
	retval = connect(file_sock, (struct sockaddr *)&cli_addr, len);

	//send_buf = (char*)file_pdu;
	retval = send(file_sock, (char*)file_pdu, 1 + 1 + 1 + file_pdu->len, 0);
	if (retval < 1 + 1 + file_pdu->len){
		//����ж�
		ShowInfo(TEXT("�����ļ��б���֡����"));
	}

	free(file_pdu);

	//�����׽��ֲ��뵽�����У��ı�״̬ΪS_SNDFLSTART
	//user_list[i]->InsertSocketList(file_sock, S_SNDFL0, handle);
	user_list[i]->fl_sock = file_sock;
	user_list[i]->fls_status = S_SENDFL_START;

	WSAAsyncSelect(file_sock, m_hWnd, UM_FL_SOCK, FD_READ | FD_CLOSE);

	return i;
}


//�����������ļ��б���
//���ܣ��յ��û�ack�ɹ�������һ���ļ��б����ݺ󣬷�����һ���ļ��б�����
//���룺�׽��� s
//����� i (>=0) �ɹ��� -1 ʧ��
short CServerDlg::SendFilelist(SOCKET s)
{
	int i; 
	int retval;
	int buf_len;
	char tmp_No;
	char* recv_buf;
	FilePDU *file_pdu;
	CString filename;

	//���Ҷ�Ӧ�û�
	for (i = 0; i < USER_LIST_SIZE; i++){
		if (user_list[i]->fl_sock == s){
			break;
		}
	}
	if (i == USER_LIST_SIZE){
		ShowInfo(TEXT("�Ҳ�����Ӧ�û���"));
		return -1;
	}

	recv_buf = (char*)malloc(BUF_SIZE);
	//�������ݣ������в���ж�
	buf_len = Receive(s, recv_buf, BUF_SIZE);
	if (buf_len <= 0){
		if (buf_len == -1)
			user_list[i]->DeleteFilelistSock();
		free(recv_buf);
		return -1;
	}



	//����ת��
	file_pdu = (FilePDU *)recv_buf;

	//�������ݸ�ʽ
	if (file_pdu->type != FILE_PDU_TYPE){
		////////////////////////////////////////////////����
		return -1;
	}
	if (file_pdu->len != 3){
		return -1;
	}
	CString ack;
	ack = "ACK";
	if (file_pdu->buf != ack){
		return -1;
	}


	switch (user_list[i]->fls_status){
	case S_SENDFL_START:
		if (file_pdu->No == 1){
			//////////////////////////////////////////δ����
			return -1;
		}
		tmp_No = 1;
		user_list[i]->fls_status = S_SENDFL_1;
		break;
	case S_SENDFL_0:
		if (file_pdu->No == 1){
			//////////////////////////////////////////δ����
			tmp_No = 0;
			//user_list[i]->fls_handle;
			break;
		}
		tmp_No = 1;
		user_list[i]->fls_status = S_SENDFL_1;
		break;
	case S_SENDFL_1:
		if (file_pdu->No == 0){
			/////////////////////////////////////////!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!δ����
			tmp_No = 1;
			//user_list[i]->fls_handle;
			break;
		}
		tmp_No = 0;
		user_list[i]->fls_status = S_SENDFL_0;
		break;
	case S_CLOSING:
		free(recv_buf);
		user_list[i]->fl_finder.Close();
		return 0;
	default:
		free(recv_buf);
		user_list[i]->fl_finder.Close();
		return -1;
	}
	free(recv_buf);



	//��д��һ���ļ��б�����FilePDU
	file_pdu = (FilePDU *)malloc(sizeof(FilePDU));
	file_pdu->type = FILE_PDU_TYPE;
	file_pdu->No = tmp_No;

	//��֮ǰһ���ж��Ƿ��Ѿ��������
	if (bWorking == 0){
		//����֡
		file_pdu->buf[0] = 0;
		user_list[i]->fls_status = S_CLOSING;
	}
	while (bWorking){
		bWorking = user_list[i]->fl_finder.FindNextFile();
		if (user_list[i]->fl_finder.IsDots()){
			continue;
		}
		else if (user_list[i]->fl_finder.IsDirectory()){
			filename = user_list[i]->fl_finder.GetFilePath();
			strcpy_s(file_pdu->buf, FILE_BUF_SIZE, user_list[i]->fl_finder.GetFileName());
			break;
		}
		else{
			filename = user_list[i]->fl_finder.GetFileName();
			strcpy_s(file_pdu->buf, FILE_BUF_SIZE, user_list[i]->fl_finder.GetFileName());
			break;
		}
	}
	file_pdu->len = strlen(file_pdu->buf);

	buf_len = 1 + 1 + 1 + file_pdu->len;

	retval = send(s, (char*)file_pdu, buf_len, 0);
	if (retval <= 0){
		//����ֵ�ж�
		MessageBox(TEXT("�������ӷ�������PDU��"), TEXT("�������Ӵ���"), MB_OK);
		return retval;
	}

	free(file_pdu);
	file_pdu = NULL;

	return i;
}



//������������ѯֵ
//���ܣ��ȽϽ��յ�����ѯֵ�뻺�����û���Ӧ����ѯֵ��һ�����¼�ɹ�����һ�������µ�¼
//���룺
//�����i�����û������е�λ�ã�����ɹ���-1�����ɹ�
short CServerDlg::RecvInquiry(SOCKET s, struct InquiryPDU *inq_pdu)
{
	int i;
	short x;
	short sum;
	long inq_num=0;

	for (i = 0; i < USER_LIST_SIZE; i++){
		if (user_list[i]->ctl_sock == s){
			break;
		}
	}

	//���ҵ��û����ȼ��PDU��ʽ
	if (inq_pdu->num == 0){
		//����ѯ����ֵ���ַ���תΪlong��
		//inq_num = atol(inq_pdu->each_len);////////////////////////////////////////////////////////////////////////

		for (char i = 0; i < 4; i++)
		{
			inq_num = (inq_num << 8);
			inq_num |= (0x000000FF)&(inq_pdu->each_len[i]);
		}
		//�Ա��û���������ѯ�������û��б��л���ֵ�Ƿ���ͬ
		if (user_list[i]->inquiry_num == inq_num){
			//��ͬ����ͨ��
			user_list[i]->ctl_sock_status = S_AUTHED;

			SendCtlPDU(s, RE_ACCESS);

			ShowInfo(TEXT("��֤�ɹ���"));
			return i;
		}
		else{
			//��ͬ���򱨴�
			ShowInfo(TEXT("INQ_PDU ERROR : Wrong inquiry num !\n"));
		}
	}
	else{
		ShowInfo(TEXT("INQ_PDU ERROR : Wrong inq_pdu type !\n"));
	}
	//pdu�����Ž��е���һ�����ڴ˷��ʹ�����

	//ʧ��
	ShowInfo(TEXT("��֤ʧ�ܣ�"));
	//��ѯ����
	SendCtlPDU(s, RE_ERR_INQUIRY);
	//�����ʺ�
	SendCtlPDU(s, REQ_ACCOUNT);
	return -1;
}

//���������ͻظ�
//���ܣ��½�CtlPDU������type�ֶ�ΪCTL_PDU_TYPE������len�ֶ�Ϊ0��
//		���׽���sock��Ӧ�û�������2���ֽ�
int CServerDlg::SendCtlPDU(SOCKET sock, char cmd)
{
	char* send_buf;
	struct CtlPDU ctl_pdu;
	int retval;

	ctl_pdu.type = CTL_PDU_TYPE;
	ctl_pdu.cmd = cmd;
	ctl_pdu.len = 0;
	send_buf = (char *)&ctl_pdu;
	retval = send(sock, send_buf, 2, 0);
	//delete ctl_pdu;
	send_buf = NULL;

	return retval;
}




/////////////////////////////////////////////////////////////////////////
//�����������ļ���������
//���ܣ��յ��ļ��������󣬼����ļ����Ϸ��ԣ����ͺϷ��ļ������ļ���С
//���룺
//�����
short CServerDlg::StartSendFile(SOCKET s, char *filename)
{
	int i, j;
	int len;
	long retval;
	SOCKET send_sock;
	struct sockaddr_in cli_addr;
	char file_dir[FILE_BUF_SIZE] = "C:\\ServerData\\";
	char *send_buf;
	FILE *fp;

	for (i = 0; i < USER_LIST_SIZE; i++){
		if (user_list[i]->ctl_sock == s){
			break;
		}
	}

	strcat_s(file_dir, FILE_BUF_SIZE, filename);
	
	fopen_s(&fp, file_dir, "rb");
	if (fp == NULL){
		ShowInfo(TEXT("�����ļ������ڣ��޷����ͣ�"));

		//��֪�û���������
		SendCtlPDU(s, RE_ERR_FILENAME);
		return -1;
	}

	//������ʱ�ļ�������������������������������������������������������������������

	cli_addr.sin_family = AF_INET;
	cli_addr.sin_addr.S_un.S_addr = user_list[i]->user_ip;
	cli_addr.sin_port = htons(CLIENT_PORT);

	send_sock = socket(AF_INET, SOCK_STREAM, 0);
	len = sizeof(cli_addr);
	retval = connect(send_sock, (struct sockaddr *)&cli_addr, len);

	//����Ҫ���ݴ����û���Ϣ��Ԫ��
	j = user_list[i]->InsertSocketList(send_sock, S_SENDFILE_START, fp);	//fp!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	WSAAsyncSelect(send_sock, m_hWnd, UM_SEND_FILE_SOCK, FD_READ | FD_CLOSE);

	//������֡
	struct FilePDU file_pdu;
	file_pdu.type = FILE_PDU_TYPE;
	file_pdu.No = DOWN_FILE_START;
	strcpy(file_pdu.buf, filename);
	file_pdu.len = strlen(filename);

	retval = send(send_sock, (char *)&file_pdu, 1 + 1 + 1 + file_pdu.len, 0);
	if (retval < 1 + 1 + 1 + file_pdu.len){
		//����ж�
		ShowInfo(TEXT("������֡ʧ�ܣ�"));
		SendCtlPDU(send_sock, RE_ERR_DOWNFILE);
		//////////////////////////////////////////////////////////�ر��׽���
		return retval;
	}

	user_list[i]->last_len[j] = retval - 3;

	UINT timer_id = (i + 1)*SOCK_LIST_SIZE + j;
	SetTimer(timer_id, TIMEOUT, 0);

	return i;
}


//�����������ļ�
//���ܣ��õ���һ֡ACK�ظ�֮�󣬷�����һ֡�����ļ�һ����β������len=0��֡��ʾ����
//
//
short CServerDlg::SendFile(SOCKET sock)
{
	int i, j;
	int retval;
	int buf_len;
	char tmp_No;
	char* recv_buf;
	char *send_buf;
	FilePDU *file_pdu;

	//���û��б����ҵ���Ӧ�û��׽���
	Find_ij(sock, i, j);

	recv_buf = (char*)malloc(BUF_SIZE);
	//�������ݣ������в���ж�
	buf_len = Receive(sock, recv_buf, BUF_SIZE);
	if (buf_len <= 0){
		if (buf_len == -1)
			user_list[i]->DeleteSockList(j);
		free(recv_buf);
		return -1;
	}

	//��ʽת��
	file_pdu = (FilePDU *)recv_buf;

	//��ʽУ��
	if (file_pdu->type != FILE_PDU_TYPE){
		////////////////////////////////////////////////����
		return -1;
	}
	if (file_pdu->len != 3){
		return -1;
	}
	CString ack;
	ack = "ACK";
	if (file_pdu->buf != ack){
		return -1;
	}


	switch (user_list[i]->sock_status[j]){
	case S_SENDFILE_START:
		if (file_pdu->No == 1){
			//////////////////////////////////////////
			return -1;
		}
		tmp_No = 1;
		user_list[i]->sock_status[j] = S_SENDFILE_1;
		break;
	case S_SENDFILE_0:
		if (file_pdu->No == 1){
			//////////////////////////////////////////
			tmp_No = 0;
			buf_len = user_list[i]->last_len[j];
			fseek(user_list[i]->file_pointer[j], -buf_len, 1);
			break;
		}
		tmp_No = 1;
		user_list[i]->sock_status[j] = S_SENDFILE_1;
		break;
	case S_SENDFILE_1:
		if (file_pdu->No == 0){
			/////////////////////////////////////////
			tmp_No = 1;
			buf_len = user_list[i]->last_len[j];
			fseek(user_list[i]->file_pointer[j], -buf_len, 1);
			break;
		}
		tmp_No = 0;
		user_list[i]->sock_status[j] = S_SENDFILE_0;
		break;
	case S_CLOSING:
		user_list[i]->DeleteSockList(j);
		break;
	default:
		free(recv_buf);
		return -1;
	}
	free(recv_buf);

	//�½��ļ�PDU
	file_pdu = (FilePDU *)malloc(sizeof(FilePDU));

	file_pdu->type = FILE_PDU_TYPE;
	file_pdu->No = tmp_No;

	if (feof(user_list[i]->file_pointer[j])){
		file_pdu->buf[0] = 0;
		file_pdu->len = 0;
		retval = 0;
		user_list[i]->sock_status[j] = S_CLOSING;
		user_list[i]->last_len[j] = tmp_No;
		ShowInfo(TEXT("�ļ�������ɣ�"));
	}
	else{
		retval = fread(file_pdu->buf, 1, FILE_BUF_SIZE, user_list[i]->file_pointer[j]);
		file_pdu->len = retval;
	}

	buf_len = 1 + 1 + 1 + retval;

	retval = send(sock, (char*)file_pdu, buf_len, 0);
	if (retval <= 0){
		//����ֵ�ж�
		MessageBox(TEXT("�������ӷ�������PDU��"), TEXT("�������Ӵ���"), MB_OK);
		return retval;
	}

	UINT timer_id = (i + 1)*SOCK_LIST_SIZE + j;
	SetTimer(timer_id, TIMEOUT, 0);

	user_list[i]->last_len[j] = retval - 3;

	free(file_pdu);
	file_pdu = NULL;
	send_buf = NULL;
	return i;
}




//��������ʼ�����ļ�
//���ܣ������ϴ��ļ������½��׽��֣������ļ���Ϣ
//���룺
//�����
short CServerDlg::StartRecvFile(SOCKET s, char *filename)
{
	int i;
	int len;
	long retval;
	int buf_len;
	//short x;
	SOCKET recv_sock;
	struct sockaddr_in cli_addr;
	char file_dir[100] = "C://ServerData//";
	char *send_buf;
	FILE *fp;

	//�ҵ����û����б���λ��
	for (i = 0; i < USER_LIST_SIZE; i++){
		if (user_list[i]->ctl_sock == s){
			break;
		}
	}

	strcat_s(file_dir, 100, filename);
	//�����ļ��Ƿ���ڣ������ڣ�����ֹ����֪�û�����
	if (_access(file_dir, 0) == 0){
		//CString show_info;
		//show_info += file_dir;
		//show_info += "�Ѵ��ڣ�";
		ShowInfo(TEXT("�����ļ��Ѵ��ڣ��޷����գ�"));
		SendCtlPDU(s, RE_ERR_FILENAME);
		return -1;
	}

	fopen_s(&fp, file_dir, "wb");

	//������ʱ�ļ�������������������������������������������������������������������

	cli_addr.sin_family = AF_INET;
	cli_addr.sin_addr.S_un.S_addr = user_list[i]->user_ip;
	cli_addr.sin_port = htons(CLIENT_PORT);

	recv_sock = socket(AF_INET, SOCK_STREAM, 0);
	len = sizeof(cli_addr);
	retval = connect(recv_sock, (struct sockaddr *)&cli_addr, len);

	//������֡
	struct FilePDU file_pdu;
	file_pdu.type = FILE_PDU_TYPE;
	file_pdu.No = UP_FILE_START;
	strcpy(file_pdu.buf, filename);
	file_pdu.len = strlen(filename);
	
	buf_len = 1 + 1 + 1 + file_pdu.len;
	retval = send(recv_sock, (char *)&file_pdu, buf_len, 0);
	if (retval < 1+1+1+file_pdu.len){
		//������֡ʧ��
		ShowInfo(TEXT("������֡ʧ�ܣ�"));
		//�ر��׽���
		closesocket(recv_sock);
		return retval;
	}

	//����Ҫ���ݴ����û���Ϣ��Ԫ��
	user_list[i]->InsertSocketList(recv_sock, S_RECVFILE_START, fp);
	WSAAsyncSelect(recv_sock, m_hWnd, UM_RECV_FILE_SOCK, FD_READ | FD_CLOSE);

	return i;
}

//�����������ļ�
//���ܣ����յ�һ֡�Ϸ�������֡�󣬷���ACK�����յ�len=0��֡����ر��׽��֡�
//���룺
//�����
short CServerDlg::RecvFile(SOCKET sock)
{
	int i, j;
	unsigned int buf_len;
	int retval;
	char tmp_No;
	char* recv_buf;
	struct FilePDU* file_pdu;

	//���û��б����ҵ���Ӧ�û��׽���
	Find_ij(sock, i, j);

	recv_buf = (char*)malloc(BUF_SIZE);
	//�������ݣ������в���ж�
	buf_len = Receive(sock, recv_buf, BUF_SIZE);
	if (buf_len <= 0){
		if (buf_len == -1)
			user_list[i]->DeleteSockList(j);
		free(recv_buf);
		return -1;
	}
	//��P_AbandRand���ʶ���
	if (AbandonRand(P_AbandRand) == 0){
		free(recv_buf);
		ShowInfo(TEXT("���Գ�ʱ�ط�������һ�Σ�"));
		return 0;
	}

	//file_pdu = (struct FilePDU*)malloc(sizeof(struct FilePDU));
	file_pdu = (struct FilePDU*)recv_buf;

	if (file_pdu->type != FILE_PDU_TYPE){
		ShowInfo(TEXT("�������Ͳ����Ͻ��������׽��֣�"));
		free(recv_buf);
		return -1;
	}

	//����յ��������ģ���ر�����׽���
	if (file_pdu->len == 0){
		user_list[i]->DeleteSockList(j);
		
		free(recv_buf);

		ShowInfo(TEXT("�ļ�������ɣ�"));
		return 0;
	}

	switch (user_list[i]->sock_status[j]){
	case S_RECVFILE_START:
		if (file_pdu->No == 1){
			///////////////////////////////////////////////////////������
			ShowInfo(TEXT("�������ݰ���Ŵ���"));
			free(recv_buf);
			///////////////////////////////////////////////////////���ǹرգ�
			return -1;
		}
		tmp_No = 0;
		user_list[i]->sock_status[j] = S_RECVFILE_0;
		break;
	case S_RECVFILE_1:
		if (file_pdu->No == 1){
			tmp_No = 1;
			//ShowInfo(TEXT("�������ݰ���Ŵ���"));
			break;
		}
		tmp_No = 0;
		user_list[i]->sock_status[j] = S_RECVFILE_0;
		break;
	case S_RECVFILE_0:
		if (file_pdu->No == 0){
			tmp_No = 0;
			//ShowInfo(TEXT("�������ݰ���Ŵ���"));
			break;
		}
		tmp_No = 1;
		user_list[i]->sock_status[j] = S_RECVFILE_1;
		break;
	default:
		ShowInfo(TEXT("�׽���״̬����"));
		free(recv_buf);
		return -1;
	}

	//buf_len = (unsigned int)file_pdu->len;
	retval = fwrite(file_pdu->buf, 1, file_pdu->len, user_list[i]->file_pointer[j]);
	if (retval < file_pdu->len){
		ShowInfo(TEXT("д�ļ�ʧ�ܣ�"));
		////////////////////////////////////////////////////////////////////////����ֵ�ж�
	}

	free(recv_buf);
	recv_buf = NULL;
	file_pdu = NULL;

	//���ͻظ�����
	file_pdu = (struct FilePDU*)malloc(sizeof(struct FilePDU));
	file_pdu->type = FILE_PDU_TYPE;
	file_pdu->No = tmp_No;		//(file_pdu->No + 1) % 2;
	file_pdu->len = 3;
	strcpy(file_pdu->buf, "ACK");
	buf_len = 1 + 1 + 1 + 3;
	retval = send(sock, (char*)file_pdu, buf_len, 0);
	if (retval < 0){
		/////////////////////////////////////////////////////����ֵ�ж�
		/////////////////////////////////////////////////////�ر��׽���
		free(file_pdu);
		file_pdu = NULL;
		return -1;
	}
	else if (retval < buf_len){
		user_list[i]->file_pointer[j] -= (buf_len - retval);
	}

	free(file_pdu);
	file_pdu = NULL;

	return retval;
}


void CServerDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	int i, j;

	i = nIDEvent / SOCK_LIST_SIZE - 1;
	j = nIDEvent % SOCK_LIST_SIZE;

	if (i == -1){
		ShowInfo(TEXT("��ʱ��"));
	}
	else if (i >= 0){
		SendFileAgain(i, j);
	}
	

	CDialogEx::OnTimer(nIDEvent);
}


short CServerDlg::SendFileAgain(int i, int j)
{
	int retval;
	unsigned char tmp_No;
	int buf_len;
	struct FilePDU* file_pdu;

	//�½��ļ�PDU
	file_pdu = (FilePDU *)malloc(sizeof(FilePDU));

	file_pdu->type = FILE_PDU_TYPE;

	switch (user_list[i]->sock_status[j]){
	case S_SENDFILE_START:
	case S_SENDFILE_0:
		file_pdu->No = 0;
		buf_len = user_list[i]->last_len[j];
		fseek(user_list[i]->file_pointer[j], -buf_len, 1);
		//user_list[i]->file_pointer[j] -= buf_len;
		break;
	case S_SENDFILE_1:
		file_pdu->No = 1;
		buf_len = user_list[i]->last_len[j];
		fseek(user_list[i]->file_pointer[j], -buf_len, 1);
		//user_list[i]->file_pointer[j] -= buf_len;
		break;
	case S_CLOSING:
		file_pdu->No = user_list[i]->last_len[j];
		buf_len = 0;
		break;
	default:
		return -1;
	}

	////////////////////////////////////
	retval = fread(file_pdu->buf, 1, buf_len, user_list[i]->file_pointer[j]);
	file_pdu->len = retval;

	buf_len = 1 + 1 + 1 + retval;

	retval = send(user_list[i]->sock_list[j], (char*)file_pdu, buf_len, 0);
	if (retval <= 0){
		//����ֵ�ж�
		MessageBox(TEXT("�����ط�ʧ�ܣ�"), TEXT("�ط�����"), MB_OK);
	}

	user_list[i]->last_len[j] = retval - 3;

	UINT timer_id = (i + 1)*SOCK_LIST_SIZE + j;
	SetTimer(timer_id, TIMEOUT, 0);

	free(file_pdu);
	file_pdu = NULL;
	return i;
}