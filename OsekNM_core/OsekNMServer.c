/*
*ʵ��OSEK_NM�淶�ķ��񣬸÷����ṩ���ϲ����
*/
#include "OsekNMServer.h"
#include "OsekNM.h"

//�ڵ㵱ǰ״̬
extern NMStateType_t NMCurrentState;
extern NMStateType_t NMCurrentSubState;
//�ڵ���һ��״̬
extern NMStateType_t  NMPreState;

extern NMTypeU8_t CalledGotoModeAwake;
extern NMTypeU8_t CalledGotoModeBusSleep;
extern NMNodeCfg_t NodeCfg;
extern ConfPara_t ConfPara;
extern NMTypeU8_t ConfigedPara;
extern int TidTimer;
/*��������StartNM
*������ͨ������ĵ�ַ
*����ֵ��E_OK �޴�
*˵�������������������״̬��NMOFFת�Ƶ�NMON
*/
/*��������StartNM
*������ͨ������ĵ�ַ
*����ֵ��E_OK �޴�
*˵�������������������״̬��NMOFFת�Ƶ�NMON
*/
StatusType_t StartNM()
{
	/*NM״̬��NMOFF��NMON*/
	NMCurrentState = NM_INIT;
	NMPreState = NM_OFF;
	NMStateManage();
	return E_OK;
}

/*��������StopNM
*������ͨ������ĵ�ַ
*����ֵ��E_OK �޴�
*˵�����ر������������״̬��NMONת�Ƶ�NMShutdown��������NMShutdown�������ת�Ƶ�NMOFF
*/
StatusType_t StopNM()
{
	/*NM״̬��NMON��NMOFF*/
	NMCurrentState = NM_SHUTDOWN;
	NMPreState = NM_INIT;
	//�ر���Դ
	return E_OK;
}

/*��������GotoMode
*������Netid��ͨ������ĵ�ַ��NewMode��Ҫ���õ�ģʽ��ֻ��BusSleep��Awake
*����ֵ��E_OK �޴�
*˵�������ýڵ��networkstatus.BusSleep
*/
StatusType_t GotoMode(NMModeName_t NewMode)
{
	switch (NewMode)
	{
	case Awake:
		NodeCfg.networkstatus.BusSleep = 0;
		CalledGotoModeAwake = 1;
		break;
	case Bussleep:
		NodeCfg.networkstatus.BusSleep = 1;
		CalledGotoModeBusSleep = 1;
		break;
	}
	return E_OK;
}

/*��������InitDirectNMParams
*������Netid��ͨ������ĵ�ַ��NodeId���ڵ��ַ��TimerTyp������Ring���ļ�ĵ���ʱ������TimerMax������Ring���ļ�����ʱ������
       TimerError������LimpHome���ļ��ʱ������TimerWBS������BusSleep�ĵȴ�ʱ�䣬TimerTx�����´���������ӳ�
*����ֵ��E_OK �޴�
*˵�����ر������������״̬��NMONת�Ƶ�NMShutdown��������NMShutdown�������ת�Ƶ�NMOFF
*/
StatusType_t InitDirectNMParams(NMTypeU32_t Netid, NMTypeU8_t NodeId, NMTypeU16_t TimerTyp,
	NMTypeU16_t TimerMax, NMTypeU16_t TimerError, NMTypeU16_t TimerWBS, NMTypeU16_t TimerTx)
{
	/*NM״̬��NMOFF��NMON*/
	ConfPara.NMAddr = Netid;
	ConfPara.NodeAddr = NodeId;
	ConfPara.TError = TimerError;
	ConfPara.TMAX = TimerMax;
	ConfPara.TTYP = TimerTyp;
	ConfPara.TWBS = TimerWBS;
	ConfigedPara = 1;
	return E_OK;
}

/*��������SilentNM
*������Netid��ͨ������ĵ�ַ
*����ֵ��E_OK �޴�
*˵������ֹ����ͨ�ţ�����״̬��NMActiveת�Ƶ�NMPassive
*/
StatusType_t SilentNM()
{
	/*NM״̬��NMOFF��NMON*/
	NodeCfg.networkstatus.NMActive = 0;
	return E_OK;
}

/*��������TalkNM
*������Netid��ͨ������ĵ�ַ
*����ֵ��E_OK �޴�
*˵����ʹ������ͨ�ţ�����״̬��NMPassiveת�Ƶ�NMActive
*/
StatusType_t TalkNM()
{
	/*NM״̬��NMOFF��NMON*/
	NodeCfg.networkstatus.NMActive = 1;
	return E_OK;
}
