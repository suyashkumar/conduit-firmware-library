/*
server.ino
Example for my library that handles ESP8266 communication with a server (even on private
networks). Consumers of this library can simply write functions and have them
be fired whenver the server fires a given event directed at this device. There is
a 1-1 mapping of event to function. For example the "led" event may fire the
ledToggle function on the device. The communication needed to get that event to the
device and decide what funciton to all is abstracted away entirely by this library.

@author: Suyash Kumar <suyashkumar2003@gmail.com>
*/
#include <Arduino.h> 
#include <wifi_info.h> // comment this out and fill in the below two lines
#include <Conduit.h>

#define LED D0

// Fill out the below Github peeps:
//const char* ssid = "MOTOE0E4";
//const char* password = "";

Conduit conduit("suyash", "192.168.1.217", "a"); // or "suyash", "home.suyash.io"
int ledStatus = 0;

int ledToggle(){
  digitalWrite(LED, (ledStatus) ? LOW : HIGH);
  ledStatus = (ledStatus) ? 0 : 1;
  Serial.println("Toggled");
}


void setup(void){
  Serial.begin(115200); // Start serial
  Serial.print("START");
  pinMode(LED, OUTPUT); // Set LED pin to output
  digitalWrite(LED, HIGH);

  conduit.startWIFI(ssid, password); // Config/start wifi
  conduit.init();
  conduit.addHandler("ledToggle", &ledToggle);

}

void loop(void){
  conduit.handle();
}
