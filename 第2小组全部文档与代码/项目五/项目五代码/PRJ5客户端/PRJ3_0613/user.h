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


//״̬��
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


#define BUF_SIZE	FILE_BUF_SIZE + 1 + 1 + 1 + 1 + 1//��ʱ���壬��������Ҫ��׼ȷ����ֵ


#define INQUIRY_PDU_TYPE 1
#define INQUIRY_BUF_SIZE 16*4      //��ѯ��������ռ�õ�����ֽ���
typedef struct InquiryPDUS {
	char type;     //INQUIRING_PDU_TYPE = 1
	char num;    //�洢��ѯ����������Χ1-16��
	char each_len[4];    //���ÿ����ѯ���ĳ��ȡ�ÿ�����ռ2λ��ֵ1��2��3��0�ֱ��ʾ��ѯ������Ϊ1��2��3��4��4���ֽ�32bit������ܱ��16������
	char inquiry_buf[INQUIRY_BUF_SIZE];     //ÿ����ѯ������λ�洢��ǰ����λ�洢�ں󣬷��������ֽ���
}InquiryPDU;


#define CTL_PDU_TYPE	2
#define CTL_BUF_SIZE	255    	//һ�������غ����ܴ洢������ֽ���
//����Ϊ����λcmdȡֵ����Ϊ2��ͷ����λ������CTL_PDU_TYPE����
#define REQ_ACCOUNT 	20     	//��������ͻ��������û��ʺ�
#define REQ_FILELIST 	21   	//�ͻ���������������ȡ�ļ��б�
#define REQ_DOWNFILE 	22   	//�ͻ���������������ļ�����
#define REQ_UPFILE 		23    	//�ͻ���������������ļ��ϴ�
#define REQ_TRANSLATE	24		//�ļ���תȷ����Ϣ
#define RE_ACCESS  		25		//��������֪�ͻ�����½�ɹ�
#define RE_TIMEOUT		26		//��������֪�ͻ�����ʱ
#define RE_ERR_ACCOUNT	27		//��������֪�ͻ����û�������
#define RE_ERR_INQUIRY	28		//��������֪�ͻ�����ѯ�����ͨ��
#define RE_ERR_FILENAME 29   	//��������֪�ͻ����ļ�������

typedef struct CtlPDUS{
	unsigned char type;      		//PDU���ͣ�ֵΪCTL_PDU_TYPE = 2
	unsigned char cmd;				//���������ѡ�����������ĺ궨��
	unsigned char len;				//�������غɵĳ��ȣ�0~255
	unsigned char id;				//���ļ��ϴ�ʱʹ�ã���ʶ�ļ��������Ϊ���������������û�
	char buf[CTL_BUF_SIZE];			//�غɣ����ݲ�ͬ������cmdֵ
	//�����û����������ص��ļ�����
} CtlPDU;


#define	 FILE_PDU_TYPE	 	4	//�ļ���������
#define	 FILE_BUF_SIZE 		255	//һ�������غ����ܴ洢������ֽ���
//�������ӽ�����ĵ�һ֡���ɷ������������ͻ���
//ͨ����ͬNoֵ����֪�����ӵĹ���
#define GET_FILELIST_START 	2
#define DOWN_FILE_START 	3
#define UP_FILE_START 		4

typedef struct FilePDUS{
	unsigned char type;		//PDU���ͣ�ֵΪFILE_PDU_TYPE = 4
	unsigned char No;		//����ʱ��PDU��ţ���ѡ����0 ��1����һ���ļ�/�ļ��б�PDU��Noֵ��ѡ2��3��4
	unsigned char len;		//Ĭ��Ϊ���ĳ����ܳ��ȡ���ֵΪ0ʱ����ʾ�������
	char buf[FILE_BUF_SIZE];	//�غɣ���������ļ��б���ļ������ļ�������
} FilePDU;


#define	CHAT_PDU_TYPE	6		//���챨�����ʹ���
#define CHAT_BUF_SIZE 	255		//һ�������غ����ܴ洢������ֽ���
#define ADD_FRD 		60		//������֪ͨ�ͻ�������������û�ָ��
#define DEL_FRD 		61		//������֪ͨ�ͻ�����ɾ�������û�ָ��
#define FRD_MSG 		62		//����������Ϣ
#define BROCAST_MSG 	63		//Ⱥ��������Ϣ

typedef struct ChatPDUS{
	unsigned char type;		// PDU���ͣ�ֵΪCHAT_PDU_TYPE = 6
	unsigned char cmd;		//��������������ߡ����ߡ�������Ϣ���㲥��Ϣ
	unsigned char frdno;	//�û�ID��Friend No��
	unsigned char len;
	char msg[CHAT_BUF_SIZE];		//�غɣ�����������Ϣ���������û�����
} ChatPDU;


unsigned int translatenum(InquiryPDU *buff, int Password);
unsigned int getnum(char *key, char *idx, InquiryPDU *buff);