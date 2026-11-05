// Khai báo các nguyên m?u hàm
void INIT();
void PORT();
void LED7_OUT(unsigned char num);
void DELAY_MS(unsigned int mili_count);
/**************************************************************************
Hàm INIT() là hàm không có tham s? và không tr? l?i giá tr?, do ngu?i l?p 
trình t? xây d?ng d? kh?i t?o tr?ng thái các PORT c?a vi di?u khi?n. Nhu 
b?t k? hàm t? xây d?ng nào khác, ngu?i l?p trình có th? d?i tên, chia nh?, 
g?p to, ho?c thay d?i các l?nh trong hàm này theo d?ng ý riêng khi l?p 
trình.
**************************************************************************/
void INIT()

{
// Kh?i t?o tr?ng thái Output cho các chân n?i t?i các LED don
DDRD |= 0xFF;
// Kh?i t?o tr?ng thái logic 1 cho các chân n?i t?i các LED don
PORTD |= 0xFF;
// Kh?i t?o tr?ng thái Output cho các chân n?i t?i LED 7 thanh
DDRC |= 0xFF;
// Kh?i t?o tr?ng thái logic 1 cho các chân n?i t?i LED 7 thanh
PORTC |= 0xFF;
}
/**************************************************************************
Hàm PORT() là hàm không có tham s? và không tr? l?i giá tr?, do ngu?i l?p 
trình t? xây d?ng d? di?u khi?n tr?ng thái logic 0/1 c?a các chân trong các 
PORT c?a vi di?u khi?n. Trong m?ch Kit này, tr?ng thái logic làm các dèn 
LED sáng/t?t theo quy t?c: 0 – LED sáng, 1 – LED t?t.
**************************************************************************/
void PORT()
{
/* Khai báo các bi?n s? dùng t?i trong hàm này */
// Bi?n led_shift d? di?u khi?n các LED don
// Giá tr? d?u là 255 = 0xFF = 0b11111111 -> t?t c? các LED d?u t?t
unsigned char led_shift = 255;
// Bi?n d?m cho LED 7 thanh, giá tr? d?u là 0
unsigned char led_7_count = 0;
// Vòng for giúp các LED sáng/t?t theo quy lu?t l?p di l?p l?i
for(;;)
{
/* Ðo?n mã di?u khi?n các LED don */
// Các LED sáng/t?t theo 8 bit c?a bi?n led_shift
PORTD = led_shift;
// Thay d?i bi?n led_shift
if(led_shift != 0) // N?u led_shift khác 0
led_shift = led_shift << 1; // D?ch trái 1 bit
else
led_shift = 255; // Tr? l?i giá tr? 255
/* Ðo?n mã di?u khi?n LED 7 thanh */
// Xu?t giá tr? d?m ra LED 7 thanh
LED7_OUT(led_7_count);
// Ð?o tr?ng thái PC3 d? nh?p nháy d?u ch?m trên LED 7 thanh

PORTC ^= (1<<PC3);
// Tang d?n giá tr? d?m
led_7_count = led_7_count + 1;
// Khi vu?t quá 9, giá tr? d?m du?c reset v? 0
if(led_7_count > 9)
led_7_count = 0;
// Hàm tr? kho?ng 0.5 s = 500 ms
DELAY_MS(500);
}
}
/**************************************************************************
Hàm LED7_OUT() là hàm có tham s? num nhung không tr? l?i giá tr?, do ngu?i 
l?p trình t? xây d?ng d? di?u khi?n LED 7 thanh ch? th? giá tr? c?a num (0-
9) b?ng cách sáng/t?t các do?n LED m?t cách phù h?p. Hàm LED7_OUT() không 
làm thay d?i tr?ng thái sáng/t?t c?a d?u ch?m trên LED 7 thanh. Trong m?ch 
Kit này, tr?ng thái logic làm các thanh LED sáng/t?t nhu sau: 0 – thanh LED 
sáng, 1 – thanh LED t?t.
**************************************************************************/
void LED7_OUT(unsigned char num)
{
// Khai báo bi?n temp luu tr?ng thái c?a PORTC
unsigned char temp = PORTC;
// Các chân vi di?u khi?n ?ng v?i các thanh LED
// a - PC5 PC5
// b - PC4 PC6 PC4
// c - PC2 PC6 PC4
// d - PC1 PC7
// e - PC0 PC0 PC2
// f - PC6 PC0 PC2
// g - PC7 PC1 PC3
// dot - PC3
// T?t các do?n LED hi?n dang sáng tru?c khi sáng các do?n LED m?i
temp &= 0B00001000;
// Gán m?c logic cho 8 bit c?a bi?n temp ?ng v?i giá tr? c?a bi?n num
switch(num)
{
case 0: temp |= 0B10000000; break;
case 1: temp |= 0B11100011; break;
case 2: temp |= 0B01000100; break;
case 3: temp |= 0B01000001; break;
case 4: temp |= 0B00100011; break;
case 5: temp |= 0B00010001; break;
case 6: temp |= 0B00010000; break;
case 7: temp |= 0B11000011; break;
case 8: temp |= 0B00000000; break;
case 9: temp |= 0B00000001; break;
}
// Xu?t giá tr? logic m?i ra PORTC d? làm sáng LED 7 thanh
PORTC = temp;
}
/**************************************************************************
Hàm DELAY_MS() là hàm có tham s? mili_count nhung không tr? l?i giá tr?, do 
ngu?i l?p trình t? xây d?ng d? t?o ra kho?ng th?i gian tr? (th?i gian ch?) 
tính b?ng mili giây. Vi?c tr? duoc th?c hi?n b?ng các vòng l?p r?ng. Vòng 
l?p r?ng (c? th? là vòng for) tuy không th?c hi?n công vi?c gì nhung v?n 
làm CPU tiêu t?n m?t kho?ng th?i gian nh?t d?nh cho vi?c kh?i t?o và k?t 
thúc. Nhi?u vòng for liên ti?p s? t?o m?t kho?ng tr? dáng k?.
**************************************************************************/
void DELAY_MS(unsigned int mili_count)
{
// Khai báo hai bi?n ch?y cho hai vòng for
unsigned int i,j;
// Xung nh?p c?a h? th?ng càng cao, s? vòng l?p càng tang
mili_count = mili_count * FRE;
// Các vòng for gây tr?
for(i = 0; i < mili_count; i++)
for(j = 0; j < 53; j++);
}
