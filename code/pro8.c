#include <reg51.h>

char table[3]={0xfa,0xf9,0xfc};
char index=0;
char cnt=0;   //记录20次中断

int Timer0() interrupt 1
{
	TH0 = (65536-50000)/256;   //T0定时器复位
	TL0 = (65536-50000)%256;
	cnt++;
	if(cnt == 20){    //1s到达，数码管显一个数
		cnt = 0;      //计数复原
		index++;      //数码管数字增加1,1次1s
		if(index == 1 || index==2){
			P1 = table[1];
		}
		else if(index == 3 || index==4 || index==5){
			P1 = table[2];
		}
		else if(index == 6){  //数码管数字跑完
			index = 0;        //数码管下标复位0
			P1 = table[0];
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
	P1 = table[0];  //在1s进入中断功能前先赋值，index=0
	while(1);
}