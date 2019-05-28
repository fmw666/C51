#include <reg51.h>

sbit P1_0 = P1^0;
sbit P1_1 = P1^1;

void main()
{
	while(1){
		if(P1_0==0 && P1_1==0){
			P0 = 0xFF;
		}
		if(P1_0==0 && P1_1==1){
			P0 = 0xF0;
		}
		if(P1_0==1 && P1_1==0){
			P0 = 0xCC;
		}
		if(P1_0==1 && P1_1==1){
			P0 = 0x00;
		}
	}
}