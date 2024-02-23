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

//�����¶ȡ�ʪ�ȱ���
u8 humH;	  //ʪ����������
u8 humL;	  //ʪ��С������
u8 temH;   //�¶���������
u8 temL;   //�¶�С������
float Light = 0;
extern char oledBuf[18];
u8 alarm = 0; //������־
u8 Led_Status = 0;  //��״̬
u8 Beep_Status = 0;  //BEEP״̬

char PubBuf[200];//�ϴ����ݵ�buf
const char *devSubTopic[] = {"/inGreenTopic/sub"};
const char devPubTopic[] = "/inGreenTopic/pub";
u8 ESP8266_INIT_OK = 0;//esp8266��ʼ����ɱ�־

 int main(void)
 {
  unsigned short timeCount = 0;	//���ͼ������
  unsigned char *dataPtr = NULL;
  Usart1_Init(115200);  
  DEBUG_LOG("\r\n");
  DEBUG_LOG("UART1��ʼ��			[OK]");
  Usart2_Init(115200);  //8266-01S����
  DEBUG_LOG("UART2��ʼ��			[OK]");     
	delay_init();	    	 //��ʱ������ʼ��
  DEBUG_LOG("��ʱ������ʼ��			[OK]");
  OLED_Init();
  OLED_ColorTurn(0);
  OLED_DisplayTurn(0);
  OLED_Clear();
  DEBUG_LOG("OLED1��ʼ��			[OK]");
  OLED_Refresh_Line("OLED");

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
  DEBUG_LOG("�ж����ȳ�ʼ��			[OK]");
  OLED_Refresh_Line("NVIC");
	LED_Init();//��ʼ����LED���ӵ�Ӳ���ӿ�
  DEBUG_LOG("LED��ʼ��			[OK]");
  OLED_Refresh_Line("LED");
  KEY_Init();//��ʼ���밴�����ӵ�Ӳ���ӿ�
  DEBUG_LOG("������ʼ��			[OK]");
  OLED_Refresh_Line("KEY");
  EXTIX_Init();//�ⲿ�жϳ�ʼ��
  DEBUG_LOG("�ⲿ�жϳ�ʼ��			[OK]");
  OLED_Refresh_Line("EXTI");
  BEEP_Init();//��������ʼ��
  DEBUG_LOG("��������ʼ��			[OK]");
  OLED_Refresh_Line("BEEP");  
  DHT11_Init();//��ʪ�ȴ�������ʼ��
  DEBUG_LOG("��ʪ�ȴ�������ʼ��			[OK]");
  OLED_Refresh_Line("DHT11");  
  BH1750_Init();//���նȴ�������ʼ��
  DEBUG_LOG("���նȴ�������ʼ��			[OK]");
  OLED_Refresh_Line("BH1750");
  delay_ms(500);
	DEBUG_LOG("��ʼ��ESP8266 WIFIģ��...");
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
  //OLED_Clear();   //��ʾ���ӳ�һ�����תЧ��

  
  while(OneNet_DevLink())			//����Onenet
    delay_ms(500); 
  OLED_Clear();
  
  TIM2_Int_Init(999,7199); //100ms,����Ķ�ֵ
  TIM3_Int_Init(1199,7199);
	
  BEEP = 0; //������ʾ����ɹ�
  delay_ms(100);
  BEEP = 1;
    
  
  OneNet_Subscribe(devSubTopic,1);
  
  
	while(1)
	{
      if(timeCount %10 == 0)									//���ݱ任���Ϊ0.2s
		{
      DHT11_Read_Data(&humH,&humL,&temH,&temL);//��ȡ��ʪ��   
      //UsartPrintf(USART_DEBUG,"ʪ��:%d.%d    �¶�:%d.%d",humH,humL,temH,temL);
      if(!i2c_CheckDevice(BH1750_Addr))
      {
        Light = LIght_Intensity();//��ȡ���ն�
        //UsartPrintf(USART_DEBUG,"���նȣ�%.1f lx\r\n",Light);
      }
      Led_Status = GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_4);//��ȡLED0��״̬
      Beep_Status = GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0);//��ȡBEEP��״̬
      
      if(humH >= 60||temH >= 30||Light >= 1000)
        alarm = 1;  //����
      else
        alarm = 0;
    
      DEBUG_LOG("ʪ�ȣ�%d.%d%%   �¶ȣ�%d.%d��C   ���նȣ�%.4fLux   LED1:%s   ��������%s",
      humH,humL,temH,temL,Light,Led_Status?"�ر�":"����",alarm?"����":"�ر�");
    }
    	if(++timeCount >= 75)									//���ͼ��3s,������������ϴ�����ʧ�ܣ���Ϊռ����ѷ�����
		{
      Led_Status = GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_4);//��ȡLED0��״̬
      DEBUG_LOG("-------------------------");
      DEBUG_LOG("�������ݣ�OneNet_Publish");
			UsartPrintf(USART_DEBUG, "OneNet_Publish\r\n");
      sprintf(PubBuf,"{\"Humid\":%d.%d,\"Temp\":%d.%d,\"Light\":%.4f,\"Led\":%d,\"Beep\":%d}",
      humH,humL,temH,temL,Light,Led_Status?0:1,Beep_Status?0:1);
      //��֧�����ģ������룬�Լ�JSONֻ���մ����ֵ����ݣ��ʵ�λ������ȥ
			OneNet_Publish(devPubTopic,PubBuf);   //��������2��������Ҫ��PubBuf������Ϊ����
      DEBUG_LOG("-------------------------");
			timeCount = 0;
			ESP8266_Clear();
		}
		
		dataPtr = ESP8266_GetIPD(3);  //�����ԣ�����Ϊ3������,��15ms
		if(dataPtr != NULL)
			OneNet_RevPro(dataPtr);
    delay_ms(10); //25=15+10ms
	}
 }




