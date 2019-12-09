#include "lcd12864.h"
#include "snake.h"
//��ʼ���ڴ�غ����Լ���������ɺ���
#include <stdlib.h>

//---�Ĵ���λ����---//
sbit Key_Up = P0^0;
sbit Key_Down = P0^1;
sbit Key_Left = P0^2;
sbit Key_Right = P0^3;
sbit Key_Pause = P0^4;
sbit Key_Enter = P0^5;
sbit Key_Return = P0^6;

//---�궨�峣��---//
#define SYS_EASY 1
#define SYS_HARD 2
#define SYS_TIPS 3

#define TOWDS_UP 0
#define TOWDS_DOWN 1
#define TOWDS_RIGHT 2
#define TOWDS_LEFT 3

//---ȫ�ֱ�������---//
unsigned char snake_towds = TOWDS_RIGHT;						//��ͷ�ĳ���,Ĭ�ϳ��� TOWDS_RIGHT
unsigned int snake_speed = 30000;								//С���ƶ��ٶ�
Snake snake;
Point snakeRear;												//��β������
Point FruitLocation;											//��ʵ������
unsigned char InitSeed = 0;										//��ʼ�����ӣ����ڲ��������
unsigned char FruitFlag = 0;									//��ʾ��ʵ�Ƿ񻹴���
bit PressRet = 0;												//Return�����Ƿ񱻰���
bit PressPause = 0;												//Pause�����Ƿ񱻰���

//��Ϸ��غ���
void Timer_Set();												//��ʱ�����ú���
void delay(unsigned int time);									//��ʱ����
void Key_Scan();												//����ɨ�躯��
void Inv_Key_Scan();											//����ģʽ�µİ���ɨ��
unsigned char Game_Menu();										//��Ϸ��ʼ����˵�
void Game_Over();												//��Ϸʧ�ܺ����
bit Snake_Check_Dead(Snake *snake, Point *elemNew);				//������Ƿ�ҧ���Լ�
Point Snake_Move(Snake *snake, Point *FruitLocation);			//����ǰ�ƶ�һ����λ
void LCD_Draw_Snake(Snake *snake, Point *ClearLocation);		//������λ�û�һ����
void LCD_Draw_Fruit(Point *FruitLocation, Snake *snake_tempt);	//�����λ�û�����ʵ
void LCD_Write_Score(unsigned char Score);						//д�����
void Sys_Easy();												//��Ϸ�ļ�ģʽ
void Sys_Hard();												//��Ϸ������ģʽ
void Sys_Tips();												//��Ϸ����ʿ����

//��ʱ�����ú���
void Timer_Set(){
	EA = 1;														//�������ж�����
	ET0 = 1;													//�򿪶�ʱ��0����
	TMOD = 0x01;												//���ö�ʱ���Ĺ�����ʽ�Ĵ���TCON
	TH0 = 0;													//���ö�ʱ����ֵ
	TL0 = 0;
	TR0 = 1;													//�����TCON�Ĵ��������TR0��������ʱ��
}

//��ʱ�� 0 ���жϷ�����
void TimerInterrupt() interrupt 1{
	TH0 = 0;
	TL0 = 0;
	++InitSeed;													//��ʼ����������
}

//��ʱ����
void delay(unsigned int time){				
	while(--time){
		Key_Scan();
	}
}

//����ɨ�躯��
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
			} else {
				LCD_Clear_DDRAM();
				while(!Key_Pause);
				PressPause = 0;
			}
		}
	}
}

//����ģʽ�İ���ɨ�躯��
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
			} else {
				LCD_Clear_DDRAM();
				while(!Key_Pause);
				PressPause = 0;
			}
		}
	}
}
// ת������
void change_Screen(){
	LCD_Clear_DDRAM();
	LCD_Write_Strs(0x88, " Please wait...", 15);
	LCD_Draw_Clear();
	LCD_Clear_DDRAM();
}

//��ʼ��
void init(){
	/* 
		#define _MALLOC_MEM_ xdata
		51����û�в���ϵͳ����Ҫ����Ϊ��̬�ڴ�����䲢�����
		��Ҫͷ�ļ�<stdlib.h>�������init_mempool����ʼ���ڴ��
	*/
	unsigned char _MALLOC_MEM_ memoryTempt[100];				//�������һ������,���ڳ�ʼ���ڴ�أ�xdata����
	init_mempool(memoryTempt, sizeof(memoryTempt));				//��ʼ���ڴ��
	
	LCD_Init();													//��ʼ��LCD, 8λ����
	Timer_Set();												//��ʼ����ʱ��0
	Snake_Init(&snake);											//��ʼ����
}

// ��ϵͳ
void Sys_Easy(){
	Snake_Init(&snake);											//��ʼ����
	change_Screen();
	while(!PressRet){
		delay(snake_speed);										//��ʱ,�������ߵĿ���
		
		snakeRear = Snake_Move(&snake, &FruitLocation);			//�����ƶ�һ��,����ȡ����β������
		
		if(FruitLocation.Y == 100){
			FruitLocation.Y = 99;								//˵��Game_Over
			continue;											//������䲻ִ��,ֱ�ӽ�����һ��ѭ��
		}else if(FruitLocation.Y == 200){
			FruitFlag = 0;										//˵���Ե���ʵ�ˣ���չ�ʵ���ڱ�־λ
			snake_speed -= 300;									//ÿ�Ե�һ����ʵ,�ߵ��ٶȼӿ�
		}
		
																//�ж��Ƿ�Ҫ������һ����ʵ
		if(FruitFlag == 0){
			//Key_Scan();											//����ɨ�躯��
			LCD_Draw_Fruit(&FruitLocation, &snake);				//�����µĹ�ʵ,��������
			//Key_Scan();											//����ɨ�躯��
			
			continue;											//������β���ٲ���,������ͷҲ����Ҫ�ػ�,����������䲻ִ��,ֱ�ӽ�����һ��ѭ��
		}
		
		//Key_Scan();												//����ɨ�躯��
		
		LCD_Draw_Snake(&snake, &snakeRear);						//�����ߵ�λ��
	}
	PressRet = 0;
}

// ����ϵͳ
void Sys_Hard(){
	Snake_Init(&snake);											//��ʼ����
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

// ��ʾϵͳ
void Sys_Tips(){
	LCD_Clear_DDRAM();
	LCD_Write_Strs(0x80, "|-----Tips-----|", 16); 	 	// The-OdD-SnAKee 
    LCD_Write_Strs(0x90, "| fmw-lyx-zgt  |", 16);		//      easy
    LCD_Write_Strs(0x88, "|--------------|", 16);		//   -> hard
    LCD_Write_Strs(0x98, "     ->ok", 9);				//      tips

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




//������λ�û���������
void LCD_Draw_Snake(Snake *snake, Point *ClearLocation){
	//����������
	SnakeNode *p;	//����һ��ָ��,���ڱ���������
	
	for(p=snake->front; p!=NULL; p=p->next){
		//���ݵ�ǰ�ڵ�����껭��������
		LCD_Draw_Square(p->Location.X, p->Location.Y, 1);
	}
	
	//�ٸ��ݲ���λ�õ��������һ��������
	//˵��û�гԵ���ʵ,��ʱ������β
	if(FruitFlag != 0){
		LCD_Draw_Square(ClearLocation->X, ClearLocation->Y, 0);
	}
}

//������Ϸ��ʼ����
unsigned char Game_Menu(){
	unsigned char i = 0;
	unsigned char ch = SYS_HARD;
	unsigned char code str_point[] = "   ->";
	unsigned char code str_point_null[] = "     ";
	change_Screen();
	
	//д��˵�
	LCD_Write_Strs(0x80, " The-OdD-SnAKee", 14);
	LCD_Write_Strs(0x90, "      easy", 10);
	LCD_Write_Strs(0x88, "   -> hard", 10);
	LCD_Write_Strs(0x98, "      tips", 10);
	
	while(1){
		//ѡ�� easy
		if(ch == SYS_EASY){
			// �����޷�Ӧ
			// ������Ӧ
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
		// ѡ�� hard
		if(ch == SYS_HARD){
			// ������Ӧ
			if(Key_Up == 0){
				Delay_1ms(5);
				if(Key_Up == 0){
					ch = SYS_EASY;
					LCD_Write_Strs(0x88, str_point_null, 5);
					LCD_Write_Strs(0x90, str_point, 5);
					while(!Key_Up);
				}
			}
			// ������Ӧ
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
		// ѡ�� tips
		if(ch == SYS_TIPS){
			// ������Ӧ
			// �����޷�Ӧ
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
		//ֻ�е�����"��ʼ"��ʱ,��Ϸ�ŻῪʼ
		if(Key_Enter == 0){
			Delay_1ms(5);
			if(Key_Enter == 0){			
				//�������������ʵ,����������������ö�ʱ����ȡ,������"��ʼ"�����������,֮�����Ӳ��ٸı�
				srand(InitSeed);//��ʼ������
				//��������֮��ʱ��0�Ϳ��Թص���
				TR0 = 0;
				while(!Key_Enter);
				//�˳���ѭ��
				return ch;
			}
		}
	}
}

//������Ϸʧ�ܵĻ���
void Game_Over(){
	unsigned int i = 0;//����forѭ��
	
	//�����Ϸ����,�����DDRAM
	LCD_Clear_DDRAM();

	//д��"Game_Over"
	LCD_Write_Strs(0x92, "GameOver", 8);
	
	//��д��"�÷�: "
	LCD_Write_Strs(0x8A, "score:", 6);
	
	//��д�����
	LCD_Write_Score(snake.length);//д������ĺ���
	
	while(1){
		//ֻ�е�����"�س�"��ʱ,��Ϸ�ŻῪʼ
		if(Key_Enter == 0 || Key_Return == 0){
			Delay_1ms(10);
			if(Key_Enter == 0 || Key_Return == 0){			
				//֮��������,���ٴγ�ʼ��
				Snake_Destroy(&snake);
				Snake_Init(&snake);
				
				//��չ�ʵ���ڵı�־
				FruitFlag = 0;
				
				//�������ó�ʼ����Ϊ����
				snake_towds = TOWDS_RIGHT;
				
				while(!Key_Enter || !Key_Return);
				//�˳���ѭ��
				PressRet = 1;
				break;
			}
		}
	}
}

//������Ƿ�ҧ���Լ���0��ʾ����ҧ��  1��ʾ��ҧ���Լ�
bit Snake_Check_Dead(Snake *snake, Point *elemNew){
	SnakeNode *p = snake->front->next->next;//�ʼָ��3��λ��,Ϊ��ͷ
	
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

//д������ĺ���
void LCD_Write_Score(unsigned char Score){
	Score -= 2;//������ʼ����2
	
	//д�����ʱ,Ҫ�жϷ�����λ��
	if(Score < 10){
		LCD_Write(MODE_DATA, '0');	//48������0
		LCD_Write(MODE_DATA, Score + '0');
	}else{
		unsigned char Score_H = Score / 10;
		unsigned char Score_L = Score % 10;
		
		LCD_Write(MODE_DATA, Score_H + '0');
		LCD_Write(MODE_DATA, Score_L + '0');
	}
}

//������ʵ�ĺ���
void LCD_Draw_Fruit(Point *FruitLocation, Snake *snake_tempt){
	//����һ��ָ��,����������
	SnakeNode *p;
	
	//����һ����־λ,���ڱ�ʾ���ɵĹ�ʵ�Ƿ���ߵ������غ�,1��ʾ�غ�
	bit FruitQualifiedFlag = 1;
	
	//�ж����ɵĹ�ʵ�Ƿ�ϸ�,���ϸ�Ļ���������һ���¹�ʵ
	while(FruitQualifiedFlag){
		InitSeed = rand();	//�²���һ�������
		FruitLocation->X = (InitSeed % 63) * 2;	//X �����124,���ұ�����ż��
		FruitLocation->Y = (InitSeed % 31) * 2;	//Y �����60,���ұ�����ż��
		
		for(p=snake_tempt->front; p!=NULL; p=p->next){
			if((p->Location.X == FruitLocation->X) && 
				 (p->Location.Y == FruitLocation->Y)){
				break;
			}
		}
		
		//��ѭ��֮��,�����ʱ p Ϊ NULL,˵��û���ظ�,���p��ΪNULL,˵�����ظ�
		if(p == NULL){
			FruitQualifiedFlag = 0;
		}
	}
	
	LCD_Draw_Square(FruitLocation->X, FruitLocation->Y, 1);
	
	FruitFlag = 1;//���ù�ʵ��,��־λ��λ
}

//�����ߵ��߶��ĺ���
Point Snake_Move(Snake *snake, Point *FruitLocation){
	bit EatFruitFlag = 0;
	bit Game_Over_Falg = 0;
	Point elemNew;
	
	switch(snake_towds){
		//����:
		case(TOWDS_UP):	
			//�ж��Ƿ��ײǽ������Ϸ����
			if(snake->front->Location.Y == 0){
				Game_Over_Falg = 1;
				Game_Over();
			}else{
				Key_Scan();
				//�ж��Ƿ��Ե���ʵ
				if((snake->front->Location.Y == FruitLocation->Y + 2) &&
				    (snake->front->Location.X == FruitLocation->X)){
					//�Ե���ʵ
					EatFruitFlag = 1;
				}
				elemNew.X = snake->front->Location.X;
				elemNew.Y = snake->front->Location.Y - 2;

				//�ж��Ƿ��ҧ���Լ�������Ϸ����,.����ǰ���������ߵĳ��ȴ���3
				if(((snake->length) >= 4) && (Snake_Check_Dead(snake, &elemNew) == 1)){
					//������Ƿ�ҧ���Լ�,�����ҧ���Լ�����ĺ����᷵��1
					Game_Over_Falg = 1;
					Game_Over();
				}
			}
			break;
		//����:
		case(TOWDS_DOWN):
			//�ж��Ƿ��ײǽ������Ϸ����
			if (snake->front->Location.Y == 62){
				Game_Over_Falg = 1;
				Game_Over();
			}else{
				Key_Scan();
				//�ж��Ƿ��Ե���ʵ
				if((snake->front->Location.Y == FruitLocation->Y - 2) &&
				    (snake->front->Location.X == FruitLocation->X)){
					//�Ե���ʵ
					EatFruitFlag = 1;
				}
				elemNew.X = snake->front->Location.X;
				elemNew.Y = snake->front->Location.Y + 2;
				
				//�ж��Ƿ��ҧ���Լ�������Ϸ����,.����ǰ���������ߵĳ��ȴ���3
				if(((snake->length) >= 4) && (Snake_Check_Dead(snake, &elemNew) == 1)){
					//������Ƿ�ҧ���Լ�,�����ҧ���Լ�����ĺ����᷵��1
					Game_Over_Falg = 1;
					Game_Over();
				}
			}
			break;
		//����:
		case(TOWDS_LEFT):
			//�ж��Ƿ��ײǽ������Ϸ����
			if((snake->front->Location.X == 0 ) || 
			    (snake->front->Location.X == 128)){
				Game_Over_Falg = 1;
				Game_Over();
			}else{
				Key_Scan();
				//�ж��Ƿ��Ե���ʵ
				if((snake->front->Location.Y == FruitLocation->Y) &&
				    (snake->front->Location.X == FruitLocation->X + 2)){
					//�Ե���ʵ
					EatFruitFlag = 1;
				}
				elemNew.X = snake->front->Location.X - 2;
				elemNew.Y = snake->front->Location.Y;
				
				//�ж��Ƿ��ҧ���Լ�������Ϸ����,.����ǰ���������ߵĳ��ȴ���3
				if(((snake->length) >= 4) && (Snake_Check_Dead(snake, &elemNew) == 1)){
					//������Ƿ�ҧ���Լ�,�����ҧ���Լ�����ĺ����᷵��1
					Game_Over_Falg = 1;
					Game_Over();
				}
			}
			break;
		//����
		case(TOWDS_RIGHT):
			//�ж��Ƿ��ײǽ������Ϸ����
			if((snake->front->Location.X == 126) || 
			    (snake->front->Location.X == 254)){
				Game_Over_Falg = 1;
				Game_Over();
			}else{
				Key_Scan();
				//�ж��Ƿ��Ե���ʵ
				if((snake->front->Location.Y == FruitLocation->Y) &&
				    (snake->front->Location.X == FruitLocation->X - 2)){
					//�Ե���ʵ
					EatFruitFlag = 1;
				}
				elemNew.X = snake->front->Location.X + 2;
				elemNew.Y = snake->front->Location.Y;
				
				//�ж��Ƿ��ҧ���Լ�������Ϸ����,.����ǰ���������ߵĳ��ȴ���3
				if(((snake->length) >= 4) && (Snake_Check_Dead(snake, &elemNew) == 1)){
					//������Ƿ�ҧ���Լ�,�����ҧ���Լ�����ĺ����᷵��1
					Game_Over_Falg = 1;	
					Game_Over();
				}
			}
			break;
		default:
			break;
	}
	
	//�ж��Ƿ���Ϸ����
	if(Game_Over_Falg == 1){
		FruitLocation->Y = 100;//���������Y���62
		elemNew.X = elemNew.Y = 0;//��ʵ�˴���ֵҲ����Ч��
		
		return elemNew;//��ʵ���ﷵ��Ҳ����Ч��,��Ϊ����֮���ò����������
	}else{
		Key_Scan();//����ɨ�躯��
		
		//���ȸ�����ͷ�ĳ�������ͷ�����½��
		Insert_At_Snake_Front(snake, &elemNew);//����������,ֻ�������ݲ��������ͷ
		
		//EatFruitFlag = 1 ˵���Ե��˹�ʵ
		if (EatFruitFlag == 1){
			FruitLocation->Y = 200;//���������Y���62
			elemNew.X = elemNew.Y = 0;//��ʵ�˴���ֵҲ����Ч��
			
			return elemNew;//��ʵ���ﷵ��Ҳ����Ч��,��Ϊ����֮���ò����������
		}else{
			//�ͷŵ�һ��β�ڵ�
			return Remove_At_Snake_Rear(snake);
		}
	}
}
