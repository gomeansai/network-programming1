// AttackClient.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "windows.h"
#include "winsock.h"
#include "stdio.h"
#include "time.h"
#pragma comment (lib,"wsock32.lib")
#pragma warning(disable:4996)

#define MAX_THRD_SIZE 10000			// 10K
#define MAX_BUF_SIZE 1000000		// 1M
#define MIN_BUF_SIZE 100			// 1K
#define SLEEP_TIME 1000				// 1s
#define DOUBLE_SIZE 20				//���ʱ����̬����һ���ַ���

void my_printf(char * p)
{
	printf("%s\n", p);
}
void my_scanf(char * buf, int buf_size)
{
	scanf("%s", buf);
}

struct InPara{
	SOCKET sock;
	LARGE_INTEGER first_send;	//���ڼ�¼���͵�һ֡��ʱ��
	LARGE_INTEGER last_send;	//���ڼ�¼�������һ֡��ʱ��
	LARGE_INTEGER first_recv;
	LARGE_INTEGER last_recv;
	long data_total;			//�ܼ���Ҫ�������ݴ�С
	long send_total;			//�ѷ��͵����ݴ�С
	long recv_total;			//�ѽ��յ����ݴ�С
	char flag;					//Ϊ0ʱ�����ʹ�ã�Ϊ1ʱ������������ʹ�ã�
	//Ϊ2ʱ�������߳���ʹ�ã�Ϊ3ʱ��������߳���ʹ��
};
struct InParaArray{
	long num_thread;
	struct InPara *p_InPara[MAX_THRD_SIZE];
};

DWORD WINAPI client_send_proc(LPVOID lpParam);
DWORD WINAPI client_recv_proc(LPVOID lpParam);
DWORD WINAPI show_thread(LPVOID lpParam);

int _tmain(int argc, _TCHAR* argv[])
{
	SOCKET s;
	struct sockaddr_in server;
	WSAData wsa;
	int retval;
	int i;
	FILE *fp;

	char scanbuf[80];
	//char *sendbuf = (char*) malloc(MAX_SENDBUF_SIZE);
	long sum_char;
	//long num_thread;
	//���ڼ�����ʱ�ı���
	LARGE_INTEGER Freq;
	double t_measure;
	//���ڴ���
	//struct InPara *p_InPara[MAX_THRD_SIZE];
	struct InParaArray *p_IPA;
	DWORD dwThrdParam[MAX_THRD_SIZE];
	DWORD dwThreadID[MAX_THRD_SIZE];
	HANDLE hSendThrd[MAX_THRD_SIZE];
	HANDLE hRecvThrd[MAX_THRD_SIZE];

	DWORD dwShowThrdParam;
	HANDLE hShowThrd;
	DWORD dwShowThreadID;
	WSAStartup(0x101, &wsa);

	printf("�շ�˫�̹߳����ͻ��˿�����\n\n");
	p_IPA = (struct InParaArray *)malloc(sizeof(struct InParaArray));

	//�������û�������
	server.sin_family = AF_INET;

	printf("�����������IP��ַ : ");
	my_scanf(scanbuf, sizeof(scanbuf));
	server.sin_addr.S_un.S_addr = inet_addr(scanbuf);

	printf("������������˿ں� : ");
	my_scanf(scanbuf, sizeof(scanbuf));
	server.sin_port = htons(atoi(scanbuf));

	printf("�����뽨����������(������%d): ", MAX_THRD_SIZE);
	my_scanf(scanbuf, sizeof(scanbuf));
	p_IPA->num_thread = atol(scanbuf);

	printf("������ÿ���������跢���ֽ��� : ");
	my_scanf(scanbuf, sizeof(scanbuf));
	sum_char = atol(scanbuf);

	printf("\n");

	printf("�ѽ��������� �� %8d", 0);
	//��ʼѭ�������߳�
	for (i = 0; i < p_IPA->num_thread; i++){
		p_IPA->p_InPara[i] = (struct InPara *)malloc(sizeof(struct InPara));

		p_IPA->p_InPara[i]->sock = socket(AF_INET, SOCK_STREAM, 0);
		retval = connect(p_IPA->p_InPara[i]->sock, (sockaddr*)&server, sizeof(server));
		if (retval != 0){
			printf("ERROR : �׽��� s=%d �޷����ӷ�������\n", p_IPA->p_InPara[i]->sock);
			printf("�������ӣ�\n");
			//������������1���ص�ѭ�����½���
			printf("�ѽ��������� �� %8d", i);
			i--;
			continue;
		}

		//��ӡ�ѽ�����������
		printf("\b\b\b\b\b\b\b\b");
		printf("%8d", i + 1);

		//����������ʼ��
		//p_InPara[i]->sock = sock;
		p_IPA->p_InPara[i]->data_total = sum_char;
		p_IPA->p_InPara[i]->send_total = 0;
		p_IPA->p_InPara[i]->recv_total = 0;
		p_IPA->p_InPara[i]->flag = 0;
		p_IPA->p_InPara[i]->first_send.LowPart = 0;
		p_IPA->p_InPara[i]->first_send.HighPart = 0;
		p_IPA->p_InPara[i]->last_send.LowPart = 0;
		p_IPA->p_InPara[i]->last_send.HighPart = 0;
		p_IPA->p_InPara[i]->first_recv.LowPart = 0;
		p_IPA->p_InPara[i]->first_recv.HighPart = 0;
		p_IPA->p_InPara[i]->last_recv.LowPart = 0;
		p_IPA->p_InPara[i]->last_recv.HighPart = 0;

		dwThrdParam[i] = (DWORD)(p_IPA->p_InPara[i]);// �ӻ�ָ�룡��������������
		hSendThrd[i] = CreateThread(
			NULL,
			0,
			client_send_proc,
			&dwThrdParam[i],
			CREATE_SUSPENDED,	//����
			&dwThreadID[i]);

		hRecvThrd[i] = CreateThread(
			NULL,
			0,
			client_recv_proc,
			&dwThrdParam[i],
			CREATE_SUSPENDED,	//����
			&dwThreadID[i]);

		//printf("�׽��� s=%d ��Ӧ�շ��̴߳����ɹ���\n\n", s);
	}

	printf("\n");

	for (i = 0; i < p_IPA->num_thread; i++){		// �� MAX_THRD_SIZE ��Ϊ num_thread
		ResumeThread(hSendThrd[i]);
		ResumeThread(hRecvThrd[i]);
	}

	/*
	//������ӡ��Ļ�߳�
	dwShowThrdParam = (DWORD)p_IPA;
	hShowThrd = CreateThread(
	NULL,
	0,
	show_thread,
	&dwShowThrdParam,
	0,
	&dwShowThreadID);
	*/

	//��ȡƵ��
	QueryPerformanceFrequency(&Freq);

	int num;
	char c_num[DOUBLE_SIZE];
	LPDWORD lpExitCode;
	DWORD d_retval;

	//��ʼ������¼��ʱ
	fopen_s(&fp, "test_time.txt", "w");

	fwrite("�߳���ʱͳ��(us)\n", 17, 1, fp);

	fwrite("�߳�����\t", 30, 1, fp);
	_itoa(p_IPA->num_thread, c_num, 10);	//10->ʮ����
	fwrite(c_num, strlen(c_num), 1, fp);
	fwrite("\n", 1, 1, fp);

	fwrite("ÿ���̷߳�����������\t", 21, 1, fp);
	_itoa(sum_char, c_num, 10);
	fwrite(c_num, strlen(c_num), 1, fp);
	fwrite("\n", 1, 1, fp);

	fwrite("sock", 4, 1, fp);
	fwrite("\t", 1, 1, fp);
	fwrite("first_send~last_send", 20, 1, fp);
	fwrite("\t", 1, 1, fp);
	fwrite("first_send~first_recv", 21, 1, fp);
	fwrite("\t", 1, 1, fp);
	fwrite("first_send~last_recv", 20, 1, fp);
	fwrite("\n", 1, 1, fp);

	for (i = 0, num = 0; num < p_IPA->num_thread; i = (i + 1) % p_IPA->num_thread){
		Sleep(SLEEP_TIME);
		if (hRecvThrd[i] == NULL){
			continue;
		}
		//printf("s = %d\n<--%8d\n-->%8d\n", p_InPara[i]->sock, p_InPara[i]->send_total, p_InPara[i]->recv_total);
		d_retval = WaitForSingleObject(hRecvThrd[i], 0);
		//�ж��߳��Ƿ����
		if (d_retval == WAIT_OBJECT_0){
			//�߳��ѽ���
			//�ж��̶߳�Ӧ�׽����Ƿ��Ѿ����������ʱ
			if (p_IPA->p_InPara[i]->flag == 1){
				//δ�����������ʼ����
				//д���׽��ִ�С

				//sock
				_itoa(p_IPA->p_InPara[i]->sock, c_num, 10);	//10->ʮ����
				fwrite(c_num, strlen(c_num), 1, fp);
				fwrite("\t", 1, 1, fp);

				//first_send ~ last_send
				t_measure = ((p_IPA->p_InPara[i]->last_send.QuadPart - p_IPA->p_InPara[i]->first_send.QuadPart) * 1000000) / (double)Freq.QuadPart;
				_gcvt(t_measure, DOUBLE_SIZE - 2, c_num);
				fwrite(c_num, DOUBLE_SIZE - 2, 1, fp);
				fwrite("\t", 1, 1, fp);

				//first_send ~ first_recv
				t_measure = ((p_IPA->p_InPara[i]->first_recv.QuadPart - p_IPA->p_InPara[i]->first_send.QuadPart) * 1000000) / (double)Freq.QuadPart;
				_gcvt(t_measure, DOUBLE_SIZE - 2, c_num);
				fwrite(c_num, DOUBLE_SIZE - 2, 1, fp);
				fwrite("\t", 1, 1, fp);

				//first_send ~ last_recv
				t_measure = ((p_IPA->p_InPara[i]->last_recv.QuadPart - p_IPA->p_InPara[i]->first_send.QuadPart) * 1000000) / (double)Freq.QuadPart;
				_gcvt(t_measure, DOUBLE_SIZE - 2, c_num);
				fwrite(c_num, DOUBLE_SIZE - 2, 1, fp);
				fwrite("\n", 1, 1, fp);

				//closesocket(p_InPara[i]->sock);
				p_IPA->p_InPara[i]->flag = 2;
				num++;
			}
			continue;
		}
		//Sleep(SLEEP_TIME);		//��ʱ
	}
	fclose(fp);

	system("pause");
	WSACleanup();
	return 0;
}

DWORD WINAPI show_thread(LPVOID lpParam)
{
	long former_send_total, latter_send_total;	//long long ? ///////////////////////////
	long former_recv_total, latter_recv_total;
	long flag_total;
	int i;

	struct InParaArray *para_array;
	para_array = *(struct InParaArray**)lpParam;

	former_send_total = 0;
	former_recv_total = 0;
	latter_send_total = 0;
	latter_recv_total = 0;

	printf("�ѽ���������\t��������\t��������\n%4d\t", para_array->num_thread);
	printf("%4d\t%4d", latter_send_total - former_send_total, latter_recv_total - former_recv_total);

	while (1){
		latter_send_total = 0;
		latter_recv_total = 0;
		for (i = 0; i < para_array->num_thread; i++){
			latter_send_total += para_array->p_InPara[i]->send_total;
			latter_recv_total += para_array->p_InPara[i]->recv_total;
		}

		printf("\b\b\b\b\b\b\b\b\b");
		//printf("�ѽ���������\t��������\t��������\n%4d\t", para_array->num_thread);
		printf("%4d\t%4d", latter_send_total - former_send_total, latter_recv_total - former_recv_total);

		former_send_total = latter_send_total;
		former_recv_total = latter_send_total;

		Sleep(SLEEP_TIME);
		//printf("%d\n", para_array->p_InPara[0]->data_total);
	}
	return 0;
}

DWORD WINAPI client_send_proc(LPVOID lpParam)
{
	struct InPara* para;
	long window_size;	//���ڴ�С
	long send_len;		//�������͵ĳ���
	long total;			//�ѷ�������
	int retval;
	char *sendbuf = NULL;

	para = *(struct InPara**)lpParam;	// ָ��ӿӿӣ���������
	window_size = MIN_BUF_SIZE;			//////////////////////////////////////////////�����̶̹߳����ڴ�С����ѡ���Ϊ MAX_BUF_SIZE
	send_len = 0;
	//total = 0;

	//printf("�׽��� s = %d �����߳̿����ɹ���\n", para->sock);

	//��ʼѭ������
	while (para->send_total != para->data_total){
		//ȷ�����η��͵����ݳ���
		send_len = para->data_total - para->send_total;
		if (send_len > window_size){
			send_len = window_size;
		}
		//���붯̬�ڴ�ռ�
		sendbuf = (char*)malloc(window_size);
		if (sendbuf == NULL){
			printf("ERROR : �׽��� s=%d û���㹻���棡\n", para->sock);
			//Sleep(SLEEP_TIME);		//��ʱ
			continue;
		}

		retval = send(para->sock, sendbuf, send_len, 0);
		if (retval < 0){
			//Sleep(SLEEP_TIME);		//��ʱ
			printf("ERROR : �׽��� s=%d ���ʹ��� Retval=%d\n", para->sock, retval);
			free(sendbuf);
			sendbuf = NULL;
			continue;
		}
		if (retval == 0){
			printf("ERROR : �׽��� s=%d ���ͳ���\n", para->sock);
			continue;
		}

		//���ǵ�һ�η��ͣ���¼ʱ��
		if (para->send_total == 0){
			QueryPerformanceCounter(&para->first_send);
		}

		free(sendbuf);
		sendbuf = NULL;

		para->send_total += retval;
	}
	QueryPerformanceCounter(&para->last_send);

	printf("�׽��� s = %d �����̷߳�����ϣ�\n", para->sock);

	return 0;
}

DWORD WINAPI client_recv_proc(LPVOID lpParam)
{
	struct InPara *para;
	char *recvbuf;
	long retval;
	long recv_len;

	para = *(struct InPara**)lpParam;
	recv_len = MAX_BUF_SIZE;	//////////////////////////////////////////////�����̶̹߳����ڴ�С����ѡ���Ϊ MIN_BUF_SIZ

	//printf("�׽��� s = %d �����߳̿����ɹ���\n", para->sock);

	while (para->recv_total != para->data_total){
		//������ջ�����
		recvbuf = (char*)malloc(recv_len);
		if (recvbuf == NULL){
			//Sleep(SLEEP_TIME);		//��ʱ
			printf("ERROR : �׽��� s=%d �ڴ治�㣡\n", para->sock);
			continue;
		}

		//����
		retval = recv(para->sock, recvbuf, recv_len, 0);
		if (retval == 0){
			printf("\n");
			printf("ERROR : �׽��� s=%d �ѽ��� %d �ֽ����ݣ� ��ֹ��\n", para->sock, para->recv_total);
			return -1;
		}
		else if (retval == -1) {
			retval = WSAGetLastError();
			if (retval == WSAEWOULDBLOCK){
				continue;
			}
			printf("\nERROR NUM : %d\n", retval);
			printf("ERROR : �׽��� s=%d �ѽ��� %d �ֽ����ݣ� ��ֹ��\n", para->sock, para->recv_total);
			return -1;
		}

		//���ǵ�һ�ν��գ���¼ʱ��
		if (para->recv_total == 0){
			QueryPerformanceCounter(&para->first_recv);
			para->flag = 1;
		}

		para->recv_total += retval;

		free(recvbuf);
	}
	QueryPerformanceCounter(&para->last_recv);

	printf("�׽��� s = %d �����߳̽�����ϣ�\n", para->sock);

	closesocket(para->sock);
	return 0;
}