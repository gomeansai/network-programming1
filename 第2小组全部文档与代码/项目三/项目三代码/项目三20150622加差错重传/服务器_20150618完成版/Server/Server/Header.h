#pragma once

#include "stdafx.h"

/*
#include "CtlPDU.h"
#include "FilePDU.h"
#include "Inquiry.h"
*/

#define SERVER_PORT 12345
#define CLIENT_PORT 54321

//注册套接字异步响应事件
#define UM_SOCK WM_USER + 100
#define UM_FL_SOCK UM_SOCK + 1
#define UM_SEND_FILE_SOCK UM_SOCK + 2
#define UM_RECV_FILE_SOCK UM_SOCK + 3

//接收缓存区大小---> 比 FIlePDU 大 1
#define BUF_SIZE	FILE_BUF_SIZE + 1 + 1 + 1 + 1


//PDU -> type 类型值
#define INQUIRY_PDU_TYPE	1
#define CTL_PDU_TYPE		2
#define CHAT_PDU_TYPE		3
#define FILE_PDU_TYPE		4

#define NUMRANGE			16		//质询
#define INQUIRY_BUF_SIZE	16*4	//质询数所可能占用的最大字节数
#define CTL_BUF_SIZE		255     //一个字节所能存储的最大数值
#define CHAT_BUF_SIZE		255
#define FILE_BUF_SIZE		255     //一个字节所能存储的最大数值

//#define INQUIRY_BUF_SIZE 16*4	//质询数所可能占用的最大字节数
struct InquiryPDU {
	char type;		//INQUIRY_PDU_TYPE = 1
	char num;		//存储质询数个数（范围1-16）
	char each_len[4];		//表达每个质询数的长度。每个表达占2位，值1、2、3、0分别表示质询数长度为1、2、3、4。4个字节32bit，最多能表达16个数。
	char inquiry_buf[INQUIRY_BUF_SIZE];		//每个质询数，高位存储在前，低位存储在后，符合网络字节序
};

//#define CTL_BUF_SIZE 256     //一个字节所能存储的最大数值
struct CtlPDU {
	UCHAR type;      //CTL_PDU_TYPE = 2
	UCHAR cmd;		//20:ACCOUNT;	21:REQ_FILELIST;	22:REQ_DOWNFILE;	23:REQ_UPFILE
	UCHAR len;
	char buf[CTL_BUF_SIZE];
};
/*
struct ChatPDU{
UCHAR type;		//CHAT_PDU_TYPE = 3
UCHAR cmd;		//上线下线普通群发
USHORT id;
char buf[CHAT_BUF_SIZE];
};
*/
//#define FILE_BUF_SIZE 256     //一个字节所能存储的最大数值
struct FilePDU {
	UCHAR type;     //FILE_PDU_TYPE = 4
	UCHAR No;        //0 or 1
	UCHAR len;       //len == 0: 传输结束
	char buf[FILE_BUF_SIZE];
};




//CtlPDU -> cmd 命令值
#define REQ_ACCOUNT		20	//Server ask Client, Ask
#define REQ_FILELIST	21	//Client to Server, Requir Filelist
#define REQ_DOWNFILE	22	//Client to Server, Download File
#define REQ_UPFILE		23	//Client to Server, Upload File
#define R_ACCOUNT		20	//Server ask Client, Ask
#define R_FILELIST		21	//Client to Server, Requir Filelist
#define R_DOWNFILE		22	//Client to Server, Download File
#define R_UPFILE		23	//Client to Server, Upload File

#define RE_ACCESS		25
#define RE_TIMEOUT		26
#define RE_ERR_ACCOUNT	27
#define RE_ERR_INQUIRY	28
#define RE_ERR_FILENAME 29
#define RE_ERR_DOWNFILE 30

//FilePDU-->No
#define GET_FILELIST_START 2
#define DOWN_FILE_START 3
#define UP_FILE_START 4


//状态机
#define S_CLOSE		200
#define S_LISTENIND	201
#define S_AUTHSTART	202
#define S_AUTHING		203
#define S_AUTHED		204
#define S_CLOSING		209

#define S_SENDFL_START	210
#define S_SENDFL_0		211
#define S_SENDFL_1		212

#define S_SENDFILE_START	220
#define S_SENDFILE_0		221
#define S_SENDFILE_1		222

#define S_RECVFILE_START	230
#define S_RECVFILE_0		231
#define S_RECVFILE_1		232


//长度宏定义
#define SOCK_NUM 256
#define MSG_LIST_SIZE 5
#define NICKNAME_SIZE 20

//#define RECV_BUF_SIZE max( 1+1+4+INQUIRY_BUF_SIZE, max( 1+1+1+CTL_BUF_SIZE, 1+1+1+FILE_BUF_SIZE ) )

#define BUFLEN 1+4+4*16
#define	MAXNUM	0x100000000	//2^32

#define MAX_DIR_SIZE 1000
//#define MAX_SNUM_SIZE 10



#define TIMER1  1 
#define TIMER2  2
#define TIMER3	3
#define TIMER4	4
#define TIMER5	5 
#define TIMER6	6
#define TIMER7	7 