
/*
* ʵ��OSEK_NM����״̬�µ��߼�
*/
//��ΪOSEKNM�ڱ���ʱ��Ӧ��ȷ�����ĸ�ƽ̨ʹ�ã������������ƽ̨��ص�ͷ�ļ�
#include "OsekNM.h"
#include "Driver_Common.h"

//DEBUG
#define OSEKNM_DEBUG
#ifdef OSEKNM_DEBUG
#define OSEKNM_PRINT(...) printf(__VA_ARGS__)
#else 
#define OSEKNM_PRINT(...)
#endif

//�ڵ㵱ǰ״̬
NMStateType_t NMCurrentState = NM_OFF;
//�ڵ㵱ǰ��״̬
NMStateType_t NMCurrentSubState = NM_OFF;
//�ڵ���һ��״̬
NMStateType_t  NMPreState = NM_OFF;
//���ʹ������
static NMCountType_t NMTXCount = 0;
//���ܴ������
static NMCountType_t NMRXCount = 0;

//��ʱ��ID
static NMTidType_t NMTidTTYP = 0;
static NMTidType_t NMTidTMAX = 0;
static NMTidType_t NMTidTError = 0;
static NMTidType_t NMTidTWBS = 0;
//GotoMode(xxx)���ñ�־����Ϊ��Ϣʹ��
NMTypeU8_t CalledGotoModeAwake = 0;
NMTypeU8_t CalledGotoModeBusSleep = 0;

//�����Ѿ����ù�
NMTypeU8_t ConfigedPara = 0;
//NMNodeCfg����
NMNodeCfg_t NodeCfg;
//��ʼ�����ò���
ConfPara_t ConfPara;

/*
*˵��������NMInit״̬������
* TXCount����;
* RXCount����;
*
*��������
*����ֵ:��
*/
static void NMInit()
{
	NMTypeU8_t i = 0;
	OSEKNM_PRINT("NMInit\n");
	/*�ڵ����ó�ʼ��*/
	NodeCfg.networkstatus.NMActive = 1;
	NodeCfg.Self = ConfigedPara ? ConfPara.NodeAddr : ADDR_SELF;
	NodeCfg.LogicSuccessor = ConfigedPara ? ConfPara.NodeAddr : ADDR_SELF;
	NodeCfg.LogicPre = ConfigedPara ? ConfPara.NodeAddr : ADDR_SELF;
	NodeCfg.networkstatus.BusSleep = 0;
	NodeCfg.nmpara.NMAddr = ConfigedPara ? ConfPara.NodeAddr : NMID;
	NodeCfg.nmpara.NMTtyp = ConfigedPara ? ConfPara.TTYP : NM_TIMETYPE_TTYP;
	NodeCfg.nmpara.NMTMax = ConfigedPara ? ConfPara.TMAX : NM_TIMETYPE_TMAX;
	NodeCfg.nmpara.NMTError = ConfigedPara ? ConfPara.TError : NM_TIMETYPE_TERROR;
	NodeCfg.nmpara.NMTWBS = ConfigedPara ? ConfPara.TWBS : NM_TIMETYPE_TWBS;
	//��ʼ��Ӳ��    
#ifdef OSEKOS
	D_Init();
#endif
	InitPlatform();
	/*��Ӧ�ó����ύLimpHome����*/
	for (i = 0; i < NM_LIMPHOMENUM; i++)
	{
		NodeCfg.config.LimpHome[i] = 0;
	}
	/*��Ӧ�ó����ύPresent����*/
	for (i = 0; i < NM_PRESENTNUM; i++)
	{
		NodeCfg.config.Present[i] = 0;
	}
	NMCurrentState = NM_RESET;
	NMCurrentSubState = NM_INIT_RESET;
	NMPreState = NM_INIT;
}


/*
*˵��������NMReset״̬������
*      1.����Alive����
*      2.TXCount++
*      3.RXCount++
*��������
*����ֵ:��
*/
static void NMReset()
{
	NMTypeU8_t i = 0;
	NMPDU_t NMMsgTx;
	NMTypeU8_t NMTxFlag = 0;//��Ϊ���ķ��ͱ�־
	OSEKNM_PRINT("NMReset\n");
	/*NMInitReset��״̬������*/
	if (NMCurrentSubState == NM_INIT_RESET)
	{    
       #ifdef OSEKOS
		//ʹ��Ӧ�ó���ͨ��
		D_Online();
       #endif
		NMTXCount = 0;
		NMRXCount = 0;
	}
	/*������NMReset������*/
	/*1.����ϵͳĬ������
	*�����߼���̺�present
	*/
	for (; i < NM_PRESENTNUM; i++)
	{
		NodeCfg.config.Present[i] = 0;
	}
	NodeCfg.LogicSuccessor = NodeCfg.Self;

	/*2.NMRXCount++*/
	NMRXCount++;

	/*3.��ʼ��NMPDU*/
	InitNMPDU(&NMMsgTx);
	//�ж��Ƿ������缤��״̬
	if (NodeCfg.networkstatus.NMActive){
		/*4.����Alive����,NMTXCOUNT++*/
		NMMsgTx.MsgID = NodeCfg.nmpara.NMAddr;
		NMMsgTx.MsgCtl |= NMMSGTYPE_ALIVE;
		NMMsgTx.MsgDA = NodeCfg.Self;
		TX_CAN_Transmit(&NMMsgTx);
	  NMTXCount++;
	}

	if ((NMTXCount <= TXLIMIT) && (NMRXCount <= RXLIMIT))
	{
		//����TTYP��ʱ��
		NMTidTTYP = SetAlarm(NM_TIMER_TTYP);
		//����NMNormal״̬
		NMCurrentState = NM_NORMAL;
		NMCurrentSubState = NM_NORMAL;
		NMPreState = NM_RESET;
	}
	//����LimpHome״̬
	else
	{
		NMTidTError = SetAlarm(NM_TIMER_TERROR);
		NMCurrentState = NM_LIMPHOME;
		NMCurrentSubState = NM_LIMPHOME;
		NMPreState = NM_RESET;
	}
}

/*
*˵��������NMLimpHome״̬������
*      ��TError����LimpHome����
*      ˯���������㣬����SI=1��LimpHome���ģ��л���PrepareSleep״̬
*      ˯�������������յ�SA=1�ı��ģ��л���NMTwbsNormal״̬
*      LimpHome���ķ��ͳɹ����յ������ڵ��NM���ģ�����NMReset״̬��
*��������
*����ֵ:��
*/
static void NMLimpHome()
{
	NMPDU_t NMMsgRecv;
	NMPDU_t NMMsgTx;
	NMTypeU8_t NMTxFlag = 0;//��Ϊ���ķ��ͱ�־
	OSEKNM_PRINT("NMLimpHome\n");
	while (1)
	{
		//TERROR��ʱ�¼�����
		if (GetTimerIsOut(NM_TIMER_TERROR))
		{
#ifdef OSEKOS
			D_Online();
#endif
			OSEKNM_PRINT("NMLimpHome\n");
			ClcTimerOutFlag(NM_TIMER_TERROR);//�峬ʱ��־
			//����˯������
			if (NodeCfg.networkstatus.BusSleep){
				NMTidTMAX = SetAlarm(NM_TIMER_TMAX);
				if (NodeCfg.networkstatus.NMActive)
				{
					/*3.����SI=1��LimpHome����*/
					InitNMPDU(&NMMsgTx);
					NMMsgTx.MsgCtl |= (NMMSGTYPE_LIMPHOME | NMMSGTYPE_SI);
					NMMsgTx.MsgDA = NodeCfg.Self;
					NMMsgTx.MsgID = NodeCfg.nmpara.NMAddr;
					NMTxFlag = TX_CAN_Transmit(&NMMsgTx);
				}
				NMCurrentState = NM_LIMPHOME_PREPSLEEP;
				NMCurrentSubState = NM_LIMPHOME_PREPSLEEP;
				NMPreState = NM_LIMPHOME;
				CancelAlarm(NM_TIMER_TERROR);
				break;
			}
			//������˯������
			else {
				NMTidTError = SetAlarm(NM_TIMER_TERROR);
				if (NodeCfg.networkstatus.NMActive)
				{
					/*3.����LimpHome����*/
					InitNMPDU(&NMMsgTx);
					NMMsgTx.MsgCtl |= NMMSGTYPE_LIMPHOME;
					NMMsgTx.MsgDA = NodeCfg.Self;
					NMMsgTx.MsgID =NodeCfg.nmpara.NMAddr;
					NMTxFlag = TX_CAN_Transmit(&NMMsgTx);
				}
				continue;
			}
		}
		//�����������NM����
		if (NMTxFlag)
		{
			NMTxFlag = 0;
			if (NMMsgTx.MsgCtl & NMMSGTYPE_LIMPHOME){
				NodeCfg.nmmarker.LimpHome = 1;
			}
			continue;
		}
		//�յ������NM����
		if (GetFromFIFO(&NMMsgRecv))
		{
			OSEKNM_PRINT("Id:%lx DA:%x\n", NMMsgRecv.MsgID, NMMsgRecv.MsgDA);
			if (((NodeCfg.networkstatus.NMActive) && !(NodeCfg.nmmarker.LimpHome) && (!(NMMsgRecv.MsgCtl & NMMSGTYPE_SA))) ||
				((NodeCfg.networkstatus.NMActive) && (NodeCfg.nmmarker.LimpHome) && (NodeCfg.networkstatus.BusSleep) && (!(NMMsgRecv.MsgCtl & NMMSGTYPE_SA))) ||
				(!(NodeCfg.networkstatus.NMActive) && (NodeCfg.networkstatus.BusSleep) && (!(NMMsgRecv.MsgCtl & NMMSGTYPE_SA))) ||
				(!(NodeCfg.networkstatus.NMActive) && !(NodeCfg.networkstatus.BusSleep))||
				((NodeCfg.networkstatus.NMActive) && (NodeCfg.nmmarker.LimpHome) && !(NodeCfg.networkstatus.BusSleep)))
			{
				CancelAlarm(NMTidTError);
				NodeCfg.nmmarker.LimpHome = 0;			
				NMCurrentState = NM_RESET;
				NMCurrentSubState = NM_INIT_RESET;
				NMPreState = NM_LIMPHOME;
				break;

			}
			else if ((!(NodeCfg.networkstatus.NMActive) && (NodeCfg.networkstatus.BusSleep) && (NMMsgRecv.MsgCtl & NMMSGTYPE_SA)) ||
				((NodeCfg.networkstatus.NMActive) && (NodeCfg.nmmarker.LimpHome) && (NodeCfg.networkstatus.BusSleep) && (NMMsgRecv.MsgCtl & NMMSGTYPE_SA)) ||
				((NodeCfg.networkstatus.NMActive) && !(NodeCfg.nmmarker.LimpHome) && (NMMsgRecv.MsgCtl & NMMSGTYPE_SA)))
			{
#ifdef OSEKOS
				D_Offline();
#endif
				CancelAlarm(NMTidTError);
				NMTidTWBS = SetAlarm(NM_TIMER_TWBS);
				NMCurrentState = NM_TWBS_LIMPHOME;
				NMCurrentSubState = NM_TWBS_LIMPHOME;
				NMPreState = NM_LIMPHOME;
				break;
			}
		}
	}
}

/*
*˵��������NMLimpHomePrepSleep״̬������
*��������
*����ֵ:��
*/
static void NMLimpHomePrepSleep()
{
	NMPDU_t NMMsgRecv;
	OSEKNM_PRINT("NMLimpHomePrepSleep\n");
	while (1)
	{
		//GotoMode(Awake)������
		if (CalledGotoModeAwake)
		{
			CalledGotoModeAwake = 0;
			NMCurrentState = NM_LIMPHOME;
			NMCurrentSubState = NM_LIMPHOME;
			NMPreState = NM_LIMPHOME_PREPSLEEP;
			NMTidTError = SetAlarm(NM_TIMER_TERROR);
			break;
		}
		//�յ������NM����
		if (GetFromFIFO(&NMMsgRecv)){
			//�յ��ı��ĵ�SI=0
			if (!(NMMsgRecv.MsgCtl & NMMSGTYPE_SI))
			{
				NMCurrentState = NM_LIMPHOME;
				NMCurrentSubState = NM_LIMPHOME;
				NMPreState = NM_LIMPHOME_PREPSLEEP;
				NMTidTError = SetAlarm(NM_TIMER_TERROR);
				break;
			}
			/*�յ�SA=1�ı���*/
			//�ο���SDLδר�Ŵ���
			else{
				continue;
			}
		}
		//TMAX��ʱ
		if (GetTimerIsOut(NM_TIMER_TMAX))
		{
			ClcTimerOutFlag(NM_TIMER_TMAX);
#ifdef OSEKOS
			D_Offline();
#endif
			CancelAlarm(NM_TIMER_TMAX);
			NMTidTWBS = SetAlarm(NM_TIMER_TWBS);
			NMCurrentState = NM_TWBS_LIMPHOME;
			NMCurrentSubState = NM_TWBS_LIMPHOME;
			NMPreState = NM_LIMPHOME;
			break;
		}
	}
}

/*
*˵��������NMTwbsLimpHome״̬������
*��������
*����ֵ:��
*/
static void NMTwbsLimpHome()
{
	NMPDU_t NMMsgRecv;
	OSEKNM_PRINT("NMTwbsLimpHome\n");
	while (1)
	{
		//GotoMode(Awake)������
		if (CalledGotoModeAwake)
		{
			CalledGotoModeAwake = 0;
			CancelAlarm(NMTidTWBS);
			NMTidTError = SetAlarm(NM_TIMER_TERROR);
			NMCurrentState = NM_LIMPHOME;
			NMCurrentSubState = NM_LIMPHOME;
			NMPreState = NM_TWBS_LIMPHOME;
			break;
		}
		//�յ������NM����
		if (GetFromFIFO(&NMMsgRecv)){
			//�յ��ı��ĵ�SI=0
			if (!(NMMsgRecv.MsgCtl & NMMSGTYPE_SI))
			{
				NMCurrentState = NM_LIMPHOME;
				NMCurrentSubState = NM_LIMPHOME;
				NMPreState = NM_TWBS_LIMPHOME;
				CancelAlarm(NMTidTWBS);
				NMTidTError = SetAlarm(NM_TIMER_TERROR);
				break;
			}
			else{
				continue;
			}
		}
		//TWBS��ʱ
		if (GetTimerIsOut(NM_TIMER_TWBS))
		{
			NMCurrentState = NM_BUSSLEEP;
			NMCurrentSubState = NM_BUSSLEEP;
			NMPreState = NM_TWBS_LIMPHOME;
			CancelAlarm(NMTidTWBS);
#ifdef OSEKOS
			D_Offline();
#endif
			break;
		}
	}
}

/*
*˵�������ݽڵ��ַ����LimpHome�е�ָ���Ľڵ㣬ͬһ���εĽڵ���������20
*
*������Node �ڵ��ַ
*����ֵ:�ڵ���LimpHome�е�λ�� -1:����
*/
static char FindLimpHomeNode(NMTypeU8_t Node)
{
	//��������LimpHome����
	NMTypeU8_t i;
	for (i = 0; (i < NM_PRESENTNUM); i++)
	{
		if (NodeCfg.config.LimpHome[i] == 0)
			return i;
		if (NodeCfg.config.LimpHome[i] == Node)
		{
			return i;
		}
	}
	return -1;
}

/*
*˵�������ݽڵ��ַ����Present�е�ָ���Ľڵ㣬ͬһ���εĽڵ���������20
*      
*������Node �ڵ��ַ
*����ֵ:�ڵ���Present�е�λ��,-1 ʧ��
*/
static char FindPresentNode(NMTypeU8_t Node)
{
	//��������Present����
	NMTypeU8_t i;
	for (i = 0; (i < NM_PRESENTNUM); i++)
	{
		//�ҵ���һ��0�ͷ���
		if (NodeCfg.config.Present[i] == 0)
			return i;
		//�ҵ��ýڵ��Ѿ�����
		if (NodeCfg.config.Present[i] == Node)
		{
			return i;
		}
	}
	return -1;
}
//�����߼����
static void EnsureLogicSuccessor(NMPDU_t* NMMsg)
{
	NMTypeU8_t SA = ((NMMsg->MsgID) & 0xff);
	NMTypeU8_t LA = NodeCfg.LogicSuccessor;
	NMTypeU8_t RA = NodeCfg.Self;
	if ((LA == RA) || ((SA < LA) && (LA < RA)) || ((LA < RA) && (RA < SA)) || ((RA < SA) && (SA < LA)))
	{
		NodeCfg.LogicSuccessor = SA;
	}
}

//�ж��Ƿ����� 1:����
static NMTypeU8_t IsSkiped(NMPDU_t* NMMsg)
{
	NMTypeU8_t DA = NMMsg->MsgDA;
	NMTypeU8_t SA = ((NMMsg->MsgID)&0xff);
	if (((DA < SA) && (SA < NodeCfg.Self)) || ((SA < NodeCfg.Self) && (NodeCfg.Self < DA)) || ((NodeCfg.Self < DA) && (DA < SA)))
	{
		return 1;
	}
	return 0;
}
/*
*˵��������NMNormal״̬��������
*      ���RXCount
*      �����յ���Alive���ĺ�Ring���ģ�������������
*      �����յ���LimpHome���ģ�������������
*      �ж��Ƿ�����������������Alive����
*������NMPDU
*����ֵ:��
*�˴���Ҫ���ã�������ϵ����
*/
static void NormalStandardNM(NMPDU_t* NMMsg,NMTypeU8_t* NMTxFlag)
{
	
}

/*
*˵��������NMNormal״̬������
*      ��ʼTTyp��ʱ��׼�����͵�һ��Ring����
*      �����յ���Alive���ĺ�Ring���ģ�������������
*      �����ͺͽ��ܳɹ��������TXCount��RXCount
*      �ж��Ƿ�����������������Alive����
*��������
*����ֵ:��
*�˴���Ҫ���ã�������ϵ����
*/
static void NMNormal()
{
	
}

/*
*˵��������NMNormalPrepSleep״̬������
*��������
*����ֵ:��
*˵����
*/
static void NMNormalPrepSleep()
{
	NMPDU_t NMMsgRecv;
	NMPDU_t NMMsgTx;
	NMTypeU8_t NMTxTTYPFlag = 0;
	NMTypeU8_t NMTxRecvFlag = 0;
	OSEKNM_PRINT("NMPrepareBusSleep\n");
	while (1)
	{
		//�յ���NM����
		if (GetFromFIFO(&NMMsgRecv))
		{
			NormalStandardNM(&NMMsgRecv, &NMTxRecvFlag);
			//SI=1
			if (NMMsgRecv.MsgCtl & NMMSGTYPE_SI)
			{
				if (NMMsgRecv.MsgCtl & NMMSGTYPE_SA)//ACK=1
				{
					//if (NodeCfg.networkstatus.BusSleep)
					{
						//NMInitWaitBusSleep
#ifdef OSEKOS
						D_Offline();
#endif
						CancelAlarm(NMTidTMAX);
						CancelAlarm(NMTidTTYP);
						NMTidTWBS = SetAlarm(NM_TIMER_TWBS);
						NMCurrentState = NM_TWBS_NORMAL;
						NMCurrentSubState = NM_TWBS_NORMAL;
						NMPreState = NM_NORMAL_PREPSLEEP;
						break;
					}
				}
				//ACK=0
				else{
					continue;
				}
			}
			//SI=0
			else {
				NMCurrentState = NM_NORMAL;
				NMCurrentSubState = NM_NORMAL;
				NMPreState = NM_NORMAL_PREPSLEEP;
				break;
			}
		}
		//TTYP��ʱ
		if (GetTimerIsOut(NM_TIMER_TTYP))
		{
			ClcTimerOutFlag(NM_TIMER_TTYP);
			if (NodeCfg.networkstatus.NMActive)
			{
				InitNMPDU(&NMMsgTx);
				NMMsgTx.MsgCtl |= ((NMMSGTYPE_SA) | (NMMSGTYPE_RING) | (NMMSGTYPE_SI));
				NMMsgTx.MsgDA = NodeCfg.LogicSuccessor;
				NMMsgTx.MsgID = NodeCfg.nmpara.NMAddr;
				NMTxTTYPFlag = TX_CAN_Transmit(&NMMsgTx);	

			}
			continue;
		}
		//TMAX��ʱ
		if (GetTimerIsOut(NM_TIMER_TMAX))
		{
			ClcTimerOutFlag(NM_TIMER_TMAX);
			//����NMInitReset״̬
			NMCurrentState = NM_RESET;
			NMCurrentSubState = NM_INIT_RESET;
			NMPreState = NM_NORMAL_PREPSLEEP;
			break;
		}
		//GotoMode(Awake)������
		if (CalledGotoModeAwake)
		{
			CalledGotoModeAwake = 0;
			NMCurrentState = NM_NORMAL;
			NMCurrentSubState = NM_NORMAL;
			NMPreState = NM_NORMAL_PREPSLEEP;
			break;
		}
		//������NM����
	    if (NMTxTTYPFlag || NMTxRecvFlag)
		{
			NMTxTTYPFlag = 0;
			NMTxRecvFlag = 0;
			//NMInitWaitBusSleep
#ifdef OSEKOS
			D_Offline();
#endif
			CancelAlarm(NMTidTMAX);
			CancelAlarm(NMTidTTYP);
			NMTidTWBS = SetAlarm(NM_TIMER_TWBS);
			NMCurrentState = NM_TWBS_NORMAL;
			NMCurrentSubState = NM_TWBS_NORMAL;
			NMPreState = NM_NORMAL_PREPSLEEP;
			break;
		}
	}
	
}

/*
*˵��������NMTwbsNormal״̬������
*��������
*����ֵ:��
*/
static void NMTwbsNormal()
{
	NMPDU_t NMMsgRecv;
	NMTypeU8_t NMTxFlag = 0;
	OSEKNM_PRINT("NMTwbsNormal\n");
	while (1)
	{
		//TWBS��ʱ
		if (GetTimerIsOut(NM_TIMER_TWBS))
		{
#ifdef OSEKOS
			D_Init();
#endif
			NMCurrentState = NM_BUSSLEEP;
			NMCurrentSubState = NM_BUSSLEEP;
			NMPreState = NM_TWBS_NORMAL;
			break;
		}
		//�յ�NM����
		if (GetFromFIFO(&NMMsgRecv))
		{
			//SI = 1
			if (NMMsgRecv.MsgCtl & NMMSGTYPE_SI)
			{
				continue;
			}
			//SI = 0
			else{
				CancelAlarm(NMTidTWBS);
				NMCurrentState = NM_RESET;
				NMCurrentSubState = NM_INIT_RESET;
				NMPreState = NM_TWBS_NORMAL;
				break;
			}
		}
		//GotoMode(Awake)����
		if (CalledGotoModeAwake)
		{
			CancelAlarm(NMTidTWBS);
			NMCurrentState = NM_RESET;
			NMCurrentSubState = NM_INIT_RESET;
			NMPreState = NM_TWBS_NORMAL;
			break;
		}
	}
}


/*
*˵��������NMBusSleep״̬������
*��������
*����ֵ:��
*/
static void NMBusSleep()
{
	NMPDU_t NMMsg;
	OSEKNM_PRINT("NMBusSleep\n");
	while (1)
	{
		//GotoMode(Awake)������
		if (CalledGotoModeAwake)
		{
			CalledGotoModeAwake = 0;
			NMCurrentState = NM_INIT;
			NMCurrentSubState = NM_INIT;
			NMPreState = NM_BUSSLEEP;

			break;
		}
		//�յ����߻����źţ�SI = 0
		if (GetFromFIFO(&NMMsg)){
			//�յ��ı��ĵ�SI=0
			if (!(NMMsg.MsgCtl & NMMSGTYPE_SI))
			{
				NMCurrentState = NM_INIT;
				NMCurrentSubState = NM_INIT;
				NMPreState = NM_BUSSLEEP;
				break;
			}
			else{
				continue;
			}
		}
	}
}
//������״̬��ת�����й���
void NMStateManage()
{
	while (1)
	{
		switch (NMCurrentState){
		case NM_INIT:
			NMInit();
			break;
		case NM_RESET:
			NMReset();
			break;
		case NM_NORMAL:
			NMNormal();
			break;
		case NM_LIMPHOME:
			NMLimpHome();
			break;
		case NM_NORMAL_PREPSLEEP:
			NMNormalPrepSleep();
			break;
		case NM_LIMPHOME_PREPSLEEP:
			NMLimpHomePrepSleep();
			break;
		case NM_TWBS_NORMAL:
			NMTwbsNormal();
			break;
		case NM_TWBS_LIMPHOME:
			NMTwbsLimpHome();
			break;
		case NM_BUSSLEEP:
			NMBusSleep();
			break;
		}		  
	}
}



