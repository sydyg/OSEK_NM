# OSEK_NM
基于OsekNM 2.5.3协议实现的OsekNM，跨平台的结构，目前只实现了STM32F407ZGT6平台的驱动。

# 1. 目录结构
1.1 App目录是一个keil5工程，App/keil/Objects/OsekNM.hex是一个可以直接在STM32F407ZGT6平台运行的hex文件。<br />
1.2 Driver目录包含了stm32f407子目录和Win7子目录，stm32f407目录下就是实现的STM32F407ZGT6平台的底层驱动，包括定时器和CAN模块的驱动，以及Stm32标准库的     东西；Win7子目录是在windows7平台下执行的一些文件；Driver_Common.c是一些公用的驱动，由OsekNM_core统一调用。<br />
1.3 OsekNM_core实现了OsekNM 2.5.3协议的核心逻辑，OsekNM.c实现了对各个节点各状态的处理，OsekNMServer.c实现了OsekNM 2.5.3协议提供给应用程序的API。<br />

# 2. 移植
移植过程在IND4平台有专门的讲解，大家可以去看视频教程，https://www.ind4.net/#/HomePage/CourseDetail?navType=Curriculum&courseId=2652

# 3. 附注
整个代码移植到我的开发板，并且验证通过。<br />
专门在Vspy平台实现了虚拟的网络管理节点。<br />
如果在移植过程遇到问题，可以付费获得移植指导。<br />
![无标题](https://github.com/sydyg/OSEK_NM/assets/24352068/c4e07dd3-8d54-403a-a609-8dd2ceefdc35)

微信：sydygys  <br />


