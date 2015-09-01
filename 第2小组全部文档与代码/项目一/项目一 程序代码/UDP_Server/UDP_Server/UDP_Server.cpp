// ConsoleApplication1.cpp : Defines the entry point for the console application.
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

	char buf[128] = "";  

	WSAStartup(0x101, &wsa);

	s = socket(AF_INET, SOCK_DGRAM, 0);
	if (s == SOCKET_ERROR) {
		retval = WSAGetLastError();
	}

	local.sin_family = AF_INET;
	local.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	local.sin_port = htons(0x1234);

	retval = bind(s, (struct sockaddr*)&local, sizeof(local));

	printf("retval:%d", retval);

	if (retval != 0){
		retval = WSAGetLastError();
	}

	memset(&remote, 0, sizeof(remote));
	while (1){
		//Ω” ’
		len = sizeof(remote);
		recvfrom(s, buf, 128, 0, (struct sockaddr*)&remote, &len);

		//len = sizeof(remote);
		sendto(s, buf, sizeof(buf), 0, (struct sockaddr*)&remote, len);

		printf("buf: %s", buf);
	}
	closesocket(s);
	WSACleanup();
	printf("Server Exit!\n");
	return 0;
}
