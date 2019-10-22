
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
void di(unsigned int x){  //���������� x��ʱ��
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
			case 0:  // Ѱ��
				LED_WAIT =0;
				tmp = PcdRequest(0x52,&RevBuffer);
				if (tmp == 0){
					status =1;
				}
			break;
			
			case 1:   //����ײ��ȡ��id
				LED_FAIL =0; 
				tmp = PcdAnticoll(&ID);
				if (tmp == 0){
					status = 2;
				}else{
					status = 0;
				}
				break;
			case 2:   //��֤��
				verify();
				status =3;
				break;
			case 3:  //��ֹ����
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
��ʼ��
***********************************************************************/
void Uart_Init(void)
{
     TMOD = 0x20;   //��ʱ�������ڶ�ʱ��1�ķ�ʽ2
     PCON = 0x00;   //����Ƶ
     SCON = 0x50;	//���ڹ����ڷ�ʽ1�������������н���	
     TH1 = 0xFD;    //���ò����� 9600
     TL1 = 0xFd;
     TR1 = 1;		//������ʱ��1
		 ES = 1;		//�������ж�
		 EA = 1;		//�����ж�		
	
		 PcdReset();   //����
     PcdAntennaOff(); 
     PcdAntennaOn();  
		 M500PcdConfigISOType( 'A' );
}
/********************************************************************
* ���� : Uart_SendByte(uchar Byte)
* ���� : ���ڷ���1�ֽ�����
* ���� : ��
* ��� : ��
***********************************************************************/
void Uart_SendByte(unsigned char Byte)
{
	SBUF =	Byte;
	while(!TI)                   //���������ϣ�Ӳ������λTI
	{
		_nop_();	
	}	
}
/********************************************************************
* ���� : Main()
* ���� : ������
* ���� : ��
* ��� : ��
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
* ���� : Uart_Int()
* ���� : �����ж��Ӻ���
* ���� : ��
* ��� : ��
***********************************************************************/
void Uart_Int(void) interrupt 4
{
//	static uchar i = 7;    //����Ϊ��̬�����������½�������Ӻ���ʱ i ��ֵ���ᷢ���ı�
	EA = 0;
	if(RI == 1)   //��Ӳ�����յ�һ������ʱ��RI����λ
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





