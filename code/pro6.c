#include <reg51.h>

char hello[5]={0x89,0x86,0xc7,0xc7,0xc0};
char index=0;
char cnt=0;

int Timer0() interrupt 1
{
	TH0 = (65536-50000)/256;   //T0定时器复位
	TL0 = (65536-50000)%256;
	cnt++;
	if(cnt == 20){    //1s到达，数码管显一个数
		cnt = 0;      //计数复原
		P0 = hello[index];
		index++;      //数码管数字增加1
		if(index == 5){   //数码管数字跑完
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
	TR0 = 1;
	P0 = hello[index++];  //在1s进入中断功能前先赋值
	while(1);
}