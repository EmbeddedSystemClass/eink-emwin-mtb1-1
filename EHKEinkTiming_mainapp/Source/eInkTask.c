/******************************************************************************
* File Name: main_cm4.c
*
* Version: 1.10
*
* Description: This file main application code for the CE223727 EmWin Graphics
*				library EInk Display.
*
* Hardware Dependency: CY8CKIT-028-EPD E-Ink Display Shield
*					   CY8CKIT-062-BLE PSoC6 BLE Pioneer Kit
*
*******************************************************************************
* Copyright (2018), Cypress Semiconductor Corporation. All rights reserved.
*******************************************************************************
* This software, including source code, documentation and related materials
* (“Software”), is owned by Cypress Semiconductor Corporation or one of its
* subsidiaries (“Cypress”) and is protected by and subject to worldwide patent
* protection (United States and foreign), United States copyright laws and
* international treaty provisions. Therefore, you may use this Software only
* as provided in the license agreement accompanying the software package from
* which you obtained this Software (“EULA”).
*
* If no EULA applies, Cypress hereby grants you a personal, nonexclusive,
* non-transferable license to copy, modify, and compile the Software source
* code solely for use in connection with Cypress’s integrated circuit products.
* Any reproduction, modification, translation, compilation, or representation
* of this Software except as specified above is prohibited without the express
* written permission of Cypress.
*
* Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND, 
* EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED 
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. Cypress 
* reserves the right to make changes to the Software without notice. Cypress 
* does not assume any liability arising out of the application or use of the 
* Software or any product or circuit described in the Software. Cypress does 
* not authorize its products for use in any products where a malfunction or 
* failure of the Cypress product may reasonably be expected to result in 
* significant property damage, injury or death (“High Risk Product”). By 
* including Cypress’s product in a High Risk Product, the manufacturer of such 
* system or application assumes all risk of such use and in doing so agrees to 
* indemnify Cypress against all liability.
*******************************************************************************/
/******************************************************************************
* This file contains the main application code for CE223727 that demonstrates
* controlling a EInk display using the EmWin Graphics Library.
* The project displays a start up screen with Cypress logo and text "CYPRESS EMWIN
* GRAPHICS DEMO EINK DISPLAY".  The project then displays the following screens
* in a loop
*
*	1. A screen showing various text alignments, styles and modes
*   2. A screen showing normal fonts
*	3. A screen showing bold fonts
*	4. A screen showing 2D graphics with horizontal lines, vertical lines
*		arcs and filled rounded rectangle
*	5. A screen showing 2D graphics with concentric circles and ellipses
*	6. A screen showing a text box with wrapped text
*
 *******************************************************************************/
#include "cycfg.h"
#include "GUI.h"
#include "pervasive_eink_hardware_driver.h"
#include "cy_eink_library.h"
#include "LCDConf.h"
#include "FreeRTOS.h"
#include "task.h"
#include <stdio.h>


/* Frame buffers */
uint8 imageBufferCache[CY_EINK_FRAME_SIZE] = {0};
uint8 imageBuffer[CY_EINK_FRAME_SIZE] = {0};

/* Reference to the bitmap image for the startup screen */
extern GUI_CONST_STORAGE GUI_BITMAP bmCypressLogoFullColor_PNG_1bpp;
extern GUI_CONST_STORAGE GUI_BITMAP bmIOTexpert_Logo_Vericalbw;



/*******************************************************************************
* Function Name: void UpdateDisplay(void)
********************************************************************************
*
* Summary: This function updates the display with the data in the display 
*			buffer.  The function first transfers the content of the EmWin
*			display buffer to the primary EInk display buffer.  Then it calls
*			the Cy_EINK_ShowFrame function to update the display, and then
*			it copies the EmWin display buffer to the Eink display cache buffer
*
* Parameters:
*  None
*
* Return:
*  None
*
* Side Effects:
*  It takes about a second to refresh the display.  This is a blocking function
*  and only returns after the display refresh
*
*******************************************************************************/
void UpdateDisplay(cy_eink_update_t updateMethod, bool powerCycle)
{
    /* Copy the EmWin display buffer to imageBuffer*/
    LCD_CopyDisplayBuffer(imageBuffer, CY_EINK_FRAME_SIZE);

 //   uint32_t startCount = xTaskGetTickCount();
    /* Update the EInk display */
    Cy_EINK_ShowFrame(imageBufferCache, imageBuffer, updateMethod, powerCycle);
 //   uint32_t endCount = xTaskGetTickCount();
    //printf("Update Display Time = %d\n",(int)(endCount - startCount));

    /* Copy the EmWin display buffer to the imageBuffer cache*/
    LCD_CopyDisplayBuffer(imageBufferCache, CY_EINK_FRAME_SIZE);
}

uint32_t runNumbers(cy_eink_update_t updateMode,bool powerMode,int num)
{
	char buff[128];
	uint32_t startTime;
	uint32_t rval=0;
	GUI_Clear();
    GUI_SetFont(GUI_FONT_13B_1);

    char *text="";

    switch(updateMode)
    {
    case CY_EINK_PARTIAL:
    		text = "Partial";
    		break;
    case CY_EINK_FULL_2STAGE:
    	text = "2Stage";
    	    		break;
    case CY_EINK_FULL_4STAGE:
    	text = "4Stage";
    	    		break;

    }


	sprintf(buff,"Mode =%s Power=%s",text,powerMode?"True":"False");
	GUI_SetTextAlign(GUI_TA_LEFT|GUI_TA_TOP);
	GUI_DispStringAt(buff,0,0);

	Cy_EINK_Power(CY_EINK_ON);
	UpdateDisplay(CY_EINK_FULL_4STAGE, powerMode);
    GUI_SetFont(GUI_FONT_D80);
    rval = 0;

	for(int i=0;i<num;i++)
    {
		GUI_SetTextAlign(GUI_TA_HCENTER|GUI_TA_VCENTER);
	    startTime = xTaskGetTickCount();
	    GUI_DispDecAt(i, GUI_GetScreenSizeX()/2, GUI_GetScreenSizeY()/2,3);
	    	UpdateDisplay(updateMode,powerMode);
	    rval += (xTaskGetTickCount() - startTime);

    }
	rval = rval / num;
	sprintf(buff,"Average = %d ms",(int)(rval/num));


    GUI_SetFont(GUI_FONT_13B_1);
	GUI_SetTextAlign(GUI_TA_HCENTER|GUI_TA_BOTTOM);
	GUI_DispStringAt(buff,GUI_GetScreenSizeX()/2, GUI_GetScreenSizeY());
    Cy_EINK_Power(CY_EINK_ON);
	UpdateDisplay(updateMode,false);
    vTaskDelay(2000);
    Cy_EINK_Power(CY_EINK_OFF);
	return rval/num;

}


void loopDisplay()
{

    GUI_SetColor(GUI_BLACK);
    GUI_SetBkColor(GUI_WHITE);
    char buff[128];
vTaskDelay(3000);

    uint32_t fp,f2,f4;
    uint32_t tp,t2,t4;
#define ROUNDS 5
    Cy_EINK_Power(CY_EINK_ON);
    fp= runNumbers(CY_EINK_PARTIAL,false,30);
    f2 = runNumbers(CY_EINK_FULL_2STAGE,false,ROUNDS);
    f4 = runNumbers(CY_EINK_FULL_4STAGE,false,ROUNDS);
    tp = runNumbers(CY_EINK_PARTIAL,true,ROUNDS);
    t2 = runNumbers(CY_EINK_FULL_2STAGE,true,ROUNDS);
    t4 = runNumbers(CY_EINK_FULL_4STAGE,true,ROUNDS);

    GUI_SetFont(GUI_FONT_8X13_1);
   	GUI_SetTextAlign(GUI_TA_LEFT|GUI_TA_TOP);
	GUI_Clear();

#define START 20
#define SPACING 15
#define XPOS 20

   	sprintf(buff,"M=Partial Pwr=False T=%d ms",(int)fp);
   	GUI_SetTextAlign(GUI_TA_LEFT|GUI_TA_TOP);

   	GUI_DispStringAt(buff,XPOS, START + SPACING);

   	sprintf(buff,"M= 2Stage Pwr=False T=%d ms",(int)f2);
   	GUI_SetTextAlign(GUI_TA_LEFT|GUI_TA_TOP);
	GUI_DispStringAt(buff,XPOS, START + 2*SPACING);

   	sprintf(buff,"M= 4Stage Pwr=False T=%d ms",(int)f4);
   	GUI_SetTextAlign(GUI_TA_LEFT|GUI_TA_TOP);
	GUI_DispStringAt(buff,XPOS, START + 3*SPACING);

   	sprintf(buff,"M=Partial Pwr=True  T=%d ms",(int)tp);
   	GUI_SetTextAlign(GUI_TA_LEFT|GUI_TA_TOP);
	GUI_DispStringAt(buff,XPOS, START + 4*SPACING);

	sprintf(buff,"M= 2Stage Pwr=True  T=%d ms",(int)t2);
   	GUI_SetTextAlign(GUI_TA_LEFT|GUI_TA_TOP);
	GUI_DispStringAt(buff,XPOS, START + 5*SPACING);

   	sprintf(buff,"M= 4Stage Pwr=True  T=%d ms",(int)t4);
   	GUI_SetTextAlign(GUI_TA_LEFT|GUI_TA_TOP);
	GUI_DispStringAt(buff,XPOS, START + 6*SPACING);

   	UpdateDisplay(CY_EINK_FULL_4STAGE,true);

}


/*******************************************************************************
* Function Name: void ShowStartupScreen(void)
********************************************************************************
*
* Summary: This function displays the startup screen with Cypress Logo and 
*			the demo description text
*
* Parameters:
*  None
*
* Return:
*  None
*
*******************************************************************************/
void ShowStartupScreen(void)
{
    /* Set foreground and background color and font size */
    GUI_SetFont(GUI_FONT_16B_1);
    GUI_SetColor(GUI_BLACK);
    GUI_SetBkColor(GUI_WHITE);
    GUI_Clear();

    GUI_DrawBitmap(&bmCypressLogoFullColor_PNG_1bpp, 2, 40);

    /* Send the display buffer data to display*/
    UpdateDisplay(CY_EINK_FULL_4STAGE, true);
}


void ShowIoTScreen(void)
{
    /* Set foreground and background color and font size */
    GUI_Clear();

    GUI_DrawBitmap(&bmIOTexpert_Logo_Vericalbw, 2, 2);

    /* Send the display buffer data to display*/
    UpdateDisplay(CY_EINK_FULL_4STAGE, true);
}


/*******************************************************************************
* Function Name: void ClearScreen(void)
********************************************************************************
*
* Summary: This function clears the screen
*
* Parameters:
*  None
*
* Return:
*  None
*
*******************************************************************************/
void ClearScreen(void)
{
    GUI_SetColor(GUI_BLACK);
    GUI_SetBkColor(GUI_WHITE);
    GUI_Clear();
    UpdateDisplay(CY_EINK_FULL_4STAGE, true);
}



/*******************************************************************************
* Function Name: int main(void)
********************************************************************************
*
* Summary: This is the main function.  Following functions are performed
*			1. Initialize the EmWin library
*			2. Display the startup screen for 3 seconds
*			3. Display the instruction screen and wait for key press and release
*			4. Inside a while loop scroll through the 6 demo pages on every
*				key press and release
*
* Parameters:
*  None
*
* Return:
*  None
*
*******************************************************************************/
void eInkTask(void *arg)
{
	(void)arg;

	printf("Started EINK Task\n");

    GUI_Init();
    Cy_EINK_Start(20);
    Cy_EINK_Power(1);

	ShowIoTScreen();
	vTaskDelay(3000);

    ShowStartupScreen();
    vTaskDelay(2000);

    loopDisplay();
}

/* [] END OF FILE */
