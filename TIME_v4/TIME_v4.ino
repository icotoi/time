#include <SoftwareSerial.h>
#include <Wire.h>
#include "RTClib.h"
#include <avr/pgmspace.h>
#include <SPI.h>
#include <WiFlyHQ.h>


unsigned char screen[5*17];   // 85 LED
unsigned int TEXT[100];   // Bufer for integer unicode characters we get from internet
char asciiString[] = "Display printing test.";   // unsigned char ASCII text for testing

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

// Legat de internet
byte server[] = { 
  192, 168, 1, 72 };

WiFly wifly;
SoftwareSerial wifiSerial(A0, A1); 

const char mySSID[] = "fotbalrobotic";
const char myPassword[] = "dorelrobotel";

const char site[] = "hunt.net.nz";

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
  char buf[32];  
  // pinMode(2, INPUT);      // This is the main button, tied to INT0
  // digitalWrite(2, HIGH);  // Enable internal pull up on button
  Serial.begin(9600);
  wifiSerial.begin(9600);
  Serial.print("Free memory: ");
  Serial.println(wifly.getFreeMemory(),DEC);

  //WiFi
  if (!wifly.begin(&wifiSerial, &Serial)) {
    Serial.println("Failed to start wifly");
    terminal();
  }

  /* Join wifi network if not already associated */
  if (!wifly.isAssociated()) {
    /* Setup the WiFly to connect to a wifi network */
    Serial.println("Joining network");
    wifly.setSSID(mySSID);
    wifly.setPassphrase(myPassword);
    wifly.enableDHCP();

    if (wifly.join()) {
      Serial.println("Joined wifi network");
    } 
    else {
      Serial.println("Failed to join wifi network");
      terminal();
    }
  } 
  else {
    Serial.println("Already joined network");
  }

  Serial.print("MAC: ");
  Serial.println(wifly.getMAC(buf, sizeof(buf)));
  Serial.print("IP: ");
  Serial.println(wifly.getIP(buf, sizeof(buf)));
  Serial.print("Netmask: ");
  Serial.println(wifly.getNetmask(buf, sizeof(buf)));
  Serial.print("Gateway: ");
  Serial.println(wifly.getGateway(buf, sizeof(buf)));

  wifly.setDeviceID("Wifly-WebClient");
  Serial.print("DeviceID: ");
  Serial.println(wifly.getDeviceID(buf, sizeof(buf)));

  if (wifly.isConnected()) {
    Serial.println("Old connection active. Closing");
    wifly.close();
  }

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

  cli();
  init_display_pins();
  digitalWrite(11, HIGH); // Display off
  init_display_refresh();
  sei();  
  clear_screen();

  Serial.println(F("Connected to wireless network!"));
}

void loop() 
{ 
  put_time();
  delay(2000);
  // e is UNDEFINED in the lookup table
  load_ascii_string("T e S T   DE   A F I SARE !");
  TEXT[7] = 3; // Draw the heart
  scroll_text(500, TEXT);
}

/* Connect the WiFly serial to the serial monitor. */
void terminal()
{
  while (1) {
    if (wifly.available() > 0) {
      Serial.write(wifly.read());
    }


    if (Serial.available() > 0) {
      wifly.write(Serial.read());
    }
  }
}



