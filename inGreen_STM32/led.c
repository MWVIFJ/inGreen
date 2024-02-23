#include "led.h"
//LED0是控制LED1的灯是否闪亮
//LED1是控制PC13的灯是否闪亮

//初始化PA4和PC13为输出口.并使能这两个口的时钟		    
//LED IO初始化
void LED_Init(void)
{
 
 GPIO_InitTypeDef  GPIO_InitStructure;
 	
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOC, ENABLE);	 //使能PA,PC端口时钟
	
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;				 //LED0-->PA.4端口配置
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
 GPIO_Init(GPIOA, &GPIO_InitStructure);					 //根据设定参数初始化GPIOA.4
 GPIO_SetBits(GPIOA,GPIO_Pin_4);						 //PA.4输出高

 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;	    		 //LED1-->PC.13端口配置,推挽输出
 GPIO_Init(GPIOC, &GPIO_InitStructure);	  				 //推挽输出，IO口速度为50MHz
 GPIO_SetBits(GPIOC,GPIO_Pin_13); 						 //PC.13 输出高 
}
 
