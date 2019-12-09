#include "lcd12864.h"
#include "intrins.h"

void Delay_1ms(unsigned int n){
    unsigned char i;
	unsigned char cnt = 0;
    for(i=1; i<=n; i++)
		while(cnt < 250) 
			cnt++;
}

//等待忙标志函数
void LCD_Check_Busy(){
	LCD_DATA = 0xff;
	LCD_EN = 0;				//确保使能端为低电平
	LCD_RS = 0;				//1为数据，0为命令
	LCD_RW = 1;				//1为从12864读取，0为向12864写入
	LCD_EN = 1;
	
	while(LCD_DATA & 0x80);	//忙状态检测
	LCD_EN = 0;
}

//并口模式初始化函数
void LCD_Init(){
	LCD_Write(MODE_CMD, 0x30);	//基本指令集,8位并行
	LCD_Write(MODE_CMD, 0x01);	//清除显示DDRAM
	LCD_Write(MODE_CMD, 0x06);	//启始点设定：光标右移
	LCD_Write(MODE_CMD, 0x0C);	//显示状态开关：整体显示开，光标显示关，光标显示反白关
	LCD_Write(MODE_CMD, 0x02);	//地址归零
}

//并口写入函数
void LCD_Write(unsigned char Write_Mode, unsigned char Data_Command){
/*
	LCD_Check_Busy();
	LCD_EN = 0;					//确保使能端为低电平
	LCD_RS = Write_Mode;		//1为写入数据，0为写入命令
	LCD_RW = 0;					//1为从12864读取，0为向12864写入
	
	LCD_DATA = Data_Command;
	LCD_EN = 1;
	Delay_1ms(1);
	LCD_EN = 0;
*/
	LCD_Check_Busy();
	LCD_RS = Write_Mode;
	LCD_RW = 0;
	LCD_EN = 0;
	LCD_DATA = Data_Command;
	_nop_();
	_nop_();
	LCD_EN = 1;
	_nop_();
	_nop_();
	LCD_EN = 0;
	LCD_RW = 1;
	LCD_RS = 1;

}

//读取RAM中相应数据,1为读取数据,0为读取命令(即忙标志)
unsigned char LCD_Read(unsigned char Read_Mode){
	unsigned char Data_Command;
	LCD_Check_Busy();				//等待忙标志函数
	LCD_DATA = 0xff;				//由于IO口是准双向,所以读取之前先写入高电平
	LCD_RS = Read_Mode;				//1为读取数据，0为读取命令,读命令读的是忙标志和此时光标的位置
	LCD_RW = 1;						//1为从12864读取，0为向12864写入
	LCD_EN = 0;
	LCD_EN = 1;
	Data_Command = LCD_DATA;
	LCD_EN = 0;
	
	return Data_Command;
}

void LCD_Write_Strs(unsigned char addr, unsigned char *str, unsigned char count){
	unsigned char i;
	LCD_Write(MODE_CMD, 0x30);
	LCD_Write(MODE_CMD, addr);	//设定DDRAM地址
	for(i=0; i<count; i++){	
		LCD_Write(MODE_DATA, str[i]);
    }
}

//清空DDRAM显示
void LCD_Clear_DDRAM(){
	LCD_Write(MODE_CMD, 0x01);//清空显示命令,即将DDRAM填满20H(20H就是空格)
}


//清空绘图RAM的函数
void LCD_Draw_Clear(){
	unsigned char i=0, j=0, k=0;

	LCD_Write(MODE_CMD, 0x34);//开启拓展指令集
	LCD_Write(MODE_CMD, 0x36);//开启GDRAM显示,,即显示绘图RAM,不显示字符RAM
	
	//分上下两屏写
    for(i=0; i<2; i++){
		//每个屏有32行,每个半屏都是1到32行,但是横坐标不一样
        for(j=0; j<32; j++){
            LCD_Write(MODE_CMD, 0x80 + j);//写Y坐标,即选择第几行
			//选择上下屏幕
            if(i == 0){
                LCD_Write(MODE_CMD, 0x80);//选择上半屏幕,横坐标是:0X80到0X87
            }else{
                LCD_Write(MODE_CMD, 0x88);//选择下半屏幕
            }
			
			//写一整行数据
            for(k=0; k<16; k++){
                LCD_Write(MODE_DATA, 0x00) ;
            }
        }
	}
	
    LCD_Write(MODE_CMD, 0x30);//关闭扩充指令集
}


//画一个点函数
void LCD_Draw_Point(unsigned char location_x, unsigned char location_y, unsigned char DrawOrClear){
	//画一个点的思路:
	//首先要选好要显示的是是哪一块显示区域
	//选的方法就是先送入显示区域的纵坐标,再送入显示区域的横坐标
	
    //y指垂直地址, x指水平地址
	unsigned char location_x_port;						//指示当前横坐标的数据应该写入第几块(每16位一块)
	unsigned char Point_data_old_H;						//存储之前图片的数据的高八位
	unsigned char Point_data_old_L;						//存储之前图片的数据的低八位

	LCD_Write(MODE_CMD, 0x34);							//开启拓展指令集
	LCD_Write(MODE_CMD, 0x36);							//开启GDRAM显示,,即显示绘图RAM,不显示字符RAM
	
	//显示(location_x,location_y)这个点
	//先计算显示区域的纵坐标(0-63)
	//其中(0-31)是上半屏幕,(32-63)是下半屏幕
	
	//说明是下半屏幕
	//再计算显示区域的横坐标(0-127)
	if(location_y > 31){
		location_y %= 32;							// location_y 要对32取余
	}
	
	//再计算显示区域的横坐标(0-127)
	location_x_port = location_x / 16;				//先计算要写入第几块
	
	LCD_Write(MODE_CMD, 0x80 + location_y);			//显示区域的纵坐标,即第几行
	LCD_Write(MODE_CMD, 0x88 + location_x_port);	//横坐标:选择横坐标的第几块
	
	//有了坐标之后,取出此坐标处的旧图片数据
	//Point_data_old_H = LCD_Read(1);					//假读
	LCD_Read(1);									//假读
	Point_data_old_H = LCD_Read(1);
	Point_data_old_L = LCD_Read(1);
	
	LCD_Write(MODE_CMD, 0x80 + location_y);			//显示区域的纵坐标,即第几行
	LCD_Write(MODE_CMD, 0x88 + location_x_port);	//横坐标:选择横坐标的第几块
	
	//上面之所有要再写入一遍地址,是因为第一次写入的是要读取的数据的地址
	//现在第二次写入的是要绘制的点的地址


	//下面计算出要向已经选好的显示区域写入的数据
	location_x %= 16;//首先 location_x 要对16取余
	
	if(location_x >= 8){
		location_x %= 8;
	}	
	//这里要判断是擦除点还是画一个点
	if(DrawOrClear == 1){
		LCD_Write(MODE_DATA, Point_data_old_H);
		LCD_Write(MODE_DATA, (Point_data_old_L) | (0X80>>location_x));// ||:这个是逻辑或  |:这个才是按位或运算
	}else{
		LCD_Write(MODE_DATA, Point_data_old_H);
		LCD_Write(MODE_DATA, (Point_data_old_L) & (~(0X80>>location_x)));
	}
	
	LCD_Write(MODE_CMD, 0x30);//关闭扩充指令集
}

//画一个2X2的正方形函数
void LCD_Draw_Square(unsigned char location_x, unsigned char location_y, unsigned char DrawOrClear){
	LCD_Draw_Point(location_x, location_y, DrawOrClear);
	LCD_Draw_Point(location_x + 1, location_y, DrawOrClear);
	LCD_Draw_Point(location_x, location_y + 1, DrawOrClear);
	LCD_Draw_Point(location_x + 1, location_y + 1, DrawOrClear);
}


