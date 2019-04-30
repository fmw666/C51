#include <reg51.h>
sbit P0_0 = P0^0;
sbit P0_1 = P0^1;
sbit P0_2 = P0^2;
sbit P0_3 = P0^3;
sbit P0_4 = P0^4;
sbit P0_5 = P0^5;

char numbers[10]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90};
char index = 0;
char i = 0;
char cnt = 0;    //记录中断次数，每20次为1s

void green(){
	P0_0 = 0;   //绿灯亮	
	P0_1 = P0_2 = 1;
}

void yellow(){
	P0_2 = 0;   //黄灯亮
	P0_0 = P0_1 = 1;
}

void red(){
	P0_1 = 0;
	P0_0 = P0_2 = 1;
}

void clear(){
	P0_0 = P0_1 = P0_2 = 1;
}

int Timer0() interrupt 1
{
	TH0 = (65536-50000)/256;   //T0定时器复位
	TL0 = (65536-50000)%256;
	cnt++;
	if(cnt == 20){    //1s到达，数码管显一个数
		cnt = 0;      //计数复原
		index++;      //数码管数字增加1
		if(1<=index && index<=9){  //前9s绿灯亮
			i = 9-index;
			green();
		}
		if(10<=index && index<=12){  //中间3s黄灯亮
			i = 3-(index-9);
			yellow();
		}
		if(13<=index && index<=21){  //后面9s红灯亮
			i = 9-(index-12);
			red();
		}
		P1 = numbers[i]; //数码管置数
		if(index == 22){   //数码管数字跑完一共需要9+3+9=21s
			clear();
			TR0 = 0;      //关闭定时器T0
			index = 0;    //数码管下标复位0
		}
	}
}

int Timer1() interrupt 3
{
	TH1 = (65536-50000)/256;   //T0定时器复位
	TL1 = (65536-50000)%256;
	cnt++;
	if(cnt == 20){    //1s到达，数码管显一个数
		cnt = 0;      //计数复原
		index++;      //数码管数字增加1
		if(index == 10){   //数码管数字跑完
			TR1 = 0;      //关闭定时器T0
			index = 0;    //数码管下标复位0
		}
	}
}

void main()
{
	EA = EX0 = EX1 = ET0 = ET1 = 1;
	TMOD = 0x01;       //定时器方式控制寄存器 T0、T1为01方式16位
	TH0 = (65536-50000)/256;   //T0定时器每次中断50ms
	TL0 = (65536-50000)%256;
	TH1 = (65536-50000)/256;
	TL1 = (65536-50000)%256;
	TR0 = 1; 
	TR1 = 1;   //打开T0、T1计数器
	P1 = numbers[0];
	while(1);
}