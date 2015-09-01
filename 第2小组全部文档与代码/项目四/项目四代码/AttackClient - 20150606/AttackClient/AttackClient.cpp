// AttackClient.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "windows.h"
#include "winsock.h"
#include "stdio.h"
#include "time.h"
#pragma comment (lib,"wsock32.lib")
#pragma warning(disable:4996)

#define MAX_THRD_SIZE 10000			// 10K
#define MAX_BUF_SIZE 1000000		// 1M
#define MIN_BUF_SIZE 100			// 1K
#define SLEEP_TIME 1000				// 1s
#define DOUBLE_SIZE 20				//输出时，动态申请一个字符串

void my_printf(char * p)
{
	printf("%s\n", p);
}
void my_scanf(char * buf, int buf_size)
{
	scanf("%s", buf);
}

struct InPara{
	SOCKET sock;
	LARGE_INTEGER first_send;	//用于记录发送第一帧的时间
	LARGE_INTEGER last_send;	//用于记录发送最后一帧的时间
	LARGE_INTEGER first_recv;
	LARGE_INTEGER last_recv;
	long data_total;			//总计需要发送数据大小
	long send_total;			//已发送的数据大小
	long recv_total;			//已接收的数据大小
	char flag;					//为0时代表可使用，为1时代表主函数在使用，
	//为2时代表发送线程在使用，为3时代表接收线程在使用
};
struct InParaArray{
	long num_thread;
	struct InPara *p_InPara[MAX_THRD_SIZE];
};

DWORD WINAPI client_send_proc(LPVOID lpParam);
DWORD WINAPI client_recv_proc(LPVOID lpParam);
DWORD WINAPI show_thread(LPVOID lpParam);

int _tmain(int argc, _TCHAR* argv[])
{
	SOCKET s;
	struct sockaddr_in server;
	WSAData wsa;
	int retval;
	int i;
	FILE *fp;

	char scanbuf[80];
	//char *sendbuf = (char*) malloc(MAX_SENDBUF_SIZE);
	long sum_char;
	//long num_thread;
	//用于计算延时的变量
	LARGE_INTEGER Freq;
	double t_measure;
	//用于传参
	//struct InPara *p_InPara[MAX_THRD_SIZE];
	struct InParaArray *p_IPA;
	DWORD dwThrdParam[MAX_THRD_SIZE];
	DWORD dwThreadID[MAX_THRD_SIZE];
	HANDLE hSendThrd[MAX_THRD_SIZE];
	HANDLE hRecvThrd[MAX_THRD_SIZE];

	DWORD dwShowThrdParam;
	HANDLE hShowThrd;
	DWORD dwShowThreadID;
	WSAStartup(0x101, &wsa);

	printf("收发双线程攻击客户端开启！\n\n");
	p_IPA = (struct InParaArray *)malloc(sizeof(struct InParaArray));

	//输入设置基本参数
	server.sin_family = AF_INET;

	printf("请输入服务器IP地址 : ");
	my_scanf(scanbuf, sizeof(scanbuf));
	server.sin_addr.S_un.S_addr = inet_addr(scanbuf);

	printf("请输入服务器端口号 : ");
	my_scanf(scanbuf, sizeof(scanbuf));
	server.sin_port = htons(atoi(scanbuf));

	printf("请输入建立连接数量(不超过%d): ", MAX_THRD_SIZE);
	my_scanf(scanbuf, sizeof(scanbuf));
	p_IPA->num_thread = atol(scanbuf);

	printf("请输入每个连接所需发送字节数 : ");
	my_scanf(scanbuf, sizeof(scanbuf));
	sum_char = atol(scanbuf);

	printf("\n");

	printf("已建立连接数 ： %8d", 0);
	//开始循环生成线程
	for (i = 0; i < p_IPA->num_thread; i++){
		p_IPA->p_InPara[i] = (struct InPara *)malloc(sizeof(struct InPara));

		p_IPA->p_InPara[i]->sock = socket(AF_INET, SOCK_STREAM, 0);
		retval = connect(p_IPA->p_InPara[i]->sock, (sockaddr*)&server, sizeof(server));
		if (retval != 0){
			printf("ERROR : 套接字 s=%d 无法连接服务器！\n", p_IPA->p_InPara[i]->sock);
			printf("重新连接！\n");
			//连接数量减少1，回到循环重新建立
			printf("已建立连接数 ： %8d", i);
			i--;
			continue;
		}

		//打印已建立连接数量
		printf("\b\b\b\b\b\b\b\b");
		printf("%8d", i + 1);

		//参数变量初始化
		//p_InPara[i]->sock = sock;
		p_IPA->p_InPara[i]->data_total = sum_char;
		p_IPA->p_InPara[i]->send_total = 0;
		p_IPA->p_InPara[i]->recv_total = 0;
		p_IPA->p_InPara[i]->flag = 0;
		p_IPA->p_InPara[i]->first_send.LowPart = 0;
		p_IPA->p_InPara[i]->first_send.HighPart = 0;
		p_IPA->p_InPara[i]->last_send.LowPart = 0;
		p_IPA->p_InPara[i]->last_send.HighPart = 0;
		p_IPA->p_InPara[i]->first_recv.LowPart = 0;
		p_IPA->p_InPara[i]->first_recv.HighPart = 0;
		p_IPA->p_InPara[i]->last_recv.LowPart = 0;
		p_IPA->p_InPara[i]->last_recv.HighPart = 0;

		dwThrdParam[i] = (DWORD)(p_IPA->p_InPara[i]);// 坑货指针！！！！！！！！
		hSendThrd[i] = CreateThread(
			NULL,
			0,
			client_send_proc,
			&dwThrdParam[i],
			CREATE_SUSPENDED,	//挂起
			&dwThreadID[i]);

		hRecvThrd[i] = CreateThread(
			NULL,
			0,
			client_recv_proc,
			&dwThrdParam[i],
			CREATE_SUSPENDED,	//挂起
			&dwThreadID[i]);

		//printf("套接字 s=%d 对应收发线程创建成功！\n\n", s);
	}

	printf("\n");

	for (i = 0; i < p_IPA->num_thread; i++){		// 由 MAX_THRD_SIZE 改为 num_thread
		ResumeThread(hSendThrd[i]);
		ResumeThread(hRecvThrd[i]);
	}

	/*
	//开启打印屏幕线程
	dwShowThrdParam = (DWORD)p_IPA;
	hShowThrd = CreateThread(
	NULL,
	0,
	show_thread,
	&dwShowThrdParam,
	0,
	&dwShowThreadID);
	*/

	//获取频率
	QueryPerformanceFrequency(&Freq);

	int num;
	char c_num[DOUBLE_SIZE];
	LPDWORD lpExitCode;
	DWORD d_retval;

	//开始处理并记录延时
	fopen_s(&fp, "test_time.txt", "w");

	fwrite("线程延时统计(us)\n", 17, 1, fp);

	fwrite("线程总数\t", 30, 1, fp);
	_itoa(p_IPA->num_thread, c_num, 10);	//10->十进制
	fwrite(c_num, strlen(c_num), 1, fp);
	fwrite("\n", 1, 1, fp);

	fwrite("每个线程发送数据总量\t", 21, 1, fp);
	_itoa(sum_char, c_num, 10);
	fwrite(c_num, strlen(c_num), 1, fp);
	fwrite("\n", 1, 1, fp);

	fwrite("sock", 4, 1, fp);
	fwrite("\t", 1, 1, fp);
	fwrite("first_send~last_send", 20, 1, fp);
	fwrite("\t", 1, 1, fp);
	fwrite("first_send~first_recv", 21, 1, fp);
	fwrite("\t", 1, 1, fp);
	fwrite("first_send~last_recv", 20, 1, fp);
	fwrite("\n", 1, 1, fp);

	for (i = 0, num = 0; num < p_IPA->num_thread; i = (i + 1) % p_IPA->num_thread){
		Sleep(SLEEP_TIME);
		if (hRecvThrd[i] == NULL){
			continue;
		}
		//printf("s = %d\n<--%8d\n-->%8d\n", p_InPara[i]->sock, p_InPara[i]->send_total, p_InPara[i]->recv_total);
		d_retval = WaitForSingleObject(hRecvThrd[i], 0);
		//判断线程是否结束
		if (d_retval == WAIT_OBJECT_0){
			//线程已结束
			//判断线程对应套接字是否已经被计算过延时
			if (p_IPA->p_InPara[i]->flag == 1){
				//未被计算过，则开始计算
				//写入套接字大小

				//sock
				_itoa(p_IPA->p_InPara[i]->sock, c_num, 10);	//10->十进制
				fwrite(c_num, strlen(c_num), 1, fp);
				fwrite("\t", 1, 1, fp);

				//first_send ~ last_send
				t_measure = ((p_IPA->p_InPara[i]->last_send.QuadPart - p_IPA->p_InPara[i]->first_send.QuadPart) * 1000000) / (double)Freq.QuadPart;
				_gcvt(t_measure, DOUBLE_SIZE - 2, c_num);
				fwrite(c_num, DOUBLE_SIZE - 2, 1, fp);
				fwrite("\t", 1, 1, fp);

				//first_send ~ first_recv
				t_measure = ((p_IPA->p_InPara[i]->first_recv.QuadPart - p_IPA->p_InPara[i]->first_send.QuadPart) * 1000000) / (double)Freq.QuadPart;
				_gcvt(t_measure, DOUBLE_SIZE - 2, c_num);
				fwrite(c_num, DOUBLE_SIZE - 2, 1, fp);
				fwrite("\t", 1, 1, fp);

				//first_send ~ last_recv
				t_measure = ((p_IPA->p_InPara[i]->last_recv.QuadPart - p_IPA->p_InPara[i]->first_send.QuadPart) * 1000000) / (double)Freq.QuadPart;
				_gcvt(t_measure, DOUBLE_SIZE - 2, c_num);
				fwrite(c_num, DOUBLE_SIZE - 2, 1, fp);
				fwrite("\n", 1, 1, fp);

				//closesocket(p_InPara[i]->sock);
				p_IPA->p_InPara[i]->flag = 2;
				num++;
			}
			continue;
		}
		//Sleep(SLEEP_TIME);		//延时
	}
	fclose(fp);

	system("pause");
	WSACleanup();
	return 0;
}

DWORD WINAPI show_thread(LPVOID lpParam)
{
	long former_send_total, latter_send_total;	//long long ? ///////////////////////////
	long former_recv_total, latter_recv_total;
	long flag_total;
	int i;

	struct InParaArray *para_array;
	para_array = *(struct InParaArray**)lpParam;

	former_send_total = 0;
	former_recv_total = 0;
	latter_send_total = 0;
	latter_recv_total = 0;

	printf("已建立连接数\t发送速率\t接收速率\n%4d\t", para_array->num_thread);
	printf("%4d\t%4d", latter_send_total - former_send_total, latter_recv_total - former_recv_total);

	while (1){
		latter_send_total = 0;
		latter_recv_total = 0;
		for (i = 0; i < para_array->num_thread; i++){
			latter_send_total += para_array->p_InPara[i]->send_total;
			latter_recv_total += para_array->p_InPara[i]->recv_total;
		}

		printf("\b\b\b\b\b\b\b\b\b");
		//printf("已建立连接数\t发送速率\t接收速率\n%4d\t", para_array->num_thread);
		printf("%4d\t%4d", latter_send_total - former_send_total, latter_recv_total - former_recv_total);

		former_send_total = latter_send_total;
		former_recv_total = latter_send_total;

		Sleep(SLEEP_TIME);
		//printf("%d\n", para_array->p_InPara[0]->data_total);
	}
	return 0;
}

DWORD WINAPI client_send_proc(LPVOID lpParam)
{
	struct InPara* para;
	long window_size;	//窗口大小
	long send_len;		//即将发送的长度
	long total;			//已发送总量
	int retval;
	char *sendbuf = NULL;

	para = *(struct InPara**)lpParam;	// 指针坑坑坑！！！！！
	window_size = MIN_BUF_SIZE;			//////////////////////////////////////////////发送线程固定窗口大小，可选择改为 MAX_BUF_SIZE
	send_len = 0;
	//total = 0;

	//printf("套接字 s = %d 发送线程开启成功！\n", para->sock);

	//开始循环发送
	while (para->send_total != para->data_total){
		//确定本次发送的数据长度
		send_len = para->data_total - para->send_total;
		if (send_len > window_size){
			send_len = window_size;
		}
		//申请动态内存空间
		sendbuf = (char*)malloc(window_size);
		if (sendbuf == NULL){
			printf("ERROR : 套接字 s=%d 没有足够缓存！\n", para->sock);
			//Sleep(SLEEP_TIME);		//延时
			continue;
		}

		retval = send(para->sock, sendbuf, send_len, 0);
		if (retval < 0){
			//Sleep(SLEEP_TIME);		//延时
			printf("ERROR : 套接字 s=%d 发送错误！ Retval=%d\n", para->sock, retval);
			free(sendbuf);
			sendbuf = NULL;
			continue;
		}
		if (retval == 0){
			printf("ERROR : 套接字 s=%d 发送出错！\n", para->sock);
			continue;
		}

		//若是第一次发送，记录时间
		if (para->send_total == 0){
			QueryPerformanceCounter(&para->first_send);
		}

		free(sendbuf);
		sendbuf = NULL;

		para->send_total += retval;
	}
	QueryPerformanceCounter(&para->last_send);

	printf("套接字 s = %d 发送线程发送完毕！\n", para->sock);

	return 0;
}

DWORD WINAPI client_recv_proc(LPVOID lpParam)
{
	struct InPara *para;
	char *recvbuf;
	long retval;
	long recv_len;

	para = *(struct InPara**)lpParam;
	recv_len = MAX_BUF_SIZE;	//////////////////////////////////////////////接收线程固定窗口大小，可选择改为 MIN_BUF_SIZ

	//printf("套接字 s = %d 接收线程开启成功！\n", para->sock);

	while (para->recv_total != para->data_total){
		//申请接收缓存区
		recvbuf = (char*)malloc(recv_len);
		if (recvbuf == NULL){
			//Sleep(SLEEP_TIME);		//延时
			printf("ERROR : 套接字 s=%d 内存不足！\n", para->sock);
			continue;
		}

		//接收
		retval = recv(para->sock, recvbuf, recv_len, 0);
		if (retval == 0){
			printf("\n");
			printf("ERROR : 套接字 s=%d 已接收 %d 字节数据， 终止！\n", para->sock, para->recv_total);
			return -1;
		}
		else if (retval == -1) {
			retval = WSAGetLastError();
			if (retval == WSAEWOULDBLOCK){
				continue;
			}
			printf("\nERROR NUM : %d\n", retval);
			printf("ERROR : 套接字 s=%d 已接收 %d 字节数据， 终止！\n", para->sock, para->recv_total);
			return -1;
		}

		//若是第一次接收，记录时间
		if (para->recv_total == 0){
			QueryPerformanceCounter(&para->first_recv);
			para->flag = 1;
		}

		para->recv_total += retval;

		free(recvbuf);
	}
	QueryPerformanceCounter(&para->last_recv);

	printf("套接字 s = %d 接收线程接收完毕！\n", para->sock);

	closesocket(para->sock);
	return 0;
}