#include <reg51.h> 

char table[8]={0xFE,0xFD,0xFB,0xF7,0xEF,0xDF,0xBF,0x7F};
char index = 0;
char cnt = 0;    //记录中断次数，每20次为1s

int Timer0() interrupt 1
{
	TH0 = (65536-50000)/256;   //T0定时器复位
	TL0 = (65536-50000)%256;
	cnt++;
	if(cnt == 10){    //0.5s到达
		cnt = 0;      //计数复原
		index++;      //数码管数字增加1
		switch(index){
			case 4:P0=table[0];break;
			case 8:P0=table[1];break;
			case 12:P0=table[2];break;
			case 16:P0=table[3];break;
			case 20:P0=table[4];break;
			case 24:P0=table[5];break;
			case 28:P0=table[6];break;
			case 32:P0=table[7];break;
			
			case 35:P0=table[6];break;
			case 38:P0=table[5];break;
			case 41:P0=table[4];break;
			case 44:P0=table[3];break;
			case 47:P0=table[2];break;
			case 50:P0=table[1];break;
			case 53:P0=table[0];break;
		}
		if(index == 53){   //数码管数字跑完
			index = 2;    //数码管下标复位0
		}
	}
}


void main()
{
	EA = EX0 = ET0 = 1;
	TMOD = 0x01;       //定时器方式控制寄存器 01方式16位
	TH0 = (65536-50000)/256;   //T0定时器每次中断50ms
	TL0 = (65536-50000)%256;
	P0 = 0xff;   //小灯初始化全灭
	TR0 = 1;
	while(1);
	//经过1s进入中断，所以开始时要等待1s
}