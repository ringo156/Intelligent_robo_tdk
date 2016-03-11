/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/
#include <project.h>
#include <stdio.h>
#include "PS2_Controller.h"

void I2C_Color_init(void);
void I2C_LCD_Position(uint8 row, uint8 column);
void I2C_LCD_Init(void);
void Motor_Right(int16 speed);
void Motor_Left(int16 speed);

int main()
{
    char value[20];
    PS2Controller psData;
    CyGlobalIntEnable; /* Enable global interrupts. */
    CyDelay(500);
    /* Place your initialization/startup code here (e.g. MyInst_Start()) */
    Motor_PWM_a_Start();
    Motor_PWM_b_Start();
    PS2_Start();
    I2C_1_Start();
    I2C_LCD_1_Start();
    I2C_LCD_Init();
    Motor_Right(0);
    Motor_Left(0);
    Debug_LED_Write(0);
    CyDelay(1000);
    sprintf(value, "PSoC5 Start");
    I2C_LCD_Position(0u,0u);
    I2C_LCD_1_PrintString(value);
    for(;;)
    {
        /* Place your application code here. */
        psData = PS2_Controller_get();
        
        if(psData.UP)
        {
            Motor_Right(200);
            Motor_Left(200);
        }

        else if(psData.DOWN)
        {
            Motor_Right(-150);
            Motor_Left(-150);
        }
        else if(psData.LEFT)
        {
            Motor_Right(200);
            Motor_Left(0);
        }
        else if(psData.RIGHT)
        {
            Motor_Right(0);
            Motor_Left(200);
        }
        else
        {
            Motor_Right(0);
            Motor_Left(0);
        }
    }
}

void Motor_Right(int16 speed){
    
    
    if((0<speed)&&(speed<255))
    {
        Motor_PWM_a_WriteCompare1(0);
        Motor_PWM_a_WriteCompare2(speed);   
    }
    else if((-255<speed)&&(speed<0))
    {
        Motor_PWM_a_WriteCompare1(-speed);
        Motor_PWM_a_WriteCompare2(0);
    }
    else
    {
        Motor_PWM_a_WriteCompare1(0);
        Motor_PWM_a_WriteCompare2(0);
    }
}

void Motor_Left(int16 speed){
    
    
    if((0<speed)&&(speed<255))
    {
        Motor_PWM_b_WriteCompare1(0);
        Motor_PWM_b_WriteCompare2(speed);   
    }
    else if((-255<speed)&&(speed<0))
    {
        Motor_PWM_b_WriteCompare1(-speed);
        Motor_PWM_b_WriteCompare2(0);
    }
    else
    {
        Motor_PWM_b_WriteCompare1(0);
        Motor_PWM_b_WriteCompare2(0);
    }
}

void I2C_Color_init(void)
{
    unsigned char txCBuf[2] = { 0x0, 0x89 };
    //I2C_1_Start();
    
    I2C_1_MasterWriteBuf(0x2A,(uint8 *)&txCBuf,2,I2C_1_MODE_COMPLETE_XFER);
    while(0u==(I2C_1_MasterStatus() & I2C_1_MSTAT_WR_CMPLT));
    I2C_1_MasterClearStatus();
    txCBuf[1] = 0x09;
    I2C_1_MasterWriteBuf(0x2A,(uint8 *)&txCBuf,2,I2C_1_MODE_COMPLETE_XFER);
    while(0u==(I2C_1_MasterStatus() & I2C_1_MSTAT_WR_CMPLT));
    I2C_1_MasterClearStatus();
}

void I2C_LCD_Position(uint8 row, uint8 column) {
 
  if(row == 0){
    I2C_LCD_1_SetDDRAM(0x80u + column);
  }else{
    I2C_LCD_1_SetDDRAM(0xC0u + column);
  }
}

void I2C_LCD_Init(void) {
    CyDelay(40u);     //40mS 待機
    I2C_LCD_1_FunctionSetNormal();
    I2C_LCD_1_ReturnHome();
    I2C_LCD_1_FunctionSetExtended();
    I2C_LCD_1_InternalOscFrequency();
    I2C_LCD_1_ContrastSet();
    I2C_LCD_1_PwrIconContrast();
    I2C_LCD_1_FollowerCtrl();
    CyDelay(200u);     //200mS 待機
    I2C_LCD_1_FunctionSetNormal();
    I2C_LCD_1_DisplayOn();
    I2C_LCD_1_Clear();
    I2C_LCD_1_EntryModeSet();
    CyDelay(1);    //I2C_LCD_1_WriteControl(0x0fu); //Disp:On Cursor:On Position:On
}
/* [] END OF FILE */
