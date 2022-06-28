//���ݲ�ͬ��ƽ̨�������ã����ú������ֲ����ͬƽ̨
#ifndef CONFIGH_H
#define CONFIGH_H

#include "OsekNM.h"

#define STM32F407  //����STM32F407ƽ̨�ͽ���ע�͵�
#ifdef STM32F407
#include "CAN.h"
#include "Timer.h"
#include "usart.h"
#include "delay.h"
#endif

/*ƽ̨�޹صĶ���*/
typedef  int TimerOutFlag_t;

//��������ĵ�ID
#define NMID (0x0cffA019)

//����Ĭ�Ͻڵ�Դ��ַ,�ڵ��ַ����Ϊ0
#define ADDR_SELF 0x19

//����ʱ�����ͣ�δ��ʼ��(δ����InitDirectNMParams)��Щ�����������涨���
#define NM_TIMETYPE_TTYP     100
#define NM_TIMETYPE_TMAX     260
#define NM_TIMETYPE_TERROR   1000
#define NM_TIMETYPE_TWBS     5000
//���嶨ʱ������
#define NM_TIMER_TTYP    0
#define NM_TIMER_TMAX    1
#define NM_TIMER_TERROR  2
#define NM_TIMER_TWBS    3
//����ֵ����
#define E_OK     0
#define E_ERROR -1

//ƽ̨��صĶ���
//#define OSEKOS //��Ҫʹ��OSEKOS

//#define PRINT
char SetToFIFO(NMPDU_t* msg);
char GetFromFIFO(NMPDU_t* msg);
void ClearFIFO(void);
void InitPlatform(void);
void InitNMPDU(NMPDU_t* NMPDU);
NMTypeU8_t TX_CAN_Transmit(NMPDU_t* NMPDU);
TimerOutFlag_t GetTimerIsOut(NMTimerType_t TimerType);
void ClcTimerOutFlag(NMTimerType_t TimerType);
void Recv_EveryMessage(NMPDU_t* p_Msg);

//��ʹ��OS�Ļ���ʹ���Զ���Ķ�ʱ������
#ifndef OSEKOS
int SetAlarm(NMTimerType_t timer);
void CancelAlarm(NMTimerType_t timer);
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
//NM_xxx���Ķ���

#endif

