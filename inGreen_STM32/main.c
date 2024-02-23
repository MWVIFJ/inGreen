#include "led.h"
#include "key.h"
#include "beep.h"
#include "delay.h"
#include "sys.h"
#include "timer.h"
#include "usart.h"
#include "dht11.h"
#include "bh1750.h"
#include "oled.h"
#include "exti.h"
#include "stdio.h"
#include "esp8266.h"
#include "onenet.h"
#include "cJSON.h"

//定义温度、湿度变量
u8 humH;	  //湿度整数部分
u8 humL;	  //湿度小数部分
u8 temH;   //温度整数部分
u8 temL;   //温度小数部分
float Light = 0;
extern char oledBuf[18];
u8 alarm = 0; //报警标志
u8 Led_Status = 0;  //灯状态
u8 Beep_Status = 0;  //BEEP状态

char PubBuf[200];//上传数据的buf
const char *devSubTopic[] = {"/inGreenTopic/sub"};
const char devPubTopic[] = "/inGreenTopic/pub";
u8 ESP8266_INIT_OK = 0;//esp8266初始化完成标志

 int main(void)
 {
  unsigned short timeCount = 0;	//发送间隔变量
  unsigned char *dataPtr = NULL;
  Usart1_Init(115200);  
  DEBUG_LOG("\r\n");
  DEBUG_LOG("UART1初始化			[OK]");
  Usart2_Init(115200);  //8266-01S串口
  DEBUG_LOG("UART2初始化			[OK]");     
	delay_init();	    	 //延时函数初始化
  DEBUG_LOG("延时函数初始化			[OK]");
  OLED_Init();
  OLED_ColorTurn(0);
  OLED_DisplayTurn(0);
  OLED_Clear();
  DEBUG_LOG("OLED1初始化			[OK]");
  OLED_Refresh_Line("OLED");

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
  DEBUG_LOG("中断优先初始化			[OK]");
  OLED_Refresh_Line("NVIC");
	LED_Init();//初始化与LED连接的硬件接口
  DEBUG_LOG("LED初始化			[OK]");
  OLED_Refresh_Line("LED");
  KEY_Init();//初始化与按键连接的硬件接口
  DEBUG_LOG("按键初始化			[OK]");
  OLED_Refresh_Line("KEY");
  EXTIX_Init();//外部中断初始化
  DEBUG_LOG("外部中断初始化			[OK]");
  OLED_Refresh_Line("EXTI");
  BEEP_Init();//蜂鸣器初始化
  DEBUG_LOG("蜂鸣器初始化			[OK]");
  OLED_Refresh_Line("BEEP");  
  DHT11_Init();//温湿度传感器初始化
  DEBUG_LOG("温湿度传感器初始化			[OK]");
  OLED_Refresh_Line("DHT11");  
  BH1750_Init();//光照度传感器初始化
  DEBUG_LOG("光照度传感器初始化			[OK]");
  OLED_Refresh_Line("BH1750");
  delay_ms(500);
	DEBUG_LOG("初始化ESP8266 WIFI模块...");
	if(!ESP8266_INIT_OK){
		OLED_Clear();
		sprintf(oledBuf,"Waiting for");
		OLED_ShowString(16,0,(u8*)oledBuf,16,1);
		sprintf(oledBuf,"WiFi");
		OLED_ShowString(48,18,(u8*)oledBuf,16,1);
		sprintf(oledBuf,"Connection");
		OLED_ShowString(24,36,(u8*)oledBuf,16,1);
		OLED_Refresh();
	}
  ESP8266_Init();	
  OLED_Clear();
	sprintf(oledBuf,"Waiting for");
	OLED_ShowString(16,0,(u8*)oledBuf,16,1);
	sprintf(oledBuf,"MQTT Server");
	OLED_ShowString(16,18,(u8*)oledBuf,16,1);
	sprintf(oledBuf,"Connection");
	OLED_ShowString(24,36,(u8*)oledBuf,16,1);
	OLED_Refresh();	
  UsartPrintf(USART_DEBUG, " Hardware init OK\r\n");
  //OLED_Clear();   //显示更延迟一点的跳转效果

  
  while(OneNet_DevLink())			//接入Onenet
    delay_ms(500); 
  OLED_Clear();
  
  TIM2_Int_Init(999,7199); //100ms,后面的定值
  TIM3_Int_Init(1199,7199);
	
  BEEP = 0; //鸣叫提示接入成功
  delay_ms(100);
  BEEP = 1;
    
  
  OneNet_Subscribe(devSubTopic,1);
  
  
	while(1)
	{
      if(timeCount %10 == 0)									//数据变换间隔为0.2s
		{
      DHT11_Read_Data(&humH,&humL,&temH,&temL);//读取温湿度   
      //UsartPrintf(USART_DEBUG,"湿度:%d.%d    温度:%d.%d",humH,humL,temH,temL);
      if(!i2c_CheckDevice(BH1750_Addr))
      {
        Light = LIght_Intensity();//读取光照度
        //UsartPrintf(USART_DEBUG,"光照度：%.1f lx\r\n",Light);
      }
      Led_Status = GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_4);//读取LED0的状态
      Beep_Status = GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0);//读取BEEP的状态
      
      if(humH >= 60||temH >= 30||Light >= 1000)
        alarm = 1;  //警报
      else
        alarm = 0;
    
      DEBUG_LOG("湿度：%d.%d%%   温度：%d.%d°C   光照度：%.4fLux   LED1:%s   蜂鸣器：%s",
      humH,humL,temH,temL,Light,Led_Status?"关闭":"开启",alarm?"开启":"关闭");
    }
    	if(++timeCount >= 75)									//发送间隔3s,间隔多了容易上传数据失败，因为占用免费服务器
		{
      Led_Status = GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_4);//读取LED0的状态
      DEBUG_LOG("-------------------------");
      DEBUG_LOG("发布数据：OneNet_Publish");
			UsartPrintf(USART_DEBUG, "OneNet_Publish\r\n");
      sprintf(PubBuf,"{\"Humid\":%d.%d,\"Temp\":%d.%d,\"Light\":%.4f,\"Led\":%d,\"Beep\":%d}",
      humH,humL,temH,temL,Light,Led_Status?0:1,Beep_Status?0:1);
      //不支持中文，会乱码，以及JSON只接收纯数字的数据，故单位必须舍去
			OneNet_Publish(devPubTopic,PubBuf);   //参数仅有2个，所以要用PubBuf数组作为缓存
      DEBUG_LOG("-------------------------");
			timeCount = 0;
			ESP8266_Clear();
		}
		
		dataPtr = ESP8266_GetIPD(3);  //经测试，参数为3更合适,即15ms
		if(dataPtr != NULL)
			OneNet_RevPro(dataPtr);
    delay_ms(10); //25=15+10ms
	}
 }




