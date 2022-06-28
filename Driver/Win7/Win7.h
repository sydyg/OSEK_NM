//���ݲ�ͬ��ƽ̨�������ã����ú������ֲ����ͬƽ̨,��Ҫ���ö�ʱ����������
#ifndef WIN7_H
#define WIN7_H

//#define WIN7PC  //����Windowsƽ̨����ע�ʹ˴�
#ifdef WIN7PC
#include"../../OsekNM_core/OsekNM.h"
#include<stdio.h>
/*ƽ̨�޹صĶ���*/
typedef unsigned char TimerOutFlag_t;
//����ʱ�����ͣ�δ��ʼ��(δ����InitDirectNMParams)��Щ�����������涨���
//��������ĵ�ID
#define NMID 0x18ffA019
//���ýڵ�Դ��ַ,�ڵ��ַ����Ϊ0
#define ADDR_SELF 0x19
#define NM_TIMETYPE_TTYP     100
#define NM_TIMETYPE_TMAX     260
#define NM_TIMETYPE_TERROR   1000
#define NM_TIMETYPE_TWBS     5000
//���嶨ʱ������
#define NM_TIMER_TTYP    0
#define NM_TIMER_TMAX    1
#define NM_TIMER_TERROR  2
#define NM_TIMER_TWBS    3

//��ӡ����
//#define PRINT
//����ֵ����
#define E_OK     0
#define E_ERROR -1
//ƽ̨��صĶ���
//#define OSEKOS //��Ҫʹ��OSEKOS

#pragma comment(lib, "Winmm.lib")
#include <stdio.h>
#include <Windows.h>
#include <Mmsystem.h>

char SetToFIFO(NMPDU_t* msg);
char GetFromFIFO(NMPDU_t* msg);
void ClearFIFO(void);
void InitPlatform();
void InitNMPDU();
NMTypeU8_t TX_CAN_Transmit(NMPDU_t* NMPDU);
int SetAlarm(NMTimerType_t timer);
void CancelAlarm(int TimerId);
TimerOutFlag_t GetTimerIsOut(NMTimerType_t TimerType);
void ClcTimerOutFlag(NMTimerType_t TimerType);

//��ʱ������
void WINAPI SpyTmr_TimerTMax1(UINT wTimerID, UINT msg, DWORD dwUser, DWORD dwl, DWORD dw2);
void WINAPI SpyTmr_TimerRing1(UINT wTimerID, UINT msg, DWORD dwUser, DWORD dwl, DWORD dw2);
void WINAPI SpyTmr_TimerLimpHome1(UINT wTimerID, UINT msg, DWORD dwUser, DWORD dwl, DWORD dw2);
void WINAPI SpyTmr_TimerWaitBusSleep(UINT wTimerID, UINT msg, DWORD dwUser, DWORD dwl, DWORD dw2);


//��ʹ��OS�Ļ���ʹ���Զ���Ķ�ʱ������
#ifndef OSEKOS
int SetAlarm(NMTimerType_t timer);
void CancelAlarm(int TimerId);
#endif

//FIFO��С
#define FIFOMAX 20

//���ܱ���FIFO
typedef struct {
	NMPDU_t MSGs[FIFOMAX];//FIFO������
	char Total;//FIFO�еı�������
	char Head;//ָ�����ͷ
	char Tail;//ָ���β
	char FullFlag;//��������
	char EmptyFlag;//��������
	char(*GetMsg) (NMPDU_t* msg); //�ӻ�������ȡ����
	char(*SetMsg) (NMPDU_t* smsg); //���뱨�ĵ�������
	void(*ClearBuff)(void);//���FIFO
}RecvFIFO_t;
#endif
#endif
