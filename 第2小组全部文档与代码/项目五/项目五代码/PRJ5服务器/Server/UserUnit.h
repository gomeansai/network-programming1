#pragma once
#include "Header.h"

#define USER_MAX_NUM 64
#define SOCK_LIST_SIZE 8

class UserUnit
{
public:
	//short user_num;
	ULONG user_id;
	CString user_name;

	SOCKET ctl_sock;
	UCHAR ctl_sock_status;
	ULONG user_ip;
	USHORT user_port;

	unsigned long inquiry_num;
	//bool permission[USER_MAX_NUM][USER_MAX_NUM];

	SOCKET fl_sock;
	UCHAR fls_status;
	CFileFind fl_finder;

	short sock_num;
	SOCKET sock_list[SOCK_LIST_SIZE];	//用户内部套接字队列，最好改用list
	UCHAR sock_status[SOCK_LIST_SIZE];
	FILE *file_pointer[SOCK_LIST_SIZE];
	int last_len[SOCK_LIST_SIZE];

	CString filename[SOCK_LIST_SIZE];
	UCHAR translate_id[SOCK_LIST_SIZE];
	int translate_i[SOCK_LIST_SIZE];
	int translate_j[SOCK_LIST_SIZE];

public:
	UserUnit(unsigned long id, SOCKET s, ULONG ip, USHORT port);
	~UserUnit();
	
	short InsertSocketList(SOCKET s, char status, FILE *fp);
	void DeleteSockList(int j);
	unsigned long f_producenum(struct InquiryPDU *p_pdu, long secret, unsigned char *bufflen);
	short f_FindSock(SOCKET s, int i);
	void UserUnit::DeleteUserUnit();
	void UserUnit::DeleteFilelistSock();

protected:
	
	
};