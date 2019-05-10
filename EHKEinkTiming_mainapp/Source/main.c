/***************************************************************************//**
* \file main.c
* \version 1.0
*
* \brief
* Minimal new application template for 150MHz PSoC 6 devices. Debug is enabled 
* and platform clocks are set for high performance (144MHz CLK_FAST for CM4 
* core and 72MHz CLK_SLOW for CM0+) but with a peripheral-friendly CLK_PERI 
* frequency (72MHz).
*
********************************************************************************
* \copyright
* Copyright 2017-2019 Cypress Semiconductor Corporation
* SPDX-License-Identifier: Apache-2.0
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*******************************************************************************/

#include "cy_device_headers.h"
#include "cycfg.h"
#include "FreeRTOS.h"
#include "task.h"

extern void eInkTask(void *);

void blinkTask(void *arg)
{
	(void)arg;

    for(;;)
    {
    		vTaskDelay(500);
    		Cy_GPIO_Inv(LED_RED_PORT,LED_RED_PIN);
    }
}
void vApplicationStackOverflowHook( TaskHandle_t xTask,
                                    signed char *pcTaskName )
{
	printf("Task name=%s\n",pcTaskName);
}

int main(void)
{
    init_cycfg_all();
    __enable_irq();

	cy_stc_scb_uart_context_t UART_context;

	    Cy_SCB_UART_Init(UART_HW,&UART_config,&UART_context);
		Cy_SCB_UART_Enable(UART_HW);
		//setvbuf( stdin, NULL, _IONBF, 0 );

		Cy_SCB_UART_ClearRxFifo(UART_HW);
		printf("Started\n");


		BaseType_t rval;
  	rval = xTaskCreate( blinkTask,"blinkTask", configMINIMAL_STACK_SIZE,  0,  1, 0  );
	printf("Task 1 Created %d\n",rval);
  	rval = xTaskCreate( eInkTask,"eInkTask", 1024*8,  0,  1, 0  );
	printf("Task 2 Created %d\n",rval);
  	vTaskStartScheduler();
  	while(1);// Will never get here
}
