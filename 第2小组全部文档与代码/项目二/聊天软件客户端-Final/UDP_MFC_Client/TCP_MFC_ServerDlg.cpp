
// UDP_MFC_ClientDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "TCP_MFC_Server.h"
#include "TCP_MFC_ServerDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CUDP_MFC_ClientDlg �Ի���



CUDP_MFC_ClientDlg::CUDP_MFC_ClientDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CUDP_MFC_ClientDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CUDP_MFC_ClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, Combo_Bind, m_Combo_Bind);
	DDX_Control(pDX, Edit_IP_Addr, m_Edit_IP_Addr);
	DDX_Control(pDX, Combo_Friend_List, m_Combo_Friend_List);
	DDX_Control(pDX, List_Message, m_List_Msg);
	DDX_Control(pDX, Button_Send, m_Button_Send);
}

BEGIN_MESSAGE_MAP(CUDP_MFC_ClientDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(Button_Exit, &CUDP_MFC_ClientDlg::OnBnClickedExit)
	ON_BN_CLICKED(Button_Start, &CUDP_MFC_ClientDlg::OnBnClickedStart)
	ON_BN_CLICKED(Button_Stop, &CUDP_MFC_ClientDlg::OnBnClickedStop)
	ON_CBN_SELCHANGE(Combo_Bind, &CUDP_MFC_ClientDlg::OnCbnSelchangeBind)
	ON_BN_CLICKED(Button_Send, &CUDP_MFC_ClientDlg::OnBnClickedSend)
END_MESSAGE_MAP()


// CUDP_MFC_ClientDlg ��Ϣ�������

BOOL CUDP_MFC_ClientDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	ShowWindow(SW_NORMAL);

	// TODO:  �ڴ���Ӷ���ĳ�ʼ������
	UserInit();
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CUDP_MFC_ClientDlg::OnPaint()
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
HCURSOR CUDP_MFC_ClientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CUDP_MFC_ClientDlg::OnBnClickedExit()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	closesocket(sock);
	WSACleanup();
	exit(0);
}


void CUDP_MFC_ClientDlg::OnBnClickedStart()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	CString addr_str, port_str;
	int retval;
	sockaddr_in local, server;
	sock = socket(AF_INET, SOCK_STREAM, 0);

	int nIndex = m_Combo_Bind.GetCurSel();
	if (nIndex == 1){
		GetDlgItem(Button_Start)->EnableWindow(FALSE);
		local.sin_family = AF_INET;
		GetDlgItemText(Edit_IP_Addr, addr_str);
		local.sin_addr.S_un.S_addr = inet_addr(addr_str.GetBuffer(sizeof(addr_str)));
		GetDlgItemText(Edit_Port, port_str);
		local.sin_port = htons(atoi(port_str.GetBuffer(sizeof(addr_str))));
		if (bind(sock, (sockaddr*)&local, sizeof(local)) != 0){
			retval = WSAGetLastError();
			MessageBox((LPCTSTR)"��ʽ��ʧ��", (LPCTSTR)"cli", MB_OK);
			GetDlgItem(Button_Start)->EnableWindow(TRUE);
			return;
		}
	}

	server.sin_family = AF_INET;
	server.sin_addr.S_un.S_addr = inet_addr(DEFAULT_SERVER_IP);
	server.sin_port = htons(DEFAULT_SERVER_PORT);

	WSAAsyncSelect(sock, m_hWnd, WM_SOCKET, FD_CLOSE | FD_CONNECT);
	connect(sock, (sockaddr*)&server, sizeof(server));
}


void CUDP_MFC_ClientDlg::OnBnClickedStop()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	GetDlgItem(Button_Start)->EnableWindow(TRUE);
	GetDlgItem(Button_Stop)->EnableWindow(FALSE);
	GetDlgItem(Button_Send)->EnableWindow(FALSE);
	GetDlgItem(Combo_Bind)->EnableWindow(TRUE);
	GetDlgItem(Edit_Name)->EnableWindow(TRUE);
	closesocket(sock);
	GetDlgItem(Label_State)->SetWindowText("������");
}



LRESULT CUDP_MFC_ClientDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO: Add your specialized code here and/or call the base class
	int Event, len, retval, serial;
	CString name, notify;
	switch (message){
	case WM_SOCKET:
		Event = LOWORD(lParam);
		switch (Event){
		case FD_CONNECT:
			GetDlgItem(Label_State)->SetWindowText("�������ӷ���������");
			WSAAsyncSelect(sock, m_hWnd, WM_SOCKET, FD_WRITE | FD_READ | FD_CLOSE);
			break;
		case FD_WRITE:
			GetDlgItem(Label_State)->SetWindowText("������");
			GetDlgItem(Button_Send)->EnableWindow(TRUE);
			GetDlgItem(Button_Stop)->EnableWindow(TRUE);
			GetDlgItem(Button_Start)->EnableWindow(FALSE);
			GetDlgItem(Combo_Bind)->EnableWindow(FALSE);
			GetDlgItem(Edit_Name)->EnableWindow(FALSE);
			GetDlgItemText(Edit_Name, name);
			name += '\0';
			send(sock, name, name.GetLength(), 0);
			break;
		case FD_READ:
			len = recv(sock, msgpack.char_buf, sizeof(msgpack), 0);
			if (len <= 0){
				retval = WSAGetLastError();
				if (retval != WSAEWOULDBLOCK){
					GetDlgItem(Label_State)->SetWindowText("�����쳣");
					closesocket(sock);
				}
				break;
			}
			switch (msgpack.msg.command){
			case 'U':
				listlen++;
				notify = "���ѡ�";
				notify += msgpack.msg.msg_buf;
				notify += "��������";
				GetDlgItem(Label_State)->SetWindowText(notify);
				for (int i = 0; i < LISTMAXLEN; i++)
				{
					if (infomanage[i].id == 0){
						infomanage[i].id = msgpack.msg.ID;
						strcpy(infomanage[i].name, msgpack.msg.msg_buf);
						break;
					}
				}
				Modify_Friend_List();
				break;
			case 'D':
				listlen--;
				notify = "���ѡ�";
				notify += msgpack.msg.msg_buf;
				notify += "��������";
				GetDlgItem(Label_State)->SetWindowText(notify);
				for (int i = 0; i < LISTMAXLEN; i++)
				{
					if (infomanage[i].id == msgpack.msg.ID){
						infomanage[i].id = 0;
						infomanage[i].num = 0;
						break;
					}
				}
				serial = 1;
				Modify_Friend_List();
				break;
			case 'M':
				for (int i = 0; i < LISTMAXLEN; i++)
				{
					if (infomanage[i].id == msgpack.msg.ID){
						//�²��������������
						int nIndex = m_List_Msg.GetItemCount();
						LV_ITEM lvItem;
						lvItem.mask = LVIF_TEXT;
						lvItem.iItem = nIndex; //����
						lvItem.iSubItem = 0;
						lvItem.pszText = infomanage[i].name; //��һ��
						//�����һ�в����¼ֵ.
						m_List_Msg.InsertItem(&lvItem);
						//����������
						CString  strTime;
						CTime time;
						time = CTime::GetCurrentTime();
						strTime = time.Format("%H:%M:%S");
						m_List_Msg.SetItemText(nIndex, 1, strTime);
						m_List_Msg.SetItemText(nIndex, 2, msgpack.msg.msg_buf);
						break;
					}
				}
				break;
			}
			break;
		case FD_CLOSE:
			GetDlgItem(Label_State)->SetWindowText("�����쳣");
			closesocket(sock);
			break;
		}
		break;
	}
	return CDialog::WindowProc(message, wParam, lParam);
}

void CUDP_MFC_ClientDlg::OnCbnSelchangeBind()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	int nIndex = m_Combo_Bind.GetCurSel();
	if (nIndex == 0){
		GetDlgItem(Edit_IP_Addr)->EnableWindow(FALSE);
		GetDlgItem(Edit_Port)->EnableWindow(FALSE);
	}
	else{
		GetDlgItem(Edit_IP_Addr)->EnableWindow(TRUE);
		GetDlgItem(Edit_Port)->EnableWindow(TRUE);
	}
}

void CUDP_MFC_ClientDlg::UserInit()
{
	GetDlgItem(Button_Send)->EnableWindow(FALSE);
	GetDlgItem(Button_Stop)->EnableWindow(FALSE);
	GetDlgItem(Edit_IP_Addr)->EnableWindow(FALSE);
	GetDlgItem(Edit_Port)->EnableWindow(FALSE);
	m_Combo_Bind.SetCurSel(0);
	GetDlgItem(Label_State)->SetWindowText("�ȴ�����");
	LONG lStyle;
	lStyle = GetWindowLong(m_List_Msg.m_hWnd, GWL_STYLE);//��ȡ��ǰ����style
	lStyle &= ~LVS_TYPEMASK; //�����ʾ��ʽλ
	lStyle |= LVS_REPORT; //����style
	SetWindowLong(m_List_Msg.m_hWnd, GWL_STYLE, lStyle);//����style
	DWORD dwStyle = m_List_Msg.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT;//ѡ��ĳ��ʹ���и�����ֻ������report����listctrl��
	dwStyle |= LVS_EX_GRIDLINES;//�����ߣ�ֻ������report����listctrl��
	m_List_Msg.SetExtendedStyle(dwStyle); //������չ���
	m_List_Msg.InsertColumn(0, (LPCTSTR)_T("�ǳ�"), LVCFMT_CENTER, 80);
	m_List_Msg.InsertColumn(1, (LPCTSTR)_T("ʱ��"), LVCFMT_CENTER, 80);
	m_List_Msg.InsertColumn(2, (LPCTSTR)_T("��Ϣ"), LVCFMT_CENTER, 180);

	for (int i = 0; i < LISTMAXLEN; i++)
	{
		infomanage[i].id = 0;
	}
}

void CUDP_MFC_ClientDlg::Modify_Friend_List()
{
	int serial = 1;
	for (int i = 0; i < LISTMAXLEN; i++)
	{
		if (infomanage[i].id != 0)
			infomanage[i].num = serial++;
	}
	m_Combo_Friend_List.ResetContent();
	m_Combo_Friend_List.InsertString(0, "Ⱥ��");
	for (int i = 1; i < listlen + 1; i++)
	{
		for (int j = 0; j < LISTMAXLEN; j++)
		{
			if (infomanage[j].num == i)
			{
				m_Combo_Friend_List.InsertString(i, infomanage[j].name);
				break;
			}
		}
	}
}

void CUDP_MFC_ClientDlg::OnBnClickedSend()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	int i;
	CString cs_msg_in;
	int nIndex = m_Combo_Friend_List.GetCurSel();
	if (nIndex == 0)
		msgpack.msg.ID = 0;
	else
	{
		for (i = 0; i < LISTMAXLEN; i++)
		{
			if (infomanage[i].num == nIndex)
			{
				msgpack.msg.ID = infomanage[i].id;
				break;
			}
		}
	}
	msgpack.msg.command = 'M';
	GetDlgItemText(Edit_Send, cs_msg_in);
	cs_msg_in += '\0';
	strcpy(msgpack.msg.msg_buf, cs_msg_in.GetBuffer(sizeof(cs_msg_in)));
	send(sock, msgpack.char_buf, sizeof(msgpack.char_buf), 0);
	GetDlgItem(Edit_Send)->SetWindowText(_T(""));
}

