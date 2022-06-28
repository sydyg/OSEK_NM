#include "Driver_Common.h"

//��ƽ̨��صĹ��õ���������������ʵ�֣�����NM���Ļ���������ʱ��

//��ʱ������
static char SetAlarm_TTYP = 0;
static char SetAlarm_TMAX = 0;
static char SetAlarm_TERROR = 0;
static char SetAlarm_TWBS = 0;
//��ʱ��������
static int TTYP_Count = 0;
static int TMAX_Count = 0;
static int TERROR_Count = 0;
static int TWBS_Count = 0;
//���Ļ���������
static RecvFIFO_t RecvFIFO;

//��ʱ����ʱ��־
static TimerOutFlag_t TimerOutFlag_TTYP = 0;
static TimerOutFlag_t TimerOutFlag_TMAX = 0;
static TimerOutFlag_t TimerOutFlag_TERROR = 0;
static TimerOutFlag_t TimerOutFlag_TWBS = 0;

//�ڵ㵱ǰ״̬
extern NMStateType_t NMCurrentState;
//�ڵ㵱ǰ��״̬
extern NMStateType_t NMCurrentSubState;
//�ڵ���һ��״̬
extern NMStateType_t  NMPreState;
extern NMNodeCfg_t NodeCfg;
//DEBUG
//#define DRVCOM_DEBUG
#ifdef DRVCOM_DEBUG
#define DRVCOM_PRINT(...) printf(__VA_ARGS__)
#else 
#define DRVCOM_PRINT(...)
#endif

/*��������TX_CAN_Transmit
*������NMPDU
*����ֵ���ɹ� 1
*˵��������ƽ̨��صı��ķ��ͺ���
*/
NMTypeU8_t TX_CAN_Transmit(NMPDU_t* NMPDU)
{
	/*���ͱ��ĵ�����*/
	#ifdef STM32F407
	return STM32_TX_CAN_Transmit(NMPDU);
	#endif
}

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

//���ض�ʱ���Ƿ�ʱ,-1 ʧ��
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
* ����ֵ��1:�ɹ����뱨�ĵ�FIFO��0������ʧ��
*/
char SetToFIFO(NMPDU_t* msg)
{
	if (RecvFIFO.FullFlag == 1)//���жϻ���������
		return 0;
	/*���뱨�ĵ�������*/
	RecvFIFO.MSGs[RecvFIFO.Tail% FIFOMAX] = *msg;
	RecvFIFO.Tail = (RecvFIFO.Tail + 1) % FIFOMAX;
	/*����ձ�ʶ*/
	RecvFIFO.EmptyFlag = 0;
	if ((RecvFIFO.Tail + 1) == RecvFIFO.Head)//��������
		RecvFIFO.FullFlag = 1;
	return 1;
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
	for (; i < OSEKNM_DLC; i++)
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

//��ʱ����ʱ����
/*LimpHome��ʱ��*/
static void TimerOutTERROR()
{
	TimerOutFlag_TERROR = 1;
#ifdef PRINT_LOG
	char buf[100];
	sprintf(buf, "TError out State:%d\n", NMCurrentState);
	LogOutPut(buf);
#endif
}

/*TMax��ʱ��ʱ��*/
static void TimerOutTMAX()
{
	TimerOutFlag_TMAX = 1;
#ifdef PRINT_LOG
	char buf[100];
	sprintf(buf, "TMAX out State:%d\n", NMCurrentState);
	LogOutPut(buf);
#endif
}

/*TTYP��ʱ��*/
static void TimerOutTTYP()
{
	TimerOutFlag_TTYP = 1;
#ifdef PRINT_LOG
	char buf[100];
	sprintf(buf, "TTYP out State:%d\n", NMCurrentState);
	LogOutPut(buf);
#endif
}
/*WaitBusSleep��ʱ��*/
static void TimerOutTWBS()
{
	TimerOutFlag_TWBS = 1;
#ifdef PRINT_LOG
	char buf[100];
	sprintf(buf, "TWBS out State:%d\n", NMCurrentState);
	LogOutPut(buf);
#endif
}

//10ms��ʱ��
void Timer10()
{
	/*����SetAlarm(xx)����ʱ����ʼ����*/
	if (SetAlarm_TTYP)
	{
		TTYP_Count++;

		if (TTYP_Count >= 10)//TTYP=100ms
		{
			TTYP_Count = 0;//���¼���
			TimerOutTTYP();
			SetAlarm_TTYP = 0;//ÿ�����궨ʱ��������ر�,�Ӷ��򻯶�ʱ������
		}
	}
	else {
		TTYP_Count = 0;
	}
	if (SetAlarm_TMAX)
	{
		TMAX_Count++;
		if (TMAX_Count >= 26)//TMAX=260ms
		{
			TMAX_Count = 0;//���¼���
			TimerOutTMAX();
			SetAlarm_TMAX = 0;//ÿ�����궨ʱ��������ر�
		}
	}
	else {
		TMAX_Count = 0;
	}
	if (SetAlarm_TERROR)
	{
		TERROR_Count++;
		if (TERROR_Count >= 100)//TError=1000ms
		{
			TERROR_Count = 0;//���¼���
			TimerOutTERROR();
			SetAlarm_TERROR = 0;//ÿ�����궨ʱ��������ر�
		}
	}
	else {
		TERROR_Count = 0;
	}
	if (SetAlarm_TWBS)
	{
		TWBS_Count++;
		if (TWBS_Count >= 500)//TError=5000ms
		{
			TWBS_Count = 0;//���¼���
			TimerOutTWBS();
			SetAlarm_TWBS = 0;//ÿ�����궨ʱ��������ر�
		}
	}
	else {
		TWBS_Count = 0;
	}
}

//ƽ̨��صĳ�ʼ��
void InitPlatform()
{
	/*��������ʼ��*/
	RecvFIFO.GetMsg = GetFromFIFO;
	RecvFIFO.SetMsg = SetToFIFO;
	RecvFIFO.ClearBuff = ClearFIFO;
	RecvFIFO.ClearBuff();
	/*1.STM32��صĳ�ʼ��*/
	#ifdef STM32F407
	/*��ʱ����ʼ��*/
	Stm32Timer3Init();//10ms�ж�һ��
	/*CANģ���ʼ��*/
	STM32_CAN1_Init();
	#endif
}


/*
*�Զ��嶨ʱ��������SetAlarm
��������ʱ������
˵����ÿ�ε��ö�ʹ��ʱ�����´�0��ʼ
*����ֵ����ʱ��ID���ö�ʱ������ID���涨ʱ��ID
*/
int SetAlarm(NMTimerType_t timer)
{
	int Tid = 0;
	switch (timer)
	{
	case NM_TIMER_TTYP:
		SetAlarm_TTYP = 1;
		TTYP_Count = 0;
		TimerOutFlag_TTYP = 0;//ÿ�����ö�ʱ��ǰ�������־λ
		Tid = NM_TIMER_TTYP;
		break;
	case NM_TIMER_TMAX:
		TMAX_Count = 0;
		SetAlarm_TMAX = 1;
		TimerOutFlag_TMAX = 0;//ÿ�����ö�ʱ��ǰ�������־λ
		Tid = NM_TIMER_TMAX;
		break;
	case NM_TIMER_TERROR:
		TERROR_Count = 0;
		SetAlarm_TERROR = 1;
		TimerOutFlag_TERROR = 0;//ÿ�����ö�ʱ��ǰ�������־λ
		Tid = NM_TIMER_TERROR;
		break;
	case NM_TIMER_TWBS:
		TWBS_Count = 0;
		SetAlarm_TWBS = 1;
		TimerOutFlag_TWBS = 0;//ÿ�����ö�ʱ��ǰ�������־λ
		Tid = NM_TIMER_TWBS;
		break;
	}
	return Tid;
}
/*
*�Զ��嶨ʱ��������CancelAlarm
��������ʱ������
˵������ʱ����0�����ټ���
*/
void CancelAlarm(NMTimerType_t timer)
{
	switch (timer)
	{
	case NM_TIMER_TTYP:
		TTYP_Count = 0;
		SetAlarm_TTYP = 0;
		TimerOutFlag_TTYP = 0;//ÿ�ιرն�ʱ���������־λ
		break;
	case NM_TIMER_TMAX:
		TMAX_Count = 0;
		SetAlarm_TMAX = 0;
		TimerOutFlag_TMAX = 0;//ÿ�ιرն�ʱ���������־λ
		break;
	case NM_TIMER_TERROR:
		TERROR_Count = 0;
		SetAlarm_TERROR = 0;
		TimerOutFlag_TERROR = 0;//ÿ�ιرն�ʱ���������־λ
		break;
	case NM_TIMER_TWBS:
		TWBS_Count = 0;
		SetAlarm_TWBS = 0;
		TimerOutFlag_TWBS = 0;//ÿ�ιرն�ʱ���������־λ
		break;
	}
}
/*CAN�ж��յ��ı���*/
void Recv_EveryMessage(NMPDU_t* p_Msg)
{
	/*�������յ���NM���ķ��뻺����FIFO*/
	if (((p_Msg->MsgID) != NMID) && ((p_Msg->MsgID>>8) == (NMID>>8)))//�������籨�ģ��������Լ�����ȥ��
	{
		RecvFIFO.SetMsg(p_Msg);//��ʱ��������ֵ
	}
}

