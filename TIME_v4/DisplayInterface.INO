/***************************************************
*         Hardware interface to display            *
*                     BEGIN                        *
****************************************************/
#define SRCLK      8
#define RCLK       9
#define SER        10
#define OE         11

#define SRCLK_low   digitalWrite(SRCLK, LOW)
#define SRCLK_high  digitalWrite(SRCLK, HIGH)
#define RCLK_low    digitalWrite(RCLK, LOW)
#define RCLK_high   digitalWrite(RCLK, HIGH)
#define SER_low     digitalWrite(SER, LOW)
#define SER_high    digitalWrite(SER, HIGH)

#define LINE1ON    digitalWrite(7, LOW)
#define LINE1OFF   digitalWrite(7, HIGH)
#define LINE2ON    digitalWrite(6, LOW)
#define LINE2OFF   digitalWrite(6, HIGH)
#define LINE3ON    digitalWrite(5, LOW)
#define LINE3OFF   digitalWrite(5, HIGH)
#define LINE4ON    digitalWrite(4, LOW)
#define LINE4OFF   digitalWrite(4, HIGH)
#define LINE5ON    digitalWrite(3, LOW)
#define LINE5OFF   digitalWrite(3, HIGH)

#define DISPLAYON  digitalWrite(11, LOW);
#define DISPLAYOFF digitalWrite(11, HIGH);

unsigned char multiplex_wich_counter=0;
unsigned long ulong1;
int screen_counter=0;

void shift(unsigned long data)
{
    RCLK_low;		
    for (int i = 0; i <=20; i++) {
	SRCLK_low; 
	if (data & 0x100000) SER_high;   // 0x80000 = 10000 00000000 00000000
	else SER_low;		
        SRCLK_high;		
	data <<= 1;
    }
    RCLK_high;		    
}

// Load one line from display buffer to ulong1 variable
// Shift register number             3        2        1
// Shift registers pins           00010000 00000000 00000000
// Display conections                1111  1111111  111111 
// ulong1 variable       00000000 00011110 11111110 11111100
void generate_ulong1()
{
    int i;   
    
    ulong1 = 0;
    for (i=0; i<4; i++) {
        ulong1 <<= 1;      
        ulong1 |= screen[screen_counter];
        screen_counter++;  
    }
    ulong1 <<= 1;   // Jump one bit

    for (i=0; i<7; i++) {
        ulong1 <<= 1;
        ulong1 |= screen[screen_counter];
        screen_counter++;  
    }
    ulong1 <<= 1;   // Jump one bit
    for (i=0; i<6; i++) {
        ulong1 <<= 1;
        ulong1 |= screen[screen_counter];
        screen_counter++;  
    }
    ulong1 <<= 2;   // Jump two bit
}

void show_line1(void)
{ 
    generate_ulong1();
    LINE5OFF;
    LINE1ON;                           
    shift(ulong1);
}

void show_line2(void)
{
    generate_ulong1();
    LINE1OFF; LINE2ON;
    shift(ulong1);
}

void show_line3(void)
{
    generate_ulong1();
    LINE2OFF; LINE3ON;
    shift(ulong1);
}

void show_line4(void)
{
    generate_ulong1();
    LINE3OFF; LINE4ON;
    shift(ulong1);
}	

// Next time screen_counter start again from 0
void show_line5(void)
{
    generate_ulong1();
    LINE4OFF; LINE5ON;
    shift(ulong1);
    screen_counter = 0;
}

ISR (TIMER2_COMPA_vect)
{
    DISPLAYOFF; 
    if ( multiplex_wich_counter++ == 6) multiplex_wich_counter = 1;
    if (multiplex_wich_counter == 1) show_line1();
    if (multiplex_wich_counter == 2) show_line2();
    if (multiplex_wich_counter == 3) show_line3();
    if (multiplex_wich_counter == 4) show_line4();
    if (multiplex_wich_counter == 5) show_line5();
    DISPLAYON;
}

void set_pixel(int x, int y)
{
    if ((x>=0) && (x<=16) && (y>=0) && (y<5)) screen[x+y*17] = 1;    
}

void clear_pixel(int x, int y)
{
    if ((x>=0) && (x<=16) && (y>=0) && (y<5)) screen[x+y*17] = 0;    
}

void clear_screen(void)
{
    for (int i=0; i<85; i++) screen[i] = 0;
}

void init_display_pins(void)
{
    // Shift registers pins set as outputs
    pinMode(8, OUTPUT);    // SRCLK
    pinMode(9, OUTPUT);    // RCLK
    pinMode(10, OUTPUT);   // SER
    pinMode(11, OUTPUT);   // OE
    digitalWrite(11, LOW);
    digitalWrite(8, HIGH);
    digitalWrite(9, HIGH);
    digitalWrite(10, HIGH);
    
    // Lines pins set as outputs
    pinMode(3, OUTPUT);   // LINE 5
    pinMode(4, OUTPUT);   // LINE 4
    pinMode(5, OUTPUT);   // LINE 3
    pinMode(6, OUTPUT);   // LINE 2
    pinMode(7, OUTPUT);   // LINE 1
    // All lines off
    digitalWrite(3, LOW);
    digitalWrite(4, LOW);
    digitalWrite(5, LOW);
    digitalWrite(6, LOW);
    digitalWrite(7, LOW);
}

void init_display_refresh(void)
{
    TCCR2A  = (1<<WGM21);
    TCCR2B |= (1<<CS22)|(1<<CS21); 
    OCR2A = 100;   // 208.(3)
    TIMSK2 |= (1<<OCIE2A);
}
/***************************************************
*         Hardware interface to display            *
*                      END                         *
****************************************************/

