////////////////////////////////////////
// 应用程序，调用OSEK_NM的服务
////////////////////////////////////////
#include "OsekNM.h"
#include "Driver_Common.h" //平台相关的配置
#include "OsekNMServer.h"
#include "Timer.h"
#include "usart.h"
#include "CAN.h"
#include "delay.h"
<<<<<<< HEAD
=======
#include "led.h"
>>>>>>> b223e6d9d90ad0f6d082b671cff5374d07f338be
#define PRINT
int main()
{
	/*平台初始化*/
	#ifdef STM32F407
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组2
	uart_init(115200);	//初始化串口波特率为115200
	delay_init(168);    //初始化延时函数
  /*收到报文闪烁LED0*/
<<<<<<< HEAD
	//InitLed0();
=======
	InitLed0();
>>>>>>> b223e6d9d90ad0f6d082b671cff5374d07f338be
	#ifdef PRINT
	printf("at main\n");
	#endif
	#endif
	StartNM();
}


