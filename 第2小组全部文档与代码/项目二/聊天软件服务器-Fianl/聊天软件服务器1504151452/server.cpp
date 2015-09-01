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
	//������Ϣģ�飬IDָʾ����/�������׽��֣�
	union{
		struct{ SOCKET ID; char command; char msg_buf[MSG_BUF_SIZE]; }msg;
		char char_buf[CHAR_BUF_SIZE];
	};
};
struct user_block{
	//�û���Ϣ�洢ģ��
	SOCKET ID;
	char nickname[NICKNAME_LEN];
	int msg_num;
	struct msg_block *p_recv_msg[RECV_MSG_LEN];
};
struct socket_list{
	//�׽��ֹ�����нṹ
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

	SOCKET ID;	//�û����
	char nickname_buf[NICKNAME_LEN];	//���ڽ����û��ǳƵĻ�����
	struct msg_block* p_msg;	//ָ�� ���ڽ����û����͵����ݵġ���Ϣ����ģ�顱�ṹ�� ��ָ��
	struct user_block user_list[MAX_SOCK_NUM];	//���ڴ洢�����û����û��б�
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

	//��select��صı������׽��ֶ��еȳ�ʼ��
	timeout.tv_sec = 1;		//���ó�ʱ
	timeout.tv_usec = 0;
	FD_ZERO(&readfds);
	FD_ZERO(&writefds);
	FD_ZERO(&exceptfds);
	init_list(&sock_list);
	sock_list.MainSock = s;

	//���׽�������Ϊ����
	arg = 0;
	ioctlsocket(sock_list.MainSock, FIONBIO, &arg);

	while (1){
		//����������״̬����
		make_fdlist(&sock_list, &readfds);
		make_fdlist(&sock_list, &writefds);
		make_fdlist(&sock_list, &exceptfds);

		retval = select(0, &readfds, &writefds, &exceptfds, &timeout);
		if (retval == SOCKET_ERROR){
			retval = WSAGetLastError();
			break;
		}
		if (FD_ISSET(sock_list.MainSock, &readfds)){
			//���׽����ϵ�read�¼������������ӽ���
			len = sizeof(remote_addr);
			sock = accept(sock_list.MainSock, (sockaddr*)&remote_addr, &len);	//sockaddr_in*
			if (sock == SOCKET_ERROR){
				continue;
			}
			printf("Accept a Connection !\n");

			//��sock����sock_list�������׽���sock���׽��ֶ���sock_list�е�λ��i
			i = insert_list(sock, &sock_list);

			//�����û��ǳ�
			retval = recv(sock, nickname_buf, NICKNAME_LEN, 0);
			if (retval == 0){
				closesocket(sock);
				retval = delete_list(sock, &sock_list);	//�����׽���sock���׽��ֶ���sock_list�е�λ��
				delete_user_list(&user_list[retval]);		//ɾ���û��б�user_list�еĶ�Ӧ�û�������
				broadcast_user_state(i, &sock_list, user_list, 'D');
				printf("A socket is closed !\n");
			} /*else if ( retval == -1 ) {
				//����״̬����ȥ���˲���
				retval = WSAGetLastError();
				if( retval != WSAEWOULDBLOCK ){
				closesocket( sock );
				retval = delete_list( sock, &sock_list );	//�����׽���sock���׽��ֶ���sock_list�е�λ��
				delete_user_list( &user_list[retval] );		//ɾ���û��б�user_list�еĶ�Ӧ�û�������
				printf("A socket is closed !\n");
				}
				}*/ else {
				//�����׽�������Ϊ������״̬
				arg = 1;
				ioctlsocket(sock, FIONBIO, &arg);

				//�����û����뵽�û��б�user_list��
				insert_user_list(&user_list[i], sock, nickname_buf);

				//���������û���Ϣ�����뵽���û��ķ��Ͷ���user_list[].p_recv_buf[]��
				getall_user_state(i, &sock_list, user_list);

				//���㲥�� �����û���������Ϣ�����뵽���������û��ķ��Ͷ���user_list[].p_recv_buf[]��
				broadcast_user_state(i, &sock_list, user_list, 'U');
			}
		}
		for (i = 0; i < MAX_SOCK_NUM; i++){
			//���׽����ϵĸ����¼�����
			//ע��˴�Ӧ���Ϊ���ϴ��׽��ֹ�����������ȡ��sock��ѭ��ģʽ
			if (sock_list.sock_array[i] == 0){
				continue;
			}
			sock = sock_list.sock_array[i];

			//����׽�����д�����У����Ͷ�Ӧ���ͻ������е�����
			if ((user_list[i].msg_num != 0) && FD_ISSET(sock, &writefds)){
				for (j = 0; j < RECV_MSG_LEN; j++){
					if (user_list[i].p_recv_msg[j] != NULL){
						retval = send(sock, user_list[i].p_recv_msg[j]->char_buf, CHAR_BUF_SIZE, 0);
						if (retval == CHAR_BUF_SIZE){
							printf("Msg from user(%d) to %s(%d) is sended successfully ! \n",
								user_list[i].p_recv_msg[j]->msg.ID, user_list[i].nickname, user_list[i].ID);
							//�ͷŶ�̬�ڴ�ռ�
							free(user_list[i].p_recv_msg[j]);
							//���һ�η��ͣ���Ϣ��������1
							user_list[i].msg_num -= 1;
							//���ָ��ֵ
							user_list[i].p_recv_msg[j] = NULL;
						}
						else {
							printf("ERROR : Send error ! \n");
						}
					}
				}
				//��д�����г�ȥsock�׽���
				//FD_CLR( sock, &writefds );
			}

			//����׽����ڶ������У����ȡ�׽��ֻ������е����ݵ����ͻ�����
			if (FD_ISSET(sock, &readfds)){
				p_msg = (struct msg_block*) malloc(sizeof(struct msg_block));
				retval = recv(sock, p_msg->char_buf, CHAR_BUF_SIZE, 0);
				if (retval == 0){
					closesocket(sock);
					broadcast_user_state(i, &sock_list, user_list, 'D');
					retval = delete_list(sock, &sock_list);	//�����׽���sock���׽��ֶ���sock_list�е�λ��
					delete_user_list(&user_list[retval]);		//ɾ���û��б�user_list�еĶ�Ӧ�û�������
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
					retval = delete_list(sock, &sock_list);	//�����׽���sock���׽��ֶ���sock_list�е�λ��
					delete_user_list(&user_list[retval]);		//ɾ���û��б�user_list�еĶ�Ӧ�û�������
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
								//��������Ϣ���е�ID��Ϊ������ID���ٷ�������߶����еȴ����͡�
								p_msg->msg.ID = sock;
								retval = insert_user_list_msg(&user_list[j], p_msg);
								if (retval != -1){
									//�����׽���sock���������
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
	//��ʼ���׽���
	int i;
	list->MainSock = 0;
	list->num = 0;
	for (i = 0; i < MAX_SOCK_NUM; i++){
		list->sock_array[i] = 0;
	}
}
int insert_list(SOCKET s, socket_list *list)
{
	//��s���뵽�׽��ֹ��������
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
	//���׽��ֹ��������ɾ��s
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
	//����������е��׽��������ӵ�ָ����״̬����fd_list�У�
	//ע������FD_SET()
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
			insert_user_list_msg(&user_list[n], p_msg);	///////////�漰����������룬�д��Ż�///////////
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