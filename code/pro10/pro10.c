#include <reg51.h>
#include <intrins.h> //引入_nop_();函数
/*
	_nop_();指令需要的只是一个机械周期
	也就是12个时钟周期（震荡周期）
	_nop_(); 指令的延迟时间为 1us。
	可以较为精确得控制延迟时间。
*/

#include "zh-hans.h" //导入我们自己写的中文编码库

#define LCD_databus P1 //LCD8位数据口

sbit INT = P3^2;
sbit CS1 = P3^3;  //片选1，1有效，控制左半屏
sbit CS2 = P3^4;  //片选2，1有效，控制右半屏
sbit E = P3^5;    //使能信号
sbit RW = P3^6;   //读/写，1为读、0为写
sbit RS = P3^7;   //代码/数据，1为数据、0为代码

sbit BU = P3^1; //蜂鸣器，1为响
sbit LED = P3^0;    //小灯，1为亮

unsigned char key = 0x77;     //按键值
char key_press = 0; //按键是否按下

/*
	AMPIRE12864的组成：可以看成是两个64*64的屏幕
	通过CS1、CS2两个片选信号来控制，低电平选通
	RST复位信号，我们这里置为高电平表示不用复位
	
	DBx是并行输入端
	
	E是使能信号，置1可以设置DBx，置0表示交换结束
	RW为低电平表示写入数据，高电平表示读出数据
	
	RS为低电平表示指令操作，高电平表示数据操作
*/

/*
	键盘4x4
	1 2 3 返回
	4 5 6 清空
	7 8 9 退位
	# 0 * 确定
*/

/*************************函数声明***************************/
void delay(int z);
void read_busy();
void write_LCD_cmd(char value);
void write_LCD_data(char value);
void set_page(char page);
void set_line(char startline);
void set_column(char column);
void set_on_or_off(char choose);
void select_screen(char screen);
void clear_screen(char screen);
void clear_bottom_screen();
void init_LCD();
void display(char screen,char page,char column,char *p);
void display_charactars(char screen,char page,char column,char datacode);
void init_sys();
void tip_overlength();
void tip_lesslength();
void tip_pwderror();
void input_passwd();
void enter_sys();
/**************************************************************/


// 延时
void delay(int z)  
{
	int x,y;
	for(x=z;x>0;x--)
		for(y=114;y>0;y--);
}

//读“忙”函数——数据线最高位DB7=1则busy
void read_busy()
{
	P1 = 0x00; //DB送数
	RS = 0;  //代码
	RW = 1;  //读
	E = 1;   //1可设置DB
	while(P1 & 0x80);
	E = 0;
}

/***************************写入LCD***************************/
//向LCD中写入命令
void write_LCD_cmd(char value)
{
	read_busy(); //每次读写都要进行忙判断
	RS = 0;      //选择命令
	RW = 0;      //写操作  
	LCD_databus = value;
	E = 1;       //E由1->0锁存有效数据
	_nop_();     //空操作，用作延时一个机械周期1us
	_nop_();     //同上
	E = 0;
}

/*可以写成一个函数简便一点，
但是为了让大家好理解就先这样吧*/

//向LCD中写入数据
void write_LCD_data(char value)
{
	read_busy(); //每次读写都要进行忙判断
	RS = 1;      //选择数据
	RW = 0;      //写操作
	LCD_databus = value;
	E = 1;       //E由1->0锁存有效数据
	_nop_();     //空操作，用作延时一个机械周期1us
	_nop_();     //同上
	E = 0;
}
/**************************************************************/

/***************************设置*******************************/
//设置“页”，LCD12864共8页，一页是8行
void set_page(char page)
{
	page = 0xB8|page; //页的首地址0xB8
	write_LCD_cmd(page);
}

//设置显示的起始行
void set_line(char startline)
{
	startline = 0xC0|startline; //起始地址0xC0
	write_LCD_cmd(startline);
	//设置从哪行开始：共0-63；一般从0行开始显示
}

//设置显示的列
void set_column(char column)
{
	column = column & 0x3F; //列的最大值为64
	column = 0x40|column;   //列的首地址0x40
	write_LCD_cmd(column);  //列位置
}

//显示开、关函数，0x3E是关闭显示，0x3F是开启显示
void set_on_or_off(char choose)
{
	choose = 0x3E|choose; //0011 111x, choose只能为0或者1
	write_LCD_cmd(choose);
}
/**************************************************************/

/**************************屏幕选择***************************/
//选屏
void select_screen(char screen)
{
	switch(screen){
		case 0: CS1=0;CS2=0;break; //全屏
		case 1: CS1=0;CS2=1;break; //左半屏
		case 2: CS1=1;CS2=0;break; //右半屏
		default:break;
	}
}

//清屏
void clear_screen(char screen)
{
	char i,j;
	select_screen(screen);
	for(i=0;i<8;i++){ //控制页数0-7，共8页
		set_page(i);
		set_column(0);
		for(j=0;j<64;j++){ //控制列数0-63，共64列
			write_LCD_data(0x00); //写入0，地址指针自加1
		}
	}
}

//清最后一行屏
void clear_bottom_screen()
{
	char i;
	select_screen(0);
	
	set_page(6);   //最后一行
	set_column(0); //从第一列开始
	for(i=0;i<64;i++){ //控制列数0-63，共64列
		write_LCD_data(0x00); //写入0，地址指针自加1
	}
	set_page(7);
	set_column(0); //从第一列开始
	for(i=0;i<64;i++){ //控制列数0-63，共64列
		write_LCD_data(0x00); //写入0，地址指针自加1
	}
}
/**************************************************************/

//LCD的初始化
void init_LCD()
{
	read_busy();
	select_screen(0); //全屏
	set_on_or_off(0); //关闭显示
	select_screen(0); //全屏
	set_on_or_off(1); //开显示
	select_screen(0); //全屏
	clear_screen(0);  //清屏
	set_line(0);      //开始行：0
}

//显示汉字
void display(char screen,char page,char column,char *p)
{
	char i;
	select_screen(screen); //选屏
	set_page(page);        //控制页
	set_column(column);    //控制列
	for(i=0;i<16;i++){     //控制16列的数据输出
		write_LCD_data(p[i]); 
		//汉字的上半部分
	}
	set_page(page+1);        //写下半页
	set_column(column);
	for(i=0;i<16;i++){
		write_LCD_data(p[i+16]); 
		//汉字的下半部分
	}
}

//显示字符
void display_charactars(char screen,char page,char column,char datacode)
{
	char i;
	select_screen(screen); //选屏
	set_page(page);        //控制页
	set_column(column);    //控制列
	for(i=0;i<16;i++){     //控制16列的数据输出
		write_LCD_data(datacode); 
	}
}

/**************************************************************/
//初始化屏幕显示
void init_sys()
{	
	clear_screen(0); //清屏
	display(1,3,2*16,huan);
	display(1,3,3*16,ying);
	display(2,3,0*16,guang);
	display(2,3,1*16,lin);
	
	while(1){
		if(key_press){ //有按键按下
			if(key == 0x77){ //按下确认键
				key_press = 0;
				break;
			}
		}
	}
	input_passwd();
}

//提示语：超出长度
void tip_overlength()
{
	clear_bottom_screen();
	display(1,6,2*16,zuokuohao);
	display(1,6,3*16,chao);
	display(2,6,0*16,chu);
	display(2,6,1*16,chang);
	display(2,6,2*16,du);
	display(2,6,3*16,youkuohao);
}

//提示语：请输入四位
void tip_lesslength()
{
	clear_bottom_screen();
	display(1,6,1*16,zuokuohao);
	display(1,6,2*16,qing);
	display(1,6,3*16,shu);
	display(2,6,0*16,ru);
	display(2,6,1*16,si);
	display(2,6,2*16,wei);
	display(2,6,3*16,youkuohao);
}

//提示语：密码错误
void tip_pwderror()
{
	clear_bottom_screen();
	display(1,6,2*16,zuokuohao);
	display(1,6,3*16,mi);
	display(2,6,0*16,ma);
	display(2,6,1*16,cuo);
	display(2,6,2*16,wu);
	display(2,6,3*16,youkuohao);
}

//输入密码界面
void input_passwd()
{
	//竖线每个半秒闪烁
	char flag=0;
	//四位密码
	char pwd[4] = {0}; 
	//当前输入密码的长度
	char pwd_length = 0;
	
	clear_screen(0); //清屏
	display(1,0,0*16,qing);
	display(1,0,1*16,shu);
	display(1,0,2*16,ru);
	display(1,0,3*16,si);
	display(2,0,0*16,wei);
	display(2,0,1*16,mi);
	display(2,0,2*16,ma);
	display(2,0,3*16,maohao);
	
	while(1){
		if(key_press){ //有按键按下
			if(key == 0xEE){ //当按下1
				//已输入4位密码
				if(pwd_length >= 4){
					//清底部屏幕，显示提示语
					tip_overlength();
				}
				else{
					clear_bottom_screen();
					display(1,2,pwd_length*16,yi);
					pwd[pwd_length] = 1;
					pwd_length++;
				}
			}
			else if(key == 0xDE){ //当按下2
				//已输入4位密码
				if(pwd_length >= 4){
					//清底部屏幕，显示提示语
					tip_overlength();
				}
				else{
					clear_bottom_screen();
					display(1,2,pwd_length*16,er);
					pwd[pwd_length] = 2;
					pwd_length++;
				}
			}
			else if(key == 0xBE){ //当按下3
				//已输入4位密码
				if(pwd_length >= 4){
					//清底部屏幕，显示提示语
					tip_overlength();
				}
				else{
					clear_bottom_screen();
					display(1,2,pwd_length*16,san);
					pwd[pwd_length] = 3;
					pwd_length++;
				}
			}
			else if(key == 0xED){ //当按下4
				//已输入4位密码
				if(pwd_length >= 4){
					//清底部屏幕，显示提示语
					tip_overlength();
				}
				else{
					clear_bottom_screen();
					display(1,2,pwd_length*16,si);
					pwd[pwd_length] = 4;
					pwd_length++;
				}
			}
			else if(key == 0xDD){ //当按下5
				//已输入4位密码
				if(pwd_length >= 4){
					//清底部屏幕，显示提示语
					tip_overlength();
				}
				else{
					clear_bottom_screen();
					display(1,2,pwd_length*16,wu1);
					pwd[pwd_length] = 5;
					pwd_length++;
				}
			}
			else if(key == 0xBD){ //当按下6
				//已输入4位密码
				if(pwd_length >= 4){
					//清底部屏幕，显示提示语
					tip_overlength();
				}
				else{
					clear_bottom_screen();
					display(1,2,pwd_length*16,liu);
					pwd[pwd_length] = 6;
					pwd_length++;
				}
			}
			else if(key == 0xEB){ //当按下7
				//已输入4位密码
				if(pwd_length >= 4){
					//清底部屏幕，显示提示语
					tip_overlength();
				}
				else{
					clear_bottom_screen();
					display(1,2,pwd_length*16,qi);
					pwd[pwd_length] = 7;
					pwd_length++;
				}
			}
			else if(key == 0xDB){ //当按下8
				//已输入4位密码
				if(pwd_length >= 4){
					//清底部屏幕，显示提示语
					tip_overlength();
				}
				else{
					clear_bottom_screen();
					display(1,2,pwd_length*16,ba);
					pwd[pwd_length] = 8;
					pwd_length++;
				}
			}
			else if(key == 0xBB){ //当按下9
				//已输入4位密码
				if(pwd_length >= 4){
					//清底部屏幕，显示提示语
					tip_overlength();
				}
				else{
					clear_bottom_screen();
					display(1,2,pwd_length*16,jiu);
					pwd[pwd_length] = 9;
					pwd_length++;
				}
			}
			else if(key == 0xD7){ //当按下0
				//已输入4位密码
				if(pwd_length >= 4){
					//清底部屏幕，显示提示语
					tip_overlength();
				}
				else{
					clear_bottom_screen();
					display(1,2,pwd_length*16,ling);
					pwd[pwd_length] = 0;
					pwd_length++;
				}
			}
			else if(key == 0xE7){ //当按下#
				
			}
			else if(key == 0xB7){ //当按下*
				
			}
			else if(key == 0x7E){ //当按下返回
				init_sys();
			}
			else if(key == 0x7D){ //当按下清空
				clear_bottom_screen();
				display(1,2,0*16,kongbai);
				display(1,2,1*16,kongbai);
				display(1,2,2*16,kongbai);
				display(1,2,3*16,kongbai);
				display(2,2,0*16,kongbai);
				pwd_length = 0;
			}
			else if(key == 0x7B){ //当按下退位
				clear_bottom_screen();
				if(pwd_length > 0){
					if(pwd_length == 4){
						display(2,2,(pwd_length-4)*16,kongbai);
					}
					else if(pwd_length < 4){
						display(1,2,pwd_length*16,kongbai);
					}
					pwd_length--;
				}
			}
			else if(key == 0x77){ //当按下确定
				//已输入4位密码
				if(pwd_length < 4){
					//清底部屏幕，显示提示语
					tip_lesslength();
				}
				else if(pwd_length == 4){
					if(pwd[0]==0&&pwd[1]==7&&pwd[2]==1&&pwd[3]==8){
						enter_sys();
					}
					else{
						tip_pwderror();
						BU = 1;
						delay(50);
						BU = 0;
					}
				}
			}
			
			key_press = 0; //按下状态还原
		}
		
		
		//闪烁竖线
		if(flag <= 50){
			if(pwd_length >= 4){
				//右半屏
				display(2,2,(pwd_length-4)*16,shuxian);
			}
			else{
				//左半屏
				display(1,2,pwd_length*16,shuxian);
			}
			flag++;
		}
		else if(flag < 200){
			if(pwd_length >= 4){
				//右半屏
				display(2,2,(pwd_length-4)*16,kongbai);
			}
			else{
				//左半屏
				display(1,2,pwd_length*16,kongbai);
			}
			flag++;
			if(flag == 200){
				flag = 0;
			}
		}
	}
}

//密码正确进入系统
void enter_sys()
{
	LED = 1; 
	clear_screen(0); //清屏
	display(1,3,2*16,mi);
	display(1,3,3*16,ma);
	display(2,3,0*16,zheng);
	display(2,3,1*16,que);
	while(1){
		if(key_press){ //有按键按下
			if(key == 0x7E){ //按下返回键
				LED = 0;
				key_press = 0;
				break;
			}
		}
	}
	init_sys();
}
/**************************************************************/

//键盘扫描
void key_scan() interrupt 0
{
	char value_h,value_l;
	
	P2 = 0xF0;
	if(P2 != 0xF0){
		P2 = 0xF0;
		value_l = P2;
		delay(10); //按键消抖
		P2 = 0x0F;
		value_h = P2;
		
		key = value_l+value_h; //得到键值
	}
	P2 = 0xF0;
	
	key_press = 1;
}

void main()
{
	LED = BU = 0;  // 小灯和蜂鸣器处于不工作状态
	P1 = 0xF0;
	P2 = 0xF0;
	
	EA = 1; // 中断总允许
	EX0 = 1; // 外部中断0
	IT0 = 1; // 下降沿触发
	
	/*
	一个汉字16x16大小
	2页表示32行开始，8页共128行
	每个半屏（64/2=32列）只能显示四个汉字
	32/16=2
	*/
	init_sys();
	
	while(1);
}