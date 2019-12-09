#include "snake.h"
#include <stdio.h>
#include <stdlib.h>

//初始化一条蛇
void Snake_Init(Snake *snake){
	//初始化的蛇有两个结点
	SnakeNode *pNew;
	
    pNew = (SnakeNode*)malloc(sizeof(SnakeNode));
	snake->front = snake->rear = pNew;
    snake->length = 1;
	
    pNew->next = NULL;
	pNew->Location.X = 2;
	pNew->Location.Y = 0;
	
	pNew = (SnakeNode*)malloc(sizeof(SnakeNode));
	pNew->next = NULL;
	pNew->Location.X = 0;
	pNew->Location.Y = 0;
	
	snake->front->next = pNew;
	snake->rear = pNew;
	snake->length = 2;
}

//销毁一条蛇
void Snake_Destroy(Snake *snake){
	//释放掉所有的结点
    SnakeNode *p = snake->front;
    SnakeNode *q;

    while(p){
        q = p->next;
        free(p);
        p = q;
    }
	
    snake->rear = snake->front = NULL;
    snake->length = 0;
}

//在蛇头处插入一个结点
Status Insert_At_Snake_Front(Snake *snake, Point *elem){
    SnakeNode *pNew;
	
    pNew = (SnakeNode*)malloc(sizeof(SnakeNode));//新开辟一个结点
	pNew->next = NULL;
    pNew->Location.X = elem->X;
	pNew->Location.Y = elem->Y;
	
	//之后将新结点插入到蛇头处
	pNew->next = snake->front;
	//调整蛇头指针
	snake->front = pNew;
    //调整蛇身的长度
    ++(snake->length);//长度加一

    return  OK;
}

//在蛇尾处释放一个结点
Point Remove_At_Snake_Rear(Snake *snake){
	Point ClearLocation;					//保存擦除的结点的坐标
    SnakeNode *p = snake->front;			//p最开始指向第一个结点
	
	while(p->next != snake->rear){
		p = p->next;
	}

	ClearLocation.X = snake->rear->Location.X;
	ClearLocation.Y = snake->rear->Location.Y;
	
    free(snake->rear);
	p->next = NULL;
	snake->rear = p;
	--(snake->length);

    return ClearLocation;
}

