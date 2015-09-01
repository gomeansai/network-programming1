#include "stdafx.h"
#include "UserUnit.h"


//函数：初始化用户单元
//功能：初始化用户ID，控制套接字及对应状态（S_AUTHSTART），用户IP地址及端口号
//输出：void
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

//生成质询数函数
//位置：发送方
//功能：生成发送的随机数数组，并计算出质询值用以验证
//输入：
//	buff[]——存储待发送的随机数PDU
//	bufflen——存储待发送的数组实际长度
//输出：
//  计算得到的质询值
unsigned long UserUnit::f_producenum(struct InquiryPDU *p_pdu, long secret, unsigned char *bufflen)		//unsigned char buff[]
{
	char key;
	unsigned long sum = 0;
	for (char i = 0; i < 4; i++)
	{
		p_pdu->each_len[i] = 0;
	}
	srand((int)time(0));	//srand()用来设置rand()产生随机数时的随机数种子。
	//char num = rand() % NUMRANGE + 1;		//质询数个数取值范围：1-16
	char num = 2;
	p_pdu->num = num;
	unsigned char idx = 0;		//数组指针
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
		//printf("第%d个生成数（16进制）：%x\n", i + 1, temp);
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

////////////////////////// 内部子函数用于找到 sock 位置 i ///////////////////////////////////////
short UserUnit::f_FindSock(SOCKET s, int i)
{
	//若-1<i<SOCK_LIST_SIZE，而第i位存储套接字与传入的套接字不同，则报错，终止
	if (i > -1 && i < SOCK_LIST_SIZE && sock_list[i] != s){
		printf("ERROR : Program is Changed !\n");
		return -1;
	}
	//如果i为-1，或传入值大于SOCK_LIST_SIZE，则将i赋值为0，准备开始循环查找
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

//函数：插入套接字管理队列
//目标：将新套接字（s）插入到套接字管理队列（list）中
//输入：套接字 SOCKET s，套接字管理队列 SocketList* list
//输出：套接字插入队列中的位置 int x， -1 代表 未找到
short UserUnit::InsertSocketList(SOCKET s, char status, FILE *fp)
{
	//如果套接字队列已满，报错，终止插入
	if (sock_num == SOCK_LIST_SIZE){
		printf("INSERT ERROR : SocketList is Full !\n");
		return -1;
	}

	int x, j;
	//找到一个值为0的位置x，给新套接字插入
	x = f_FindSock(0, -1);

	sock_list[x] = s;
	sock_status[x] = status;
	file_pointer[x] = fp;
	sock_num += 1;

	return x;
}


//函数：删除套接字
//目标：从套接字管理队列（list）中删除套接字（s）
//输入：套接字在队列中位置 int j;
//输出：无 void
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




//函数：改变套接字状态
//目标：将套接字（s）状态改为状态（status）
//输入：套接字 SOCKET s， 套接字管理队列 SocketList *list， 新状态 char status， 可选参数 套接字在队列中位置 int i = -1
//输出： -1 代表 未找到， i（>0）代表 套接字对应位置
short UserUnit::F_SetSockStatus(SOCKET s, struct SocketList *list, char status, int i)
{
	i = f_FindSock(s, list, i);
	if (i == -1)
		return -1;

	sock_status[i] = status;
	return i;
}

*/