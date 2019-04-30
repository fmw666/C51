#include <reg51.h>
sbit P1_0 = P1^0;
sbit P1_1 = P1^1;
char cnt = 0;    //记录中断次数，每40次为1s
char index = 0;  //控制LED灯亮2s，灭2s

int Timer0() interrupt 1
{
	TH0 = (65536-50000)/256;   //T0定时器复位
	TL0 = (65536-50000)%256;
	if(++cnt == 10){    //1s到达，数码管显一个数
		cnt = 0;      //计数复原
		index++;
		if(1<=index && index<=2){ //亮2s
			P1_1 = 0;
		}
		if(3<=index && index<=4){
			P1_1 = 1;
			if(index == 4){   //数码管数字跑完
				index = 0;    //数码管下标复位0
			}
		}
		
	}
}


void main()
{
	EA = EX0 = ET0 = 1;
	TMOD = 0x01;       //定时器方式控制寄存器 01方式16位
	TH0 = (65536-50000)/256;   //T0定时器每次中断50ms
	TL0 = (65536-50000)%256;
	TR0 = 1;
	while(1);
}