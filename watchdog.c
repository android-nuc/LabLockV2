#ifndef __INC_GUARD_WATCHDOG
#define __INC_GUARD_WATCHDOG

#include <REG52.H>
sfr WDT_CONTR = 0xE1;
#define LED_PORT P2 //定义LED 控制端口为P2 口

void WatchDog_Init(int scale) 
{
    EX1 = 1; //允许外部中断1 中断
    IT1 = 0; //低电平触发
    EA = 1; //允许所有中断
    WDT_CONTR = scale; //使能看门狗，预分频64
}

void WatchDog_Enable() {
    WDT_CONTR |= 1 << 5;
}

void WatchDog_Feed() 
{
    WDT_CONTR = 0x37;
}

void WatchDog_EnableIdleState() 
{
    WDT_CONTR |= 1 << 3;
}

void WatchDog_QuitIdleState() 
{
    WDT_CONTR &= ~(1 << 3);
}


void EXT1IRQ(void) interrupt 2
{
    WatchDog_Feed(); //喂狗
}

#endif