#include "dht11.h"
#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_tim.h"
#include "stm32f4xx_usart.h"
#include "misc.h"
#include "stdio.h"
#include "math.h"
#include "LCD.h"


GPIO_InitTypeDef  GPIO_InitStructure;
TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

void DHT11initTIM4(void){
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
	TIM_TimeBaseStructure.TIM_Period = 84000000-1;
	TIM_TimeBaseStructure.TIM_Prescaler =84;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);
	TIM_Cmd(TIM4 ,ENABLE);
}

void DHT11initGPIOasOutput(void){

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
}


void DHT11initGPIOasInput(void){
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
}


void DHT11_delay_us(int us){
	TIM4->CNT = 0;
	while((TIM4->CNT) <= us);
}



void DHT11Read(u8 *Rh,u8 *RhDec,u8 *Temp,u8 *TempDec, u8 *ChkSum){
	u8 temp;
	u8 j;
	u8 i;
	u8 Value[5]={0x00,0x00,0x00,0x00,0x00};

	DHT11initGPIOasOutput();
	GPIO_ResetBits(GPIOD,GPIO_Pin_6);
	DHT11_delay_us(18000);
	GPIO_SetBits(GPIOD,GPIO_Pin_6);
	DHT11_delay_us(40);
	DHT11initGPIOasInput();

	while(!GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_6)){}

	while(GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_6)){}

	for (j = 0; j < 5; ++j) {
		for (i = 0; i < 8; ++i) {
			while(!GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_6)){}
			TIM_SetCounter(TIM4,0);
			while(GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_6)){}

			temp=TIM_GetCounter(TIM4);
			if (temp<30) {
				Value[j]=Value[j]<<1;
			}
			else {
				Value[j]=Value[j]<<1;
				Value[j] =Value[j]+1;
			}
		}
	}

    *Rh=Value[0];
    *RhDec=Value[1];
    *Temp=Value[2];
    *TempDec=Value[3];
    *ChkSum=Value[4];
}
