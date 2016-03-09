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


int main()
{
    uint32 a,val[8];
    uint8 i,x=0,tx=0;
    char value[40];
    //Line line;
    
    CyGlobalIntEnable; /* Enable global interrupts. */

    /* Place your initialization/startup code here (e.g. MyInst_Start()) */
    UART_1_Start();
    ADC_SAR_Seq_1_Start();
    ADC_SAR_Seq_1_StartConvert();
    
    sprintf(value, "HelloWorld\n");
    UART_1_PutString(value);                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          
    for(;;)
    {
        /* Place your application code here. */
        ADC_SAR_Seq_1_IsEndConversion(ADC_SAR_Seq_1_WAIT_FOR_RESULT);
        a = ADC_SAR_Seq_1_GetResult16(0);
        /*8個同時に読んでblack/whiteの判別をする
        for(i=0;i<8;i++)
        {
            ADC_SAR_Seq_1_IsEndConversion(ADC_SAR_Seq_1_WAIT_FOR_RESULT);
            val[i]=ADC_SAR_Seq_1_GetResult16(i);
            if(val[i]>100)//black
            {
                x |= 1 < i;
                //iビット目を1にする
            }
            else//white
            {
                x &= ~(1 << i);
                //iビット目を0にする
            }
        }
        */
        
        sprintf(value, "a=%lu\n", a);
        UART_1_PutString(value);
        /*
        tx = line.slave.Trans;
        UART_1_PutChar(tx);
        */
    }
}

/* [] END OF FILE */
