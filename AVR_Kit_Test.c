#define F_CPU 8000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include "hunget_lcd.h"

// ================= ADC INIT =================
void ADC_INIT(void)
{
    // PA1 (ADC1) input
    DDRA &= ~(1 << PA1);

    /*
     * Dùng INTERNAL 2.56V
     * LM35 max ~1V => d? phân gi?i t?t hon nhi?u so v?i 5V
     */
    ADMUX = (1 << REFS1) | (1 << REFS0); // Vref = 2.56V

    // Enable ADC, Prescaler = 128 (ADC clk = 62.5kHz)
    ADCSRA = (1 << ADEN) |
             (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);

    _delay_ms(10); // Ch? Vref ?n d?nh
}

// ================= ADC READ (B? M?U Ð?U) =================
uint16_t ADC_READ(uint8_t channel)
{
    // Gi? nguyên REFS, d?i kênh
    ADMUX = (ADMUX & 0xF0) | (channel & 0x0F);

    // --- M?U GI? (B?) ---
    ADCSRA |= (1 << ADSC);
    while (ADCSRA & (1 << ADSC));

    // --- M?U TH?T ---
    ADCSRA |= (1 << ADSC);
    while (ADCSRA & (1 << ADSC));

    return ADCW;
}

// ================= READ TEMPERATURE =================
uint16_t read_temperature(void)
{
    uint32_t sum = 0;

    // L?y 32 m?u d? gi?m nhi?u
    for (uint8_t i = 0; i < 32; i++)
    {
        sum += ADC_READ(1); // ADC1 = PA1
        _delay_ms(2);
    }

    uint16_t adc_avg = sum / 32;

    /*
     * V?i Vref = 2.56V:
     * 1 LSB = 2.5 mV
     * LM35: 10 mV = 1°C
     *
     * => nhi?t d? (0.1°C) = adc * 25 / 10
     */
    uint16_t temp_x10 = (adc_avg * 25) / 10;

    return temp_x10; // don v?: 0.1°C
}

// ================= MAIN =================
int main(void)
{
    // T?T JTAG (B?T BU?C v?i LCD dùng PORTC)
    MCUCSR |= (1 << JTD);
    MCUCSR |= (1 << JTD);

    LCD4_INIT(0, 0);
    ADC_INIT();

    // LED PD0–PD4 (Active LOW)
    DDRD |= 0x1F;
    PORTD |= 0x1F; // T?t LED ban d?u

    // Màn hình chào
    LCD4_CUR_GOTO(1, 3);
    LCD4_OUT_STR("LM35 SENSOR");
    LCD4_CUR_GOTO(2, 1);
    LCD4_OUT_STR("Temperature");
    _delay_ms(1500);
    LCD4_OUT_CMD(0x01);
    _delay_ms(2);

    char buf[16];
    uint16_t temp;
    uint16_t temp_filt = 0; // l?c m?m

    while (1)
    {
        temp = read_temperature();

        // Moving average (l?c m?m)
        temp_filt = (temp_filt * 7 + temp) / 8;

        // Hi?n th?
        LCD4_CUR_GOTO(1, 0);
        sprintf(buf, "Temp: %u.%u C  ",
                temp_filt / 10,
                temp_filt % 10);
        LCD4_OUT_STR(buf);

        LCD4_CUR_GOTO(2, 0);

        if (temp_filt > 300) // > 30.0°C
        {
            LCD4_OUT_STR("WARNING: HIGH ");
            PORTD &= 0xE0; // b?t LED
        }
        else
        {
            LCD4_OUT_STR("Status: Normal");
            PORTD |= 0x1F; // t?t LED
        }

        _delay_ms(300);
    }
}
