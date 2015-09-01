
// PRJ3_0613Dlg.cpp : 实现文件
//

#include "stdafx.h"
#include "PRJ3_0613.h"
#include "PRJ3_0613Dlg.h"
#include "afxdialogex.h"
//#include "user.h"
#include "io.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define TIMEOUT 2000
//丢包函数！！
#define P_AbandRand 0.6

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


// CPRJ3_0613Dlg 对话框

CPRJ3_0613Dlg::CPRJ3_0613Dlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CPRJ3_0613Dlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CPRJ3_0613Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	//  DDX_Control(pDX, DaKaiAnNiu, m_ShangChuanLieBiao);
	//DDX_Control(pDX, IDC_LIST3, m_ShangChuanLuJing);
	DDX_Control(pDX, ShangChuanLuJing, m_ShangChuanLuJing);
	DDX_Control(pDX, XiaZaiLieBiao, m_XiaZaiLieBiao);
}

BEGIN_MESSAGE_MAP(CPRJ3_0613Dlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(ShangXianAnNiu, &CPRJ3_0613Dlg::OnBnClickedShangxiananniu)
	ON_BN_CLICKED(XiaXianAnNiu, &CPRJ3_0613Dlg::OnBnClickedXiaxiananniu)
	ON_BN_CLICKED(TuiChuAnNiu, &CPRJ3_0613Dlg::OnBnClickedTuichuanniu)
	ON_BN_CLICKED(DaKaiAnNiu, &CPRJ3_0613Dlg::OnBnClickedDakaianniu)
	ON_BN_CLICKED(ShangChuanAnNiu, &CPRJ3_0613Dlg::OnBnClickedShangchuananniu)
	ON_BN_CLICKED(GengXinLieBiao, &CPRJ3_0613Dlg::OnBnClickedGengxinliebiao)
	ON_BN_CLICKED(XiaZaiAnNiu, &CPRJ3_0613Dlg::OnBnClickedXiazaianniu)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CPRJ3_0613Dlg 消息处理程序

BOOL CPRJ3_0613Dlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	sock_num = 0;
	// TODO:  在此添加额外的初始化代码
	GUIOffline();
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CPRJ3_0613Dlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CPRJ3_0613Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


//解决回车退出问题
BOOL CPRJ3_0613Dlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO:  在此添加专用代码和/或调用基类
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE)
		return TRUE;
	else if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
		return TRUE;
	else
		return CDialog::PreTranslateMessage(pMsg);
	return CDialogEx::PreTranslateMessage(pMsg);
}



void CPRJ3_0613Dlg::OnBnClickedShangxiananniu()
{
	// TODO:  在此添加控件通知处理程序代码
	WSADATA wsaData;
	int iResult;
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0)
	{
		MessageBox(_T("WSAStartup()出错"));
		return;
	}
	CtlSock = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
	if (CtlSock == INVALID_SOCKET)
	{
		MessageBox(_T("CtlSock申请失败"));
		WSACleanup();
		return;
	}
	SOCKADDR_IN ServAddr;
	ServAddr.sin_family = AF_INET;
	ServAddr.sin_port = htons(SERV_PORT);
	ServAddr.sin_addr.S_un.S_addr = htonl(SERV_IP);
	//unsigned long iMode = 1;
	//iResult = ioctlsocket(CtlSock, FIONBIO, &iMode);
	//if (iResult != 0)
	//{
	//	MessageBox(_T("设置CtlSock非堵塞模式失败"));
	//	return;
	//}
	while (connect(CtlSock, (sockaddr*)&ServAddr, sizeof(ServAddr)) == SOCKET_ERROR)
	{
		MessageBox(_T("CtlSock连接失败"));
		return;
	}
	MessageBox(_T("CtlSock连接成功"));
	Is_On = 1;
	WSAAsyncSelect(CtlSock, m_hWnd, WM_SOCKET, FD_READ | FD_CLOSE);


	///////////////////////////////////////////////////////////////////////
	//初始化数据套接字！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！
	int retval;
	SOCKADDR_IN MyAddr;

	DatSock = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
	if (DatSock == INVALID_SOCKET)
	{
		MessageBox(_T("DatSock申请失败"));
		WSACleanup();
		return;
	}

	MyAddr.sin_family = AF_INET;
	MyAddr.sin_port = htons(LOCAL_PORT);		//端口号为12345！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！
	MyAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);	//任意IP地址

	retval = bind(DatSock, (sockaddr*)&MyAddr, sizeof(MyAddr));
	if (retval == SOCKET_ERROR){
		//绑定失败判断
		MessageBox(_T("DatSock绑定IP地址与端口号失败！"));
		return;
	}

	retval = listen(DatSock, 5);
	if (retval == -1){
		MessageBox(_T("DatSock开启监听失败！"));
		return;
	}

	MessageBox(_T("DatSock成功开启监听!"));
	WSAAsyncSelect(DatSock, m_hWnd, WM_DATA_SOCKET, FD_ACCEPT);

	filelist_sock = 0;
	fl_sock_status = 0;

	//变量初始化
	int i;
	for (i = 0; i < SOCK_LIST_SIZE; i++){
		sock_list[i] = 0;
		sock_status[i] = 0;
		FileName[i] = "";
		FilePath[i] = "";
		file_pointer[i] = NULL;
	}
	//////////////////////////////////////////////////////////////////////////
}

void CPRJ3_0613Dlg::OnBnClickedXiaxiananniu()
{
	// TODO:  在此添加控件通知处理程序代码
	if (Is_On)
	{
		OffLine();
		Is_On = 0;
	}
}


void CPRJ3_0613Dlg::OnBnClickedTuichuanniu()
{
	// TODO:  在此添加控件通知处理程序代码
	if (Is_On)
	{
		OffLine();
	}
	exit(0);
}

void CPRJ3_0613Dlg::OffLine()
{
	closesocket(CtlSock);
	WSACleanup();
}

//位置：接收方
//功能：输入随机数数组，并计算出质询值
//输入：
//	buff[]——接收到的随机数数组
//输出：
//  计算得到的质询值
unsigned int translatenum(InquiryPDU *buff, int Password)
{
	unsigned int recvnum[16];
	unsigned int sum = 0;
	char num = buff->num;
	char idx = 0;
	char key;
	for (char i = 0; i < num; i++)
	{
		switch (i % 4)
		{
		case 0:
			key = (buff->each_len[i / 4] & 0xC0) >> 6;
			break;
		case 1:
			key = (buff->each_len[i / 4] & 0x30) >> 4;
			break;
		case 2:
			key = (buff->each_len[i / 4] & 0x0C) >> 2;
			break;
		case 3:
			key = (buff->each_len[i / 4] & 0x03);
			break;
		}
		recvnum[i] = getnum(&key, &idx, buff);
		sum += recvnum[i];
		//printf("得到%d个生成数(16进制)：%x\n", i + 1, recvnum[i]);
	}
	return sum ^Password;
}


//位置：接收方
//功能：从数组反向生成整数
//输入：
//	key——整数字节数
//	idx_——数组下标指针
//	buff——接收到的随机数数组
//输出：
//  得到的整数值
unsigned int getnum(char *key, char *idx, InquiryPDU *buff)
{
	unsigned int targetnum = 0;
	char len = *key;
	if (len == 0)
	{
		len = 4;
	}
	for (char i = 0; i < len; i++)
	{
		targetnum = (targetnum << 8);
		targetnum |= (0x000000FF)&(buff->inquiry_buf[*idx]);
		*idx = (*idx) + 1;
	}
	return targetnum;
}




LRESULT CPRJ3_0613Dlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)		//消息处理程序（自定义）
{
	// TODO: Add your specialized code here and/or call the base class
	int Event, len, retval, serial;
	CString notify;
	CtlPDU* ctlbuff;

	SOCKET s = static_cast<SOCKET>(wParam);
	Event = LOWORD(lParam);
	switch(message){
	//控制套接字
	case WM_SOCKET:
		if (s != CtlSock){
			MessageBox(_T("套接字错误！"));
			break;
		}
		switch (Event)
		{
		case FD_READ:
			char tempcmdbuff[300];
			len = recv(CtlSock, tempcmdbuff, sizeof(tempcmdbuff), 0);
			if (len <= 0){
				retval = WSAGetLastError();
				if (retval != WSAEWOULDBLOCK){
					closesocket(CtlSock);
				}
			}

			ctlbuff = (CtlPDU *)tempcmdbuff;
			switch (ctlbuff->type){
			//质询PDU
			case INQUIRY_PDU_TYPE:
				Re_Inquiry(CtlSock, tempcmdbuff);
				break;
			//控制信息PDU
			case CTL_PDU_TYPE:
				switch (ctlbuff->cmd){
				//收到服务器请求帐号报文
				case REQ_ACCOUNT:
					Re_Account(CtlSock);
					break;
				case RE_ACCESS:
					MessageBox(_T("认证成功，欢迎使用！"));
					GUIOnline();
					break;
				case RE_ERR_ACCOUNT:
					MessageBox(_T("无此用户！"));
					OffLine();
					break;
				case RE_ERR_INQUIRY:
					MessageBox(_T("密码错误！"));
					OffLine();
					break;
				case RE_ERR_FILENAME:
					MessageBox(_T("文件名错误！"));
					break;
				}
				break;
			default:
				break;
			}
			break;
		case FD_CLOSE:
			closesocket(CtlSock);
			break;
		}
		break;

	//文件传输套接字
	case WM_DATA_SOCKET:
		switch (Event){
		case FD_ACCEPT:
			AcceptDataSocket(s);
			break;
		}
		break;

	case WM_FL_SOCK:
		switch (Event){
		case FD_READ:
			RecvFileList(s);
			break;
		case FD_CLOSE:
			break;
		}
		break;

	case WM_RECV_FILE_SOCK:
		switch (Event){
		case FD_READ:
			RecvFile(s);
			break;
		case FD_CLOSE:
			break;
		}
		break;

	case WM_SEND_FILE_SOCK:
		switch (Event){
		case FD_READ:
			SendFile(s);
			break;
		case FD_CLOSE:
			int i;
			for (i = 0; i < SOCK_LIST_SIZE; i++){
				if (sock_list[i] == s){
					break;
				}
			}
			KillTimer(i + 1);
			break;
		}
		break;
	}
	return CDialog::WindowProc(message, wParam, lParam);
}

void CPRJ3_0613Dlg::OnBnClickedDakaianniu()
{
	// TODO:  在此添加控件通知处理程序代码
	CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, "All Files(*.*)|*.*||", AfxGetMainWnd());
	if (dlg.DoModal() == IDOK)
	{
		POSITION m_Position = dlg.GetStartPosition();
		while (m_Position != NULL)
		{
			strPath = dlg.GetNextPathName(m_Position);
			m_ShangChuanLuJing.InsertString(m_ShangChuanLuJing.GetCount(), strPath);
		}
	}
	m_ShangChuanLuJing.SetHorizontalExtent(600);
}


void CPRJ3_0613Dlg::OnBnClickedShangchuananniu()
{
	// TODO:  在此添加控件通知处理程序代码
	CtlPDU *sendbuff = (CtlPDU *)malloc(sizeof(CtlPDU));
	sendbuff->type = CTL_PDU_TYPE;
	sendbuff->cmd = REQ_UPFILE;
	CString strFileName;
	//m_ShangChuanLuJing.GetText(0, pathstr);
	strFileName = strPath.Mid(strPath.ReverseFind('\\') + 1);
	strFileName += '\0';
	sendbuff->len = strFileName.GetLength();
	strcpy(sendbuff->filename, strFileName.GetBuffer(sizeof(strFileName)));
	send(CtlSock, (char *)sendbuff, sizeof(CtlPDU), 0);

	////////////////////////////////////////////////////////////////
	////////////////////
	int i;
	if (sock_num == SOCK_LIST_SIZE){
		MessageBox(_T("数据队列已满，请稍候！"), _T("队列问题"), MB_OK);
		return;
	}
	for (i = 0; i < SOCK_LIST_SIZE; i++){
		if (sock_list[i] == 0 && FileName[i] == ""){
			break;
		}
	}
	sock_num += 1;
	FileName[i] = strFileName;
	FilePath[i] = strPath;
	////////////////////
	////////////////////////////////////////////////////////////////
}


void CPRJ3_0613Dlg::OnBnClickedGengxinliebiao()
{
	int retval;

	//清空原有的文件列表
	ClearFileList();
	if (filelist_sock != 0){
		closesocket(filelist_sock);
		filelist_sock = 0;
		fl_sock_status = S_CLOSE;
	}

	//Recurse(_T("E:\\实验程序"));
	// TODO:  在此添加控件通知处理程序代码
	CtlPDU *sendbuff = (CtlPDU *)malloc(sizeof(CtlPDU));
	sendbuff->type = CTL_PDU_TYPE;
	sendbuff->cmd = REQ_FILELIST;
	sendbuff->len = 0;
	retval = send(CtlSock, (char *)sendbuff, 1 + 1 + 1, 0);

	if (retval < 1 + 1 + 1){
		MessageBox(_T("发送文件列表请求失败！"), _T("发送错误"), MB_OK);
		return;
	}

}


void CPRJ3_0613Dlg::OnBnClickedXiazaianniu()
{
	// TODO:  在此添加控件通知处理程序代码
	int buf_len;
	CString strText;
	int XiaZaiCurSel = m_XiaZaiLieBiao.GetCurSel();
	if (XiaZaiCurSel < 0)
	{
		MessageBox(_T("未选中下载文件！"));
		return;
	}
	m_XiaZaiLieBiao.GetText(XiaZaiCurSel, strText);
	strText += '\0';
	
	CtlPDU *sendbuff = (CtlPDU *)malloc(sizeof(CtlPDU));
	sendbuff->type = CTL_PDU_TYPE;
	sendbuff->cmd = REQ_DOWNFILE;
	sendbuff->len = strText.GetLength();
	strcpy(sendbuff->filename, strText.GetBuffer(sizeof(strText)));

	buf_len = 1 + 1 + 1 + sendbuff->len;
	send(CtlSock, (char *)sendbuff, buf_len, 0);
	//fp = fopen(szBuf, "w");

	////////////////////////////////////////////////////////////////
	////////////////////
	int i;
	if (sock_num == SOCK_LIST_SIZE){
		MessageBox(_T("数据队列已满，请稍候！"), _T("队列问题"), MB_OK);
		return;
	}
	for (i = 0; i < SOCK_LIST_SIZE; i++){
		if (sock_list[i] == 0 && FileName[i] == ""){
			break;
		}
	}
	sock_num += 1;
	FileName[i] = strText;
	FilePath[i] = "C://ClientData//" + strText;		//////////////////////////////////////////////////////////临时路径！！！
	////////////////////
	////////////////////////////////////////////////////////////////
}

void CPRJ3_0613Dlg::GUIOffline()
{
	GetDlgItem(GengXinLieBiao)->EnableWindow(FALSE);
	GetDlgItem(XiaZaiAnNiu)->EnableWindow(FALSE);
	GetDlgItem(DaKaiAnNiu)->EnableWindow(FALSE);
	GetDlgItem(ShangChuanAnNiu)->EnableWindow(FALSE);
	GetDlgItem(XiaXianAnNiu)->EnableWindow(FALSE);
	GetDlgItem(ShangXianAnNiu)->EnableWindow(TRUE);
	GetDlgItem(ZhangHaoShuRuKuang)->EnableWindow(TRUE);
	GetDlgItem(MiMaShuRuKuang)->EnableWindow(TRUE);
}
void CPRJ3_0613Dlg::GUIOnline()
{
	GetDlgItem(GengXinLieBiao)->EnableWindow(TRUE);
	GetDlgItem(XiaZaiAnNiu)->EnableWindow(TRUE);
	GetDlgItem(DaKaiAnNiu)->EnableWindow(TRUE);
	GetDlgItem(ShangChuanAnNiu)->EnableWindow(TRUE);
	GetDlgItem(XiaXianAnNiu)->EnableWindow(TRUE);
	GetDlgItem(ShangXianAnNiu)->EnableWindow(FALSE);
	GetDlgItem(ZhangHaoShuRuKuang)->EnableWindow(FALSE);
	GetDlgItem(MiMaShuRuKuang)->EnableWindow(FALSE);
}

//函数：回复帐号（参数无用……）
void CPRJ3_0613Dlg::Re_Account(SOCKET sock)
{
	int retval;
	CString name;
	CtlPDU* ctlbuff = (CtlPDU *)malloc(sizeof(CtlPDU));

	GetDlgItemText(ZhangHaoShuRuKuang, name);
	name += '\0';
	ctlbuff->type = CTL_PDU_TYPE;
	ctlbuff->cmd = REQ_ACCOUNT;
	ctlbuff->len = name.GetLength();
	strcpy(ctlbuff->filename, name.GetBuffer(sizeof(name)));
	retval = send(CtlSock, (char *)ctlbuff, 1 + 1 + 1 + ctlbuff->len, 0);
	if (retval < 1 + 1 + 1 + ctlbuff->len){
		MessageBox(_T("回复帐号错误！"), _T("错误"), MB_OK);
	}
	free(ctlbuff);
	return;
}

//函数：回复质询值（参数中sock无用……）
void CPRJ3_0613Dlg::Re_Inquiry(SOCKET sock, char *tempcmdbuff)
{
	int retval;
	InquiryPDU* inqbuff;

	inqbuff = (InquiryPDU *)tempcmdbuff;
	CString MiMa;
	GetDlgItemText(MiMaShuRuKuang, MiMa);
	MiMa += '\0';
	int Password;
	Password = atoi(MiMa);
	unsigned int ClcValue;
	ClcValue = translatenum(inqbuff, Password);


	InquiryPDU *sendbuff = (InquiryPDU *)malloc(sizeof(InquiryPDU));
	sendbuff->type = INQUIRY_PDU_TYPE;
	sendbuff->num = 0;
	sendbuff->each_len[0] = (ClcValue & 0xFF000000) >> 24;
	sendbuff->each_len[1] = (ClcValue & 0xFF0000) >> 16;
	sendbuff->each_len[2] = (ClcValue & 0xFF00) >> 8;
	sendbuff->each_len[3] = ClcValue & 0xFF;
	retval = send(CtlSock, (char *)sendbuff, sizeof(InquiryPDU), 0);
	if (retval < sizeof(InquiryPDU)){
		MessageBox(_T("返回质询值错误！"), _T("错误"), MB_OK);
	}

	free(sendbuff);
	inqbuff = NULL;

	return;
}


//函数：接收文件套接字
//功能：在客户端向服务器请求下载或上传之后，服务器会主动与客户机连接，该函数即处理该连接的建立
//
void CPRJ3_0613Dlg::AcceptDataSocket(SOCKET s)
{
	int i;
	int retval;
	int addrlen;
	sockaddr_in remote;
	SOCKET new_sock;
	char* recv_buf;

	addrlen = sizeof(remote);
	new_sock = accept(s, (sockaddr*)&remote, &addrlen);
	if (new_sock == SOCKET_ERROR){
		MessageBox(TEXT("连接意外关闭！"), TEXT("接收连接错误"), MB_OK);
		return;
	}
	//接收到一个新的数据连接
	MessageBox(TEXT("接收到一个新的数据连接！"), TEXT("服务器"), MB_OK);

	recv_buf = (char *)malloc(BUF_SIZE);
	retval = recv(new_sock, recv_buf, BUF_SIZE, 0);
	if (retval <= 0){
		//接收错误
		MessageBox(TEXT("数据连接意外关闭！"), TEXT("数据连接错误"), MB_OK);
		closesocket(new_sock);
		return;
	}
	recv_buf[retval] = 0;

	FilePDU *file_pdu;
	file_pdu = (FilePDU*)recv_buf;
	if (file_pdu->type != FILE_PDU_TYPE){
		//数据连接收到错误PDU
		MessageBox(TEXT("数据连接收到错误PDU！"), TEXT("数据连接连接错误"), MB_OK);
		closesocket(new_sock);
		return;
	}

	if (file_pdu->No == GET_FILELIST_START){
		retval = StartRecvFileList(new_sock, file_pdu);
		return;
	}


	for (i = 0; i < SOCK_LIST_SIZE; i++){
		if (FileName[i] == file_pdu->buf){
			break;
		}
	}
	if (i == SOCK_LIST_SIZE){
		//报错，未找到
		MessageBox(TEXT("数据连接收到PDU中的文件名错误！"), TEXT("数据连接连接错误"), MB_OK);
		closesocket(new_sock);
		return;
	}

	sock_list[i] = new_sock;

	switch (file_pdu->No){
	case UP_FILE_START:
		StartSendFile(new_sock, i);
		break;
	case DOWN_FILE_START:
		StartRecvFile(new_sock, i);
		break;
	}
	free(recv_buf);	
}

//函数：开始接收文件列表
//功能：
//
//输出： 0 成功， <0 失败
short CPRJ3_0613Dlg::StartRecvFileList(SOCKET s, FilePDU *file_pdu)
{
	int buf_len;
	long retval;
	char *recv_buf;
	//FilePDU *file_pdu;
	CString filename;

	//打印到屏幕
	filename = file_pdu->buf;
	AddToFileList(filename);

	//回复文件列表首帧
	file_pdu = (FilePDU *)malloc(sizeof(FilePDU));
	file_pdu->type = FILE_PDU_TYPE;
	file_pdu->No = 0;
	file_pdu->len = 3;
	strcpy(file_pdu->buf, "ACK");

	buf_len = 1 + 1 + 1 + file_pdu->len;

	retval = send(s, (char*)file_pdu, buf_len, 0);
	if (retval <= 0){
		//返回值判定
		MessageBox(TEXT("回复文件列表首帧失败！"), TEXT("回复错误"), MB_OK);
		return retval;
	}

	filelist_sock = s;
	fl_sock_status = S_RECVFL_0;

	WSAAsyncSelect(s, m_hWnd, WM_FL_SOCK, FD_READ | FD_CLOSE);

	free(file_pdu);

	return 0;
}



//函数：接收文件列表
//
//
//输出： 0 成功， -1 失败
short CPRJ3_0613Dlg::RecvFileList(SOCKET s)
{
	int buf_len;
	long retval;
	char *recv_buf;
	char tmp_No;
	FilePDU *file_pdu;
	CString filename;

	recv_buf = (char*)malloc(BUF_SIZE);		//1 + 1 + 1 + FILE_BUF_SIZE
	buf_len = recv(s, recv_buf, BUF_SIZE, 0);
	if (buf_len <= 0){
		retval = WSAGetLastError();
		if (retval != WSAEWOULDBLOCK){
			closesocket(s);
			MessageBox(TEXT("接收信息错误，连接关闭！"), TEXT("接收失败"), MB_OK);
			//m_InfoList.AddString(TEXT("套接字错误：接收信息错误，连接关闭！"));
			//ShowInfo(TEXT("套接字错误：接收信息错误，连接关闭！"));
			return -1;
		}
		else{
			//非阻塞导致的错误，继续运行即可
			return -1;
		}
	}
	recv_buf[buf_len] = 0;

	file_pdu = (FilePDU*)recv_buf;
	if (file_pdu->type != FILE_PDU_TYPE){
		////////////////////////////////////////////////////////////////
		return -1;
	}
	if (file_pdu->len == 0){
		closesocket(s);
		filelist_sock = 0;
		fl_sock_status = S_CLOSE;
		return 0;
	}
	switch (fl_sock_status){
	case S_RECVFL_0:
		if (file_pdu->No == 0){
			//////////////////////////////////////////
			tmp_No = 0;
			break;
		}
		tmp_No = 1;
		fl_sock_status = S_RECVFL_1;
		break;
	case S_RECVFL_1:
		if (file_pdu->No == 1){
			/////////////////////////////////////////
			tmp_No = 1;
			break;
		}
		tmp_No = 0;
		fl_sock_status = S_RECVFL_0;
		break;
	default:
		free(recv_buf);
		return -1;
	}

	//打印到屏幕
	filename = file_pdu->buf;
	AddToFileList(filename);

	free(recv_buf);

	//回复文件列表首帧
	file_pdu = (FilePDU *)malloc(sizeof(FilePDU));
	file_pdu->type = FILE_PDU_TYPE;
	file_pdu->No = tmp_No;
	file_pdu->len = 3;
	strcpy(file_pdu->buf, "ACK");

	buf_len = 1 + 1 + 1 + file_pdu->len;

	retval = send(s, (char*)file_pdu, buf_len, 0);
	if (retval <= 0){
		//返回值判定
		MessageBox(TEXT("回复文件列表首帧失败！"), TEXT("回复错误"), MB_OK);
		return retval;
	}

	free(file_pdu);

	return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//函数：处理文件下载请求
//功能：收到文件下载请求，检验文件名合法性，会送合法文件名，文件大小
//输入：
//输出：
short CPRJ3_0613Dlg::StartSendFile(SOCKET s, int i)
{
	int buf_len;
	long retval;
	char *send_buf;
	FILE *fp;
	FilePDU file_pdu;

	fopen_s(&fp, FilePath[i], "rb");
	if (fp == NULL){
		//显示错误信息
		CString show_info;
		show_info += FilePath[i];
		show_info += "不存在！";
		//ShowInfo(show_info);

		///////////////////////////////////////////////告知用户发生错误
		//SendCtlPDU(s, RE_ERR_FILENAME);
		closesocket(s);///////////////////////////////////关闭套接字！还有后续！！！
		return -1;
	}

	//发送首帧
	file_pdu.type = FILE_PDU_TYPE;
	file_pdu.No = 0;
	retval = fread(file_pdu.buf, 1, FILE_BUF_SIZE, fp);
	file_pdu.len = retval;
	buf_len = 1 + 1 + 1 + retval;

	//send_buf = (char*)&file_pdu;
	retval = send(s, (char*)&file_pdu, buf_len, 0);
	if (retval <= 0){
		//返回值判定
		MessageBox(TEXT("数据连接发送数据PDU！"), TEXT("数据连接错误"), MB_OK);
		return retval;
	}

	sock_status[i] = S_SENDFILE_0;
	file_pointer[i] = fp;
	last_len[i] = retval - 3;

	SetTimer(i + 1, TIMEOUT, 0);

	WSAAsyncSelect(s, m_hWnd, WM_SEND_FILE_SOCK, FD_READ | FD_CLOSE);

	//free(file_pdu);
	return i;
}


//函数：发送文件
//功能：接收到ACK后，判断正误，准备FilePDU，发送文件片段
//输入：
//输出：
short CPRJ3_0613Dlg::SendFile(SOCKET s)
{
	int i;
	int retval;
	int len;
	int buf_len;
	char tmp_No;
	char* recv_buf;
	char *send_buf;
	FilePDU *file_pdu;

	if (s == DatSock){
		////////////////////////////////报错，DatSock没有读事件！！
		return -1;
	}

	recv_buf = (char*)malloc(BUF_SIZE);
	buf_len = recv(s, recv_buf, BUF_SIZE, 0);
	if (buf_len <= 0){
		retval = WSAGetLastError();
		if (retval != WSAEWOULDBLOCK){
			closesocket(s);
			MessageBox(TEXT("接收信息错误，连接关闭！"), TEXT("接收失败"), MB_OK);
			//m_InfoList.AddString(TEXT("套接字错误：接收信息错误，连接关闭！"));
			//ShowInfo(TEXT("套接字错误：接收信息错误，连接关闭！"));
			return -1;
		}
		else{
			//非阻塞导致的错误，继续运行即可
			return -1;
		}
	}
	recv_buf[buf_len] = 0;

	file_pdu = (FilePDU *)recv_buf;

	//CString ack = "ACK";
	if (file_pdu->type != FILE_PDU_TYPE){
		////////////////////////////////////////////////报错
		return -1;
	}
	if (file_pdu->len != 3){
		return -1;
	}
	CString ack;
	ack	= "ACK";
	if (file_pdu->buf != ack){
		return -1;
	}

	//找到对应套接字
	for (i = 0; i < SOCK_LIST_SIZE; i++){
		if (sock_list[i] == s){
			break;
		}
	}

	switch (sock_status[i]){
	case S_SENDFILE_0:
		if (file_pdu->No == 1){
			//////////////////////////////////////////
			tmp_No = 0;
			buf_len = last_len[i];
			fseek(file_pointer[i], -buf_len, 1);
			break;
		}
		tmp_No = 1;
		sock_status[i] = S_SENDFILE_1;
		break;
	case S_SENDFILE_1:
		if (file_pdu->No == 0){
			/////////////////////////////////////////
			tmp_No = 1;
			buf_len = last_len[i];
			fseek(file_pointer[i], -buf_len, 1);
			break;
		}
		tmp_No = 0;
		sock_status[i] = S_SENDFILE_0;
		break;
	case S_CLOSING:
		closesocket(sock_list[i]);
		sock_list[i] = 0;
		sock_status[i] = S_CLOSE;
		fclose(file_pointer[i]);
		file_pointer[i] = NULL;
		last_len[i] = 0;
		KillTimer(i + 1);
		break;
	default:
		free(recv_buf);
		return -1;
	}
	free(recv_buf);

	file_pdu = (FilePDU *)malloc(sizeof(FilePDU));

	file_pdu->type = FILE_PDU_TYPE;
	file_pdu->No = tmp_No;

	if (feof(file_pointer[i])){
		file_pdu->buf[0] = 0;
		file_pdu->len = 0;
		retval = 0;
		sock_status[i] = S_CLOSING;
		last_len[i] = tmp_No;
		MessageBox(TEXT("文件发送完毕！"), TEXT("上传"), MB_OK);
	}
	else{
		retval = fread(file_pdu->buf, 1, FILE_BUF_SIZE, file_pointer[i]);
		file_pdu->len = retval;
	}

	len = 1 + 1 + 1 + retval;

	retval = send(s, (char*)file_pdu, len, 0);
	if (retval <= 0){
		//返回值判定
		MessageBox(TEXT("数据连接发送数据PDU！"), TEXT("数据连接错误"), MB_OK);
		return retval;
	}

	last_len[i] = retval - 3;

	SetTimer(i + 1, TIMEOUT, 0);

	free(file_pdu);
	file_pdu = NULL;
	send_buf = NULL;
	return i;
}


//函数：开始下载文件
//功能：处理下载文件请求，新建套接字，发送文件信息
//输入：
//输出：
short CPRJ3_0613Dlg::StartRecvFile(SOCKET s, int i)
{
	int buf_len;
	long retval;
	struct sockaddr_in cli_addr;
	//char file_dir[100] = "C://ClientData//";			////////////////////////////////////////////////////临时路径
	char *send_buf;
	FILE *fp;

	//strcat_s(file_dir, FILE_BUF_SIZE, FilePath[i]);

	if (_access(FilePath[i], 0) == 0){
		MessageBox(TEXT("接收文件已存在！无法接收！"), TEXT("错误"), MB_OK);
		return -1;
	}
	fopen_s(&fp, FilePath[i], "wb");
	file_pointer[i] = fp;
	sock_status[i] = S_RECVFILE_0;

	//发送首帧回复帧
	FilePDU file_pdu;
	file_pdu.type = FILE_PDU_TYPE;
	file_pdu.No = 0;
	strcpy(file_pdu.buf, "ACK");
	file_pdu.len = 3;

	buf_len = 1 + 1 + 1 + file_pdu.len;
	retval = send(s, (char *)&file_pdu, buf_len, 0);
	if (retval < 1 + 1 + 1 + file_pdu.len){
		//发送首帧失败
		MessageBox(TEXT("发送首帧回复帧失败！"), TEXT("发送错误"),MB_OK);
		//关闭套接字
		closesocket(s);
		return retval;
	}

	//将必要数据存入用户信息单元中
	WSAAsyncSelect(s, m_hWnd, WM_RECV_FILE_SOCK, FD_READ | FD_CLOSE);

	return i;
}


//函数：接收文件
//功能：接收到一帧合法的数据帧后，发送ACK；若收到len=0的帧，则关闭套接字。
//输入：
//输出：
short CPRJ3_0613Dlg::RecvFile(SOCKET sock)
{
	int i;
	int buf_len;
	int retval;
	char tmp_No;
	char* recv_buf;
	char* send_buf;
	FilePDU *file_pdu;

	recv_buf = (char*)malloc(BUF_SIZE);		//1 + 1 + 1 + FILE_BUF_SIZE
	buf_len = recv(sock, recv_buf, BUF_SIZE, 0);
	if (buf_len <= 0){
		retval = WSAGetLastError();
		if (retval != WSAEWOULDBLOCK){
			closesocket(sock);
			MessageBox(TEXT("接收信息错误，连接关闭！"), TEXT("接收失败"), MB_OK);
			free(recv_buf);
			return -1;
		}
		else{
			//非阻塞导致的错误，继续运行即可
			free(recv_buf);
			return -1;
		}
	}
	recv_buf[buf_len] = 0;


	//按概率丢弃该数据包
	if (AbandonRand(P_AbandRand) == 0){
		free(recv_buf);
		MessageBox(TEXT("丢包一次！"), TEXT("测试超时重发"), MB_OK);
		return -1;
	}


	//file_pdu = (struct FilePDU*)malloc(sizeof(struct FilePDU));
	file_pdu = (FilePDU *)recv_buf;

	if (file_pdu->type != FILE_PDU_TYPE){
		MessageBox(TEXT("报文类型不符合接收数据套接字！"),TEXT("接收错误"),MB_OK);
		free(recv_buf);
		return -1;
	}

	//在用户列表中找到对应用户套接字
	for (i = 0; i < SOCK_LIST_SIZE; i++){
		if (sock_list[i] == sock){
			break;
		}
	}

	//如果收到结束报文，则关闭相关套接字
	if (file_pdu->len == 0){
		closesocket(sock_list[i]);
		sock_list[i] = 0;
		fclose(file_pointer[i]);
		file_pointer[i] = NULL;
		sock_status[i] = S_CLOSE;
		free(recv_buf);
		return 0;
	}

	switch (sock_status[i]){
	case S_RECVFILE_1:
		if (file_pdu->No != 0){
			tmp_No = 1;
			//ShowInfo(TEXT("接收数据包编号错误！"));
			break;
		}
		tmp_No = 0;
		sock_status[i] = S_RECVFILE_0;
		break;
	case S_RECVFILE_0:
		if (file_pdu->No != 1){
			tmp_No = 0;
			//ShowInfo(TEXT("接收数据包编号错误！"));
			break;
		}
		tmp_No = 1;
		sock_status[i] = S_RECVFILE_1;
		break;
	default:
		MessageBox(TEXT("套接字状态错误！"), TEXT("接收错误"), MB_OK);
		free(recv_buf);
		return -1;
	}

	retval = fwrite(file_pdu->buf, 1, file_pdu->len, file_pointer[i]);
	if (retval){
		////////////////////////////////////////////////////////////////////////返回值判定
	}

	free(recv_buf);
	recv_buf = NULL;

	//发送回复报文
	file_pdu = (FilePDU*)malloc(sizeof(FilePDU));
	file_pdu->type = FILE_PDU_TYPE;
	file_pdu->No = tmp_No;		//(file_pdu->No + 1) % 2;
	file_pdu->len = 3;
	strcpy(file_pdu->buf, "ACK");
	buf_len = 1 + 1 + 1 + file_pdu->len;
	send_buf = (char*)file_pdu;
	retval = send(sock, send_buf, buf_len, 0);
	if (retval < 0){
		/////////////////////////////////////////////////////返回值判定
		/////////////////////////////////////////////////////关闭套接字
		free(file_pdu);
		file_pdu = NULL;
		send_buf = NULL;
		return -1;
	}
	else if (retval < buf_len){
		file_pointer[i] -= (buf_len - retval);
	}

	last_len[i] = retval;

	free(file_pdu);
	file_pdu = NULL;
	send_buf = NULL;

	return retval;
}


void CPRJ3_0613Dlg::AddToFileList(CString filename)
{
	m_XiaZaiLieBiao.AddString(filename);
}

void CPRJ3_0613Dlg::ClearFileList()
{
	m_XiaZaiLieBiao.ResetContent();
}

void CPRJ3_0613Dlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	int i;
	i = nIDEvent - 1;
	SendFileAgain(i);

	CDialogEx::OnTimer(nIDEvent);
}

short CPRJ3_0613Dlg::SendFileAgain(int i)
{
	int retval;
	unsigned char tmp_No;
	int buf_len;
	FilePDU* file_pdu;

	//新建文件PDU
	file_pdu = (FilePDU *)malloc(sizeof(FilePDU));

	file_pdu->type = FILE_PDU_TYPE;

	switch (sock_status[i]){
	case S_SENDFILE_START:
	case S_SENDFILE_0:
		file_pdu->No = 0;
		buf_len = last_len[i];
		fseek(file_pointer[i], -buf_len, 1);
		//file_pointer[i] -= buf_len;
		break;
	case S_SENDFILE_1:
		file_pdu->No = 1;
		buf_len = last_len[i];
		fseek(file_pointer[i], -buf_len, 1);
		//file_pointer[i] -= buf_len;
		break;
	case S_CLOSING:
		file_pdu->No = last_len[i];
		buf_len = 0;
		break;
	default:
		return -1;
	}

	////////////////////////////////////
	retval = fread(file_pdu->buf, 1, buf_len, file_pointer[i]);
	file_pdu->len = retval;

	buf_len = 1 + 1 + 1 + retval;

	retval = send(sock_list[i], (char*)file_pdu, buf_len, 0);
	if (retval <= 0){
		//返回值判定
		MessageBox(TEXT("数据重发失败！"), TEXT("重发错误"), MB_OK);
	}

	last_len[i] = retval - 3;

	UINT timer_id = i + 1;
	SetTimer(timer_id, TIMEOUT, 0);

	free(file_pdu);
	file_pdu = NULL;
	return i;
}