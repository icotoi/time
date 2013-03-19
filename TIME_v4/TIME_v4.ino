#include <TimerOne.h>
#include <SoftwareSerial.h>
#include <Wire.h>
#include "RTClib.h"
#include <avr/pgmspace.h>
#include <SPI.h>
#include <WiFlyHQ.h>

#define BUFF_SIZE   100
#define WAIT_REQ    10000 // miliseconds to wait before next request
#define LEDDEBUG(x) load_ascii_string(x);put_string(0, TEXT);delay(1000);

unsigned char screen[5*17];   // 85 LED
unsigned int TEXT[BUFF_SIZE];   // Bufer for integer unicode characters we get from internet
char asciiString[] = "Display printing test.";   // unsigned char ASCII text for testing
int last_time;

// Function definition
void set_pixel(int x, int y);
void clear_pixel(int x, int y);
void clear_screen(void);
void init_display_pins(void);
void init_display_refresh(void);
void init_RTC(void);
int put_char(int displayXpos, unsigned int unicodeIndex);
void put_time(void);

RTC_DS1307 RTC;
DateTime now;

WiFly wifly;
SoftwareSerial wifiSerial(A0, A1); 

const char mySSID[] = "inventeaza.ro";
const char myPassword[] = "inventeaza";

const char site[] = "smartcam.inventeaza.ro";

unsigned int put_string(int posX, unsigned int s[])
{
  int pos = posX;
  unsigned int textLenght = 0;

  // Get text lenght    
  for (textLenght = 0; textLenght < 100; textLenght++) {  
    if (s[textLenght] == '\0') break;
  }

  // Print each unicode character from the TEXT buffer
  for (int i = 0; i < textLenght; i++) pos = put_char(pos, s[i]);  

  // Return lenght in pixels of the string from the TEXT buffer
  return pos - posX;
}

void scroll_text(int speed, unsigned int s[])
{
  int pixelLenght;

  // Print outside screen just to get string X dimension in pixels
  pixelLenght = put_string(17, s);

  for (int i=17; i>-pixelLenght; i--) {
    put_string(i, s);
    delay(speed);
    clear_screen();
  }	
}

void load_ascii_string(char s[])
{
  for (int i = 0; i < 100; i++) {  
    TEXT[i] = s[i];
    if (s[i] == '\0') break;	
  }
}

void setup() 
{       
  init_display_pins();
  digitalWrite(11, HIGH); // Display off
  init_display_refresh();
  clear_screen();
  

  load_ascii_string("T I : M E");
  put_string(0, TEXT);
  
  char buf[32];  
  // pinMode(2, INPUT);      // This is the main button, tied to INT0
  // digitalWrite(2, HIGH);  // Enable internal pull up on button
  Serial.begin(9600);
  
  //Serial.print(F("Free memory: "));
  //Serial.println(wifly.getFreeMemory(),DEC);

  //WiFi
  if (!wifly.begin(&Serial, NULL)) {
    load_ascii_string("C A N N O T");
    put_string(0, TEXT);
    //terminal();
    
  }

  // Join wifi network if not already associated 
  if (!wifly.isAssociated()) {
    //Serial.println(F("Joining network"));
    wifly.setSSID(mySSID);
    wifly.setPassphrase(myPassword);
    wifly.enableDHCP();

    if (wifly.join()) {
      load_ascii_string("J O I N E D");
      put_string(0, TEXT);
    } 
    else {
      load_ascii_string("F A I L E D");
      put_string(0, TEXT);
    }
  } 
  else {
    //Serial.println(F("Already joined network"));
  }

  
  /*Serial.print(F("MAC: "));
  Serial.println(wifly.getMAC(buf, sizeof(buf)));
  Serial.print(F("IP: "));
  Serial.println(wifly.getIP(buf, sizeof(buf)));
  Serial.print(F("Netmask: "));
  Serial.println(wifly.getNetmask(buf, sizeof(buf)));
  Serial.print(F("Gateway: "));
  Serial.println(wifly.getGateway(buf, sizeof(buf)));
  */
  wifly.setDeviceID("TI:ME");
  /*Serial.print(F("DeviceID: "));
  Serial.println(wifly.getDeviceID(buf, sizeof(buf)));*/
 
  if (wifly.isConnected()) {
    //Serial.println(F("Old connection active. Closing"));
    wifly.close();
  }
  delay(1000);
  load_ascii_string("T I : M E");
  put_string(0, TEXT);
  
  // Init RTC
  pinMode(A2, OUTPUT);
  pinMode(A3, OUTPUT);
  digitalWrite(A2, LOW);   // RTC GND 
  digitalWrite(A3, HIGH);  // RTC +5V 
  Wire.begin();
  RTC.begin();  
  // In case RTC not responding signal with error message
  if (!RTC.isrunning()) {
    // Do something if RTC not responding
  }   

  // Set RTC to date & time this sketch was compiled
  RTC.adjust(DateTime(__DATE__, __TIME__));
  
  last_time = millis();
  load_ascii_string("D I S P L A Y  T E S T !");
}
void parse_html()
{
  LEDDEBUG("TRY GET");
  
  if (wifly.isConnected()) {
        LEDDEBUG("CLOSE");
	wifly.close();
  }
 
  if (wifly.open(site, 80)) {
	/* Send the request */
	wifly.println("GET http://smartcam.inventeaza.ro/time/ HTTP/1.0");
	wifly.println();
    } else {
        load_ascii_string("FAIL GET");
        put_string(0, TEXT);
        delay(1000);
    }
    delay(1000);
    
    for(int i=0;i<BUFF_SIZE;++i) {
      if(!wifly.available()) {break;}
      
      char ch = wifly.read();
      TEXT[i] = ch;
    }
}
void loop() 
{ 
  put_time(5000);
  // e is UNDEFINED in the lookup table
  //load_ascii_string("T E S T   D E   A F I S A R E !");
  scroll_text(200, TEXT);
  
  //if(now.second() - last_time > WAIT_REQ) {
  //  parse_html();
  //  last_time = now.second();
  //}

  //TEXT[7] = 3; // Draw the heart
  
  if(last_time + WAIT_REQ < millis())  {
    parse_html();
    last_time = millis();
  }
  
  // Read HTTP response in the main loop()
#if 0
  {
    if (wifly.available() > 0) {
        LEDDEBUG("IN");
	char ch = wifly.read();
        unsigned char crsr = 0;
	if(ch == 'p') {
          while(wifly.available()) {
            ch = wifly.read();
            if(ch == '>') {
              //Start caracter
              crsr = 0;
              while(wifly.available()) {
                ch = wifly.read();
                while(ch!='<' && crsr < BUFF_SIZE) {
                  TEXT[crsr] = ch;
                  crsr++;
                }
              }
            }
          }
        }
     }
  }
#endif /* 0 */  
}




