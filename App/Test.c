////////////////////////////////////////
// Ӧ�ó��򣬵���OSEK_NM�ķ���
////////////////////////////////////////
#include "OsekNM.h"
#include "Driver_Common.h" //ƽ̨��ص�����
#include "OsekNMServer.h"
#include "Timer.h"
#include "usart.h"
#include "CAN.h"
#include "delay.h"
#define PRINT
int main()
{
	/*ƽ̨��ʼ��*/
	#ifdef STM32F407
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//����ϵͳ�ж����ȼ�����2
	uart_init(115200);	//��ʼ�����ڲ�����Ϊ115200
	delay_init(168);    //��ʼ����ʱ����
  /*�յ�������˸LED0*/
	//InitLed0();
	#ifdef PRINT
	printf("at main\n");
	#endif
	#endif
	StartNM();
}


