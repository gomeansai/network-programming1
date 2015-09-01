#pragma once
#define INQUIRY_PDU_TYPE 1
#define INQUIRY_BUF_SIZE 16*4      //质询数所可能占用的最大字节数
typedef struct InquiryPDUS {
	char type;     //INQUIRING_PDU_TYPE = 1
	char num;    //存储质询数个数（范围1-16）
	char each_len[4];    //表达每个质询数的长度。每个表达占2位，值1、2、3、0分别表示质询数长度为1、2、3、4。4个字节32bit，最多能表达16个数。
	char inquiry_buf[INQUIRY_BUF_SIZE];     //每个质询数，高位存储在前，低位存储在后，符合网络字节序
}InquiryPDU;


#define CTL_PDU_TYPE 2
#define CTL_BUF_SIZE 255    //一个字节所能存储的最大数值
//改为2开头的两位数，与CTL_PDU_TYPE看齐
#define REQ_ACCOUNT 20     //Server ask Client
#define REQ_FILELIST 21   //Client to Server
#define REQ_DOWNFILE 22   //Client to Server
#define REQ_UPFILE 23    //Client to Server
typedef struct CtlPDUS{
	unsigned char type;      //CTL_PDU_TYPE = 2
	unsigned char cmd;
	unsigned char len;
	char filename[CTL_BUF_SIZE];
}CtlPDU;


#define FILELIST_PDU_TYPE 3
#define FILE_PDU_TYPE 4
#define FILE_BUF_SIZE 255     //一个字节所能存储的最大数值
typedef struct FilePDUS{
	unsigned char type;     //FILE_PDU_TYPE = 3；FILE_PDU_TYPE = 4
	unsigned char No;        //0 or 1
	unsigned char len;       //len == 0: 传输结束
	char buf[FILE_BUF_SIZE];
}FilePDU;