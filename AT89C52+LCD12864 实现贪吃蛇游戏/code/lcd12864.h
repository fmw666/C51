#ifndef _LCD12864_H_
#define _LCD12864_H_

//---包含头文件---//
#include <reg52.h>


//---数据传输口定义: P1口作为数据传输口---//
#define LCD_DATA P1

//---寄存器位声明---//
sbit LCD_RS = P3^7;               //并行数据/命令选择端(串行片选信号)
sbit LCD_RW = P3^6;               //并行读/写控制端(串行数据口)
sbit LCD_EN = P3^5;               //并行使能控制端(串行时钟信号端)

//---LCD_Write写入模式---//
#define MODE_DATA 1               //1是写入数据
#define MODE_CMD  0               //0是写入命令

//---函数声明---//
void Delay_1ms(unsigned int time);   //在51单片机12MHZ时钟下的延时函数
void LCD_Check_Busy();               //等待忙标志函数
void LCD_Init();                     //初始化子函数
void LCD_Write(unsigned char Write_Mode, unsigned char Data_Command);
unsigned char LCD_Read(unsigned char Read_Mode);
void LCD_Write_Strs(unsigned char addr, unsigned char *str, unsigned char count);

//---绘图模式声明---//
void LCD_Clear_DDRAM();             //清空DDRAM显示
void LCD_Draw_Clear();              //清空绘图RAM的函数
void LCD_Draw_Point(unsigned char location_x, unsigned char location_y, unsigned char DrawOrClear);  //画一个点
void LCD_Draw_Square(unsigned char location_x, unsigned char location_y, unsigned char DrawOrClear);  //画一个2X2的正方形

#endif
