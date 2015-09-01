// server.cpp : 
//

#include "stdafx.h"
#include "stdio.h"
#define MAX_THRD_SIZE 10000			// 10K
#define FD_SETSIZE MAX_THRD_SIZE
#include "winsock.h"
#pragma comment (lib,"wsock32.lib")
#define MAX_SOCKETS	1024
#pragma warning(disable:4996)

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
	FD_SET(list->MainSock, fd_list);
	for (i = 0; i < MAX_SOCKETS; i++){
		if (list->sock_array[i] > 0){
			FD_SET(list->sock_array[i], fd_list);
		}
	}
}

int main(int argc, char* argv[])
{
	SOCKET s, sock;
	struct sockaddr_in ser_addr, remote_addr;
	int len, total_len, total_sendlen;
	char *buf;
	WSAData wsa;
	int retval;
	struct socket_list sock_list;
	fd_set readfds, writefds, exceptfds;
	int i;
	unsigned long arg;
	unsigned int pending_count[65536];
	long sum_pending_count = 0;
	int BufLen;
	int BufFlag;
	unsigned char  NagleOn;
	int TCPRecvSize;
	int TCPSendSize;
	unsigned int offset;
	int errval;
	unsigned int closecount=0;

	//��ʼ����¼Sockets���ܷ�����������������
	for (int i = 0; i < 65536; i++)
	{
		pending_count[i] = 0;
	}

	WSAStartup(0x101, &wsa);

	printf("�����뵥Ƭ���ջ����С(0��ʾ����Ӧ��С)��\n");
	scanf("%d", &BufLen);
	printf("�Ƿ���Nagle�㷨����0-�رգ�1-������Ĭ�Ͽ�����\n");
	scanf("%d", &NagleOn);
	printf("ÿ��Sockets��TCP���ջ�������С��KB������Ĭ��8KB�����8MB��0��ʾ����Ĭ�ϣ�\n");
	scanf("%d", &TCPRecvSize);
	TCPRecvSize *= 1024;	//תΪByte
	printf("ÿ��Sockets��TCP���ͻ�������С��KB������Ĭ��8KB�����8MB��0��ʾ����Ĭ�ϣ�\n");
	scanf("%d", &TCPSendSize);
	TCPSendSize *= 1024;

	if (BufLen > 0){
		buf = (char *)malloc(BufLen);
		if (buf == NULL){
			printf("�ڴ治��\n");
			return 0;
		}
		BufFlag = 1;
	}
	else{
		BufFlag = 0;
		buf = NULL;
	}

	s = socket(AF_INET, SOCK_STREAM, 0);
	ser_addr.sin_family = AF_INET;
	ser_addr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	ser_addr.sin_port = htons(12345);
	bind(s, (sockaddr*)&ser_addr, sizeof(ser_addr));

	listen(s, 5);	//ͬʱ�����׽�����

	init_list(&sock_list);
	sock_list.MainSock = s;

	total_len = 0;
	total_sendlen = 0;

	arg = 1;
	ioctlsocket(sock_list.MainSock, FIONBIO, &arg);

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

			//����Ϊ�ر�Nagle�㷨����
			if (NagleOn == 0){
				int on = 1;
				int len = sizeof(on);
				if (setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (const char *)&on, len) < 0)
				{
					printf("Nagle�㷨�ر�ʧ��\n");
					getchar();
					return -1;
				}
			}

			//����Ϊ�޸�TCP���ͽ��ջ�����Ϊ8MB�Ĵ���
			if (TCPRecvSize != 0){
				len = sizeof(TCPRecvSize);
				if (setsockopt(sock, SOL_SOCKET, SO_RCVBUF, (char *)&TCPRecvSize, len) < 0)
				{
					printf("�޸�TCP���ջ�����ʧ�ܣ�\n");
					getchar();
					return -1;
				}
			}


			if (TCPSendSize != 0){
				len = sizeof(TCPSendSize);
				if (setsockopt(sock, SOL_SOCKET, SO_SNDBUF, (char *)&TCPSendSize, len) < 0)
				{
					printf("�޸�TCP���ͻ�����ʧ�ܣ�\n");
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
					//����Ӧ��С�����ݴ����մ�С���������С
					ioctlsocket(sock, FIONREAD, (unsigned long *)&len);
					if (len > 0){
						if (buf != NULL){
							free(buf);
							buf = NULL;
						}
						buf = (char*)malloc(len);
						if (buf == NULL){
							printf("û���㹻�ڴ�\n");
							continue;
						}
						len = recv(sock, buf, len, 0);
					}
				}
				else{
					//ʹ�ù̶���С����
					len = recv(sock, buf, BufLen, 0);
				}
				if (len == 0){
					//���뽫�׽��ִӽ��ն�����ȡ���������򲻹رվͻ᲻����read�¼���
					closesocket(sock);
					closecount++;
					printf("close a socket\n");
					delete_list(sock, &sock_list);
					printf("Sockets#%d�����쳣������ֵ0�������� %d ���޷���ʱ����������\n", sock, pending_count[sock]);
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
					printf("Sockets#%d�����쳣������ֵ-1�������� %d ���޷���ʱ����������\n", sock, pending_count[sock]);
					continue;
				}
				//buf[len] = 0;
				total_len += len;
				printf("-->%d �ֽ�\n", total_len);
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
					printf("Sockets#%d�����쳣������ֵ0�������� %d ���޷���ʱ����������\n", sock, pending_count[sock]);
					continue;
				}
				else if (retval == -1){
					closesocket(sock);
					closecount++;
					printf("close a socket\n");
					delete_list(sock, &sock_list);
					printf("Sockets#%d�����쳣������ֵ-1�������� %d ���޷���ʱ����������\n", sock, pending_count[sock]);
					continue;
				}
				total_sendlen += retval;
				printf("<--%d �ֽ�\n", total_sendlen);

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
		printf("Ŀǰ�޷���ʱ���������ܴ���Ϊ��%ld\n", sum_pending_count);
		printf("�ѹر�������Ϊ:%d\n", closecount);
	}
	if (buf != NULL)
		free(buf);
	closesocket(sock_list.MainSock);
	WSACleanup();
	return 0;
}

