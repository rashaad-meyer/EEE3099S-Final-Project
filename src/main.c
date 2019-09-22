/*
******************************************************************************
File:     main.c
Info:     Generated by Atollic TrueSTUDIO(R) 9.3.0   2019-09-20
The MIT License (MIT)
Copyright (c) 2019 STMicroelectronics
Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
******************************************************************************
*/

/* Includes */
#include "stm32f0xx.h"

/* Private macro */
/* Private variables */
int i = 0;
int pwm = 50;
int increase = 1;
int mode = 0;
/* Private function prototypes */
/* Private functions */
void init_push_buttons(void);
void init_leds(void);
void init_EXTI(void);
void init_PWM(void);
void init_TIM6(void);
void TIM6_IRQHandler(void);
void EXTI0_1_IRQHandler(void);
void EXTI2_3_IRQHandler(void);
void EXTI4_15_IRQHandler(void);
void forward(void);
void turn_right(void);
void turn_left(void);
void stop(void);
/**
**===========================================================================
**
**  Abstract: main program
**
**===========================================================================
*/
int main(void)	{

	init_push_buttons();
	init_leds();
	init_EXTI();
	init_PWM();
	init_TIM6();
	/* TODO - Add your application code here */
	int j = 0;
	/* Infinite loop */
	while (1)	{
		j++;
		if(~GPIOA->IDR & (GPIO_IDR_5|GPIO_IDR_6|GPIO_IDR_7)){

			GPIOB->ODR &= ~GPIO_ODR_7;
			GPIOB->ODR |= GPIO_ODR_7;

		}
		else{

			GPIOB->ODR &= ~GPIO_ODR_7;

		}
	}
}

void init_push_buttons(void) {

	RCC->AHBENR |= RCC_AHBENR_GPIOAEN; //enable clock for push-buttons
	// set pins to inputs
	GPIOA->MODER &= ~GPIO_MODER_MODER0; //set PA0 to input
	GPIOA->MODER &= ~GPIO_MODER_MODER1; //set PA1 to input
	GPIOA->MODER &= ~GPIO_MODER_MODER2; //set PA2 to input
	GPIOA->MODER &= ~GPIO_MODER_MODER3; //set PA3 to input
	GPIOA->MODER &= ~GPIO_MODER_MODER7; //set PA7 to input

	GPIOA->MODER &= ~GPIO_MODER_MODER5;	//set PB5 to input
	GPIOA->MODER &= ~GPIO_MODER_MODER6;	//set PB6 to input

	// enable pull-up resistors
	GPIOA->PUPDR |= GPIO_PUPDR_PUPDR0_0; //enable pull up for PA0
	GPIOA->PUPDR |= GPIO_PUPDR_PUPDR1_0; //enable pull up for PA1
	GPIOA->PUPDR |= GPIO_PUPDR_PUPDR2_0; //enable pull up for PA2
	GPIOA->PUPDR |= GPIO_PUPDR_PUPDR3_0; //enable pull up for PA3
	GPIOA->PUPDR |= GPIO_PUPDR_PUPDR7_0; //enable pull up for PA7

	GPIOA->PUPDR |= GPIO_PUPDR_PUPDR5_0; //enable pull up for PB5
	GPIOA->PUPDR |= GPIO_PUPDR_PUPDR6_0; //enable pull up for PB6

}

void init_leds(void)	{

	RCC->AHBENR |= RCC_AHBENR_GPIOBEN; //enable clock for push-buttons
	//set pins to outputs
	GPIOB->MODER &= 0xFFFFFF00;
	GPIOB->MODER |= (GPIO_MODER_MODER7_0|GPIO_MODER_MODER0_0|
			GPIO_MODER_MODER1_0|GPIO_MODER_MODER2_0|GPIO_MODER_MODER3_0|
			GPIO_MODER_MODER4_0|GPIO_MODER_MODER5_0|GPIO_MODER_MODER6_0);
	GPIOB->ODR &= ~GPIO_ODR_7;

}

void init_TIM6 (void)	{

	RCC -> APB1ENR |= RCC_APB1ENR_TIM6EN; // ENABLE TIM6 BUS CLK
	TIM6 -> PSC = 1000;
	TIM6 -> ARR = 999; // OVERFLOW OCCURS EVERY APPROX 0.5 SECOND
	TIM6 -> DIER |= TIM_DIER_UIE; // ENABLE UPDATE INTERRUPT
	TIM6 -> CR1 |= TIM_CR1_ARPE; // AUTO RELOAD PRELOAD BUFFER ENABLE
	TIM6 -> CR1 |= TIM_CR1_CEN; // START THE TIM6 COUNTER
	NVIC_EnableIRQ(TIM6_DAC_IRQn); // ENABLE TIM6 BUS

}

void TIM6_IRQHandler (void)	{

	TIM6 -> SR &= ~TIM_SR_UIF;// EXIT TIM6 INTERRUPT EVENT
	// User Interrupt Service Routine Here

	if (mode == 0){

		stop();

	}
	else if (mode == 1){

		forward();

	}
	else if (mode == 2){

		turn_left();

	}
	else if (mode == 3){

		turn_right();

	}

	/*
	if (increase == 1){
		if (pwm < 100)	{
			pwm+=5;
			//TIM2->CCR1 = pwm * 80; // Red = 20%
			//TIM2->CCR2 = (100 - pwm) * 80; // Green = 90%
			TIM2->CCR3 = pwm * 80; // Red = 20%
			TIM2->CCR4 = (100 - pwm) * 80; // Green = 90%
		}
		else{
			increase = 0;
		}
	}
	else	{
		if (pwm > 0)	{
			pwm-=5;
			//TIM2->CCR1 = pwm * 80; // Red = 20%
			//TIM2->CCR2 = (100 - pwm) * 80; // Green = 90%
			TIM2->CCR3 = pwm * 80; // Red = 20%
			TIM2->CCR4 = (100 - pwm) * 80; // Green = 90%
		}
		else	{
			increase = 1;
		}
	}
	*/
}

void init_PWM(void) {

	RCC->AHBENR |= RCC_AHBENR_GPIOBEN;
	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;

	GPIOB->MODER |= GPIO_MODER_MODER10_1; 	// PB10 = AF
	GPIOB->MODER |= GPIO_MODER_MODER11_1; 	// PB11 = AF

	GPIOB->AFR[1] |= (2 << (4*(10 - 8))); // PB10_AF = AF2 (ie: map to TIM2_CH3)
	GPIOB->AFR[1] |= (2 << (4*(11 - 8))); // PB11_AF = AF2 (ie: map to TIM2_CH4)

	TIM2->ARR = 7999;  // f = 1 KHz
	// specify PWM mode: OCxM bits in CCMRx. We want mode 1
	//TIM2->CCMR1 |= (TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1); // PWM Mode 1
	//TIM2->CCMR1 |= (TIM_CCMR1_OC2M_2 | TIM_CCMR1_OC2M_1); // PWM Mode 1
	TIM2->CCMR2 |= (TIM_CCMR2_OC3M_2 | TIM_CCMR2_OC3M_1); // PWM Mode 1
	TIM2->CCMR2 |= (TIM_CCMR2_OC4M_2 | TIM_CCMR2_OC4M_1); // PWM Mode 1
	// set PWM percantages
	//TIM2->CCR1 = 0 * 80; // Red = 20%
	//TIM2->CCR2 = 0 * 80; // Green = 90%
	TIM2->CCR3 = 0 * 80; // Red = 20%
	TIM2->CCR4 = 0 * 80; // Green = 90%

	// enable the OC channels
	//TIM2->CCER |= TIM_CCER_CC1E;
	//TIM2->CCER |= TIM_CCER_CC2E;
	TIM2->CCER |= TIM_CCER_CC3E;
	TIM2->CCER |= TIM_CCER_CC4E;

	TIM2->CR1 |= TIM_CR1_CEN; // counter enable

}

void init_EXTI (void)	{

	RCC -> APB2ENR |= RCC_APB2ENR_SYSCFGCOMPEN; // enable clock for the sys
	// conf controller
	SYSCFG -> EXTICR[0] |= SYSCFG_EXTICR1_EXTI0_PA; // map PA0 to EXTI0
	SYSCFG -> EXTICR[0] |= SYSCFG_EXTICR1_EXTI1_PA; // map PA1 to EXTI1
	SYSCFG -> EXTICR[0] |= SYSCFG_EXTICR1_EXTI2_PA; // map PA2 to EXTI2
	SYSCFG -> EXTICR[0] |= SYSCFG_EXTICR1_EXTI3_PA; // map PA3 to EXTI3
	//SYSCFG -> EXTICR[1] |= SYSCFG_EXTICR2_EXTI7_PA; // map PA7 to EXTI7

	EXTI -> IMR |= EXTI_IMR_MR0; // unmask external interrupt 0
	EXTI -> IMR |= EXTI_IMR_MR1; // unmask external interrupt 1
	EXTI -> IMR |= EXTI_IMR_MR2; // unmask external interrupt 2
	EXTI -> IMR |= EXTI_IMR_MR3; // unmask external interrupt 3
	//EXTI -> IMR |= EXTI_IMR_MR7; // unmask external interrupt 4

	EXTI -> FTSR |= EXTI_FTSR_TR0; // trigger on falling edge
	EXTI -> FTSR |= EXTI_FTSR_TR1; // trigger on falling edge
	EXTI -> FTSR |= EXTI_FTSR_TR2; // trigger on falling edge
	EXTI -> FTSR |= EXTI_FTSR_TR3; // trigger on falling edge
	//EXTI -> FTSR |= EXTI_FTSR_TR7; // trigger on falling edge

	NVIC_EnableIRQ(EXTI0_1_IRQn); // enable EXTI0 to EXTI1 interrupt in the NVIC
	NVIC_EnableIRQ(EXTI2_3_IRQn); // enable EXTI2 to EXTI3 interrupt in the NVIC
	//NVIC_EnableIRQ(EXTI4_15_IRQn); // enable EXTI4 to EXTI15 interrupt line in the NVIC

}

void EXTI0_1_IRQHandler(void)	{

	EXTI -> PR |= EXTI_PR_PR0; // clear the interrupt pending bit
	// User Interrupt Service Routine Here
	/*
	if ( i < 64 )	{
		GPIOB->ODR &= 0xFF00;
		GPIOB->ODR |= i<<2;
		i++;
	}
	else {
		GPIOB->ODR &= 0xFF00;
		i = 0;
	}
	*/
	//forward();
	mode = 1;

}

void EXTI2_3_IRQHandler(void)	{

	EXTI -> PR |= EXTI_PR_PR2; // clear the interrupt pending bit
	// User Interrupt Service Routine Here
	/*if ( i > 0 ){
		GPIOB->ODR &= 0xFF00;
		GPIOB->ODR |= i<<2;
		i--;
	}
	else{
		GPIOB->ODR &= 0xFFFF;
		i = 63;
	}*/
	//turn_right();
	if (mode == 3){

		mode = 2;

	}
	else {

		mode = 3;

	}
}
/*
void EXTI4_15_IRQHandler(void)	{

	EXTI -> PR |= EXTI_PR_PR7; // clear the interrupt pending bit
	// User Interrupt Service Routine Here
	//TIM2->CCR1 = pwm * 80; // Red = 20%
	//TIM2->CCR2 = (100 - pwm) * 80; // Green = 90%
	//TIM2->CCR3 = pwm * 80; // Red = 20%
	//TIM2->CCR4 = (100 - pwm) * 80; // Green = 90%
	//stop();
	mode = 0;

}*/

void forward(void)	{

	if ( pwm < 100 ){

		pwm += 10;
		//TIM2->CCR1 = pwm * 80; // Red = 20%
		//TIM2->CCR2= (100 - pwm) * 80; // Green = 90%


	}
	GPIOB->ODR &= 0xFF00;
	GPIOB->ODR |= 0b0110;
	TIM2->CCR3 = pwm * 80; // Red = 20%
	TIM2->CCR4 = pwm * 80; // Green = 90%

}

void stop(void)	{

	if ( pwm > 0 ){
		pwm -= 10;
	}
	GPIOB->ODR &= 0xFF00;
	GPIOB->ODR |= 0b0110;
	TIM2->CCR3 = pwm * 80; // Red = 20%
	TIM2->CCR4 = pwm * 80; // Green = 90%

}

void turn_left(void)	{

	if ( pwm < 100 )	{

		pwm += 10;
		//TIM2->CCR1 = (100 - pwm) * 80; // Red = 20%
		//TIM2->CCR2= pwm * 80; // Green = 90%
		TIM2->CCR3 = pwm * 80; // Red = 20%
		TIM2->CCR4 = pwm * 80; // Green = 90%

	}

	GPIOB->ODR &= 0xFF00;
	GPIOB->ODR |= 0b1010;

}

void turn_right(void)	{

	if ( pwm < 100 )	{

		pwm += 10;
		//TIM2->CCR1 = (100 - pwm) * 80; // Red = 20%
		//TIM2->CCR2= pwm * 80; // Green = 90%
		TIM2->CCR3 = pwm * 80; // Red = 20%
		TIM2->CCR4 = pwm * 80; // Green = 90%

	}

	GPIOB->ODR &= 0xFF00;
	GPIOB->ODR |= 0b0101;

}
