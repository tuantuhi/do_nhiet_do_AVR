#ifndef HUNGET_LCD_H
#define HUNGET_LCD_H

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdlib.h>

// =======================================================
// C?U HÌNH CHÂN THEO ?ÚNG S? ?? M?CH KIT
// =======================================================

// 1. CHÂN D? LI?U (DATA) -> PORT C
// S? d?ng 4 bit cao: D4->PC4, D5->PC5, D6->PC6, D7->PC7
#define LCD_DATA_DDR  DDRC
#define LCD_DATA_PORT PORTC
#define LCD_DATA_PIN  PINC

// 2. CHÂN ?I?U KHI?N (CONTROL) -> PORT D
#define LCD_CTRL_DDR  DDRD
#define LCD_CTRL_PORT PORTD

// ??nh ngh?a l?i chính xác theo s? ?? J7:
#define LCD_RW        5  // Pin 5 LCD n?i PD5
#define LCD_RS        6  // Pin 4 LCD n?i PD6
#define LCD_EN        7  // Pin 6 LCD n?i PD7

// =======================================================
// CÁC HÀM GIAO TI?P M?C TH?P
// =======================================================

// Hàm t?o xung Enable trên PORT D
void LCD_Enable(void) {
    LCD_CTRL_PORT |= (1 << LCD_EN);  // B?t E (PD7)
    _delay_us(5);
    LCD_CTRL_PORT &= ~(1 << LCD_EN); // T?t E
    _delay_us(50);
}

// Hàm g?i 4 bit d? li?u ra PORT C
void LCD_Send4Bit(unsigned char Data) {
    // 1. G?i 4 bit cao c?a d? li?u ra PORT C (vào các chân PC4-PC7)
    // Gi? nguyên tr?ng thái c?a 4 chân th?p PC0-PC3 b?ng m?t n? bit
    unsigned char current_port_c = LCD_DATA_PORT & 0x0F;
    LCD_DATA_PORT = current_port_c | (Data & 0xF0);
    
    // 2. T?o xung ch?t bên PORT D
    LCD_Enable();
}

// Hàm g?i l?nh (RS=0, RW=0 t?i PORT D)
void LCD4_OUT_CMD(unsigned char command) {
    LCD_CTRL_PORT &= ~(1 << LCD_RS); 
    LCD_CTRL_PORT &= ~(1 << LCD_RW);
    
    LCD_Send4Bit(command);      // G?i 4 bit cao (sang PORT C)
    LCD_Send4Bit(command << 4); // G?i 4 bit th?p (sang PORT C)
    
    if (command < 4) _delay_ms(2);
    else _delay_us(50);
}

// Hàm g?i d? li?u (RS=1, RW=0 t?i PORT D)
void LCD4_OUT_DATA(unsigned char data) {
    LCD_CTRL_PORT |= (1 << LCD_RS);
    LCD_CTRL_PORT &= ~(1 << LCD_RW);
    
    LCD_Send4Bit(data);         
    LCD_Send4Bit(data << 4);    
    _delay_us(50);
}

// =======================================================
// CÁC HÀM NG??I DÙNG (USER FUNCTIONS)
// =======================================================

void LCD4_INIT(unsigned char cursor, unsigned char blink) {
    // Kh?i t?o h??ng d? li?u (Output)
    // PC4-PC7 là Output (Data)
    LCD_DATA_DDR |= 0xF0;  
    
    // PD5, PD6, PD7 là Output (Control)
    LCD_CTRL_DDR |= (1<<LCD_RS) | (1<<LCD_RW) | (1<<LCD_EN); 
    
    // Reset m?c logic các chân ?i?u khi?n v? 0
    LCD_CTRL_PORT &= ~((1<<LCD_RS) | (1<<LCD_RW) | (1<<LCD_EN));
    
    _delay_ms(40); // Ch? LCD ?n ??nh sau khi c?p ngu?n
    
    // Chu?i Reset (G?i 0x30 ba l?n theo chu?n HD44780)
    LCD_Send4Bit(0x30); _delay_ms(5);
    LCD_Send4Bit(0x30); _delay_us(150);
    LCD_Send4Bit(0x30);
    
    // Chuy?n sang ch? ?? 4-bit
    LCD_Send4Bit(0x20);
    
    // C?u hình hi?n th?
    LCD4_OUT_CMD(0x28); // 4-bit, 2 dòng, font 5x8
    LCD4_OUT_CMD(0x08); // T?t hi?n th?
    LCD4_OUT_CMD(0x01); // Xóa màn hình
    _delay_ms(2);
    
    unsigned char cmd = 0x0C; // M?c ??nh b?t màn hình
    if(cursor) cmd |= 0x02;
    if(blink) cmd |= 0x01;
    LCD4_OUT_CMD(cmd); 
    
    LCD4_OUT_CMD(0x06); // T? ??ng t?ng con tr?
}

void LCD4_CUR_GOTO(unsigned char row, unsigned char col) {
    unsigned char address;
    if (row == 1) address = 0x80 + col;
    else address = 0xC0 + col;
    LCD4_OUT_CMD(address);
}

void LCD4_OUT_STR(char *str) {
    while (*str) LCD4_OUT_DATA(*str++);
}

void LCD4_OUT_DEC(unsigned int num, unsigned char len) {
    char buffer[10];
    sprintf(buffer, "%05u", num);
    char *ptr = &buffer[5 - len];
    LCD4_OUT_STR(ptr);
}

void LCD4_DIS_SHIFT(unsigned char direct, unsigned char step) {
    unsigned char i;
    if (direct == 0) // D?ch trái
        for(i=0; i<step; i++) LCD4_OUT_CMD(0x18);
    else             // D?ch ph?i
        for(i=0; i<step; i++) LCD4_OUT_CMD(0x1C);
}

#endif // HUNGET_LCD_H
