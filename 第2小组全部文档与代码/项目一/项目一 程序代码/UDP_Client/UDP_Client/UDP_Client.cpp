// UDP_Client.cpp : Defines the entry point for the console application.
//


#include "stdio.h"
#include "winsock.h"
#pragma comment (lib,"wsock32.lib")

int main(int argc, char* argv[])
{
	SOCKET s;
	WSAData wsa;

	sockaddr_in local;
	sockaddr_in remote;

	int len;
	int retval;
	char snd_buf[] = "Hello!";
	char recv_buf[1024] = "";

	WSAStartup(0x101, &wsa);

	s = socket(AF_INET, SOCK_DGRAM, 0);
	if (s == SOCKET_ERROR) {
		retval = WSAGetLastError();
	}

	local.sin_family = AF_INET;
	local.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	local.sin_port = htons(0x1234);

	if (bind(s, (struct sockaddr*)&local, sizeof(local)) != 0){
		retval = WSAGetLastError();
	}

	remote.sin_family = AF_INET;
	remote.sin_addr.S_un.S_addr = htonl(INADDR_LOOPBACK);
	remote.sin_port = htons(0x2345);

	len = sizeof(remote);
	sendto(s, snd_buf, sizeof(snd_buf), 0, (struct sockaddr*)&remote, len);


	//接收
	recvfrom(s, recv_buf, 1024, 0, (sockaddr*)&remote, &len);

	//显示接收到的报文
	printf("recv_buf: %s", recv_buf);

	//计时
	LARGE_INTEGER large_integer;
	double dff;
	__int64 c1, c2;

	if (QueryPerformanceFrequency(&large_integer)){
		dff = (double)large_integer.QuadPart;

		QueryPerformanceCounter(&large_integer);
		c1 = large_integer.QuadPart;
		Sleep(800);
		QueryPerformanceCounter(&large_integer);
		c2 = large_integer.QuadPart;

		printf("本机最高精度计时频率%lf\n", dff);
		printf("第一次计时器值%I64d 第二次计时器值%I64d 计时器差值%I64d\n", c1, c2, c2 - c1);
		printf("计时%lf毫秒\n", (c2 - c1) * 1000 / dff);
		printf("\n");
	}

	closesocket(s);
	WSACleanup();
	printf("Hello World!");
	return 0;
}
