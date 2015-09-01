#pragma once

#include <winsock2.h>
#include <ws2tcpip.h>
#include <tchar.h>
#pragma comment(lib,"ws2_32.lib")
#define WM_SOCKET			WM_USER + 101
#define WM_DATA_SOCKET		WM_SOCKET + 1
#define WM_FL_SOCK			WM_SOCKET + 2
#define WM_SEND_FILE_SOCK	WM_SOCKET + 3
#define WM_RECV_FILE_SOCK	WM_SOCKET + 4


//状态机
#define S_CLOSE			200
#define S_LISTENIND		201
#define S_AUTHSTART		202
#define S_AUTHING		203
#define S_AUTHED		204
#define S_CLOSING		209

#define S_RECVFL_START	210
#define S_RECVFL_0		211
#define S_RECVFL_1		212

#define S_SENDFILE_START	220
#define S_SENDFILE_0		221
#define S_SENDFILE_1		222

#define S_RECVFILE_START	230
#define S_RECVFILE_0		231
#define S_RECVFILE_1		232

///////////////////////////////////////////////////////////


#define u8 unsigned char
#define u16 unsigned short
#define u32 unsigned long

#define SERV_PORT		12345
#define LOCAL_PORT		54321
#define LOCAL_CTL_PORT	12345
#define SERV_IP			INADDR_LOOPBACK


#define BUF_SIZE	FILE_BUF_SIZE + 1 + 1 + 1 + 1 + 1//临时定义，，后期需要更准确的数值


#define INQUIRY_PDU_TYPE 1
#define INQUIRY_BUF_SIZE 16*4      //质询数所可能占用的最大字节数
typedef struct InquiryPDUS {
	char type;     //INQUIRING_PDU_TYPE = 1
	char num;    //存储质询数个数（范围1-16）
	char each_len[4];    //表达每个质询数的长度。每个表达占2位，值1、2、3、0分别表示质询数长度为1、2、3、4。4个字节32bit，最多能表达16个数。
	char inquiry_buf[INQUIRY_BUF_SIZE];     //每个质询数，高位存储在前，低位存储在后，符合网络字节序
}InquiryPDU;


#define CTL_PDU_TYPE	2
#define CTL_BUF_SIZE	255    	//一个报文载荷所能存储的最大字节数
//以下为命令位cmd取值，改为2开头的两位数，与CTL_PDU_TYPE看齐
#define REQ_ACCOUNT 	20     	//服务器向客户机请求用户帐号
#define REQ_FILELIST 	21   	//客户机向服务器请求获取文件列表
#define REQ_DOWNFILE 	22   	//客户机向服务器请求文件下载
#define REQ_UPFILE 		23    	//客户机向服务器请求文件上传
#define REQ_TRANSLATE	24		//文件中转确认信息
#define RE_ACCESS  		25		//服务器告知客户机登陆成功
#define RE_TIMEOUT		26		//服务器告知客户机超时
#define RE_ERR_ACCOUNT	27		//服务器告知客户机用户名错误
#define RE_ERR_INQUIRY	28		//服务器告知客户机质询结果不通过
#define RE_ERR_FILENAME 29   	//服务器告知客户机文件名错误

typedef struct CtlPDUS{
	unsigned char type;      		//PDU类型，值为CTL_PDU_TYPE = 2
	unsigned char cmd;				//控制命令，可选参数见于上文宏定义
	unsigned char len;				//报文中载荷的长度，0~255
	unsigned char id;				//在文件上传时使用，标识文件传输对象为服务器或者其他用户
	char buf[CTL_BUF_SIZE];			//载荷，根据不同的命令cmd值
	//可填用户名、上下载的文件名等
} CtlPDU;


#define	 FILE_PDU_TYPE	 	4	//文件传输类型
#define	 FILE_BUF_SIZE 		255	//一个报文载荷所能存储的最大字节数
//数据连接建立后的第一帧皆由服务器发送至客户机
//通过不同No值，告知该连接的功能
#define GET_FILELIST_START 	2
#define DOWN_FILE_START 	3
#define UP_FILE_START 		4

typedef struct FilePDUS{
	unsigned char type;		//PDU类型，值为FILE_PDU_TYPE = 4
	unsigned char No;		//传输时，PDU编号，可选参数0 或1，第一份文件/文件列表PDU，No值可选2、3、4
	unsigned char len;		//默认为报文长度总长度。当值为0时，表示传输结束
	char buf[FILE_BUF_SIZE];	//载荷，具体包括文件列表各文件名或文件数据流
} FilePDU;


#define	CHAT_PDU_TYPE	6		//聊天报文类型代号
#define CHAT_BUF_SIZE 	255		//一个报文载荷所能存储的最大字节数
#define ADD_FRD 		60		//服务器通知客户机，添加聊天用户指令
#define DEL_FRD 		61		//服务器通知客户机，删除聊天用户指令
#define FRD_MSG 		62		//单发聊天信息
#define BROCAST_MSG 	63		//群发聊天信息

typedef struct ChatPDUS{
	unsigned char type;		// PDU类型，值为CHAT_PDU_TYPE = 6
	unsigned char cmd;		//命令，包括好友上线、下线、单播信息、广播信息
	unsigned char frdno;	//用户ID（Friend No）
	unsigned char len;
	char msg[CHAT_BUF_SIZE];		//载荷，包括聊天消息或上下线用户名等
} ChatPDU;


unsigned int translatenum(InquiryPDU *buff, int Password);
unsigned int getnum(char *key, char *idx, InquiryPDU *buff);