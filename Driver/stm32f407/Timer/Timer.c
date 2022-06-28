
#include "Timer.h"
extern void Timer10(void);
/*����STM32�Ķ�ʱ��������ʵ��OSEKNM��Ҫ�Ķ�ʱ��*/
//ͨ�ö�ʱ��3�жϳ�ʼ��
//arr���Զ���װֵ��
//psc��ʱ��Ԥ��Ƶ��
//��ʱ�����ʱ����㷽��:Tout=((arr+1)*(psc+1))/Ft us.
//Ft=��ʱ������Ƶ��,��λ:Mhz��84MHZ��
//����ʹ�õ��Ƕ�ʱ��3,10ms�ж�һ��!

void Stm32Timer3Init()
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	/*1.ʱ��ʹ��*/
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);  ///ʹ��TIM3ʱ��
	
	/*2.timer�Ĵ�������*/
	/*10ms�ж�һ��*/
	TIM_TimeBaseInitStructure.TIM_Period = (1000-1); 	//�Զ���װ��ֵ
	TIM_TimeBaseInitStructure.TIM_Prescaler=(840-1);  //��ʱ����Ƶ100KHZ,ͨ�ö�ʱ����ʱ����APB1���߱�Ƶ����(2��)
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //���ϼ���ģʽ
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseInitStructure);//��ʼ��TIM3
	/*4.�ж�����*/
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE); //����ʱ��3�����ж�
	TIM_Cmd(TIM3,ENABLE); //ʹ�ܶ�ʱ��3
	
	NVIC_InitStructure.NVIC_IRQChannel=TIM3_IRQn; //��ʱ��3�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x01; //��ռ���ȼ�1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x03; //�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

void Stm32Timer3ShutDown()
{
/*1.ʱ�ӹر�*/
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,DISABLE);  ///�ر�TIM3ʱ��
}

//��ʱ��3�жϷ�����
void TIM3_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM3,TIM_IT_Update)==SET) //����ж�
	{
		Timer10();
	}
	TIM_ClearITPendingBit(TIM3,TIM_IT_Update);  //����жϱ�־λ
}
