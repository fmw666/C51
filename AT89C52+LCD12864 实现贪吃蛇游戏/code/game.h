#include "lcd12864.h"
#include "snake.h"
// 初始化内存池函数以及随机数生成函数
#include <stdlib.h>

//---寄存器位声明---//
sbit Key_Up = P0^0;
sbit Key_Down = P0^1;
sbit Key_Left = P0^2;
sbit Key_Right = P0^3;
sbit Key_Pause = P0^4;
sbit Key_Enter = P0^5;
sbit Key_Return = P0^6;

//---宏定义常量---//
#define SYS_EASY 1
#define SYS_HARD 2
#define SYS_TIPS 3

#define TOWDS_UP 0
#define TOWDS_DOWN 1
#define TOWDS_RIGHT 2
#define TOWDS_LEFT 3

//---全局变量声明---//
unsigned char snake_towds = TOWDS_RIGHT;  // 蛇头的朝向,默认朝右 TOWDS_RIGHT
unsigned int snake_speed = 30000;         // 小蛇移动速度
Snake snake;
Point snakeRear;                          // 蛇尾的坐标
Point FruitLocation;                      // 果实的坐标
unsigned char InitSeed = 0;               // 初始化种子，用于产生随机数
unsigned char FruitFlag = 0;              // 表示果实是否还存在
bit PressRet = 0;                         // Return按键是否被按下
bit PressPause = 0;                       // Pause按键是否被按下

// 游戏相关函数
void Timer_Set();                                               // 定时器设置函数
void delay(unsigned int time);                                  // 延时函数
void Key_Scan();                                                // 按键扫描函数
void Inv_Key_Scan();                                            // 困难模式下的按键扫描
unsigned char Game_Menu();                                      // 游戏初始界面菜单
void Game_Over();                                               // 游戏失败后界面
bit Snake_Check_Dead(Snake *snake, Point *elemNew);             // 检查蛇是否咬死自己
Point Snake_Move(Snake *snake, Point *FruitLocation);           // 蛇向前移动一个单位
void LCD_Draw_Snake(Snake *snake, Point *ClearLocation);        // 在任意位置画一条蛇
void LCD_Draw_Fruit(Point *FruitLocation, Snake *snake_tempt);  // 在随机位置画出果实
void LCD_Write_Score(unsigned char Score);                      // 写入分数
void Sys_Easy();                                                // 游戏的简单模式
void Sys_Hard();                                                // 游戏的困难模式
void Sys_Tips();                                                // 游戏的贴士界面

// 定时器设置函数
void Timer_Set(){
    EA = 1;       // 开启总中断允许
    ET0 = 1;      // 打开定时器0允许
    TMOD = 0x01;  // 设置定时器的工作方式寄存器TCON
    TH0 = 0;      // 设置定时器初值
    TL0 = 0;
    TR0 = 1;      // 最后用TCON寄存器里面的TR0来开启定时器
}

// 定时器 0 的中断服务函数
void TimerInterrupt() interrupt 1{
    TH0 = 0;
    TL0 = 0;
    ++InitSeed;  // 初始化种子自增
}

// 延时函数
void delay(unsigned int time){
    while(--time){
        Key_Scan();
    }
}

// 按键扫描函数
void Key_Scan(){
    if(Key_Up == 0){
        Delay_1ms(1);
        if(Key_Up == 0){
            if(snake_towds != TOWDS_DOWN){
                snake_towds = TOWDS_UP;
            }
        }
    }
    if(Key_Down == 0){
        Delay_1ms(1);
        if(Key_Down == 0){
            if(snake_towds != TOWDS_UP){
                snake_towds = TOWDS_DOWN;
            }
        }
    }
    if(Key_Left == 0){
        Delay_1ms(1);
        if(Key_Left == 0){
            if(snake_towds != TOWDS_RIGHT){
                snake_towds = TOWDS_LEFT;
            }
        }
    }
    if(Key_Right == 0){
        Delay_1ms(1);
        if(Key_Right == 0){
            if(snake_towds != TOWDS_LEFT){
                snake_towds = TOWDS_RIGHT;
            }
        }
    }
    if(Key_Return == 0){
        PressRet = 1;
    }
    if(Key_Pause == 0){
        Delay_1ms(20);
        if(Key_Pause == 0){
            if(PressPause == 0){
                LCD_Write_Strs(0x90, "Press the Pause", 15);
                LCD_Write_Strs(0x89, "for Continue...", 15);
                while(!Key_Pause);
                PressPause = 1;
                while(PressPause && !PressRet){
                    Key_Scan();
                }
                PressRet = 0;
            }else{
                LCD_Clear_DDRAM();
                while(!Key_Pause);
                PressPause = 0;
            }
        }
    }
}

// 困难模式的按键扫描函数
void Inv_Key_Scan(){
    if(Key_Up == 0){
        Delay_1ms(1);
        if(Key_Up == 0){
            if(snake_towds != TOWDS_UP){
                snake_towds = TOWDS_DOWN;
            }
        }
    }
    if(Key_Down == 0){
        Delay_1ms(1);
        if(Key_Down == 0){
            if(snake_towds != TOWDS_DOWN){
                snake_towds = TOWDS_UP;
            }
        }
    }
    if(Key_Left == 0){
        Delay_1ms(1);
        if(Key_Left == 0){
            if(snake_towds != TOWDS_LEFT){
                snake_towds = TOWDS_RIGHT;
            }
        }
    }
    if(Key_Right == 0){
        Delay_1ms(1);
        if(Key_Right == 0){
            if(snake_towds != TOWDS_RIGHT){
                snake_towds = TOWDS_LEFT;
            }
        }
    }
    if(Key_Return == 0){
        PressRet = 1;
    }
    if(Key_Pause == 0){
        Delay_1ms(20);
        if(Key_Pause == 0){
            if(PressPause == 0){
                LCD_Write_Strs(0x90, "Press the Pause", 15);
                LCD_Write_Strs(0x89, "for Continue...", 15);
                while(!Key_Pause);
                PressPause = 1;
                while(PressPause && !PressRet){
                    Key_Scan();
                }
                PressRet = 0;
            }else{
                LCD_Clear_DDRAM();
                while(!Key_Pause);
                PressPause = 0;
            }
        }
    }
}

// 转场动画
void change_Screen(){
    LCD_Clear_DDRAM();
    LCD_Write_Strs(0x88, " Please wait...", 15);
    LCD_Draw_Clear();
    LCD_Clear_DDRAM();
}

// 初始化
void init(){
    /* 
        #define _MALLOC_MEM_ xdata
        51由于没有操作系统，需要分配为动态内存的区间并不清楚
        需要头文件<stdlib.h>故需调用init_mempool来初始化内存池
    */
    unsigned char _MALLOC_MEM_ memoryTempt[100];     // 随机分配一个数组,用于初始化内存池，xdata区域
    init_mempool(memoryTempt, sizeof(memoryTempt));  // 初始化内存池

    LCD_Init();   // 初始化LCD, 8位并口
    Timer_Set();  // 初始化定时器0
    Snake_Init(&snake);  // 初始化蛇
}

// 简单系统
void Sys_Easy(){
    Snake_Init(&snake);  // 初始化蛇
    change_Screen();
    while(!PressRet){
        delay(snake_speed);  // 延时,控制蛇走的快慢

        snakeRear = Snake_Move(&snake, &FruitLocation);  // 让蛇移动一格,并且取出蛇尾的坐标

        if(FruitLocation.Y == 100){
            FruitLocation.Y = 99;  // 说明Game_Over
            continue;  // 下面语句不执行,直接进行新一次循环
        }else if(FruitLocation.Y == 200){
            FruitFlag = 0;  // 说明吃到果实了，清空果实存在标志位
            snake_speed -= 300;  // 每吃到一个果实,蛇的速度加快
        }

        // 判断是否要新生成一个果实
        if(FruitFlag == 0){
            //Key_Scan();  // 按键扫描函数
            LCD_Draw_Fruit(&FruitLocation, &snake);  // 生成新的果实,并画出来
            //Key_Scan();  // 按键扫描函数

            continue;  // 由于蛇尾不再擦除,而且蛇头也不需要重画,所以下面语句不执行,直接进行新一次循环
        }

        //Key_Scan();  // 按键扫描函数

        LCD_Draw_Snake(&snake, &snakeRear);  // 更新蛇的位置
    }
    PressRet = 0;
}

// 困难系统
void Sys_Hard(){
    Snake_Init(&snake);  // 初始化蛇
    change_Screen();
    while(!PressRet){
        delay(snake_speed);
        snakeRear = Snake_Move(&snake, &FruitLocation);

        if(FruitLocation.Y == 100){
            FruitLocation.Y = 99;
            continue;
        }else if(FruitLocation.Y == 200){
            FruitFlag = 0;
            snake_speed -= 300;
        }

        if(FruitFlag == 0){
            Inv_Key_Scan();
            LCD_Draw_Fruit(&FruitLocation, &snake);
            Inv_Key_Scan();	
            continue;
        }
        Inv_Key_Scan();

        LCD_Draw_Snake(&snake, &snakeRear);
    }
    PressRet = 0;
}

// 提示系统
void Sys_Tips(){
    LCD_Clear_DDRAM();
    LCD_Write_Strs(0x80, "|-----Tips-----|", 16);  // The-OdD-SnAKee 
    LCD_Write_Strs(0x90, "| fmw-lyx-zgt  |", 16);  //      easy
    LCD_Write_Strs(0x88, "|--------------|", 16);  //   -> hard
    LCD_Write_Strs(0x98, "     ->ok", 9);          //      tips

    while(1){
        if(Key_Return == 0 || Key_Enter == 0){
            Delay_1ms(20);
            if(Key_Return == 0 || Key_Enter == 0){
                while(!Key_Return || !Key_Enter);
                break;
            }
        }
    }
}


// 在任意位置画出整条蛇
void LCD_Draw_Snake(Snake *snake, Point *ClearLocation){
    // 遍历这条蛇
    SnakeNode *p;  // 定义一个指针,用于遍历这条蛇

    for(p=snake->front; p!=NULL; p=p->next){
        // 根据当前节点的坐标画出正方形
        LCD_Draw_Square(p->Location.X, p->Location.Y, 1);
    }

    // 再根据擦除位置的坐标擦除一个正方形
    // 说明没有吃到果实,此时擦除蛇尾
    if(FruitFlag != 0){
        LCD_Draw_Square(ClearLocation->X, ClearLocation->Y, 0);
    }
}

// 画出游戏初始界面
unsigned char Game_Menu(){
    unsigned char i = 0;
    unsigned char ch = SYS_HARD;
    unsigned char code str_point[] = "   ->";
    unsigned char code str_point_null[] = "     ";
    change_Screen();

    // 写入菜单
    LCD_Write_Strs(0x80, " The-OdD-SnAKee", 14);
    LCD_Write_Strs(0x90, "      easy", 10);
    LCD_Write_Strs(0x88, "   -> hard", 10);
    LCD_Write_Strs(0x98, "      tips", 10);

    while(1){
        // 选择 easy
        if(ch == SYS_EASY){
            // 按↑无反应
            // 按↓响应
            if(Key_Down == 0){
                Delay_1ms(5);
                if(Key_Down == 0){
                    ch = SYS_HARD;
                    LCD_Write_Strs(0x90, str_point_null, 5);
                    LCD_Write_Strs(0x88, str_point, 5);
                    while(!Key_Down);
                }
            }
        }
        // 选择 hard
        if(ch == SYS_HARD){
            // 按↑响应
            if(Key_Up == 0){
                Delay_1ms(5);
                if(Key_Up == 0){
                    ch = SYS_EASY;
                    LCD_Write_Strs(0x88, str_point_null, 5);
                    LCD_Write_Strs(0x90, str_point, 5);
                    while(!Key_Up);
                }
            }
            // 按↓响应
            if(Key_Down == 0){
                Delay_1ms(5);
                if(Key_Down == 0){
                    ch = SYS_TIPS;
                    LCD_Write_Strs(0x88, str_point_null, 5);
                    LCD_Write_Strs(0x98, str_point, 5);
                    while(!Key_Down);
                }
            }
        }
        // 选择 tips
        if(ch == SYS_TIPS){
            // 按↑响应
            // 按↓无反应
            if(Key_Up == 0){
                Delay_1ms(20);
                if(Key_Up == 0){
                    ch = SYS_HARD;
                    LCD_Write_Strs(0x98, str_point_null, 5);
                    LCD_Write_Strs(0x88, str_point, 5);
                    while(!Key_Up);
                }
            }
        }
        // 只有当按下"开始"键时,游戏才会开始
        if(Key_Enter == 0){
            Delay_1ms(5);
            if(Key_Enter == 0){			
                // 用随机数产生果实,其中随机数的种子用定时器获取,按下了"开始"键后产生种子,之后种子不再改变
                srand(InitSeed);  // 初始化种子
                // 产生种子之后定时器0就可以关掉了
                TR0 = 0;
                while(!Key_Enter);
                // 退出死循环
                return ch;
            }
        }
    }
}

// 画出游戏失败的画面
void Game_Over(){
    unsigned int i = 0;  // 用于for循环

    // 清空游戏界面,即清空DDRAM
    LCD_Clear_DDRAM();

    // 写入"Game_Over"
    LCD_Write_Strs(0x92, "GameOver", 8);

    // 先写入"得分: "
    LCD_Write_Strs(0x8A, "score:", 6);

    // 再写入分数
    LCD_Write_Score(snake.length);  // 写入分数的函数

    while(1){
        // 只有当按下"回车"键时,游戏才会开始
        if(Key_Enter == 0 || Key_Return == 0){
            Delay_1ms(10);
            if(Key_Enter == 0 || Key_Return == 0){			
                // 之后将蛇销毁,并再次初始化
                Snake_Destroy(&snake);
                Snake_Init(&snake);

                // 清空果实存在的标志
                FruitFlag = 0;

                // 重新设置初始方向为向右
                snake_towds = TOWDS_RIGHT;

                while(!Key_Enter || !Key_Return);
                // 退出死循环
                PressRet = 1;
                break;
            }
        }
    }
}

// 检查蛇是否咬死自己，0表示不会咬死  1表示会咬死自己
bit Snake_Check_Dead(Snake *snake, Point *elemNew){
    SnakeNode *p = snake->front->next->next;  // 最开始指向3号位置,为蛇头

    if(snake->length < 4)
        return 0;
    else{
        for(p; p!=NULL; p=p->next){
            if((elemNew->X == p->Location.X) && (elemNew->Y == p->Location.Y)){
                return 1;
            }
        }
    }

    return 0;
}

// 写入分数的函数
void LCD_Write_Score(unsigned char Score){
    Score -= 2;  // 减掉初始长度2

    // 写入分数时,要判断分数的位数
    if(Score < 10){
        LCD_Write(MODE_DATA, '0');  // 48即数字0
        LCD_Write(MODE_DATA, Score + '0');
    }else{
        unsigned char Score_H = Score / 10;
        unsigned char Score_L = Score % 10;

        LCD_Write(MODE_DATA, Score_H + '0');
        LCD_Write(MODE_DATA, Score_L + '0');
    }
}

// 画出果实的函数
void LCD_Draw_Fruit(Point *FruitLocation, Snake *snake_tempt){
    // 定义一个指针,遍历整条蛇
    SnakeNode *p;

    // 定义一个标志位,用于表示生成的果实是否和蛇的身体重合,1表示重合
    bit FruitQualifiedFlag = 1;

    // 判断生成的果实是否合格,不合格的话重新生成一个新果实
    while(FruitQualifiedFlag){
        InitSeed = rand();  // 新产生一个随机数
        FruitLocation->X = (InitSeed % 63) * 2;  // X 最大是124,并且必须是偶数
        FruitLocation->Y = (InitSeed % 31) * 2;  // Y 最大是60,并且必须是偶数

        for(p=snake_tempt->front; p!=NULL; p=p->next){
            if((p->Location.X == FruitLocation->X) && 
                (p->Location.Y == FruitLocation->Y)){
                break;
            }
        }

        // 出循环之后,如果此时 p 为 NULL,说明没有重复,如果p不为NULL,说明有重复
        if(p == NULL){
            FruitQualifiedFlag = 0;
        }
    }

    LCD_Draw_Square(FruitLocation->X, FruitLocation->Y, 1);

    FruitFlag = 1;  // 画好果实后,标志位置位
}

// 处理蛇的走动的函数
Point Snake_Move(Snake *snake, Point *FruitLocation){
    bit EatFruitFlag = 0;
    bit Game_Over_Falg = 0;
    Point elemNew;

    switch(snake_towds){
        // 向上:
        case(TOWDS_UP):	
            // 判断是否会撞墙导致游戏结束
            if(snake->front->Location.Y == 0){
                Game_Over_Falg = 1;
                Game_Over();
            }else{
                Key_Scan();
                // 判断是否会吃到果实
                if((snake->front->Location.Y == FruitLocation->Y + 2) &&
                   (snake->front->Location.X == FruitLocation->X)){
                    // 吃到果实
                    EatFruitFlag = 1;
                }
                elemNew.X = snake->front->Location.X;
                elemNew.Y = snake->front->Location.Y - 2;

                // 判断是否会咬死自己导致游戏结束,.但是前提条件是蛇的长度大于3
                if(((snake->length) >= 4) && (Snake_Check_Dead(snake, &elemNew) == 1)){
                    // 检查蛇是否咬死自己,如果会咬死自己上面的函数会返回1
                    Game_Over_Falg = 1;
                    Game_Over();
                }
            }
            break;
        // 向下:
        case(TOWDS_DOWN):
            // 判断是否会撞墙导致游戏结束
            if (snake->front->Location.Y == 62){
                Game_Over_Falg = 1;
                Game_Over();
            }else{
                Key_Scan();
                // 判断是否会吃到果实
                if((snake->front->Location.Y == FruitLocation->Y - 2) &&
                   (snake->front->Location.X == FruitLocation->X)){
                    // 吃到果实
                    EatFruitFlag = 1;
                }
                elemNew.X = snake->front->Location.X;
                elemNew.Y = snake->front->Location.Y + 2;

                // 判断是否会咬死自己导致游戏结束,.但是前提条件是蛇的长度大于3
                if(((snake->length) >= 4) && (Snake_Check_Dead(snake, &elemNew) == 1)){
                    // 检查蛇是否咬死自己,如果会咬死自己上面的函数会返回1
                    Game_Over_Falg = 1;
                    Game_Over();
                }
            }
            break;
        // 向左:
        case(TOWDS_LEFT):
            // 判断是否会撞墙导致游戏结束
            if((snake->front->Location.X == 0 ) || 
               (snake->front->Location.X == 128)){
                Game_Over_Falg = 1;
                Game_Over();
            }else{
                Key_Scan();
                // 判断是否会吃到果实
                if((snake->front->Location.Y == FruitLocation->Y) &&
                   (snake->front->Location.X == FruitLocation->X + 2)){
                    // 吃到果实
                    EatFruitFlag = 1;
                }
                elemNew.X = snake->front->Location.X - 2;
                elemNew.Y = snake->front->Location.Y;

                // 判断是否会咬死自己导致游戏结束,.但是前提条件是蛇的长度大于3
                if(((snake->length) >= 4) && (Snake_Check_Dead(snake, &elemNew) == 1)){
                    // 检查蛇是否咬死自己,如果会咬死自己上面的函数会返回1
                    Game_Over_Falg = 1;
                    Game_Over();
                }
            }
            break;
        // 向右
        case(TOWDS_RIGHT):
            // 判断是否会撞墙导致游戏结束
            if((snake->front->Location.X == 126) || 
               (snake->front->Location.X == 254)){
                Game_Over_Falg = 1;
                Game_Over();
            }else{
                Key_Scan();
                // 判断是否会吃到果实
                if((snake->front->Location.Y == FruitLocation->Y) &&
                   (snake->front->Location.X == FruitLocation->X - 2)){
                    // 吃到果实
                    EatFruitFlag = 1;
                }
                elemNew.X = snake->front->Location.X + 2;
                elemNew.Y = snake->front->Location.Y;

                // 判断是否会咬死自己导致游戏结束,.但是前提条件是蛇的长度大于3
                if(((snake->length) >= 4) && (Snake_Check_Dead(snake, &elemNew) == 1)){
                    // 检查蛇是否咬死自己,如果会咬死自己上面的函数会返回1
                    Game_Over_Falg = 1;	
                    Game_Over();
                }
            }
            break;
        default:
            break;
    }

    // 判断是否游戏结束
    if(Game_Over_Falg == 1){
        FruitLocation->Y = 100;  // 正常情况下Y最大62
        elemNew.X = elemNew.Y = 0;  // 其实此处赋值也是无效的
 
        return elemNew;  // 其实这里返回也是无效的,因为返回之后用不到这个数据
    }else{
        Key_Scan();  // 按键扫描函数

        // 首先根据蛇头的朝向在蛇头插入新结点
        Insert_At_Snake_Front(snake, &elemNew);  // 并不画出来,只是在数据层面加上蛇头

        // EatFruitFlag = 1 说明吃到了果实
        if (EatFruitFlag == 1){
            FruitLocation->Y = 200;  // 正常情况下Y最大62
            elemNew.X = elemNew.Y = 0;  // 其实此处赋值也是无效的

            return elemNew;  // 其实这里返回也是无效的,因为返回之后用不到这个数据
        }else{
            // 释放掉一个尾节点
            return Remove_At_Snake_Rear(snake);
        }
    }
}
