#include "led.h"
//LED0�ǿ���LED1�ĵ��Ƿ�����
//LED1�ǿ���PC13�ĵ��Ƿ�����

//��ʼ��PA4��PC13Ϊ�����.��ʹ���������ڵ�ʱ��		    
//LED IO��ʼ��
void LED_Init(void)
{
 
 GPIO_InitTypeDef  GPIO_InitStructure;
 	
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOC, ENABLE);	 //ʹ��PA,PC�˿�ʱ��
	
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;				 //LED0-->PA.4�˿�����
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
 GPIO_Init(GPIOA, &GPIO_InitStructure);					 //�����趨������ʼ��GPIOA.4
 GPIO_SetBits(GPIOA,GPIO_Pin_4);						 //PA.4�����

 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;	    		 //LED1-->PC.13�˿�����,�������
 GPIO_Init(GPIOC, &GPIO_InitStructure);	  				 //���������IO���ٶ�Ϊ50MHz
 GPIO_SetBits(GPIOC,GPIO_Pin_13); 						 //PC.13 ����� 
}
 
