#pragma once
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