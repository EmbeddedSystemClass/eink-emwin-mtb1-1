#include "cy_pdl.h"
#include "cycfg.h"
#include <stdio.h>

volatile uint32_t count;
cy_stc_scb_uart_context_t kitprog_context;

#if 0
void SysTick_Handler(void)
{
	count += 1;
}
#endif

void MyHander(void)
{
	count += 1;
}

int main(void)
{
	Cy_SCB_UART_Init(kitprog_HW,&kitprog_config,&kitprog_context);
	Cy_SCB_UART_Enable(kitprog_HW);
    /* Set up internal routing, pins, and clock-to-peripheral connections */
    init_cycfg_all();

    Cy_SysTick_Init ( CY_SYSTICK_CLOCK_SOURCE_CLK_CPU, 100000000/1000);
    Cy_SysTick_SetCallback(0,MyHander); // Slot 0
    Cy_SysTick_Enable();
    
//    SysTick_Config(SystemCoreClock/1000);

    /* enable interrupts */
    __enable_irq();

    for (;;)
    {
    		printf("Test count=%d\n",(int)count);
        Cy_GPIO_Inv(LED_RED_PORT, LED_RED_PIN); /* toggle the pin */
        Cy_SysLib_Delay(1000/*msec*/);
    }
}
