#include "stdafx.h"
#include "UserUnit.h"


//��������ʼ���û���Ԫ
//���ܣ���ʼ���û�ID�������׽��ּ���Ӧ״̬��S_AUTHSTART�����û�IP��ַ���˿ں�
//�����void
UserUnit::UserUnit(unsigned long id, SOCKET s, ULONG ip, USHORT port)
{
	user_id = id;
	ctl_sock = s;
	ctl_sock_status = S_AUTHSTART;
	user_ip = ip;
	user_port = port;
	//inquiry_num = inq_n;

	fl_sock = 0;
	fls_status = S_CLOSE;

	sock_num = 0;
	int j;
	for (j = 0; j < SOCK_LIST_SIZE; j++){
		sock_list[j] = 0;
		sock_status[j] = S_CLOSE;
		file_pointer[j] = NULL;
		last_len[j] = 0;
	}
}


UserUnit::~UserUnit()
{
}

//������ѯ������
//λ�ã����ͷ�
//���ܣ����ɷ��͵���������飬���������ѯֵ������֤
//���룺
//	buff[]�����洢�����͵������PDU
//	bufflen�����洢�����͵�����ʵ�ʳ���
//�����
//  ����õ�����ѯֵ
unsigned long UserUnit::f_producenum(struct InquiryPDU *p_pdu, long secret, unsigned char *bufflen)		//unsigned char buff[]
{
	char key;
	unsigned long sum = 0;
	for (char i = 0; i < 4; i++)
	{
		p_pdu->each_len[i] = 0;
	}
	srand((int)time(0));	//srand()��������rand()���������ʱ����������ӡ�
	//char num = rand() % NUMRANGE + 1;		//��ѯ������ȡֵ��Χ��1-16
	char num = 2;
	p_pdu->num = num;
	unsigned char idx = 0;		//����ָ��
	for (char i = 0; i < num; i++){
		unsigned char bytelen;
		bytelen = rand() % 4 + 1;
		unsigned int temp = (rand()*rand());
		switch (bytelen){
		case 1:
			temp = (rand()*rand()) % 0x100;
			key = 1;
			p_pdu->inquiry_buf[idx] = temp & 0xFF;
			idx++;
			break;
		case 2:
			temp = (rand()*rand()) % 0x10000;
			key = 2;
			p_pdu->inquiry_buf[idx] = (temp & 0xFF00) >> 8;
			idx++;
			p_pdu->inquiry_buf[idx] = temp & 0xFF;
			idx++;
			break;
		case 3:
			temp = (rand()*rand()) % 0x1000000;
			key = 3;
			p_pdu->inquiry_buf[idx] = (temp & 0xFF0000) >> 16;
			idx++;
			p_pdu->inquiry_buf[idx] = (temp & 0xFF00) >> 8;
			idx++;
			p_pdu->inquiry_buf[idx] = temp & 0xFF;
			idx++;
			break;
		case 4:
			key = 0;
			temp = (rand()*rand()) % 0x100000000;
			p_pdu->inquiry_buf[idx] = (temp & 0xFF000000) >> 24;
			idx++;
			p_pdu->inquiry_buf[idx] = (temp & 0xFF0000) >> 16;
			idx++;
			p_pdu->inquiry_buf[idx] = (temp & 0xFF00) >> 8;
			idx++;
			p_pdu->inquiry_buf[idx] = temp & 0xFF;
			idx++;
		}
		sum += temp;
		unsigned char tt = key << ((3 - (i % 4)) * 2);
		p_pdu->each_len[i / 4] |= key << ((3 - (i % 4)) * 2);
		//printf("��%d����������16���ƣ���%x\n", i + 1, temp);
	}
	*bufflen = idx;
	inquiry_num = sum ^ secret;
	return inquiry_num;
}

void UserUnit::DeleteUserUnit()
{
	int j;

	closesocket(ctl_sock);

	if (fl_sock != 0){
		DeleteFilelistSock();
	}

	j = 0;
	while (sock_num != 0 && j < SOCK_LIST_SIZE){
		if (sock_list[j] != 0){
			DeleteSockList(j);
		}
		j++;
	}
}

void UserUnit::DeleteFilelistSock()
{
	closesocket(fl_sock);
	fl_sock = 0;
	fls_status = 0;
	fl_finder.Close();
}

////////////////////////// �ڲ��Ӻ��������ҵ� sock λ�� i ///////////////////////////////////////
short UserUnit::f_FindSock(SOCKET s, int i)
{
	//��-1<i<SOCK_LIST_SIZE������iλ�洢�׽����봫����׽��ֲ�ͬ���򱨴���ֹ
	if (i > -1 && i < SOCK_LIST_SIZE && sock_list[i] != s){
		printf("ERROR : Program is Changed !\n");
		return -1;
	}
	//���iΪ-1������ֵ����SOCK_LIST_SIZE����i��ֵΪ0��׼����ʼѭ������
	if (i <= -1 || i >= SOCK_LIST_SIZE){
		i = 0;
	}
	while (i < SOCK_LIST_SIZE){
		if (sock_list[i] == s)
			return i;
		i++;
	}
	if (i == SOCK_LIST_SIZE){
		printf("ERROR : Cannot Find Sock %d !\n", s);
		return -1;
	}
}

//�����������׽��ֹ������
//Ŀ�꣺�����׽��֣�s�����뵽�׽��ֹ�����У�list����
//���룺�׽��� SOCKET s���׽��ֹ������ SocketList* list
//������׽��ֲ�������е�λ�� int x�� -1 ���� δ�ҵ�
short UserUnit::InsertSocketList(SOCKET s, char status, FILE *fp)
{
	//����׽��ֶ���������������ֹ����
	if (sock_num == SOCK_LIST_SIZE){
		printf("INSERT ERROR : SocketList is Full !\n");
		return -1;
	}

	int x, j;
	//�ҵ�һ��ֵΪ0��λ��x�������׽��ֲ���
	x = f_FindSock(0, -1);

	sock_list[x] = s;
	sock_status[x] = status;
	file_pointer[x] = fp;
	sock_num += 1;

	return x;
}


//������ɾ���׽���
//Ŀ�꣺���׽��ֹ�����У�list����ɾ���׽��֣�s��
//���룺�׽����ڶ�����λ�� int j;
//������� void
void UserUnit::DeleteSockList(int j)
{
	//j = f_FindSock(s, j);

	closesocket(sock_list[j]);
	sock_list[j] = 0;

	sock_status[j] = S_CLOSE;
	
	fclose(file_pointer[j]);
	file_pointer[j] = NULL;
	last_len[j] = 0;
	
	sock_num -= 1;
}




/*




//�������ı��׽���״̬
//Ŀ�꣺���׽��֣�s��״̬��Ϊ״̬��status��
//���룺�׽��� SOCKET s�� �׽��ֹ������ SocketList *list�� ��״̬ char status�� ��ѡ���� �׽����ڶ�����λ�� int i = -1
//����� -1 ���� δ�ҵ��� i��>0������ �׽��ֶ�Ӧλ��
short UserUnit::F_SetSockStatus(SOCKET s, struct SocketList *list, char status, int i)
{
	i = f_FindSock(s, list, i);
	if (i == -1)
		return -1;

	sock_status[i] = status;
	return i;
}

*/