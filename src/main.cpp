#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
/*Author: Mwape
 * Date: 2018
 * V1
 * For use with NODEMCU

*/


void shiftAnimation();
void testLED();
void toggleSwitch();
void relayOn();
void relayOFF();

char ssid[] = "xxx";
char pass[] = "xxx";
//int ip[] = new int[20]; //This is where we will store the address of our local ip address. In a integer array of 20.

ESP8266WebServer server(80);    //instatiate an object of webserver
IPAddress ip;

unsigned long TOGGLELEDBITS[2] =
{
  0b000001010111000110001110,   //binary for displaying OFF on the 3 seven segment displays
  0b111111110000010111101010  //binary for displaying ON on the 3 seven segment displays.
};

unsigned long ANIMLEDBITS[4] =
{
  0b111111110000010111101010,   //Animation sequence for dots
  0b111111100000010111101010,
  0b111111100000010011101010,
  0b111111100000010011101011
};

int latch = D5; // pin D8 on NodeMCU boards
int clockP = D6; // pin D5 on NodeMCU boards
int data = D7; // pin D7 on NodeMCU boards
int Relay = D8; //Test LED for our webserver

int current = HIGH;

void setup() {
pinMode(LED_BUILTIN, OUTPUT);     // Initialize the LED_BUILTIN pin as an output
pinMode(data, OUTPUT);
pinMode(latch, OUTPUT);
pinMode(clockP, OUTPUT);
pinMode(Relay, OUTPUT);

digitalWrite(latch, LOW);
  // shift out the bits:
shiftOut(data, clockP, LSBFIRST, TOGGLELEDBITS[0]);
shiftOut(data, clockP, LSBFIRST, TOGGLELEDBITS[0]>>8);
shiftOut(data, clockP, LSBFIRST, TOGGLELEDBITS[0]>>16);
digitalWrite(latch, HIGH);

Serial.begin(115200); /* begin serial for debug */

Serial.print("Attempting to connect...");

 WiFi.disconnect();
 WiFi.begin(ssid,pass);
 delay(500);

 while(WiFi.status() != WL_CONNECTED){
  delay(500);
  Serial.println(",");
 }


 Serial.print("SSID....,");
 Serial.println(WiFi.SSID());
 Serial.println("Successfully connected...");

// ip = WiFi.localIP();

 Serial.print("IP address of MCU: ");
 Serial.println(WiFi.macAddress());
 Serial.println(WiFi.localIP());
 WiFi.printDiag(Serial);
 ip = WiFi.localIP();
//Add route to our webserver
//if device on this network sends request throguh this rought, we send responce code, type and text.

digitalWrite(Relay, HIGH);  //because relays are active low.

server.on("/", [](){
 server.send(200, "text/plain", "Server ONLINE!");
});

server.on("/relay", toggleSwitch);
server.on("/on", relayOn);
server.on("/off", relayOFF);

 server.begin();  //start server

   digitalWrite(latch, LOW);
   // shift out the bits:
   shiftOut(data, clockP, LSBFIRST, TOGGLELEDBITS[1]);
   shiftOut(data, clockP, LSBFIRST, TOGGLELEDBITS[1]>>8);
   shiftOut(data, clockP, LSBFIRST, TOGGLELEDBITS[1]>>16);
   digitalWrite(latch, HIGH);

}

// the loop function runs over and over again forever
void loop() {
  server.handleClient();

  digitalWrite(LED_BUILTIN, LOW);   // Turn the LED on (Note that LOW is the voltage level
                                    // but actually the LED is on; this is because
                                    // it is active low on the ESP-01)
  delay(1000);                      // Wait for a second
  digitalWrite(LED_BUILTIN, HIGH);  // Turn the LED off by making the voltage HIGH
  delay(1000);                      // Wait for two seconds (to demonstrate the active low LED)

 shiftAnimation();
 //toggleSwitch();
}


void shiftAnimation(){    //animates dots on the bottom
  for(int i = 0; i<4; i++){
    digitalWrite(latch, LOW);
    // shift out the bits:
    shiftOut(data, clockP, LSBFIRST, ANIMLEDBITS[i]);
    shiftOut(data, clockP, LSBFIRST, ANIMLEDBITS[i]>>8);
    shiftOut(data, clockP, LSBFIRST, ANIMLEDBITS[i]>>16);
    digitalWrite(latch, HIGH);
    delay(200);
  }
}

void testLED(){
  delay(10);
  digitalWrite(Relay, !current);
  server.send(200,"");
  current =!current;
}

void toggleSwitch(){
  delay(10);
  digitalWrite(Relay, !current);
  server.send(200, "text/plain", "EROS II+ is a Complete Success!");
  current =!current;
}

void relayOn(){
  delay(10);
  digitalWrite(Relay, LOW);
  server.send(200, "text/plain", "EROS II+ is a Complete Success! Relay ON!");
}

void relayOFF(){
  delay(10);
  digitalWrite(Relay, HIGH);
  server.send(200, "text/plain", "EROS II+ is a Complete Success! Relay OFF!");
}
