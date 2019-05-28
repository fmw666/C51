#include <reg51.h>

sbit P3_2 = P3^2;
char table[9] = {0xFF,0xFE,0xFD,0xFB,0xF7,0xEF,0xDF,0xBF,0x7F}; //小灯亮方式
char index=0;
char i = 0;
char flag = 1;   //第一下按下会执行

/*按键消抖*/
void delay(long time)
{
	int i=0;
	while(time--)
	for(i=0;i<125;i++);
}

void pressbutton() interrupt 0
{
	if(flag){
		index++;
		P0 = table[index];
		P1 = ~table[index];
		if(index == 9){
			index = 0;
			P0 = table[index];  //复原P0全灭
			P1 = ~table[index]; //复原P1全亮
		}
		flag=0;
	}
}

void main()
{
	P1 = 0x00;   //默认P1全亮
	P0 = 0xFF;   //默认P0全灭
	IT0 = 0;     //INT0下降沿有效
	EX0 = EA = 1;  //中断允许位
	while(1){
		if(P3_2 == 0){   //按钮按下，触发中断INT0，这是按键消抖方式
			delay(1);
			if(P3_2==0){
				for(i=0;i<10;i++){
					if(P3_2==0){
						i=0;
					}
				}
				flag = 1;   //消抖后使flag=1表示在中断中可以执行功能
			}
		}
	}
}