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
    const uint8 speed = 170;
    uint8 s = 0, i = 0, sensor[3] = {};
    uint8 AreaFlag = 0, aFlag = 0, hFlag = 0;
    uint16 x=0;
    double p = 0, p0 = 0, p1 = 0, p2 = 0, dif = 0;
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
    ADC_DelSig_Distance_Start();
    ADC_DelSig_Distance_StartConvert();
    AMux_D_Sensor_Start();
    UART_Line_Sensor_Start();
    I2C_1_Start();
    I2C_LCD_1_Start();
    I2C_LCD_Init();

    CyDelay(1000);
    I2C_LCD_Position(0u,0u);
    I2C_LCD_1_PrintString("PSoC5 Start");
    CyDelay(1000);
    Motor_Right(speed);
    Motor_Left(speed);
    CyDelay(800);
    I2C_LCD_1_Clear();
    for(;;)
    {
        Debug_LED_Write(1);
        /* Place your application code here. */
        
        //ラインセンサ受信
        if(UART_Line_Sensor_GetRxBufferSize())
        {
            line.slave.Trans = (uint8)UART_Line_Sensor_GetChar();
            
            //sprintf(value, "d=%d e=%d f=%d\n", line.slave.status.d, line.slave.status.e, line.slave.status.f);
            //UART_Line_Sensor_PutString(value);
        }
        
        
        p = (double)(line.slave.status.h*(-4)+line.slave.status.g*(-3)+line.slave.status.f*(-2)+line.slave.status.e*(-1)+
        line.slave.status.d*(0)+line.slave.status.c*(1)+line.slave.status.b*(2)+line.slave.status.a*(3));
        s = line.slave.status.h + line.slave.status.g + line.slave.status.f + line.slave.status.e + 
        line.slave.status.d + line.slave.status.c + line.slave.status.b + line.slave.status.a;
        /*
        p = (double)(line.slave.status.g*(-3)+line.slave.status.f*(-2)+line.slave.status.e*(-1)+
        line.slave.status.d*(0)+line.slave.status.c*(1)+line.slave.status.b*(2));
        s = line.slave.status.g + line.slave.status.f + line.slave.status.e + 
        line.slave.status.d + line.slave.status.c + line.slave.status.b;
        */
        if(s!=0)
        {
            p/=(double)s;
            p2 = p1;
            p1 = p0;
            p0 = p;
            //p=15, i= , d=,
            //dif += 16.0 * (p0-p1) + 0.4 * p0 + 1.7 *((p0-p1) - (p1-p2));
            dif += 14.0 * (p0-p1) + 4.0 *((p0-p1) - (p1-p2));
            if(dif > speed)
            {
                dif = speed;
            }
            else if(dif < -speed)
            {
                dif = -speed;
            }
            Motor_Right(speed - (int)dif);
            Motor_Left(speed + (int)dif);
            
           
            if(dif>0)
            {
                I2C_LCD_Position(1u,7u);
                I2C_LCD_1_PrintString("right");
            }
            else if(dif<0)
            {
                I2C_LCD_Position(1u,7u);
                I2C_LCD_1_PrintString("left");
            }
        }
        sprintf(value, "dif=%f",dif);
        I2C_LCD_Position(1u,0u);
        I2C_LCD_1_PrintString(value);
        /*
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
        */
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
        //距離センサー
        for(i=0;i<3;i++)
        {
            AMux_D_Sensor_Select(i);
            ADC_DelSig_Distance_StartConvert();
            ADC_DelSig_Distance_IsEndConversion(ADC_DelSig_Distance_WAIT_FOR_RESULT);
            sensor[i] = ADC_DelSig_Distance_GetResult8();
            ADC_DelSig_Distance_StopConvert();
        }
        /*
        i = 0;
        sprintf(value, "%d=%d", i, sensor[i]);
        I2C_LCD_Position(1u,0u);
        I2C_LCD_1_PrintString(value);
        */
        if((sensor[1]>150)&&(sensor[1]<180))
        {
            for(;;)
            {
                Motor_Right(0);
                Motor_Left(0);
            }
        }           
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
