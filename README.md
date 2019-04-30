# 单片机原理与接口技术实验题

---

#### 1. 数码管数字显示控制程序设计。控制要求：用按键K1和K2分别控制数码管显示数字0~9的顺序，当按下K1键时按从0到9的顺序显示，当按下K2时按从9到0的顺序显示。时间间隔均为1秒。硬件电路如图所示。
  <details>
    <summary><i>点击展开 Proteus仿真实验图</i></summary>
    <img src="images/pro1.png" width="900">
  </details>
  <details>
    <summary><i>点击查看C语言实现代码</i></summary>
    <pre><code>
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
    </code></pre>
  </details>
  <br><br>

#### 2. 交通控制系统，如下图所示，是交叉路口，要求：绿灯亮9秒后，黄灯闪3秒，后红灯亮9秒，两个数码管分别显视各种灯所剩余时间。
  <div align="left">
    <img src="images/pro2.png" width="900">
  </div>
  <br><br>

#### 3. LED灯要求亮2秒钟，灭2秒钟，要求使用定时计数器，要求写主程序与中断程序，fosc = 6MHz
  <div align="left">
    <img src="images/pro3.png" width="900">
  </div>
  <br><br>

#### 4. 利用外部中断0，每申请一次中断使P0口连接的发光二极管点亮一个，从低位到高位，同时使P1口连接的发光二极管熄灭一个，从高位到低位。
  <div align="left">
    <img src="images/pro4.png" width="900">
  </div>
  <br><br>
  
#### 5. 完成下面电路所示的功能，K1，K2对应两个开关按键。P1口对应发光二极管的状态
|    |P1.0|P1.1|P1.2|P1.3|P1.4|P1.5|P1.6|P1.7|
|:--:|:---|:---|:---|:---|:---|:---|:---|:---|
|K1=0,K2=0|○|○|○|○|○|○|○|○|
|K1=0,K2=1|●|●|●|●|○|○|○|○|
|K1=1,K2=0|●|●|○|○|●|●|○|○|
|K1=1,K2=1|●|●|●|●|●|●|●|●|

  <div align="left">
    <img src="images/pro5.png" width="900">
  </div>
  <br><br>
  
#### 6. 在一个数码管上循环显示“H”“E” “L” “L” “O” ,循环的时间为1s。
  <div align="left">
    <img src="images/pro6.png" width="900">
  </div>
  <br><br>
  
#### 7. 试采用定时方式2，在P1.0引脚上输出频率为1KHz的方波。
  <div align="left">
    <img src="images/pro7.png" width="900">
  </div>
  <br><br>
  
#### 8. 完成下面电路的功能，P1口对应发光二极管的状态，状态1显示的时间为1s，状态2显示的时间为2s，状态3显示的时间为3s，如此循环。
|状态|P1.0|P1.1|P1.2|
|:--:|:---|:---|:---|
|状态1|●|○|●|
|状态2|○|●|●|
|状态3|●|●|○|

  <div align="left">
    <img src="images/pro8.png" width="900">
  </div>
  <br><br>
  
#### 9. 彩灯系统，实现8个发光二极管从左到右点亮，每个发光二极管点亮的时间为2s，然后从右到左亮，每个发光二极管点亮的时间为1.5s。

  <div align="left">
    <img src="images/pro9.png" width="900">
  </div>
  <br><br>
