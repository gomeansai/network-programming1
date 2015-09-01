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

///////////////////////////////////////////////////////////

#define LOCAL_PORT 54321

#define BUF_SIZE	FILE_BUF_SIZE + 1 + 1 + 1 + 1//��ʱ���壬��������Ҫ��׼ȷ����ֵ

#define GET_FILELIST_START 2
#define DOWN_FILE_START 3
#define UP_FILE_START 4

//״̬��
#define S_CLOSE		200
#define S_LISTENIND	201
#define S_AUTHSTART	202
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

#define SERV_PORT 12345
#define SERV_IP INADDR_LOOPBACK

#define INQUIRY_PDU_TYPE 1
#define INQUIRY_BUF_SIZE 16*4      //��ѯ��������ռ�õ�����ֽ���
typedef struct InquiryPDUS {
	char type;     //INQUIRING_PDU_TYPE = 1
	char num;    //�洢��ѯ����������Χ1-16��
	char each_len[4];    //���ÿ����ѯ���ĳ��ȡ�ÿ�����ռ2λ��ֵ1��2��3��0�ֱ��ʾ��ѯ������Ϊ1��2��3��4��4���ֽ�32bit������ܱ��16������
	char inquiry_buf[INQUIRY_BUF_SIZE];     //ÿ����ѯ������λ�洢��ǰ����λ�洢�ں󣬷��������ֽ���
}InquiryPDU;


#define CTL_PDU_TYPE 2
#define CTL_BUF_SIZE 255    //һ���ֽ����ܴ洢�������ֵ
//��Ϊ2��ͷ����λ������CTL_PDU_TYPE����
#define REQ_ACCOUNT 20     //Server ask Client
#define REQ_FILELIST 21   //Client to Server
#define REQ_DOWNFILE 22   //Client to Server
#define REQ_UPFILE 23    //Client to Server

#define RE_ACCESS  25
#define RE_TIMEOUT		26
#define RE_ERR_ACCOUNT	27
#define RE_ERR_INQUIRY	28
#define RE_ERR_FILENAME 29

typedef struct CtlPDUS{
	unsigned char type;      //CTL_PDU_TYPE = 2
	unsigned char cmd;
	unsigned char len;
	char filename[CTL_BUF_SIZE];
}CtlPDU;


#define FILELIST_PDU_TYPE 3
#define FILE_PDU_TYPE 4
#define FILE_BUF_SIZE 255     //һ���ֽ����ܴ洢�������ֵ
typedef struct FilePDUS{
	unsigned char type;     //FILE_PDU_TYPE = 3��FILE_PDU_TYPE = 4
	unsigned char No;        //0 or 1
	unsigned char len;       //len == 0: �������
	char buf[FILE_BUF_SIZE];
}FilePDU;

//SOCKET CtlSock = INVALID_SOCKET;
//SOCKET DatSock = INVALID_SOCKET;

unsigned int translatenum(InquiryPDU *buff,int Password);
unsigned int getnum(char *key, char *idx, InquiryPDU *buff);

//u8 Is_On;

