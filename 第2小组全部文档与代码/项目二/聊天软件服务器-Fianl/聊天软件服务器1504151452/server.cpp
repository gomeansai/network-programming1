// chatroom_server.cpp : 
//

#include "winsock.h"
#include "stdio.h"
#pragma comment (lib,"wsock32.lib")
#define SERVER_PORT 0x1234
#define MAX_SOCK_NUM 16
#define NICKNAME_LEN 20
#define RECV_MSG_LEN 16

#define MSG_BUF_SIZE 1024
#define CHAR_BUF_SIZE sizeof(SOCKET)/sizeof(char)+1+MSG_BUF_SIZE

struct msg_block{
	//数据信息模块，ID指示接收/发送者套接字，
	union{
		struct{ SOCKET ID; char command; char msg_buf[MSG_BUF_SIZE]; }msg;
		char char_buf[CHAR_BUF_SIZE];
	};
};
struct user_block{
	//用户信息存储模块
	SOCKET ID;
	char nickname[NICKNAME_LEN];
	int msg_num;
	struct msg_block *p_recv_msg[RECV_MSG_LEN];
};
struct socket_list{
	//套接字管理队列结构
	SOCKET MainSock;
	int num;
	SOCKET sock_array[MAX_SOCK_NUM];
};
void init_list(socket_list *list);
int insert_list(SOCKET s, socket_list *list);
int delete_list(SOCKET s, socket_list *list);
void make_fdlist(socket_list *list, fd_set *fd_list);

void insert_user_list(user_block *user, SOCKET sock, char *p_nickname);
int insert_user_list_msg(user_block *user, msg_block *msg);
void broadcast_user_state(int n, socket_list *sock_list, user_block *user_list, char command);
void broadcast_msg(int n, socket_list *sock_list, user_block *user_list, msg_block *origin_info);
void getall_user_state(int n, socket_list *sock_list, user_block *user_list);
void delete_user_list(user_block *user);

int main(int argc, char* argv[])
{
	SOCKET s, sock;
	struct sockaddr_in ser_addr, remote_addr;
	int len;
	WSAData wsa;
	int retval;
	struct socket_list sock_list;
	fd_set readfds, writefds, exceptfds;
	timeval timeout;
	int i, j;
	unsigned long arg;

	SOCKET ID;	//用户编号
	char nickname_buf[NICKNAME_LEN];	//用于接收用户昵称的缓存区
	struct msg_block* p_msg;	//指向 用于接收用户发送的数据的“信息数据模块”结构体 的指针
	struct user_block user_list[MAX_SOCK_NUM];	//用于存储上线用户的用户列表
	char *send_buf;

	WSAStartup(0x101, &wsa);
	s = socket(AF_INET, SOCK_STREAM, 0);
	if (s == SOCKET_ERROR){
		printf("SOCKET ERROR !\n");
		return 0;
	}
	ser_addr.sin_family = AF_INET;
	ser_addr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	ser_addr.sin_port = htons(SERVER_PORT);
	retval = bind(s, (sockaddr*)&ser_addr, sizeof(ser_addr));
	if (retval != 0){
		printf("BIND ERROR !\n");
		return 0;
	}
	retval = listen(s, MAX_SOCK_NUM);
	if (retval != 0){
		printf("LISTEN ERROR !\n");
		return 0;
	}

	//与select相关的变量、套接字队列等初始化
	timeout.tv_sec = 1;		//设置超时
	timeout.tv_usec = 0;
	FD_ZERO(&readfds);
	FD_ZERO(&writefds);
	FD_ZERO(&exceptfds);
	init_list(&sock_list);
	sock_list.MainSock = s;

	//主套接字设置为阻塞
	arg = 0;
	ioctlsocket(sock_list.MainSock, FIONBIO, &arg);

	while (1){
		//建立那三个状态队列
		make_fdlist(&sock_list, &readfds);
		make_fdlist(&sock_list, &writefds);
		make_fdlist(&sock_list, &exceptfds);

		retval = select(0, &readfds, &writefds, &exceptfds, &timeout);
		if (retval == SOCKET_ERROR){
			retval = WSAGetLastError();
			break;
		}
		if (FD_ISSET(sock_list.MainSock, &readfds)){
			//主套接字上的read事件，即有新连接建立
			len = sizeof(remote_addr);
			sock = accept(sock_list.MainSock, (sockaddr*)&remote_addr, &len);	//sockaddr_in*
			if (sock == SOCKET_ERROR){
				continue;
			}
			printf("Accept a Connection !\n");

			//将sock插入sock_list，返回套接字sock在套接字队列sock_list中的位置i
			i = insert_list(sock, &sock_list);

			//接收用户昵称
			retval = recv(sock, nickname_buf, NICKNAME_LEN, 0);
			if (retval == 0){
				closesocket(sock);
				retval = delete_list(sock, &sock_list);	//返回套接字sock在套接字队列sock_list中的位置
				delete_user_list(&user_list[retval]);		//删除用户列表user_list中的对应用户的数据
				broadcast_user_state(i, &sock_list, user_list, 'D');
				printf("A socket is closed !\n");
			} /*else if ( retval == -1 ) {
				//阻塞状态可以去掉此部分
				retval = WSAGetLastError();
				if( retval != WSAEWOULDBLOCK ){
				closesocket( sock );
				retval = delete_list( sock, &sock_list );	//返回套接字sock在套接字队列sock_list中的位置
				delete_user_list( &user_list[retval] );		//删除用户列表user_list中的对应用户的数据
				printf("A socket is closed !\n");
				}
				}*/ else {
				//将子套接字设置为非阻塞状态
				arg = 1;
				ioctlsocket(sock, FIONBIO, &arg);

				//将新用户加入到用户列表user_list中
				insert_user_list(&user_list[i], sock, nickname_buf);

				//将已在线用户信息，插入到新用户的发送队列user_list[].p_recv_buf[]中
				getall_user_state(i, &sock_list, user_list);

				//（广播） 将新用户的上线信息，插入到所有在线用户的发送队列user_list[].p_recv_buf[]中
				broadcast_user_state(i, &sock_list, user_list, 'U');
			}
		}
		for (i = 0; i < MAX_SOCK_NUM; i++){
			//从套接字上的各种事件处理
			//注意此处应设计为不断从套接字管理队列中逐个取出sock的循环模式
			if (sock_list.sock_array[i] == 0){
				continue;
			}
			sock = sock_list.sock_array[i];

			//如果套接字在写队列中，则发送对应发送缓存区中的数据
			if ((user_list[i].msg_num != 0) && FD_ISSET(sock, &writefds)){
				for (j = 0; j < RECV_MSG_LEN; j++){
					if (user_list[i].p_recv_msg[j] != NULL){
						retval = send(sock, user_list[i].p_recv_msg[j]->char_buf, CHAR_BUF_SIZE, 0);
						if (retval == CHAR_BUF_SIZE){
							printf("Msg from user(%d) to %s(%d) is sended successfully ! \n",
								user_list[i].p_recv_msg[j]->msg.ID, user_list[i].nickname, user_list[i].ID);
							//释放动态内存空间
							free(user_list[i].p_recv_msg[j]);
							//完成一次发送，信息数量减少1
							user_list[i].msg_num -= 1;
							//清空指针值
							user_list[i].p_recv_msg[j] = NULL;
						}
						else {
							printf("ERROR : Send error ! \n");
						}
					}
				}
				//从写队列中除去sock套接字
				//FD_CLR( sock, &writefds );
			}

			//如果套接字在读队列中，则读取套接字缓存区中的数据到发送缓存区
			if (FD_ISSET(sock, &readfds)){
				p_msg = (struct msg_block*) malloc(sizeof(struct msg_block));
				retval = recv(sock, p_msg->char_buf, CHAR_BUF_SIZE, 0);
				if (retval == 0){
					closesocket(sock);
					broadcast_user_state(i, &sock_list, user_list, 'D');
					retval = delete_list(sock, &sock_list);	//返回套接字sock在套接字队列sock_list中的位置
					delete_user_list(&user_list[retval]);		//删除用户列表user_list中的对应用户的数据
					printf("A socket is closed !\n");
					continue;
				}
				else if (retval == -1) {
					retval = WSAGetLastError();
					if (retval == WSAEWOULDBLOCK){
						continue;
					}
					closesocket(sock);
					broadcast_user_state(i, &sock_list, user_list, 'D');
					retval = delete_list(sock, &sock_list);	//返回套接字sock在套接字队列sock_list中的位置
					delete_user_list(&user_list[retval]);		//删除用户列表user_list中的对应用户的数据
					printf("A socket is closed !\n");
					continue;
				}
				else {
					if (p_msg->msg.command == 'U'){
						printf("User(%l) send error message ! \n", p_msg->msg.ID);
					}
					else if (p_msg->msg.command == 'D'){
						printf("User(%l) send error message ! \n", p_msg->msg.ID);
					}
					else if (p_msg->msg.command == 'M'){
						ID = p_msg->msg.ID;
						if (ID == 0){
							broadcast_msg(i, &sock_list, user_list, p_msg);
							continue;
						}
						for (j = 0; j < MAX_SOCK_NUM; j++){
							if (sock_list.sock_array[j] == ID){
								//将数据信息块中的ID改为发送者ID，再放入接收者队列中等待发送。
								p_msg->msg.ID = sock;
								retval = insert_user_list_msg(&user_list[j], p_msg);
								if (retval != -1){
									//将本套接字sock加入读队列
									//FD_SET( ID, &writefds );
									printf("Recv Msg from %s(%d) to %s(%d) : \'%s\' \n",
										user_list[i].nickname, sock, user_list[j].nickname, ID, p_msg->msg.msg_buf);
									p_msg = NULL;
								}
								else {
									printf("ERROR : User(%l)'s buffer is full ! \n", ID);
								}
								break;
							}
						}
						if (j == MAX_SOCK_NUM){
							printf("ERROR : Cannot find user(%l) ! \n", ID);
						}
					}
				}
			}

			if (FD_ISSET(sock, &exceptfds)){

			}
		}
		FD_ZERO(&readfds);
		FD_ZERO(&writefds);
		FD_ZERO(&exceptfds);
	}
	closesocket(sock_list.MainSock);
	WSACleanup();
	return 0;
}

void init_list(socket_list *list)
{
	//初始化套接字
	int i;
	list->MainSock = 0;
	list->num = 0;
	for (i = 0; i < MAX_SOCK_NUM; i++){
		list->sock_array[i] = 0;
	}
}
int insert_list(SOCKET s, socket_list *list)
{
	//将s插入到套接字管理队列中
	int i = MAX_SOCK_NUM;
	if (list->num == MAX_SOCK_NUM){
		printf("INSERT ERROR : the socket_list is full !\n");
		return -1;
	}
	else {
		for (i = 0; i < MAX_SOCK_NUM; i++){
			if (list->sock_array[i] == 0){
				list->sock_array[i] = s;
				list->num += 1;

				printf("INSERT SUCCEED !\n");
				return i;
			}
		}
		return -1;
	}
}
int delete_list(SOCKET s, socket_list *list)
{
	//从套接字管理队列中删除s
	int i;
	for (i = 0; i < MAX_SOCK_NUM; i++){
		if (list->sock_array[i] == s){
			list->sock_array[i] = 0;
			list->num -= 1;

			printf("DELETE SUCCEED !\n");
			return i;
		}
	}
	if (i == MAX_SOCK_NUM){
		printf("DELETE ERROR : cannot find the socket from sock_list where s = %d .\n", s);
		return -1;
	}

}
void make_fdlist(socket_list *list, fd_set *fd_list)
{
	//将管理队列中的套接字逐个添加到指定的状态队列fd_list中，
	//注意利用FD_SET()
	int i;
	FD_SET(list->MainSock, fd_list);
	for (i = 0; i < MAX_SOCK_NUM; i++){
		if (list->sock_array[i] > 0){
			FD_SET(list->sock_array[i], fd_list);
		}
	}

	printf("MAKE_FDLIST SUCCEED !\n");
}
void insert_user_list(user_block *user, SOCKET sock, char *p_nickname)
{
	int i;

	user->ID = sock;
	strcpy(user->nickname, p_nickname);
	user->msg_num = 0;
	for (i = 0; i < RECV_MSG_LEN; i++){
		user->p_recv_msg[i] = NULL;
	}
}

int insert_user_list_msg(user_block* user, msg_block *msg)
{
	int i;
	i = 0;
	if (user->msg_num == RECV_MSG_LEN){
		return -1;
	}
	while (user->p_recv_msg[i] != NULL){
		i++;
	}
	user->p_recv_msg[i] = msg;
	user->msg_num += 1;
	return i;

}

void broadcast_user_state(int n, socket_list *sock_list, user_block *user_list, char command)
{
	int i;
	msg_block *p_msg;
	SOCKET sock;
	char nickname[NICKNAME_LEN];
	sock = sock_list->sock_array[n];
	strcpy(nickname, user_list[n].nickname);

	for (i = 0; i < MAX_SOCK_NUM; i++){
		if (i == n)
			continue;
		if (sock_list->sock_array[i] > 0){
			p_msg = (struct msg_block*) malloc(sizeof(struct msg_block));
			p_msg->msg.ID = sock;
			p_msg->msg.command = command;
			strcpy(p_msg->msg.msg_buf, nickname);
			insert_user_list_msg(&user_list[i], p_msg);
			p_msg = NULL;
		}
	}
}

void broadcast_msg(int n, socket_list *sock_list, user_block *user_list, msg_block *origin_info)
{
	int i;
	msg_block *p_msg;
	SOCKET sock;
	char nickname[NICKNAME_LEN];


	sock = sock_list->sock_array[n];
	strcpy(nickname, user_list[n].nickname);

	for (i = 0; i < MAX_SOCK_NUM; i++){
		if (i == n)
			continue;
		if (sock_list->sock_array[i] > 0){
			p_msg = (struct msg_block*) malloc(sizeof(struct msg_block));
			p_msg->msg.ID = sock;
			p_msg->msg.command = 'M';
			strcpy(p_msg->msg.msg_buf, origin_info->msg.msg_buf);
			insert_user_list_msg(&user_list[i], p_msg);
			p_msg = NULL;
		}
	}
}

void getall_user_state(int n, socket_list *sock_list, user_block *user_list)
{
	int i;
	msg_block *p_msg;
	for (i = 0; i < MAX_SOCK_NUM; i++){
		if (i == n)
			continue;
		if (sock_list->sock_array[i] > 0){
			p_msg = (struct msg_block*) malloc(sizeof(struct msg_block));
			p_msg->msg.ID = user_list[i].ID;
			p_msg->msg.command = 'U';
			strcpy(p_msg->msg.msg_buf, user_list[i].nickname);
			insert_user_list_msg(&user_list[n], p_msg);	///////////涉及多次连续插入，有待优化///////////
			p_msg = NULL;
		}
	}
}

void delete_user_list(user_block *user)
{
	int i;
	user->ID = 0;
	user->msg_num = 0;
	for (i = 0; i < RECV_MSG_LEN; i++){
		if (user->p_recv_msg[i] != NULL){
			free(user->p_recv_msg[i]);
			user->p_recv_msg[i] = NULL;
		}
	}
}