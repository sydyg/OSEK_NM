#include "CAN.h"
#include "stm32f4xx.h"
/*��ʼ��CAN1ģ��*/
void STM32_CAN1_Init(void)
{
    CAN_InitTypeDef CAN_InitStructure;
	  NVIC_InitTypeDef NVIC_InitStructure;
	  GPIO_InitTypeDef GPIO_InitStructure;
	  
	  CAN_FilterInitTypeDef  CAN_FilterInitStructure;
  	/*1.ʹ��CAN1ʱ��*/
	  RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1,ENABLE);  ///ʹ��CAN1ʱ��
	 /*2.����GPIO*/
	 /*2.1 ʹ��GPIOʱ��*/
	  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);//ʹ��PORTAʱ��	
	 /*2.2 ѡ��GPIO����*/
	  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11| GPIO_Pin_12;
	 /*2.3 ����GPIOΪ����*/
	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//���ù���
	 /*2.4 ����GPIOΪ�������*/
	  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
	 /*2.5 ����GPIO����*/
	  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;//25MHz
	 /*2.6 ʹ������*/
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
    GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��PA11,PA12
	//���Ÿ���ӳ������                                                                                                                                                                                                                                                                                                                                                                                                                                                                        
	  GPIO_PinAFConfig(GPIOA,GPIO_PinSource11,GPIO_AF_CAN1); //GPIOA11����ΪCAN1
	  GPIO_PinAFConfig(GPIOA,GPIO_PinSource12,GPIO_AF_CAN1); //GPIOA12����ΪCAN1
	 /*3.����CAN������*/
	  CAN_InitStructure.CAN_TTCM=DISABLE;	//��ʱ�䴥��ͨ��ģʽ   
  	CAN_InitStructure.CAN_ABOM=ENABLE;	//Ӳ���Զ����߹���	  
  	CAN_InitStructure.CAN_AWUM=ENABLE;  //˯��ģʽͨ��Ӳ������(���CAN->MCR��SLEEPλ)
  	CAN_InitStructure.CAN_NART=ENABLE;	//��ֹ�����Զ����� 
  	CAN_InitStructure.CAN_RFLM=DISABLE;	//���Ĳ�����,�µĸ��Ǿɵ�  
  	CAN_InitStructure.CAN_TXFP=DISABLE;	//���ȼ��ɱ��ı�ʶ������ 
  	CAN_InitStructure.CAN_Mode= CAN_Mode_Normal;//CAN_Mode_LoopBack;//CAN_Mode_Normal;	 //ģʽ����-����ģʽ 
		/*��ʼ��Ϊ500K�Ĳ�����*/
  	CAN_InitStructure.CAN_SJW=CAN_SJW_1tq;	//����ͬ����Ծ���(Tsjw)Ϊtsjw+1��ʱ�䵥λ CAN_SJW_1tq~CAN_SJW_4tq
  	CAN_InitStructure.CAN_BS1=CAN_BS1_7tq;  //Tbs1��ΧCAN_BS1_1tq ~CAN_BS1_16tq
  	CAN_InitStructure.CAN_BS2=CAN_BS2_6tq;  //Tbs2��ΧCAN_BS2_1tq ~	CAN_BS2_8tq
  	CAN_InitStructure.CAN_Prescaler=6;      //��Ƶϵ��(Fdiv)Ϊbrp+1	
  	CAN_Init(CAN1, &CAN_InitStructure);     // ��ʼ��CAN1 
    
		//���ù�����
 	  CAN_FilterInitStructure.CAN_FilterNumber=0;	  //������0
  	CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdMask; 
  	CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_32bit; //32λ 
  	CAN_FilterInitStructure.CAN_FilterIdHigh=0x18FF; //32λID
  	CAN_FilterInitStructure.CAN_FilterIdLow=0xA000;
  	CAN_FilterInitStructure.CAN_FilterMaskIdHigh=0x0000;//32λMASK
  	CAN_FilterInitStructure.CAN_FilterMaskIdLow=0x0000;
   	CAN_FilterInitStructure.CAN_FilterFIFOAssignment=CAN_Filter_FIFO0;//������0������FIFO0
  	CAN_FilterInitStructure.CAN_FilterActivation=ENABLE; //���������0
  	CAN_FilterInit(&CAN_FilterInitStructure);//�˲�����ʼ��
	 /*4.ʹ���ж�*/
	  CAN_ITConfig(CAN1,CAN_IT_FMP0,ENABLE);//FIFO0��Ϣ�Һ��ж�����.		    
  	NVIC_InitStructure.NVIC_IRQChannel = CAN1_RX0_IRQn;
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;     // �����ȼ�Ϊ1
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;            // �����ȼ�Ϊ0
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  	NVIC_Init(&NVIC_InitStructure);
		
}
/*���ͱ��ĵ�����
* �ɹ������� 1
*/
int STM32_TX_CAN_Transmit(NMPDU_t* NMPDU)
{
  NMTypeU8_t mbox;  //���ķ���������
  int i=0;
  CanTxMsg TxMessage;
  //TxMessage.StdId=0x12;	 // ��׼��ʶ��Ϊ0
  TxMessage.ExtId = NMPDU->MsgID;	 // ������չ��ʾ����29λ��
  TxMessage.IDE = CAN_ID_EXT;		  // ʹ����չ��ʶ��
  TxMessage.RTR = 0;		  // ��Ϣ����Ϊ����֡��һ֡8λ
  TxMessage.DLC = OSEKNM_DLC;  
	TxMessage.Data[0] = NMPDU->MsgDA;
	TxMessage.Data[1] = NMPDU->MsgCtl;
  for(i=2;i<OSEKNM_DLC;i++)
    TxMessage.Data[i]=NMPDU->MsgData[i-2];				 // ��һ֡��Ϣ          
  mbox = CAN_Transmit(CAN1, &TxMessage);   
  i=0;
  while((CAN_TransmitStatus(CAN1, mbox)==CAN_TxStatus_Failed)&&(i<0XFFF))
		i++;	//�ȴ����ͽ���
  if(i >= 0XFFF)
		return 0;
  return 1;
}

//�жϷ�����			    
void CAN1_RX0_IRQHandler(void)
{
  CanRxMsg RxMessage;
	NMPDU_t NMPDU;
	int i=0;
	static unsigned int j = 0;
	j++;
  CAN_Receive(CAN1, 0, &RxMessage);
	/*��ʼ��NMPDU*/
	NMPDU.MsgCtl = RxMessage.Data[1];
	NMPDU.MsgDA = RxMessage.Data[0];
	NMPDU.MsgID = RxMessage.ExtId;
	for(i=2;i<8;i++)
	{
	    NMPDU.MsgData[i-2] = RxMessage.Data[i];
	}
	//�����ķ��뻺����
	Recv_EveryMessage(&NMPDU);
	/*LED0�ƹ���*/
	GPIO_WriteBit(GPIOE,GPIO_Pin_3,(j%2)?Bit_SET:Bit_RESET);//GPIOE3
	//GPIO_ResetBits(GPIOE,GPIO_Pin_3);
	//printf("NMID:%lx D0:%x D1:%x\n",NMPDU.MsgID,NMPDU.MsgDA,NMPDU.MsgCtl);
}
