#include "timer.h"
#include "led.h"
#include "oled.h"
#include "beep.h"
#include "stdio.h"
  
char oledBuf[18];
extern u8 humH;	  //湿度整数部分
extern u8 humL;	  //湿度小数部分
extern u8 temH;   //温度整数部分
extern u8 temL;   //温度小数部分
extern float Light;
extern u8 alarm;

//通用定时器中断初始化
//这里时钟选择为APB1的2倍，而APB1为36M
//arr：自动重装值。
//psc：时钟预分频数
//这里使用的是定时器3!
void TIM3_Int_Init(u16 arr,u16 psc)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //时钟使能

	TIM_TimeBaseStructure.TIM_Period = arr;   //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	 计数到5000为500ms
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //设置用来作为TIMx时钟频率除数的预分频值  10Khz的计数频率  
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
 
	TIM_ITConfig(  //使能或者失能指定的TIM中断
		TIM3, //TIM2
		TIM_IT_Update,
		ENABLE  //使能
		);
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;  //TIM3中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //先占优先级0级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //从优先级3级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);  //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器

	TIM_Cmd(TIM3, ENABLE);  //使能TIMx外设
							 
}

void TIM3_IRQHandler(void)   //TIM3中断
{
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) //检查指定的TIM中断发生与否:TIM 中断源 
		{
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update  );  //清除TIMx的中断待处理位:TIM 中断源 
		//LED1=!LED1;//测试PC13的灯
    //BEEP=!BEEP; //测试蜂鸣器是否亮
      if(alarm)
      {
        //LED0=!LED0; 
        LED1=!LED1;   //测试用灯来闪
        BEEP=!BEEP;
        if(LED1 == 1)
          LED1 = 0; //防止奇数次滞留亮灯
      }
      else
      {
        //LED1 = 1;
        BEEP = 1; //BEEP是低电平触发的
        //LED0 = 0;
      }
		}
}

//TIM2 section
void TIM2_Int_Init(u16 arr,u16 psc)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); //时钟使能

	TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	 计数到5000为500ms
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //设置用来作为TIMx时钟频率除数的预分频值  10Khz的计数频率  
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
 
	TIM_ITConfig(  //使能或者失能指定的TIM中断
		TIM2, //TIM2
		TIM_IT_Update ,
		ENABLE  //使能
		);
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;  //TIM2中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //先占优先级0级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //从优先级3级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);  //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器

	TIM_Cmd(TIM2, ENABLE);  //使能TIMx外设
							 
}

void TIM2_IRQHandler(void)   //TIM2中断
{
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET) //检查指定的TIM中断发生与否:TIM 中断源 
		{
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update  );  //清除TIMx的中断待处理位:TIM 中断源 
		//LED0=!LED0;//测试LED1的灯
      sprintf(oledBuf,"inGreen");
    OLED_ShowString(40,0,(u8*)oledBuf,16,1);//8px 一个空格宽

    OLED_ShowChinese(0,16,21,16,1);
    OLED_ShowChinese(16,16,22,16,1);        
      sprintf(oledBuf,":%d.%d",humH,humL);
    OLED_ShowString(32,16,(u8*)oledBuf,16,1);//8*16 一个字母的像素    
      sprintf(oledBuf,"%%");
    OLED_ShowString(72,16,(u8*)oledBuf,16,1);//防止数字位数变动而%残留，Clear会导致频闪而解决不了
    OLED_ShowChinese(0,32,20,16,1);
    OLED_ShowChinese(16,32,22,16,1);  
      sprintf(oledBuf,":%d.%d",temH,temL);
    OLED_ShowString(32,32,(u8*)oledBuf,16,1);//8*16 一个字母的像素
      sprintf(oledBuf,"C");
    OLED_ShowString(72,32,(u8*)oledBuf,16,1);
    OLED_ShowChinese(0,48,23,16,1);
    OLED_ShowChinese(16,48,24,16,1);
      if(Light <1000)
      {
          sprintf(oledBuf,":%.4f",Light);
        OLED_ShowString(32,48,(u8*)oledBuf,16,1);//8*16 一个字母的像素
          sprintf(oledBuf," Lux");
        OLED_ShowString(96,48,(u8*)oledBuf,16,1);//五位数还有残余，故要先以空为代替  
        OLED_Refresh();
      }
      else
      {
          sprintf(oledBuf,":%.2f",Light);
        OLED_ShowString(32,48,(u8*)oledBuf,16,1);//8*16 一个字母的像素
          sprintf(oledBuf," Lux");
        OLED_ShowString(96,48,(u8*)oledBuf,16,1);//五位数还有残余，故要先以空为代替  
        OLED_Refresh();
      }
    } 
}








