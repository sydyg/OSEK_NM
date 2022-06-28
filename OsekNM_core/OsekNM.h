/*
*OsekNM��״̬���ڲ��ӿڡ�ϵͳ���󡢸�������ڴ˶���
*/
#ifndef OSEKNMSTATE_H
#define OSEKNMSTATE_H

void NMStateManage(void);

typedef unsigned char NetIdType_t;
typedef unsigned char NMTypeU8_t;
typedef unsigned int NMTypeU16_t;
typedef unsigned long NMTypeU32_t;
typedef unsigned char NMStateType_t;
typedef unsigned char NMCountType_t;
typedef unsigned int  NMTimerType_t;
typedef unsigned char  NMTidType_t;
typedef unsigned long NMMsgID_t;
typedef unsigned char NMMsgDA_t;
typedef unsigned char NMMsgCtl_t;
typedef unsigned char NMMsgData_t;

//OsekNM�������ڲ�״̬��װ��OsekNM�ṹ��
/*typedef struct {
	NMNodeCfg_t NodeCfg;
	void (*NMInit)();
	void (*NMLimpHome)();
	void (*NMReset)();
	void (*NMNormal)();
	void (*NMPrepareSleep)();
	void (*NMTwbsNormal)();
	void (*NMBusSleep)();
	void (*NMPrepareBusSleep)();
	void (*NMWaitBusSleep)();
}OsekNM_t;*/

//�������ڵ���
#define MAXNODE 20;
//���ͽ��մ���ֵ
#define TXLIMIT 8
#define RXLIMIT 4
//���ĳ���
#define OSEKNM_DLC 8
//���屨������
#define NMMSGTYPE_ALIVE    (1<<0)
#define NMMSGTYPE_RING     (1<<1)
#define NMMSGTYPE_LIMPHOME (1<<2)
#define NMMSGTYPE_SI       (1<<4)
#define NMMSGTYPE_SA       (1<<5)
//�������ڵ���,һ�����εĽڵ�Ӧ�ò��ᳬ��20��
#define NM_PRESENTNUM   20
#define NM_LIMPHOMENUM  20

//��������״̬����
#define NM_OFF      0
#define NM_ON       1
#define NM_SHUTDOWN 2
#define NM_BUSSLEEP 3
#define NM_AWAKE    4
#define NM_INIT     5
#define NM_LIMPHOME 6
#define NM_NORMAL   7
#define NM_RESET    8
#define NM_TWBS_NORMAL        9
#define NM_TWBS_LIMPHOME      10
#define NM_NORMAL_PREPSLEEP   11
#define NM_LIMPHOME_PREPSLEEP 12
#define NM_INIT_RESET         13
#define NM_INIT_LIMPHOME      14
#define NM_INIT_NORMAL        15
#define NM_INIT_BUSSLEEP      17

//����ڵ������
typedef struct{
	char Self;          //�����ַ
	char LogicPre;      //�߼�ǰ�̽ڵ��ַ
	char LogicSuccessor;//�߼���̽ڵ��ַ

	struct NMPara{   //�������Ĳ���������API��ʼ���������������ļ����涨���
		NMTypeU32_t NMAddr;//NM���ĵ�ID
		NMTypeU16_t NMTtyp;
		NMTypeU16_t NMTMax;
		NMTypeU16_t NMTError;
		NMTypeU16_t NMTWBS;
	}nmpara;

	struct NetWorkStatus{  //����״̬
		char BusSleep;
		char ConfigurationStable;
		char NMActive;
	}networkstatus;   

	struct NMMarker{      //�Խڵ�����״̬�ı��
		char Stable;   //�ȶ�����
		char LimpHome;//�ڵ�ΪLimpHome
	}nmmarker;

	struct Config{       //LimpHome��Normal�����ã��ύ��Ӧ�ó���
		char Present[NM_PRESENTNUM];
		char LimpHome[NM_LIMPHOMENUM];
	}config;   
}NMNodeCfg_t;

//NM�������Ͷ���
typedef struct {
	NMMsgID_t MsgID; //����ID
	NMMsgDA_t MsgDA; //Ŀ�ĵ�ַ
	NMMsgCtl_t MsgCtl;//�����ֽ�
	NMMsgData_t MsgData[6];//������
}NMPDU_t;

//��ʱ���������ַ���ڵ��ַ�����ò���
typedef struct {
	NMTypeU32_t NMAddr;
	NMTypeU8_t NodeAddr;
	NMTypeU16_t TTYP;
	NMTypeU16_t TMAX;
	NMTypeU16_t TError;
	NMTypeU16_t TWBS;
}ConfPara_t;
#endif
