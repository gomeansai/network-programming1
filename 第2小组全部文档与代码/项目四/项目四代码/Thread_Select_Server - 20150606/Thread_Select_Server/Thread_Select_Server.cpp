// coclient.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "windows.h"
#include "winsock.h"
#include "stdio.h"
#include "time.h"
#pragma comment (lib,"wsock32.lib")
#pragma warning(disable: 4996)

#define MAX_THRD_SIZE 10000			// 10K
#define MAX_SOCKETS	1024

//#define MAX_BUF_SIZE 1000000		// 1M
//#define MIN_BUF_SIZE 1000			// 1K

//#define SLEEP_TIME 2000				// 2s
//#define DOUBLE_SIZE 20				//输出时，动态申请一个字符串

//用于控制所有线程
DWORD dwThreadID[MAX_THRD_SIZE];
HANDLE hThrd[MAX_THRD_SIZE];
DWORD dwThrdParam;

int BufLen;
unsigned char  NagleOn;
int TCPRecvSize;
int TCPSendSize;


void my_scanf(char * buf, int buf_size)
{
	scanf("%s", buf);
}

struct SerInPara{
	SOCKET MainSock;
	long num_sock;		//每个线程中套接字最大数量
	long num_thread;

};

struct socket_list{
	SOCKET MainSock;
	int num;
	SOCKET sock_array[MAX_SOCKETS];
};

void init_list(socket_list *list)
{
	int i;
	list->MainSock = 0;
	list->num = 0;
	for (i = 0; i < MAX_SOCKETS; i++){
		list->sock_array[i] = 0;
	}
}

void insert_list(SOCKET s, socket_list *list)
{
	int i;
	for (i = 0; i < MAX_SOCKETS; i++){
		if (list->sock_array[i] == 0){
			list->sock_array[i] = s;
			list->num += 1;
			break;
		}
	}
}

void delete_list(SOCKET s, socket_list *list)
{
	int i;
	for (i = 0; i < MAX_SOCKETS; i++){
		if (list->sock_array[i] == s){
			list->sock_array[i] = 0;
			list->num -= 1;
			break;
		}
	}
}

void make_fdlist(socket_list *list, fd_set *fd_list)
{
	int i;
	FD_ZERO(fd_list);
	if (list->MainSock){			//如果主套接字还存在！！！！！！！！！！！！！！！！！！！！！！！！！！！！！
		FD_SET(list->MainSock, fd_list);
	}
	for (i = 0; i < MAX_SOCKETS; i++){
		if (list->sock_array[i] > 0){
			FD_SET(list->sock_array[i], fd_list);
		}
	}
}

DWORD WINAPI server_proc(LPVOID lpParam);
DWORD WINAPI client_recv_proc(LPVOID lpParam);

int main(int argc, char* argv[])
{
	struct sockaddr_in ser_addr;
	struct SerInPara *p_para;

	unsigned long arg;
	int retval;
	char *scanbuf;
	
	WSADATA wsaData;
	//WSAData wsa;
	WSAStartup(MAKEWORD(2, 2), &wsaData);
	//WSAStartup(0x101, &wsa);

	printf("多线程下的Select模式服务器开启！\n\n");

	//输入设置基本参数

	/*
	gethostname(szPath, sizeof(szPath));
	printf("%s", szPath);
	pHost = gethostbyname(szPath);
	server.sin_addr.S_un.S_addr = (struct sockaddr_in *)*pHost->h_addr_list;
	system("pause");
	*/

	p_para = (struct SerInPara *)malloc(sizeof(struct SerInPara));
	p_para->MainSock = socket(AF_INET, SOCK_STREAM, 0);

	ser_addr.sin_family = AF_INET;
	ser_addr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	ser_addr.sin_port = htons(12345);

	scanbuf = (char *)malloc(80);
	printf("请输入每个线程中最多的套接字数量(不超过%d): ", FD_SETSIZE);
	my_scanf(scanbuf, sizeof(scanbuf));
	p_para->num_sock = atol(scanbuf);
	free(scanbuf);

	printf("请输入单片接收缓存大小(0表示自适应大小)：\n");
	scanf("%d", &BufLen);
	printf("是否开启Nagle算法？（0-关闭，1-开启，默认开启）\n");
	scanf("%d", &NagleOn);
	printf("每个Sockets的TCP接收缓冲区大小（KB）？（默认8KB，最大8MB，0表示保持默认）\n");
	scanf("%d", &TCPRecvSize);
	TCPRecvSize *= 1024;	//转为Byte
	printf("每个Sockets的TCP发送缓冲区大小（KB）？（默认8KB，最大8MB，0表示保持默认）\n");
	scanf("%d", &TCPSendSize);
	TCPSendSize *= 1024;

	p_para->num_thread = 0;		//从第0个子线程开始

	retval = bind(p_para->MainSock, (sockaddr*)&ser_addr, sizeof(ser_addr));
	retval = listen(p_para->MainSock, p_para->num_sock);	//同时监听套接字数
	
	arg = 1;
	ioctlsocket(p_para->MainSock, FIONBIO, &arg);
	
	printf("\n");

	dwThrdParam = (DWORD)p_para;// 坑货指针！！！！！！！！
	hThrd[ p_para->num_thread ] = CreateThread(
		NULL,
		0,
		server_proc,
		&dwThrdParam,
		0,
		&dwThreadID[ p_para->num_thread ]);

	printf("已建立线程 ： %4d", p_para->num_thread);
	while (1){
		printf("\b\b\b\b");
		printf("%4d", p_para->num_thread);
	}

	//closesocket(s);
	WSACleanup();
	return 0;
}

DWORD WINAPI server_proc(LPVOID lpParam)
{
	struct SerInPara* p_para;
	
	SOCKET sock;
	struct sockaddr_in remote_addr;
	int len, total_len, total_sendlen;
	char *buf;
	int retval;
	struct socket_list sock_list;
	fd_set readfds, writefds, exceptfds;
	int i;
	int BufFlag;
	unsigned int pending_count[63];
	long sum_pending_count = 0;
	int thread_flag = 0;
	unsigned int offset;
	int errval;

	p_para = *(struct SerInPara**)lpParam;	// 指针坑坑坑！！！！！
	
	unsigned int closecount = 0;

	//初始化记录Sockets不能发出发出次数的数组
	for (int i = 0; i < 63; i++)
	{
		pending_count[i] = 0;
	}
	
	printf("线程 %4d 开启！\n", p_para->num_thread);
	Sleep(10000);

	if (BufLen > 0){
		buf = (char *)malloc(BufLen);
		if (buf == NULL){
			printf("内存不够\n");
			return 0;
		}
		BufFlag = 1;
	}
	else{
		BufFlag = 0;
		buf = NULL;
	}

	init_list(&sock_list);
	sock_list.MainSock = p_para->MainSock;

	total_len = 0;
	total_sendlen = 0;

	while (1){
		FD_ZERO(&readfds);
		FD_ZERO(&writefds);
		FD_ZERO(&exceptfds);
		make_fdlist(&sock_list, &readfds);
		//make_fdlist(&sock_list,&writefds);
		//make_fdlist(&sock_list,&exceptfds);
		retval = select(0, &readfds, &writefds, &exceptfds, NULL);
		if (retval == SOCKET_ERROR){
			retval = WSAGetLastError();
			printf("select error %d\n", retval);
			break;
		}
		if (FD_ISSET(sock_list.MainSock, &readfds)){
			len = sizeof(remote_addr);
			sock = accept(sock_list.MainSock, (sockaddr*)&remote_addr, &len);
			if (sock == SOCKET_ERROR)
				continue;

			//以下为关闭Nagle算法代码
			if (NagleOn == 0){
				int on = 1;
				int len = sizeof(on);
				if (setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (const char *)&on, len) < 0)
				{
					printf("Nagle算法关闭失败\n");
					getchar();
					return -1;
				}
			}

			//以下为修改TCP发送接收缓存区为8MB的代码
			if (TCPRecvSize != 0){
				len = sizeof(TCPRecvSize);
				if (setsockopt(sock, SOL_SOCKET, SO_RCVBUF, (char *)&TCPRecvSize, len) < 0)
				{
					printf("修改TCP接收缓冲区失败！\n");
					getchar();
					return -1;
				}
			}


			if (TCPSendSize != 0){
				len = sizeof(TCPSendSize);
				if (setsockopt(sock, SOL_SOCKET, SO_SNDBUF, (char *)&TCPSendSize, len) < 0)
				{
					printf("修改TCP发送缓冲区失败！\n");
					getchar();
					return -1;
				}
			}

			printf("accept a connection\n");
			insert_list(sock, &sock_list);
		}
		for (i = 0; i < MAX_SOCKETS; i++){
			if (sock_list.sock_array[i] == 0)
				continue;
			sock = sock_list.sock_array[i];
			if (FD_ISSET(sock, &readfds)){
				if (BufFlag == 0){
					//自适应大小，根据待接收大小决定缓冲大小
					ioctlsocket(sock, FIONREAD, (unsigned long *)&len);
					if (len > 0){
						if (buf != NULL){
							free(buf);
							buf = NULL;
						}
						buf = (char*)malloc(len);
						if (buf == NULL){
							printf("没有足够内存\n");
							continue;
						}
						len = recv(sock, buf, len, 0);
					}
				}
				else{
					//使用固定大小缓存
					len = recv(sock, buf, BufLen, 0);
				}
				if (len == 0){
					//必须将套接字从接收队列中取下来！否则不关闭就会不断有read事件。
					closesocket(sock);
					closecount++;
					printf("close a socket\n");
					delete_list(sock, &sock_list);
					printf("Sockets#%d接收异常，返回值0，发生过 %d 次无法及时发出的现象\n", sock, pending_count[sock]);
					continue;
				}
				else if (len == -1){
					errval = WSAGetLastError();
					if (errval == WSAEWOULDBLOCK){
						continue;
					}
					closesocket(sock);
					closecount++;
					printf("close a socket\n");
					delete_list(sock, &sock_list);
					printf("Sockets#%d接收异常，返回值-1，发生过 %d 次无法及时发出的现象\n", sock, pending_count[sock]);
					continue;
				}
				//buf[len] = 0;
				total_len += len;
				printf("-->%d 字节\n", total_len);
				offset = 0;
				do{
					retval = send(sock, buf + offset, len + offset, 0);
					if (retval == 0)
						break;
					else if (retval < 0)
					{
						if (WSAGetLastError() == WSAEWOULDBLOCK)
						{
							pending_count[sock]++;
							sum_pending_count++;
							printf("send pending\n");
							continue;
						}
						else
							break;
					}
					offset += retval;
				} while (offset != len);
				if (retval == 0){
					closesocket(sock);
					closecount++;
					printf("close a socket\n");
					delete_list(sock, &sock_list);
					printf("Sockets#%d发送异常，返回值0，发生过 %d 次无法及时发出的现象\n", sock, pending_count[sock]);
					continue;
				}
				else if (retval == -1){
					closesocket(sock);
					closecount++;
					printf("close a socket\n");
					delete_list(sock, &sock_list);
					printf("Sockets#%d发送异常，返回值-1，发生过 %d 次无法及时发出的现象\n", sock, pending_count[sock]);
					continue;
				}
				total_sendlen += retval;
				printf("<--%d 字节\n", total_sendlen);

				if (buf != NULL && BufFlag == 0){
					free(buf);
					buf = NULL;
				}
			}
			//if(FD_ISSET(sock,&writefds)){
			//}
			if (FD_ISSET(sock, &exceptfds)){
				closesocket(sock);
				closecount++;
				printf("close a socket\n");
				delete_list(sock, &sock_list);
				continue;
			}

		}
		printf("目前无法及时发出发送总次数为：%ld\n", sum_pending_count);
		printf("已关闭连接数为:%d\n", closecount);
	}

	if (buf != NULL)
		free(buf);
	//closesocket(sock_list.MainSock);
	return 0;
}