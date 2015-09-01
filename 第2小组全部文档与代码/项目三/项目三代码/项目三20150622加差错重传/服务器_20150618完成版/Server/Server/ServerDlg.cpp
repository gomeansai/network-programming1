
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
//丢包函数！！
#define P_AbandRand 0.9

//0, 不发送；1，发送
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

	//从控件获取数据
	UpdateData(TRUE);
	//UpdateData(FALSE);

	//如果主套接字不为零，则服务器已经开启，报错。（由于已经在开启后设置按钮为不可按，则不会出现这个错误）
	if (main_sock != 0){
		MessageBox(TEXT("服务器已经开启！"), TEXT("服务器错误"), MB_OK);
		//m_InfoList.AddString(TEXT("启动错误：服务器已经开启！"));
		ShowInfo(TEXT("启动错误：服务器已经开启！"));
		return;
	}

	//如果端口号不符合要求，则报错。
	if (m_Port <= 0 || m_Port >= 65536){
		MessageBox(TEXT("请输入正确的端口号（0 < port < 65535）！"), TEXT("端口号错误"), 0);
		//m_InfoList.AddString(TEXT("启动错误：请输入正确的端口号（0 < port < 65535）！"));
		ShowInfo(TEXT("启动错误：请输入正确的端口号（0 < port < 65535）！"));
		return;
	}

	//*********************
	//******开启服务器******
	//*********************
	WSAData wsa;
	sockaddr_in ser_addr;
	int retval;

	WSAStartup(0x101, &wsa);

	//初始化主套接字
	//main_sock = socket(AF_INET, SOCK_DGRAM, 0);
	main_sock = socket(AF_INET, SOCK_STREAM, 0);
	ser_addr.sin_family = AF_INET;
	ser_addr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	ser_addr.sin_port = htons(m_Port);

	//绑定端口
	retval = bind(main_sock, (sockaddr *)&ser_addr, sizeof(ser_addr));
	if (retval == SOCKET_ERROR){
		ShowInfo(TEXT("main_sock绑定IP地址与端口号失败！"));
		return;
	}

	//开启监听
	retval = listen(main_sock, 5);
	if (retval == -1){
		MessageBox(_T("main_sock开启监听失败！"));
		return;
	}

	//注册套接字异步响应
	retval = WSAAsyncSelect(main_sock, m_hWnd, UM_SOCK, FD_ACCEPT);

	ShowInfo(TEXT("服务器已开启！"));

	//初始化用户队列
	int i;
	user_num = 0;
	for (i = 0; i < USER_LIST_SIZE; i++){
		user_list[i] = NULL;
	}

	//SetTimer(TIMER1, TIMEOUT, 0);
	//改变按键状态
	m_CloseServer.EnableWindow(TRUE);
}


void CServerDlg::OnClickedCloseServer()
{
	// TODO: Add your control notification handler code here
	m_CloseServer.EnableWindow(FALSE);

	//*******************
	//*****关闭服务器*****
	//*******************
	closesocket(main_sock);

	main_sock = 0;

	//清空用户队列
	int i, j;
	for (i = 0; i < USER_LIST_SIZE; i++){
		//如果用户i存在，则深入关闭套接字
		if (user_list[i] != NULL){
			//如果套接字队数量不为零，则遍历关闭套接字
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

	ShowInfo(TEXT("服务器已关闭"));

	WSACleanup();
	//改变按键状态
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
			//接收数据，并进行差错判断
			buf_len = Receive(sock, recv_buf, BUF_SIZE);
			if (buf_len <= 0){
				if (buf_len == -1)
					retval = CloseCtlSock(sock);
				break;
			}

			char type;
			type = recv_buf[0];

			//判断收到的PDU的类型，分类处理
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
			ShowInfo(TEXT("文件列表传输完毕"));
			break;
		}
		break;
		
	case UM_SEND_FILE_SOCK:
		//文件发送
		sock = (SOCKET)LOWORD(wParam);
		Event = LOWORD(lParam);
		switch (Event){
		case FD_READ:
			SendFile(sock);
			break;
		case FD_CLOSE:
			ShowInfo(TEXT("发送文件传输完毕"));
			//在用户列表中找到对应用户套接字
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
		//文件接收
		sock = (SOCKET)LOWORD(wParam);
		Event = LOWORD(lParam);
		switch (Event){
		case FD_READ:
			//接收文件函数，接收用户上传的文件
			RecvFile(sock);
			break;
		case FD_CLOSE:
			ShowInfo(TEXT("接收文件传输完毕"));
			break;
		}
		break;
	}
	return CDialogEx::WindowProc(message, wParam, lParam);
}

//函数：显示信息
//功能：将信息输出到名为INFO_LIST的listbox控件中
//		显示一些系统错误信息，或者用户数据信息
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
			MessageBox(TEXT("连接关闭！"), TEXT("套接字错误"), MB_OK);
			ShowInfo(TEXT("套接字错误：连接关闭！"));
			return -1;
		}
		else{
			//非阻塞导致的错误，继续运行即可
			return 0;
		}
	}
	recv_buf[buf_len] = 0;

	return buf_len;
}


//函数：查找文件套接字
//功能：根据套接字，从用户队列中，找到对应用户对应套接字
//输入：套接字 sock
//输出：用户标志 i， 套接字标志 j
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


//函数：关闭指定控制套接字
//功能：查找用户列表，找到指定套接字，关闭并删除对应用户的缓存信息
//输入：套接字 sock
//输出： i (>=0) 成功； -1 失败
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





//函数：接收新用户
//功能：接收新的连接建立，添加新用户，初始化
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
		ShowInfo(TEXT("套接字错误：连接意外关闭！"));
		OnClickedCloseServer();
		return;
	}

	ShowInfo(TEXT("接收到一个新连接！"));
	WSAAsyncSelect(new_sock, m_hWnd, UM_SOCK, FD_READ | FD_CLOSE);

	retval = RequstAccount(new_sock);
	if (retval != 2){
		//报错
		return;
	}
	
	for (i = 0; i < USER_LIST_SIZE; i++){
		if (user_list[i] != NULL){
			continue;
		}
		//新建用户信息单元，初始化函数内，已控制套接字状态为S_AUTHSTART
		user_list[i] = new class UserUnit(0, new_sock, remote.sin_addr.S_un.S_addr, remote.sin_port);
		break;
	}
}

//函数：分类CTL报文
//功能：根据Ctl报文中的cmd字段值，分发不同的处理函数
//
void CServerDlg::RecvCtl(SOCKET sock, struct CtlPDU *ctl_pdu)
{
	//判断收到的报文中，请求的类型
	switch (ctl_pdu->cmd){
	case R_ACCOUNT:
		//回复的帐号
		RecvAccount(sock, ctl_pdu);
		break;
	case R_FILELIST:
		//请求文件列表
		StartSendFilelist(sock);
		break;
	case R_DOWNFILE:
		//请求下载文件
		StartSendFile(sock, ctl_pdu->buf);
		break;
	case R_UPFILE:
		//请求上传文件
		StartRecvFile(sock, ctl_pdu->buf);
		break;
	}
}


//函数：发送“询问账号”请求
//功能：新建CtlPDU，填入type字段为CTL_PDU_TYPE，cmd字段为R_ACCOUNT，长度len字段为0；
//		向套接字sock对应用户，发送2个字节
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

//函数：接收用户帐号信息
//功能：检查用户帐号是否存在，若存在，发送质询数；若不存在，发送错误，再次请求帐号
//输入：
//输出：-1代表失败
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
		//读取用户ID，并转化为long整型
		fgets(userlist_buf, CTL_BUF_SIZE, fp);
		ID = atol(userlist_buf);
		//读取用户帐号，并比较
		fgets(userlist_buf, CTL_BUF_SIZE, fp);
		userlist_buf[strlen(userlist_buf) - 1] = 0;
		if (strcmp(userlist_buf, ctl_pdu->buf) == 0){
			//读取密码，并转化为long整型
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
				//判断发送函数的返回值

			}
			delete p_pdu;
			send_buf = NULL;

			user_list[i]->user_name = ctl_pdu->buf;
			user_list[i]->ctl_sock_status = S_AUTHING;
			user_list[i]->user_id = ID;				//////////////////////////////////////////BUG修改

			break;
		}
		fgets(userlist_buf, CTL_BUF_SIZE, fp);
	}
}

//函数：处理文件列表请求函数
//功能：收到文件列表请求后，新建套接字，发送第一份列表中文件信息，设置套接字状态为S_SNDFL0
//输入：
//输出：
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
	
	//编辑PDU
	file_pdu = (struct FilePDU*)malloc(sizeof(struct FilePDU));
	file_pdu->type = FILE_PDU_TYPE;
	file_pdu->No = GET_FILELIST_START;

	ShowInfo(filename);

	strcpy(file_pdu->buf, filename);
	file_pdu->len = strlen(file_pdu->buf);
	

	//建立连接
	cli_addr.sin_family = AF_INET;
	cli_addr.sin_addr.S_un.S_addr = htonl(INADDR_LOOPBACK);		//
	cli_addr.sin_port = htons(CLIENT_PORT);

	file_sock = socket(AF_INET, SOCK_STREAM, 0);
	len = sizeof(cli_addr);
	retval = connect(file_sock, (struct sockaddr *)&cli_addr, len);

	//send_buf = (char*)file_pdu;
	retval = send(file_sock, (char*)file_pdu, 1 + 1 + 1 + file_pdu->len, 0);
	if (retval < 1 + 1 + file_pdu->len){
		//差错判断
		ShowInfo(TEXT("发送文件列表首帧错误！"));
	}

	free(file_pdu);

	//将新套接字插入到队列中，改变状态为S_SNDFLSTART
	//user_list[i]->InsertSocketList(file_sock, S_SNDFL0, handle);
	user_list[i]->fl_sock = file_sock;
	user_list[i]->fls_status = S_SENDFL_START;

	WSAAsyncSelect(file_sock, m_hWnd, UM_FL_SOCK, FD_READ | FD_CLOSE);

	return i;
}


//函数：发送文件列表函数
//功能：收到用户ack成功接收上一份文件列表数据后，发送下一份文件列表数据
//输入：套接字 s
//输出： i (>=0) 成功； -1 失败
short CServerDlg::SendFilelist(SOCKET s)
{
	int i; 
	int retval;
	int buf_len;
	char tmp_No;
	char* recv_buf;
	FilePDU *file_pdu;
	CString filename;

	//查找对应用户
	for (i = 0; i < USER_LIST_SIZE; i++){
		if (user_list[i]->fl_sock == s){
			break;
		}
	}
	if (i == USER_LIST_SIZE){
		ShowInfo(TEXT("找不到对应用户！"));
		return -1;
	}

	recv_buf = (char*)malloc(BUF_SIZE);
	//接收数据，并进行差错判断
	buf_len = Receive(s, recv_buf, BUF_SIZE);
	if (buf_len <= 0){
		if (buf_len == -1)
			user_list[i]->DeleteFilelistSock();
		free(recv_buf);
		return -1;
	}



	//类型转换
	file_pdu = (FilePDU *)recv_buf;

	//检验数据格式
	if (file_pdu->type != FILE_PDU_TYPE){
		////////////////////////////////////////////////报错
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
			//////////////////////////////////////////未回退
			return -1;
		}
		tmp_No = 1;
		user_list[i]->fls_status = S_SENDFL_1;
		break;
	case S_SENDFL_0:
		if (file_pdu->No == 1){
			//////////////////////////////////////////未回退
			tmp_No = 0;
			//user_list[i]->fls_handle;
			break;
		}
		tmp_No = 1;
		user_list[i]->fls_status = S_SENDFL_1;
		break;
	case S_SENDFL_1:
		if (file_pdu->No == 0){
			/////////////////////////////////////////!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!未回退
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



	//编写下一份文件列表数据FilePDU
	file_pdu = (FilePDU *)malloc(sizeof(FilePDU));
	file_pdu->type = FILE_PDU_TYPE;
	file_pdu->No = tmp_No;

	//由之前一个判断是否已经遍历完成
	if (bWorking == 0){
		//结束帧
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
		//返回值判定
		MessageBox(TEXT("数据连接发送数据PDU！"), TEXT("数据连接错误"), MB_OK);
		return retval;
	}

	free(file_pdu);
	file_pdu = NULL;

	return i;
}



//函数：接收质询值
//功能：比较接收到的质询值与缓存中用户对应的质询值，一致则登录成功，不一致则重新登录
//输入：
//输出：i（在用户队列中的位置）代表成功，-1代表不成功
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

	//若找到用户，先检查PDU格式
	if (inq_pdu->num == 0){
		//将质询返回值由字符串转为long型
		//inq_num = atol(inq_pdu->each_len);////////////////////////////////////////////////////////////////////////

		for (char i = 0; i < 4; i++)
		{
			inq_num = (inq_num << 8);
			inq_num |= (0x000000FF)&(inq_pdu->each_len[i]);
		}
		//对比用户发来的质询数，与用户列表中缓存值是否相同
		if (user_list[i]->inquiry_num == inq_num){
			//相同，则通过
			user_list[i]->ctl_sock_status = S_AUTHED;

			SendCtlPDU(s, RE_ACCESS);

			ShowInfo(TEXT("认证成功！"));
			return i;
		}
		else{
			//不同，则报错
			ShowInfo(TEXT("INQ_PDU ERROR : Wrong inquiry num !\n"));
		}
	}
	else{
		ShowInfo(TEXT("INQ_PDU ERROR : Wrong inq_pdu type !\n"));
	}
	//pdu出错，才进行到这一步，在此发送错误报文

	//失败
	ShowInfo(TEXT("认证失败！"));
	//质询错误
	SendCtlPDU(s, RE_ERR_INQUIRY);
	//请求帐号
	SendCtlPDU(s, REQ_ACCOUNT);
	return -1;
}

//函数：发送回复
//功能：新建CtlPDU，填入type字段为CTL_PDU_TYPE，长度len字段为0；
//		向套接字sock对应用户，发送2个字节
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
//函数：处理文件下载请求
//功能：收到文件下载请求，检验文件名合法性，会送合法文件名，文件大小
//输入：
//输出：
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
		ShowInfo(TEXT("发送文件不存在！无法发送！"));

		//告知用户发生错误
		SendCtlPDU(s, RE_ERR_FILENAME);
		return -1;
	}

	//生成临时文件！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！

	cli_addr.sin_family = AF_INET;
	cli_addr.sin_addr.S_un.S_addr = user_list[i]->user_ip;
	cli_addr.sin_port = htons(CLIENT_PORT);

	send_sock = socket(AF_INET, SOCK_STREAM, 0);
	len = sizeof(cli_addr);
	retval = connect(send_sock, (struct sockaddr *)&cli_addr, len);

	//将必要数据存入用户信息单元中
	j = user_list[i]->InsertSocketList(send_sock, S_SENDFILE_START, fp);	//fp!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	WSAAsyncSelect(send_sock, m_hWnd, UM_SEND_FILE_SOCK, FD_READ | FD_CLOSE);

	//发送首帧
	struct FilePDU file_pdu;
	file_pdu.type = FILE_PDU_TYPE;
	file_pdu.No = DOWN_FILE_START;
	strcpy(file_pdu.buf, filename);
	file_pdu.len = strlen(filename);

	retval = send(send_sock, (char *)&file_pdu, 1 + 1 + 1 + file_pdu.len, 0);
	if (retval < 1 + 1 + 1 + file_pdu.len){
		//差错判断
		ShowInfo(TEXT("发送首帧失败！"));
		SendCtlPDU(send_sock, RE_ERR_DOWNFILE);
		//////////////////////////////////////////////////////////关闭套接字
		return retval;
	}

	user_list[i]->last_len[j] = retval - 3;

	UINT timer_id = (i + 1)*SOCK_LIST_SIZE + j;
	SetTimer(timer_id, TIMEOUT, 0);

	return i;
}


//函数：发送文件
//功能：得到上一帧ACK回复之后，发送下一帧，若文件一道结尾，则发送len=0的帧表示结束
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

	//在用户列表中找到对应用户套接字
	Find_ij(sock, i, j);

	recv_buf = (char*)malloc(BUF_SIZE);
	//接收数据，并进行差错判断
	buf_len = Receive(sock, recv_buf, BUF_SIZE);
	if (buf_len <= 0){
		if (buf_len == -1)
			user_list[i]->DeleteSockList(j);
		free(recv_buf);
		return -1;
	}

	//格式转换
	file_pdu = (FilePDU *)recv_buf;

	//格式校验
	if (file_pdu->type != FILE_PDU_TYPE){
		////////////////////////////////////////////////报错
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

	//新建文件PDU
	file_pdu = (FilePDU *)malloc(sizeof(FilePDU));

	file_pdu->type = FILE_PDU_TYPE;
	file_pdu->No = tmp_No;

	if (feof(user_list[i]->file_pointer[j])){
		file_pdu->buf[0] = 0;
		file_pdu->len = 0;
		retval = 0;
		user_list[i]->sock_status[j] = S_CLOSING;
		user_list[i]->last_len[j] = tmp_No;
		ShowInfo(TEXT("文件发送完成！"));
	}
	else{
		retval = fread(file_pdu->buf, 1, FILE_BUF_SIZE, user_list[i]->file_pointer[j]);
		file_pdu->len = retval;
	}

	buf_len = 1 + 1 + 1 + retval;

	retval = send(sock, (char*)file_pdu, buf_len, 0);
	if (retval <= 0){
		//返回值判定
		MessageBox(TEXT("数据连接发送数据PDU！"), TEXT("数据连接错误"), MB_OK);
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




//函数：开始接收文件
//功能：处理上传文件请求，新建套接字，发送文件信息
//输入：
//输出：
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

	//找到该用户在列表中位置
	for (i = 0; i < USER_LIST_SIZE; i++){
		if (user_list[i]->ctl_sock == s){
			break;
		}
	}

	strcat_s(file_dir, 100, filename);
	//检查该文件是否存在，若存在，则终止，告知用户错误
	if (_access(file_dir, 0) == 0){
		//CString show_info;
		//show_info += file_dir;
		//show_info += "已存在！";
		ShowInfo(TEXT("接收文件已存在！无法接收！"));
		SendCtlPDU(s, RE_ERR_FILENAME);
		return -1;
	}

	fopen_s(&fp, file_dir, "wb");

	//生成临时文件！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！

	cli_addr.sin_family = AF_INET;
	cli_addr.sin_addr.S_un.S_addr = user_list[i]->user_ip;
	cli_addr.sin_port = htons(CLIENT_PORT);

	recv_sock = socket(AF_INET, SOCK_STREAM, 0);
	len = sizeof(cli_addr);
	retval = connect(recv_sock, (struct sockaddr *)&cli_addr, len);

	//发送首帧
	struct FilePDU file_pdu;
	file_pdu.type = FILE_PDU_TYPE;
	file_pdu.No = UP_FILE_START;
	strcpy(file_pdu.buf, filename);
	file_pdu.len = strlen(filename);
	
	buf_len = 1 + 1 + 1 + file_pdu.len;
	retval = send(recv_sock, (char *)&file_pdu, buf_len, 0);
	if (retval < 1+1+1+file_pdu.len){
		//发送首帧失败
		ShowInfo(TEXT("发送首帧失败！"));
		//关闭套接字
		closesocket(recv_sock);
		return retval;
	}

	//将必要数据存入用户信息单元中
	user_list[i]->InsertSocketList(recv_sock, S_RECVFILE_START, fp);
	WSAAsyncSelect(recv_sock, m_hWnd, UM_RECV_FILE_SOCK, FD_READ | FD_CLOSE);

	return i;
}

//函数：接收文件
//功能：接收到一帧合法的数据帧后，发送ACK；若收到len=0的帧，则关闭套接字。
//输入：
//输出：
short CServerDlg::RecvFile(SOCKET sock)
{
	int i, j;
	unsigned int buf_len;
	int retval;
	char tmp_No;
	char* recv_buf;
	struct FilePDU* file_pdu;

	//在用户列表中找到对应用户套接字
	Find_ij(sock, i, j);

	recv_buf = (char*)malloc(BUF_SIZE);
	//接收数据，并进行差错判断
	buf_len = Receive(sock, recv_buf, BUF_SIZE);
	if (buf_len <= 0){
		if (buf_len == -1)
			user_list[i]->DeleteSockList(j);
		free(recv_buf);
		return -1;
	}
	//按P_AbandRand概率丢包
	if (AbandonRand(P_AbandRand) == 0){
		free(recv_buf);
		ShowInfo(TEXT("测试超时重发：丢包一次！"));
		return 0;
	}

	//file_pdu = (struct FilePDU*)malloc(sizeof(struct FilePDU));
	file_pdu = (struct FilePDU*)recv_buf;

	if (file_pdu->type != FILE_PDU_TYPE){
		ShowInfo(TEXT("报文类型不符合接收数据套接字！"));
		free(recv_buf);
		return -1;
	}

	//如果收到结束报文，则关闭相关套接字
	if (file_pdu->len == 0){
		user_list[i]->DeleteSockList(j);
		
		free(recv_buf);

		ShowInfo(TEXT("文件接收完成！"));
		return 0;
	}

	switch (user_list[i]->sock_status[j]){
	case S_RECVFILE_START:
		if (file_pdu->No == 1){
			///////////////////////////////////////////////////////差错控制
			ShowInfo(TEXT("接收数据包编号错误！"));
			free(recv_buf);
			///////////////////////////////////////////////////////考虑关闭！
			return -1;
		}
		tmp_No = 0;
		user_list[i]->sock_status[j] = S_RECVFILE_0;
		break;
	case S_RECVFILE_1:
		if (file_pdu->No == 1){
			tmp_No = 1;
			//ShowInfo(TEXT("接收数据包编号错误！"));
			break;
		}
		tmp_No = 0;
		user_list[i]->sock_status[j] = S_RECVFILE_0;
		break;
	case S_RECVFILE_0:
		if (file_pdu->No == 0){
			tmp_No = 0;
			//ShowInfo(TEXT("接收数据包编号错误！"));
			break;
		}
		tmp_No = 1;
		user_list[i]->sock_status[j] = S_RECVFILE_1;
		break;
	default:
		ShowInfo(TEXT("套接字状态错误！"));
		free(recv_buf);
		return -1;
	}

	//buf_len = (unsigned int)file_pdu->len;
	retval = fwrite(file_pdu->buf, 1, file_pdu->len, user_list[i]->file_pointer[j]);
	if (retval < file_pdu->len){
		ShowInfo(TEXT("写文件失败！"));
		////////////////////////////////////////////////////////////////////////返回值判定
	}

	free(recv_buf);
	recv_buf = NULL;
	file_pdu = NULL;

	//发送回复报文
	file_pdu = (struct FilePDU*)malloc(sizeof(struct FilePDU));
	file_pdu->type = FILE_PDU_TYPE;
	file_pdu->No = tmp_No;		//(file_pdu->No + 1) % 2;
	file_pdu->len = 3;
	strcpy(file_pdu->buf, "ACK");
	buf_len = 1 + 1 + 1 + 3;
	retval = send(sock, (char*)file_pdu, buf_len, 0);
	if (retval < 0){
		/////////////////////////////////////////////////////返回值判定
		/////////////////////////////////////////////////////关闭套接字
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
		ShowInfo(TEXT("计时器"));
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

	//新建文件PDU
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
		//返回值判定
		MessageBox(TEXT("数据重发失败！"), TEXT("重发错误"), MB_OK);
	}

	user_list[i]->last_len[j] = retval - 3;

	UINT timer_id = (i + 1)*SOCK_LIST_SIZE + j;
	SetTimer(timer_id, TIMEOUT, 0);

	free(file_pdu);
	file_pdu = NULL;
	return i;
}