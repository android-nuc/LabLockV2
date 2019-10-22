
#include<reg52.h>
#include<intrins.h>
#include "main.h"
#include "mfrc522.h"	
#include <string.h>

typedef unsigned int uint;
unsigned char idata ID[4];
unsigned char idata RevBuffer[30];  
unsigned char status;
unsigned char R_data;
unsigned char i = 0;
unsigned char heart = 0;
void Uart_SendByte(unsigned char Byte);
void lock();
void unlock();
void di(unsigned int x);
void verify();
void process();
void iccardcode();
void Uart_Init();



void lock(){
		LOCK_A = 1;
		LOCK_B = 0;
		delay_10ms(100);
		LOCK_A = 1;
		LOCK_B = 1;
}
void unlock(){
		LOCK_A = 0;
		LOCK_B = 1;
		delay_10ms(100);
		LOCK_A = 1;
		LOCK_B = 1;
		
}
void di(unsigned int x){  //蜂鸣器发声 x是时间
	unsigned int Count1 = 0,Count2=0,Count3=0,flag=0;
	x= x*100;
	while(1){
			Count1++;
			Count3++;
			if(Count1==100)
			{
				 Count2++;
				 if(Count2 ==4 )
				 {
					 Count1=0;
					 Count2=0;
					 flag=~flag;
				 }
			 }
			 if(!flag)
			 {
				 BEEP=~BEEP;  	 
			 }
			if (Count3 == x){
				BEEP = 1;
				return;
			}
	
	}
	 
}



void process(){
	if (R_data == 0xf2){
		heart = 0;
		return;
	}
	if (R_data == 0x01){
		unlock();
		delay_10ms(500);
		lock();
		return;
	}
	if (R_data == 0x02){
		di(100);
		return;
	}

}

void iccardcode()
{	     
		
		unsigned char tmp;
		switch (status){
			case 0:  // 寻卡
				LED_WAIT =0;
				tmp = PcdRequest(0x52,&RevBuffer);
				if (tmp == 0){
					status =1;
				}
			break;
			
			case 1:   //防冲撞获取卡id
				LED_FAIL =0; 
				tmp = PcdAnticoll(&ID);
				if (tmp == 0){
					status = 2;
				}else{
					status = 0;
				}
				break;
			case 2:   //验证卡
				verify();
				status =3;
				break;
			case 3:  //防止出错
				PcdAntennaOff(); 
				delay_10ms(1);
				PcdAntennaOn();
				status = 0;
			break;
			
			default:
				status =0;
			break;
		}
		HEART_TEST =~HEART_TEST;
		if (heart >= 100){
			di(100);
		}
		if (heart >= 40){
			Uart_SendByte(0xf2);
		}
		heart++;
}




/********************************************************************
初始化
***********************************************************************/
void Uart_Init(void)
{
     TMOD = 0x20;   //定时器工作在定时器1的方式2
     PCON = 0x00;   //不倍频
     SCON = 0x50;	//串口工作在方式1，并且启动串行接收	
     TH1 = 0xFD;    //设置波特率 9600
     TL1 = 0xFd;
     TR1 = 1;		//启动定时器1
		 ES = 1;		//开串口中断
		 EA = 1;		//开总中断		
	
		 PcdReset();   //读卡
     PcdAntennaOff(); 
     PcdAntennaOn();  
		 M500PcdConfigISOType( 'A' );
}
/********************************************************************
* 名称 : Uart_SendByte(uchar Byte)
* 功能 : 串口发送1字节数据
* 输入 : 无
* 输出 : 无
***********************************************************************/
void Uart_SendByte(unsigned char Byte)
{
	SBUF =	Byte;
	while(!TI)                   //如果发送完毕，硬件会置位TI
	{
		_nop_();	
	}	
}
/********************************************************************
* 名称 : Main()
* 功能 : 主函数
* 输入 : 无
* 输出 : 无
***********************************************************************/
void main()
{
	Uart_Init();
	HEART_TEST = 0;
	while(1)
	{
		LED_OK = 1;
		LED_WAIT = 1;
		LED_FAIL = 1;
		iccardcode();
	}		
}



/********************************************************************
* 名称 : Uart_Int()
* 功能 : 串口中断子函数
* 输入 : 无
* 输出 : 无
***********************************************************************/
void Uart_Int(void) interrupt 4
{
//	static uchar i = 7;    //定义为静态变量，当重新进入这个子函数时 i 的值不会发生改变
	EA = 0;
	if(RI == 1)   //当硬件接收到一个数据时，RI会置位
	{
		R_data= SBUF; 
		RI = 0;  
		process();
	}
	EA = 1;
}

	
void verify(){
	Uart_SendByte(0xfd);

	for(i = 0; i <4 ;i++){
		Uart_SendByte(ID[i]);
	}
}





