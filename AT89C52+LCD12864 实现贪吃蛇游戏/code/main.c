#include "game.h"
	
void main(){
	//初始化
	init();
	
	while(1){
		unsigned char ch = Game_Menu();//画出初始游戏界面
		if(ch == SYS_EASY)
			Sys_Easy();
		if(ch == SYS_HARD)
			Sys_Hard();
		if(ch == SYS_TIPS)
			Sys_Tips();
	}
}

