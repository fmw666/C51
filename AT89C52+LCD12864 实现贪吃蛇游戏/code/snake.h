/********************************************
 *      snake.h 宏定义常量 
 ********************************************/
#define OK 1
#define ERROR 0
#define true 1
#define false 0
#define OVERFLOW -1

/******************************************** 
 *      snake.h 变量申明
 *      用于蛇头插入结点结果返回
 *      返回状态有两种：OK=1、ERROR=0
 ********************************************/
typedef bit Status;


/********************************************
 *      这里的数据域的元素类型是两个坐标
 *      坐标点Point(X,Y)
 *      用于小蛇数据域和食物坐标点
 ********************************************/
typedef struct Point{
    unsigned char X;
    unsigned char Y;
}Point;

/********************************************
 *      蛇结点声明
 *      数据域：结点坐标点Location->Point(X,Y)
 *      指针域：下一个结点SnakeNode地址
 ********************************************/
typedef struct SnakeNode{
    Point Location;
    struct SnakeNode *next;
}SnakeNode, *SnakePtr;

/********************************************
 *      蛇声明
 *      蛇头指针：front->指向下一个结点地址
 *      蛇尾指针：rear->指向下一个结点地址(NULL)
 *      蛇的长度：length
 ********************************************/
typedef struct Snake{
    SnakePtr front;
    SnakePtr rear;
    unsigned char length;
}Snake;

/********************************************
 *      有关小蛇操作的函数声明
 ********************************************/
// 初始化一条蛇
void Snake_Init(Snake *snake);
// 销毁一条蛇
void Snake_Destroy(Snake *snake);
// 在蛇头处插入一个结点
Status Insert_At_Snake_Front(Snake *snake, Point *elem);
// 在蛇尾处释放一个结点,返回擦除位置的坐标
Point Remove_At_Snake_Rear(Snake *snake);
