/**
 * @file    Timer.c
 * @author  Mohamed Moaaz
 * @brief   Timer program file
 * @version 0.1
 * @date    2023-10-31
 *
 * @copyright Copyright (c) 2023
 */

//==============================================================================
// Includes
//==============================================================================
#include <math.h>
#include "STD_TYPES.h"
#include "MCAL/Timer_private.h"
#include "MCAL/Timer_configuration.h"
#include "MCAL/Timer_register.h"
#include "MCAL/Timer_interface.h"

//==============================================================================
// Extern Variables
//==============================================================================

//==============================================================================
// Private Macros & Defines
//==============================================================================

#if(TIMER0_PRESCALER == NO_PRESCALER)
#define TIMER0_PRESCALER_VALUE                    1
#elif(TIMER0_PRESCALER == DIV_BY_8)
#define TIMER0_PRESCALER_VALUE                    8
#elif(TIMER0_PRESCALER == DIV_BY_64)
#define TIMER0_PRESCALER_VALUE                    64
#elif(TIMER0_PRESCALER == DIV_BY_256)
#define TIMER0_PRESCALER_VALUE                    265
#elif(TIMER0_PRESCALER == DIV_BY_1024)
#define TIMER0_PRESCALER_VALUE                    1024
#endif

#if(TIMER1_PRESCALER == NO_PRESCALER)
#define TIMER1_PRESCALER_VALUE                    1
#elif(TIMER1_PRESCALER == DIV_BY_8)
#define TIMER1_PRESCALER_VALUE                    8
#elif(TIMER1_PRESCALER == DIV_BY_64)
#define TIMER1_PRESCALER_VALUE                    64
#elif(TIMER1_PRESCALER == DIV_BY_256)
#define TIMER1_PRESCALER_VALUE                    265
#elif(TIMER1_PRESCALER == DIV_BY_1024)
#define TIMER1_PRESCALER_VALUE                    1024
#endif

//==============================================================================
// Private Typedefs
//==============================================================================

//==============================================================================
// Static Variables
//==============================================================================

//==============================================================================
// Global Variables
//==============================================================================

//==============================================================================
// Exported Values
//==============================================================================

//==============================================================================
// Static Functions Prototypes
//==============================================================================


//==============================================================================
// Static Functions
//==============================================================================


//==============================================================================
// Global Functions
//==============================================================================

void Timer0_Init(void)
{
    /*clear Timer0 control register at first*/
    TCCR0 &= 0;
    /*Timer0 normal mode is selected*/
    #if(TIMER0_WAVE_GENERATION_MODE == TIMER0_NORMAL_MODE)
    CLEAR_BIT(TCCR0, TCCR0_WGM01);
    CLEAR_BIT(TCCR0, TCCR0_WGM00);
    /*set timer0 output compare interrupt enable bit*/
    SET_BIT(TIMSK, TIMSK_TOIE0);

    /*Timer0 PWM phase correct mode is selected*/
    #elif(TIMER0_WAVE_GENERATION_MODE == TIMER0_PWM_PHASE_CORRECT_MODE)
    CLEAR_BIT(TCCR0, TCCR0_WGM01);
    SET_BIT(TCCR0, TCCR0_WGM00);
    /*select the compare output mode*/
    TCCR0 &= (PHASE_CORRECT_PWM_OUTPUT_MODE<<4);

    /*Timer0 CTC mode is selected*/
    #elif(TIMER0_WAVE_GENERATION_MODE == TIMER0_CTC_MODE)
    SET_BIT(TCCR0, TCCR0_WGM01);
    CLEAR_BIT(TCCR0, TCCR0_WGM00);
    /*select the compare output mode*/
    TCCR0 &= (CTC_OUTPUT_MODE<<4);
    /*set timer0 output compare interrupt enable bit*/
    SET_BIT(TIMSK, TIMSK_OCIE0);

    /*Timer0 fast PWM mode is selected*/
    #elif(TIMER0_WAVE_GENERATION_MODE == TIMER0_FAST_PWM_MODE)
    SET_BIT(TCCR0, TCCR0_WGM01);
    SET_BIT(TCCR0, TCCR0_WGM00);
    /*select the compare output mode*/
    TCCR0 &= (TIMER0_PWM_OUTPUT_MODE<<4);
    #else
    #error "wrong mode configuration"
    #endif
}

E_Status Timer0_StartFastPWM(uint8_t copy_u8DutyCycle)
{
    E_Status Local_ErrorStatus = E_OK;
    /*check if the duty cycle is less than period time*/
    if (copy_u8DutyCycle <=256)
    {
        /*check for pwm output mode*/
        #if(TIMER0_PWM_OUTPUT_MODE == TIMER0_NON_INVERTING_MODE)
        OCR0 = copy_u8DutyCycle;
        
        #elif(TIMER0_PWM_OUTPUT_MODE == TIMER0_INVERTING_MODE)
        OCR0 = 256 - copy_u8DutyCycle;
        #endif

        /*set the prescaler for fast pwm*/
        TCCR0 &= ~0x07;
        TCCR0 |= TIMER0_PRESCALER;
    }
    else
    {
        Local_ErrorStatus = E_NOK;
    }
    return Local_ErrorStatus;
}

void Timer1_Init(void)
{
    /*set the wave form generation mode*/
    /*normal mode selected*/
    #if(TIMER1_WAVE_GENERATION_MODE == TIMER1_NORMAL_MODE)
    TCCR1A &= ~0x03;
    TCCR1B &= ~0x24;

    /*PWM phase correct 8-bits mode selected*/
    #elif(TIMER1_WAVE_GENERATION_MODE == TIMER1_PWM_PHASE_CORRECT_8_BITS_MODE)
    TCCR1A |=  0x01;
    TCCR1A &= ~0x02;
    TCCR1B &= ~0x24;

    /*PWM phase correct 9-bits mode selected*/
    #elif(TIMER1_WAVE_GENERATION_MODE == TIMER1_PWM_PHASE_CORRECT_9_BITS_MODE)
    TCCR1A |=  0x02;
    TCCR1A &= ~0x01;
    TCCR1B &= ~0x24;

    /*PWM phase correct 10-bits mode selected*/
    #elif(TIMER1_WAVE_GENERATION_MODE == TIMER1_PWM_PHASE_CORRECT_10_BITS_MODE)
    TCCR1A |=  0x03;
    TCCR1B &= ~0x24;

    /*CTC mode selected*/
    #elif(TIMER1_WAVE_GENERATION_MODE == TIMER1_CTC_MODE)
    TCCR1A &= ~0x03;
    TCCR1B |= 0x08;
    TCCR1B &= ~0x16;

    /*fast PWM 8-bits mode selected*/
    #elif(TIMER1_WAVE_GENERATION_MODE == TIMER1_FAST_PWM_8_BITS_MODE)
    TCCR1A |= 0x01;
    TCCR1A &= ~0x02;
    TCCR1B |= 0x08;
    TCCR1B &= ~0x16;

    /*fast PWM 9-bits mode selected*/
    #elif(TIMER1_WAVE_GENERATION_MODE == TIMER1_FAST_PWM_9_BITS_MODE)
    TCCR1A &= ~0x01;
    TCCR1A |=  0x02;
    TCCR1B |=  0x08;
    TCCR1B &= ~0x16;

    /*fast PWM 10-bits mode selected*/
    #elif(TIMER1_WAVE_GENERATION_MODE == TIMER1_FAST_PWM_10_BITS_MODE)
    TCCR1A |=  0x03;
    TCCR1B |=  0x08;
    TCCR1B &= ~0x16;

    /*PWM phase and frequency correct with ICR1 on top mode selected*/
    #elif(TIMER1_WAVE_GENERATION_MODE == TIMER1_PWM_PHASE_AND_FREQUENCY_CORRECT_TOP_ICR1_MODE)
    TCCR1A &= ~0x03;
    TCCR1B &= ~0x08;
    TCCR1B |=  0x16;

    /*PWM phase and frequency correct with OCR1A on top mode selected*/
    #elif(TIMER1_WAVE_GENERATION_MODE == TIMER1_PWM_PHASE_AND_FREQUENCY_CORRECT_TOP_OCR1A_MODE)
    TCCR1A |=  0x01;
    TCCR1A &= ~0x02;
    TCCR1B &= ~0x08;
    TCCR1B |=  0x16;

    /*PWM phase correct with ICR1 on top mode selected*/
    #elif(TIMER1_WAVE_GENERATION_MODE == TIMER1_PWM_PHASE_CORRECT_TOP_ICR1_MODE)
    TCCR1A &= ~0x01;
    TCCR1A |=  0x02;
    TCCR1B &= ~0x08;
    TCCR1B |=  0x16;

    /*PWM phase correct with OCR1A on top mode selected*/
    #elif(TIMER1_WAVE_GENERATION_MODE == TIMER1_PWM_PHASE_CORRECT_TOP_OCR1A_MODE)
    TCCR1A |=  0x03;
    TCCR1B &= ~0x08;
    TCCR1B |=  0x16;

    /*CTC with ICR1 on top mode selected*/
    #elif(TIMER1_WAVE_GENERATION_MODE == TIMER1_CTC_TOP_ICR1_MODE)
    TCCR1A &= ~0x03;
    TCCR1B |=  0x24;

    /*fast PWM with ICR1 on top mode selected*/
    #elif(TIMER1_WAVE_GENERATION_MODE == TIMER1_FAST_PWM_TOP_ICR1_MODE)
    TCCR1A &= ~0x01;
    TCCR1A |=  0x02;
    TCCR1B |=  0x24;

    /*fast PWM with OCR1A on top mode selected*/
    #elif(TIMER1_WAVE_GENERATION_MODE == TIMER1_FAST_PWM_TOP_OCR1A_MODE)
    TCCR1A |=  0x03;
    TCCR1B |=  0x24;
    #else
    #error "wrong configuration"
    #endif

    /*set OC1A output mode*/
    TCCR1A = (TIMER1_CTC_OC1A_OUTPUT_MODE<<6);
    /*set OC1B output mode*/
    TCCR1A = (TIMER1_CTC_OC1B_OUTPUT_MODE<<4);
}
void Timer1_voidStart(void)
{
    TCCR1B &= ~0x07;
    TCCR1B |= TIMER1_PRESCALER;
}

void Timer1_voidSetTimerValue(uint16_t copy_u16Value)
{
    TCNT1 = copy_u16Value;
}
uint16_t Timer1_u16ReadTimerValue(void)
{
    return TCNT1;
}
