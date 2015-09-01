
// PRJ3_0613Dlg.cpp : ʵ���ļ�
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
//������������
#define P_AbandRand 0.6

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


// CPRJ3_0613Dlg �Ի���

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


// CPRJ3_0613Dlg ��Ϣ�������

BOOL CPRJ3_0613Dlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	sock_num = 0;
	// TODO:  �ڴ���Ӷ���ĳ�ʼ������
	GUIOffline();
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CPRJ3_0613Dlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CPRJ3_0613Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


//����س��˳�����
BOOL CPRJ3_0613Dlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO:  �ڴ����ר�ô����/����û���
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
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	WSADATA wsaData;
	int iResult;
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0)
	{
		MessageBox(_T("WSAStartup()����"));
		return;
	}
	CtlSock = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
	if (CtlSock == INVALID_SOCKET)
	{
		MessageBox(_T("CtlSock����ʧ��"));
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
	//	MessageBox(_T("����CtlSock�Ƕ���ģʽʧ��"));
	//	return;
	//}
	while (connect(CtlSock, (sockaddr*)&ServAddr, sizeof(ServAddr)) == SOCKET_ERROR)
	{
		MessageBox(_T("CtlSock����ʧ��"));
		return;
	}
	MessageBox(_T("CtlSock���ӳɹ�"));
	Is_On = 1;
	WSAAsyncSelect(CtlSock, m_hWnd, WM_SOCKET, FD_READ | FD_CLOSE);


	///////////////////////////////////////////////////////////////////////
	//��ʼ�������׽��֣���������������������������������������������������������������������������������������������������������������
	int retval;
	SOCKADDR_IN MyAddr;

	DatSock = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
	if (DatSock == INVALID_SOCKET)
	{
		MessageBox(_T("DatSock����ʧ��"));
		WSACleanup();
		return;
	}

	MyAddr.sin_family = AF_INET;
	MyAddr.sin_port = htons(LOCAL_PORT);		//�˿ں�Ϊ12345����������������������������������������������������������������������������
	MyAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);	//����IP��ַ

	retval = bind(DatSock, (sockaddr*)&MyAddr, sizeof(MyAddr));
	if (retval == SOCKET_ERROR){
		//��ʧ���ж�
		MessageBox(_T("DatSock��IP��ַ��˿ں�ʧ�ܣ�"));
		return;
	}

	retval = listen(DatSock, 5);
	if (retval == -1){
		MessageBox(_T("DatSock��������ʧ�ܣ�"));
		return;
	}

	MessageBox(_T("DatSock�ɹ���������!"));
	WSAAsyncSelect(DatSock, m_hWnd, WM_DATA_SOCKET, FD_ACCEPT);

	filelist_sock = 0;
	fl_sock_status = 0;

	//������ʼ��
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
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	if (Is_On)
	{
		OffLine();
		Is_On = 0;
	}
}


void CPRJ3_0613Dlg::OnBnClickedTuichuanniu()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
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

//λ�ã����շ�
//���ܣ�������������飬���������ѯֵ
//���룺
//	buff[]�������յ������������
//�����
//  ����õ�����ѯֵ
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
		//printf("�õ�%d��������(16����)��%x\n", i + 1, recvnum[i]);
	}
	return sum ^Password;
}


//λ�ã����շ�
//���ܣ������鷴����������
//���룺
//	key���������ֽ���
//	idx_���������±�ָ��
//	buff�������յ������������
//�����
//  �õ�������ֵ
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




LRESULT CPRJ3_0613Dlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)		//��Ϣ��������Զ��壩
{
	// TODO: Add your specialized code here and/or call the base class
	int Event, len, retval, serial;
	CString notify;
	CtlPDU* ctlbuff;

	SOCKET s = static_cast<SOCKET>(wParam);
	Event = LOWORD(lParam);
	switch(message){
	//�����׽���
	case WM_SOCKET:
		if (s != CtlSock){
			MessageBox(_T("�׽��ִ���"));
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
			//��ѯPDU
			case INQUIRY_PDU_TYPE:
				Re_Inquiry(CtlSock, tempcmdbuff);
				break;
			//������ϢPDU
			case CTL_PDU_TYPE:
				switch (ctlbuff->cmd){
				//�յ������������ʺű���
				case REQ_ACCOUNT:
					Re_Account(CtlSock);
					break;
				case RE_ACCESS:
					MessageBox(_T("��֤�ɹ�����ӭʹ�ã�"));
					GUIOnline();
					break;
				case RE_ERR_ACCOUNT:
					MessageBox(_T("�޴��û���"));
					OffLine();
					break;
				case RE_ERR_INQUIRY:
					MessageBox(_T("�������"));
					OffLine();
					break;
				case RE_ERR_FILENAME:
					MessageBox(_T("�ļ�������"));
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

	//�ļ������׽���
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
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
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
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
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
		MessageBox(_T("���ݶ������������Ժ�"), _T("��������"), MB_OK);
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

	//���ԭ�е��ļ��б�
	ClearFileList();
	if (filelist_sock != 0){
		closesocket(filelist_sock);
		filelist_sock = 0;
		fl_sock_status = S_CLOSE;
	}

	//Recurse(_T("E:\\ʵ�����"));
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	CtlPDU *sendbuff = (CtlPDU *)malloc(sizeof(CtlPDU));
	sendbuff->type = CTL_PDU_TYPE;
	sendbuff->cmd = REQ_FILELIST;
	sendbuff->len = 0;
	retval = send(CtlSock, (char *)sendbuff, 1 + 1 + 1, 0);

	if (retval < 1 + 1 + 1){
		MessageBox(_T("�����ļ��б�����ʧ�ܣ�"), _T("���ʹ���"), MB_OK);
		return;
	}

}


void CPRJ3_0613Dlg::OnBnClickedXiazaianniu()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	int buf_len;
	CString strText;
	int XiaZaiCurSel = m_XiaZaiLieBiao.GetCurSel();
	if (XiaZaiCurSel < 0)
	{
		MessageBox(_T("δѡ�������ļ���"));
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
		MessageBox(_T("���ݶ������������Ժ�"), _T("��������"), MB_OK);
		return;
	}
	for (i = 0; i < SOCK_LIST_SIZE; i++){
		if (sock_list[i] == 0 && FileName[i] == ""){
			break;
		}
	}
	sock_num += 1;
	FileName[i] = strText;
	FilePath[i] = "C://ClientData//" + strText;		//////////////////////////////////////////////////////////��ʱ·��������
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

//�������ظ��ʺţ��������á�����
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
		MessageBox(_T("�ظ��ʺŴ���"), _T("����"), MB_OK);
	}
	free(ctlbuff);
	return;
}

//�������ظ���ѯֵ��������sock���á�����
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
		MessageBox(_T("������ѯֵ����"), _T("����"), MB_OK);
	}

	free(sendbuff);
	inqbuff = NULL;

	return;
}


//�����������ļ��׽���
//���ܣ��ڿͻ�����������������ػ��ϴ�֮�󣬷�������������ͻ������ӣ��ú�������������ӵĽ���
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
		MessageBox(TEXT("��������رգ�"), TEXT("�������Ӵ���"), MB_OK);
		return;
	}
	//���յ�һ���µ���������
	MessageBox(TEXT("���յ�һ���µ��������ӣ�"), TEXT("������"), MB_OK);

	recv_buf = (char *)malloc(BUF_SIZE);
	retval = recv(new_sock, recv_buf, BUF_SIZE, 0);
	if (retval <= 0){
		//���մ���
		MessageBox(TEXT("������������رգ�"), TEXT("�������Ӵ���"), MB_OK);
		closesocket(new_sock);
		return;
	}
	recv_buf[retval] = 0;

	FilePDU *file_pdu;
	file_pdu = (FilePDU*)recv_buf;
	if (file_pdu->type != FILE_PDU_TYPE){
		//���������յ�����PDU
		MessageBox(TEXT("���������յ�����PDU��"), TEXT("�����������Ӵ���"), MB_OK);
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
		//����δ�ҵ�
		MessageBox(TEXT("���������յ�PDU�е��ļ�������"), TEXT("�����������Ӵ���"), MB_OK);
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

//��������ʼ�����ļ��б�
//���ܣ�
//
//����� 0 �ɹ��� <0 ʧ��
short CPRJ3_0613Dlg::StartRecvFileList(SOCKET s, FilePDU *file_pdu)
{
	int buf_len;
	long retval;
	char *recv_buf;
	//FilePDU *file_pdu;
	CString filename;

	//��ӡ����Ļ
	filename = file_pdu->buf;
	AddToFileList(filename);

	//�ظ��ļ��б���֡
	file_pdu = (FilePDU *)malloc(sizeof(FilePDU));
	file_pdu->type = FILE_PDU_TYPE;
	file_pdu->No = 0;
	file_pdu->len = 3;
	strcpy(file_pdu->buf, "ACK");

	buf_len = 1 + 1 + 1 + file_pdu->len;

	retval = send(s, (char*)file_pdu, buf_len, 0);
	if (retval <= 0){
		//����ֵ�ж�
		MessageBox(TEXT("�ظ��ļ��б���֡ʧ�ܣ�"), TEXT("�ظ�����"), MB_OK);
		return retval;
	}

	filelist_sock = s;
	fl_sock_status = S_RECVFL_0;

	WSAAsyncSelect(s, m_hWnd, WM_FL_SOCK, FD_READ | FD_CLOSE);

	free(file_pdu);

	return 0;
}



//�����������ļ��б�
//
//
//����� 0 �ɹ��� -1 ʧ��
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
			MessageBox(TEXT("������Ϣ�������ӹرգ�"), TEXT("����ʧ��"), MB_OK);
			//m_InfoList.AddString(TEXT("�׽��ִ��󣺽�����Ϣ�������ӹرգ�"));
			//ShowInfo(TEXT("�׽��ִ��󣺽�����Ϣ�������ӹرգ�"));
			return -1;
		}
		else{
			//���������µĴ��󣬼������м���
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

	//��ӡ����Ļ
	filename = file_pdu->buf;
	AddToFileList(filename);

	free(recv_buf);

	//�ظ��ļ��б���֡
	file_pdu = (FilePDU *)malloc(sizeof(FilePDU));
	file_pdu->type = FILE_PDU_TYPE;
	file_pdu->No = tmp_No;
	file_pdu->len = 3;
	strcpy(file_pdu->buf, "ACK");

	buf_len = 1 + 1 + 1 + file_pdu->len;

	retval = send(s, (char*)file_pdu, buf_len, 0);
	if (retval <= 0){
		//����ֵ�ж�
		MessageBox(TEXT("�ظ��ļ��б���֡ʧ�ܣ�"), TEXT("�ظ�����"), MB_OK);
		return retval;
	}

	free(file_pdu);

	return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//�����������ļ���������
//���ܣ��յ��ļ��������󣬼����ļ����Ϸ��ԣ����ͺϷ��ļ������ļ���С
//���룺
//�����
short CPRJ3_0613Dlg::StartSendFile(SOCKET s, int i)
{
	int buf_len;
	long retval;
	char *send_buf;
	FILE *fp;
	FilePDU file_pdu;

	fopen_s(&fp, FilePath[i], "rb");
	if (fp == NULL){
		//��ʾ������Ϣ
		CString show_info;
		show_info += FilePath[i];
		show_info += "�����ڣ�";
		//ShowInfo(show_info);

		///////////////////////////////////////////////��֪�û���������
		//SendCtlPDU(s, RE_ERR_FILENAME);
		closesocket(s);///////////////////////////////////�ر��׽��֣����к���������
		return -1;
	}

	//������֡
	file_pdu.type = FILE_PDU_TYPE;
	file_pdu.No = 0;
	retval = fread(file_pdu.buf, 1, FILE_BUF_SIZE, fp);
	file_pdu.len = retval;
	buf_len = 1 + 1 + 1 + retval;

	//send_buf = (char*)&file_pdu;
	retval = send(s, (char*)&file_pdu, buf_len, 0);
	if (retval <= 0){
		//����ֵ�ж�
		MessageBox(TEXT("�������ӷ�������PDU��"), TEXT("�������Ӵ���"), MB_OK);
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


//�����������ļ�
//���ܣ����յ�ACK���ж�����׼��FilePDU�������ļ�Ƭ��
//���룺
//�����
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
		////////////////////////////////����DatSockû�ж��¼�����
		return -1;
	}

	recv_buf = (char*)malloc(BUF_SIZE);
	buf_len = recv(s, recv_buf, BUF_SIZE, 0);
	if (buf_len <= 0){
		retval = WSAGetLastError();
		if (retval != WSAEWOULDBLOCK){
			closesocket(s);
			MessageBox(TEXT("������Ϣ�������ӹرգ�"), TEXT("����ʧ��"), MB_OK);
			//m_InfoList.AddString(TEXT("�׽��ִ��󣺽�����Ϣ�������ӹرգ�"));
			//ShowInfo(TEXT("�׽��ִ��󣺽�����Ϣ�������ӹرգ�"));
			return -1;
		}
		else{
			//���������µĴ��󣬼������м���
			return -1;
		}
	}
	recv_buf[buf_len] = 0;

	file_pdu = (FilePDU *)recv_buf;

	//CString ack = "ACK";
	if (file_pdu->type != FILE_PDU_TYPE){
		////////////////////////////////////////////////����
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

	//�ҵ���Ӧ�׽���
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
		MessageBox(TEXT("�ļ�������ϣ�"), TEXT("�ϴ�"), MB_OK);
	}
	else{
		retval = fread(file_pdu->buf, 1, FILE_BUF_SIZE, file_pointer[i]);
		file_pdu->len = retval;
	}

	len = 1 + 1 + 1 + retval;

	retval = send(s, (char*)file_pdu, len, 0);
	if (retval <= 0){
		//����ֵ�ж�
		MessageBox(TEXT("�������ӷ�������PDU��"), TEXT("�������Ӵ���"), MB_OK);
		return retval;
	}

	last_len[i] = retval - 3;

	SetTimer(i + 1, TIMEOUT, 0);

	free(file_pdu);
	file_pdu = NULL;
	send_buf = NULL;
	return i;
}


//��������ʼ�����ļ�
//���ܣ����������ļ������½��׽��֣������ļ���Ϣ
//���룺
//�����
short CPRJ3_0613Dlg::StartRecvFile(SOCKET s, int i)
{
	int buf_len;
	long retval;
	struct sockaddr_in cli_addr;
	//char file_dir[100] = "C://ClientData//";			////////////////////////////////////////////////////��ʱ·��
	char *send_buf;
	FILE *fp;

	//strcat_s(file_dir, FILE_BUF_SIZE, FilePath[i]);

	if (_access(FilePath[i], 0) == 0){
		MessageBox(TEXT("�����ļ��Ѵ��ڣ��޷����գ�"), TEXT("����"), MB_OK);
		return -1;
	}
	fopen_s(&fp, FilePath[i], "wb");
	file_pointer[i] = fp;
	sock_status[i] = S_RECVFILE_0;

	//������֡�ظ�֡
	FilePDU file_pdu;
	file_pdu.type = FILE_PDU_TYPE;
	file_pdu.No = 0;
	strcpy(file_pdu.buf, "ACK");
	file_pdu.len = 3;

	buf_len = 1 + 1 + 1 + file_pdu.len;
	retval = send(s, (char *)&file_pdu, buf_len, 0);
	if (retval < 1 + 1 + 1 + file_pdu.len){
		//������֡ʧ��
		MessageBox(TEXT("������֡�ظ�֡ʧ�ܣ�"), TEXT("���ʹ���"),MB_OK);
		//�ر��׽���
		closesocket(s);
		return retval;
	}

	//����Ҫ���ݴ����û���Ϣ��Ԫ��
	WSAAsyncSelect(s, m_hWnd, WM_RECV_FILE_SOCK, FD_READ | FD_CLOSE);

	return i;
}


//�����������ļ�
//���ܣ����յ�һ֡�Ϸ�������֡�󣬷���ACK�����յ�len=0��֡����ر��׽��֡�
//���룺
//�����
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
			MessageBox(TEXT("������Ϣ�������ӹرգ�"), TEXT("����ʧ��"), MB_OK);
			free(recv_buf);
			return -1;
		}
		else{
			//���������µĴ��󣬼������м���
			free(recv_buf);
			return -1;
		}
	}
	recv_buf[buf_len] = 0;


	//�����ʶ��������ݰ�
	if (AbandonRand(P_AbandRand) == 0){
		free(recv_buf);
		MessageBox(TEXT("����һ�Σ�"), TEXT("���Գ�ʱ�ط�"), MB_OK);
		return -1;
	}


	//file_pdu = (struct FilePDU*)malloc(sizeof(struct FilePDU));
	file_pdu = (FilePDU *)recv_buf;

	if (file_pdu->type != FILE_PDU_TYPE){
		MessageBox(TEXT("�������Ͳ����Ͻ��������׽��֣�"),TEXT("���մ���"),MB_OK);
		free(recv_buf);
		return -1;
	}

	//���û��б����ҵ���Ӧ�û��׽���
	for (i = 0; i < SOCK_LIST_SIZE; i++){
		if (sock_list[i] == sock){
			break;
		}
	}

	//����յ��������ģ���ر�����׽���
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
			//ShowInfo(TEXT("�������ݰ���Ŵ���"));
			break;
		}
		tmp_No = 0;
		sock_status[i] = S_RECVFILE_0;
		break;
	case S_RECVFILE_0:
		if (file_pdu->No != 1){
			tmp_No = 0;
			//ShowInfo(TEXT("�������ݰ���Ŵ���"));
			break;
		}
		tmp_No = 1;
		sock_status[i] = S_RECVFILE_1;
		break;
	default:
		MessageBox(TEXT("�׽���״̬����"), TEXT("���մ���"), MB_OK);
		free(recv_buf);
		return -1;
	}

	retval = fwrite(file_pdu->buf, 1, file_pdu->len, file_pointer[i]);
	if (retval){
		////////////////////////////////////////////////////////////////////////����ֵ�ж�
	}

	free(recv_buf);
	recv_buf = NULL;

	//���ͻظ�����
	file_pdu = (FilePDU*)malloc(sizeof(FilePDU));
	file_pdu->type = FILE_PDU_TYPE;
	file_pdu->No = tmp_No;		//(file_pdu->No + 1) % 2;
	file_pdu->len = 3;
	strcpy(file_pdu->buf, "ACK");
	buf_len = 1 + 1 + 1 + file_pdu->len;
	send_buf = (char*)file_pdu;
	retval = send(sock, send_buf, buf_len, 0);
	if (retval < 0){
		/////////////////////////////////////////////////////����ֵ�ж�
		/////////////////////////////////////////////////////�ر��׽���
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

	//�½��ļ�PDU
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
		//����ֵ�ж�
		MessageBox(TEXT("�����ط�ʧ�ܣ�"), TEXT("�ط�����"), MB_OK);
	}

	last_len[i] = retval - 3;

	UINT timer_id = i + 1;
	SetTimer(timer_id, TIMEOUT, 0);

	free(file_pdu);
	file_pdu = NULL;
	return i;
}