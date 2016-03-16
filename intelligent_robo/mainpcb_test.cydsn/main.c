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

#define BLACK 1
#define WHITE 0

union Slave{
    uint8 Trans;
    struct
    {
        uint8 a     : 1;//right
        uint8 b     : 1;
        uint8 c     : 1;
        uint8 d     : 1;
        uint8 e     : 1;
        uint8 f     : 1;
        uint8 g     : 1;
        uint8 h     : 1;//left
    }status;
};

typedef struct{
    union Slave slave;
}Line;

void I2C_Color_init(void);
void I2C_LCD_Position(uint8 row, uint8 column);
void I2C_LCD_Init(void);
void Motor_Right(int16 speed);
void Motor_Left(int16 speed);

int main()
{
    uint16 x=0, i=0;
    char value[20];
    PS2Controller psData;
    Line line;
    /* Enable global interrupts. */
    CyGlobalIntEnable; 
    CyDelay(500);
    /* Place your initialization/startup code here (e.g. MyInst_Start()) */
    
    PWM_Servo_Start();
    PWM_Motor_a_Start();
    PWM_Motor_b_Start();    
    Motor_Right(0);
    Motor_Left(0);
    //PS2_Start();
    
    UART_Line_Sensor_Start();
    I2C_1_Start();
    I2C_LCD_1_Start();
    I2C_LCD_Init();

    CyDelay(1000);
    I2C_LCD_Position(0u,0u);
    I2C_LCD_1_PrintString("PSoC5 Start");
    for(;;)
    {
        Debug_LED_Write(1);
        /* Place your application code here. */
        //ラインセンサ受信
        if(UART_Line_Sensor_GetRxBufferSize())
        {
            line.slave.Trans = (uint8)UART_Line_Sensor_GetChar();
            sprintf(value, "d=%d e=%d f=%d\n", line.slave.status.d, line.slave.status.e, line.slave.status.f);
            UART_Line_Sensor_PutString(value);
        }
        
        if(line.slave.status.e == BLACK)//直進
        {
            Motor_Right(150);
            Motor_Left(150);
        }
        else if(line.slave.status.d == BLACK)//右折
        {
            Motor_Right(0);
            Motor_Left(150);
        }
        else if(line.slave.status.f == BLACK)//左折
        {
            Motor_Right(150);
            Motor_Left(0);
        }
        else
        {
            Motor_Right(150);
            Motor_Left(150);           
        }
        /*
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
        */
    }
}

void Motor_Right(int16 speed){
    
    
    if((0<speed)&&(speed<255))
    {
        PWM_Motor_a_WriteCompare1(0);
        PWM_Motor_a_WriteCompare2(speed);   
    }
    else if((-255<speed)&&(speed<0))
    {
        PWM_Motor_a_WriteCompare1(-speed);
        PWM_Motor_a_WriteCompare2(0);
    }
    else
    {
        PWM_Motor_a_WriteCompare1(0);
        PWM_Motor_a_WriteCompare2(0);
    }
}

void Motor_Left(int16 speed){
    
    
    if((0<speed)&&(speed<255))
    {
        PWM_Motor_b_WriteCompare1(0);
        PWM_Motor_b_WriteCompare2(speed);   
    }
    else if((-255<speed)&&(speed<0))
    {
        PWM_Motor_b_WriteCompare1(-speed);
        PWM_Motor_b_WriteCompare2(0);
    }
    else
    {
        PWM_Motor_b_WriteCompare1(0);
        PWM_Motor_b_WriteCompare2(0);
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
