#include <reg51.h>
sbit P1_0 = P1^0;
sbit P1_1 = P1^1;
char numbers[10]={0xC0,0xF9,0xA4,0xB0,0x99,0x92,0x82,0xF8,0x80,0x90};
char index = 0;
char cnt = 0;    //记录中断次数，每20次为1s

int Timer0() interrupt 1
{
	TH0 = (65536-50000)/256;   //T0定时器复位
	TL0 = (65536-50000)%256;
	cnt++;
	if(cnt == 20){    //1s到达，数码管显一个数
		cnt = 0;      //计数复原
		index++;      //数码管数字增加1
		if(index == 10){   //数码管数字跑完
			TR0 = 0;      //关闭定时器T0
			index = 0;    //数码管下标复位0
		}
	}
}


void main()
{
	EA = EX0 = ET0 = 1;
	TMOD = 0x01;       //定时器方式控制寄存器 01方式16位
	TH0 = (65536-50000)/256;   //T0定时器每次中断50ms
	TL0 = (65536-50000)%256;
	while(1){
		if(P1_0 == 0){     //k1按下
			TR0 = 1;       //开启T0计时器
			while(TR0){
				P0 = numbers[index];
			}
			P0 = 0xFF;
		}
		if(P1_1 == 0){     //k2按下
			TR0 = 1;       //开启T0计时器
			while(TR0){
				P0 = numbers[9-index];
			}
			P0 = 0xFF;
		}
	}
	
}