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

#define BLACK   1
#define WHITE   0

uint8 g_timeerFlag = 0;

union Slave{
    uint8 Trans;
    struct
    {
        uint8 a     : 1;
        uint8 b     : 1;
        uint8 c     : 1;
        uint8 d     : 1;
        uint8 e     : 1;
        uint8 f     : 1;
        uint8 g     : 1;
        uint8 h     : 1;
    }status;
};

typedef struct{
    union Slave slave;
}Line;

CY_ISR(UART_isr)
{
    g_timeerFlag = 1;
}

int main()
{
    uint32 val[8]  = {};
    uint32 fx[8]   = {};
    uint32 gray[8] = {};
    uint32 min[8]  = {};
    uint32 max[8]  = {};
    uint32 x[8]    = {150,150,170,180,150,150,150,150};
    uint8 i,tx=0;
    char value[40];
    //Line line;
    
    CyGlobalIntEnable; /* Enable global interrupts. */

    /* Place your initialization/startup code here (e.g. MyInst_Start()) */
    UART_2_Start();
    Timer_1_WriteCounter(1);
    Timer_1_Start();
    Timer_isr_StartEx(UART_isr);
    ADC_SAR_Seq_1_Start();
    ADC_SAR_Seq_1_StartConvert();
    
    sprintf(value, "Hello\n");
    UART_2_UartPutString(value);
    
    for(i=0;i<8;i++)
    {
        ADC_SAR_Seq_1_IsEndConversion(ADC_SAR_Seq_1_WAIT_FOR_RESULT);
        fx[i]=ADC_SAR_Seq_1_GetResult16(i);
    }
    for(;;)
    {
        /* Place your application code here. */
        /*
        ADC_SAR_Seq_1_IsEndConversion(ADC_SAR_Seq_1_WAIT_FOR_RESULT);
        a = ADC_SAR_Seq_1_GetResult16(0);
        */
        //8個同時に読んでblack/whiteの判別をする
        //右から0番目
        if(g_timeerFlag == 1)
        {
            for(i=0;i<8;i++)
            {
                ADC_SAR_Seq_1_IsEndConversion(ADC_SAR_Seq_1_WAIT_FOR_RESULT);
                val[i]=ADC_SAR_Seq_1_GetResult16(i);
                fx[i]=0.9*fx[i]+0.1*val[i];
                /*
                if(val[i]<min[i])
                {
                    min[i] = val[i];
                }
                if(val[i]>max[i])
                {
                    max[i] = val[i];
                }
                //中間値の計算
                gray[i] = (max[i] + min[i])/2;
                */
                if(fx[i]<200)//black
                {
                    tx |= 1 << i;
                    //iビット目を1にする
                }
                else//white
                {
                    tx &= ~(1 << i);
                    //iビット目を0にする
                }
                
                //sprintf(value, "x=%d\n", tx);
                //UART_2_UartPutString(value);
            }
            
            //UART_2_UartPutChar(tx);
            
            //デバッグ
            for(i=0;i<8;i++)
            {
                sprintf(value, "%d=%lu x=%d\n", i,val[i],tx);            
                UART_2_UartPutString(value);
            }
            
            g_timeerFlag = 0;
        }
    }
}

/* [] END OF FILE */
