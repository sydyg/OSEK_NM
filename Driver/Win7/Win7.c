#include "win7.h"
#ifdef WIN7PC
//��WIN7ƽ̨��صĴ���������ʵ�֣�����NM���Ļ���������ʱ��

//��ʱ������
static MMRESULT TidTTyp;
static MMRESULT TidTMax;
static MMRESULT TidTError;
static MMRESULT TidTLimpHome;
static MMRESULT TidTWBS;
//���Ļ���������
static RecvFIFO_t RecvFIFO;

//��ʱ����ʱ��־
static TimerOutFlag_t TimerOutFlag_TTYP = 0;
static TimerOutFlag_t TimerOutFlag_TMAX = 0;
static TimerOutFlag_t TimerOutFlag_TERROR = 0;
static TimerOutFlag_t TimerOutFlag_TWBS = 0;


//NMPDU��ʼ��������λ��1
void InitNMPDU(NMPDU_t* NMPDU)
{
	int i = 0;
	NMPDU->MsgCtl = 0xc8;
	//NMPDU->MsgID = ((NMID << 8) | ADDR_SELF);
	//NMPDU->MsgDA = ADDR_SELF;

	for (; i < 6; i++)
	{
		NMPDU->MsgData[i] = 0xff;
	}
}

//���ض�ʱ���Ƿ�ʱ -1:����
TimerOutFlag_t GetTimerIsOut(NMTimerType_t TimerType)
{
	switch (TimerType)
	{
	case NM_TIMER_TTYP:
		return TimerOutFlag_TTYP;
	case NM_TIMER_TMAX:
		return TimerOutFlag_TMAX;
	case NM_TIMER_TERROR:
		return TimerOutFlag_TERROR;
	case NM_TIMER_TWBS:
		return TimerOutFlag_TWBS;
	}
	return -1;
}

//�����ʱ����ʱ��־
void ClcTimerOutFlag(NMTimerType_t TimerType)
{
	switch (TimerType)
	{
	case NM_TIMER_TTYP:
		TimerOutFlag_TTYP = 0;
		break;
	case NM_TIMER_TMAX:
		TimerOutFlag_TMAX = 0;
		break;
	case NM_TIMER_TERROR:
		TimerOutFlag_TERROR = 0;
		break;
	case NM_TIMER_TWBS:
		TimerOutFlag_TWBS = 0;
		break;
	}
}


//FIFO��صĺ���
/*˵����SetFIFO�����յ��ı��ķ���FIFO,������FIFO
* ������GenericMessage* msg������ָ��
* ����ֵ��0:�ɹ����뱨�ĵ�FIFO��1������ʧ��
*/
char SetToFIFO(NMPDU_t* msg)
{
	if (RecvFIFO.FullFlag == 1)//���жϻ���������
		return 1;
	/*���뱨�ĵ�������*/
	RecvFIFO.MSGs[RecvFIFO.Tail% FIFOMAX] = *msg;
	RecvFIFO.Tail = (RecvFIFO.Tail + 1) % FIFOMAX;
	/*����ձ�ʶ*/
	RecvFIFO.EmptyFlag = 0;
	if ((RecvFIFO.Tail + 1) == RecvFIFO.Head)//��������
		RecvFIFO.FullFlag = 1;
	return 0;
}
/*˵����GetFIFO����FIFOȡ������,������FIFO
* ������GenericMessage* msg������ָ��
* ����ֵ��1:�ɹ�ȡ�����ģ�0��ȡ��ʧ��
*/
char GetFromFIFO(NMPDU_t* msg)
{
	int i = 2;
	if (RecvFIFO.EmptyFlag == 1)//���жϻ������շ�
		return 0;
	/*�ӻ�����ȡ������*/
	msg->MsgDA = RecvFIFO.MSGs[RecvFIFO.Head% FIFOMAX].MsgDA;
	msg->MsgCtl = RecvFIFO.MSGs[RecvFIFO.Head% FIFOMAX].MsgCtl;
	msg->MsgID = RecvFIFO.MSGs[RecvFIFO.Head% FIFOMAX].MsgID;
	//������ֱ�Ӹ���
	for (; i < DLC; i++)
	{
		msg->MsgData[i] = RecvFIFO.MSGs[RecvFIFO.Head% FIFOMAX].MsgData[i];
	}
	RecvFIFO.Head = (RecvFIFO.Head + 1) % FIFOMAX;
	/*�������ʶ*/
	RecvFIFO.FullFlag = 0;
	if ((RecvFIFO.Tail) == RecvFIFO.Head)//��������
		RecvFIFO.EmptyFlag = 1;
	return 1;
}
/*˵����ClearFIFO���������FIFO
* ������void
* ����ֵ��void
*/
void ClearFIFO(void)
{
	RecvFIFO.Total = 0;
	RecvFIFO.Head = 0;
	RecvFIFO.Tail = 0;
	RecvFIFO.FullFlag = 0;
	RecvFIFO.EmptyFlag = 1;
}
/*��ʱ��*/

/*��������TX_CAN_Transmit
*������NMPDU
*����ֵ���ɹ� 1
*˵����ƽ̨�Լ��ı��ķ��ͺ�����ʵ��
*/
NMTypeU8_t TX_CAN_Transmit(NMPDU_t* NMPDU)
{
	int i;
	printf("ID: %x Data: ",NMPDU->MsgID);
	printf("%x ", NMPDU->MsgDA);
	printf("%x ", NMPDU->MsgCtl);
	for (i = 0; i < 6; i++)
	{
		printf("%x ",NMPDU->MsgData[i]);
	}
	printf("\n");
	return 1;
}
//ƽ̨��صĳ�ʼ��
void InitPlatform()
{
	/*��������ʼ��*/
	RecvFIFO.GetMsg = GetFromFIFO;
	RecvFIFO.SetMsg = SetToFIFO;
	RecvFIFO.ClearBuff = ClearFIFO;
	RecvFIFO.ClearBuff();
}

//��ʱ����ʱ����
/*LimpHome��ʱ��*/
static void WINAPI TimerTError(UINT wTimerID, UINT msg, DWORD dwUser, DWORD dwl, DWORD dw2)
{
	TimerOutFlag_TERROR = 1;
#ifdef PRINT
	printf("TError out\n");
#endif
}

/*TMax��ʱ��ʱ��*/
static void WINAPI TimerTMax(UINT wTimerID, UINT msg, DWORD dwUser, DWORD dwl, DWORD dw2)
{
	TimerOutFlag_TMAX = 1;
#ifdef PRINT
	printf("TError out\n");
#endif
}

/*TTYP��ʱ��*/
static void WINAPI TimerTTYP(UINT wTimerID, UINT msg, DWORD dwUser, DWORD dwl, DWORD dw2)
{
	TimerOutFlag_TTYP = 1;
#ifdef PRINT
	printf("TTyp out\n");
#endif
}
/*WaitBusSleep��ʱ��*/
static void WINAPI  TimerTWBS(UINT wTimerID, UINT msg, DWORD dwUser, DWORD dwl, DWORD dw2)
{
	TimerOutFlag_TWBS = 1;
#ifdef PRINT
	printf("TWBS out\n");
#endif
}
/*
*�Զ��嶨ʱ��������SetAlarm
��������ʱ������
˵�������ݶ�ʱ�����ͣ��Զ��󶨶�ʱ����ʱʱ��
*/
int SetAlarm(NMTimerType_t timer)
{
	int Tid = 0;
	switch (timer)
	{
	case NM_TIMER_TTYP:
		TidTTyp = timeSetEvent(NM_TIMETYPE_TTYP, 1, (LPTIMECALLBACK)TimerTTYP, 1, TIME_ONESHOT);
		Tid = TidTTyp;
		break;
	case NM_TIMER_TMAX:
		TidTMax = timeSetEvent(NM_TIMETYPE_TMAX, 1, (LPTIMECALLBACK)TimerTMax, 1, TIME_ONESHOT);
		Tid = TidTMax;
		break;
	case NM_TIMER_TERROR:
		TidTError = timeSetEvent(NM_TIMETYPE_TERROR, 1, (LPTIMECALLBACK)TimerTError, 1, TIME_ONESHOT);
		Tid = TidTError;
		break;
	case NM_TIMER_TWBS:
		TidTWBS = timeSetEvent(NM_TIMETYPE_TWBS, 1, (LPTIMECALLBACK)TimerTWBS, 1, TIME_ONESHOT);
		Tid = TidTWBS;
		break;
	}
	return Tid;
}
/*
*�Զ��嶨ʱ��������CancelAlarm
��������ʱ��ID
˵����ɾ���Ѿ����ڵĶ�ʱ��
*/
void CancelAlarm(int TimerId)
{
	timeKillEvent(TimerId);
}
#endif