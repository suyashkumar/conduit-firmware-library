/*
Conduit.cpp
A library that handles ESP8266 communication with a server (even on private
networks). Consumers of this library can simply write functions and have them
be fired whenver the server fires a given event directed at this device. There is
a 1-1 mapping of event to function. For example the "led" event may fire the
ledToggle function on the device. The communication needed to get that event to the
device and decide what funciton to all is abstracted away entirely by this library.

@author: Suyash Kumar <suyashkumar2003@gmail.com>
*/

#include "Conduit.h"

char prefixed_name[45];
char api_key_topic[100];
char sid[30];
WiFiClient client;
SocketIoClient webSocket;
ESP8266WiFiMulti WiFiMulti;

Conduit::Conduit(const char* name, const char* server, const char* firmware_key){
    // Set name and server
    this->_name = name;
    this->_conduit_server = server;
    this->_firmware_key = firmware_key;

    // Compute _prefixed_name
    strcpy(prefixed_name, "\"");
    strcat(prefixed_name, firmware_key);
    strcat(prefixed_name, "_");
    strcat(prefixed_name, name);
    strcat(prefixed_name, "\"");
    const char* full_prefixed_name = prefixed_name;
    this->_prefixed_name = full_prefixed_name;
    Serial.print("Full prefixed name");
    Serial.print(this->_prefixed_name);

}

void Conduit::addHandler(const char* name, handler f){
    this->_f_map[name] = f;
}

void Conduit::callHandler(const char* name){
    this->_f_map[name]();
}

Conduit& Conduit::init(){
    this->_client = &webSocket;
    Serial.println("1");
    std::function<void (const char*, size_t)> setSID = [this](const char * payload, size_t length) -> void {
        this->_sid = sid;
        strcpy(sid, payload);
    };
    std::function<void (const char*, size_t)> onCall = [this](const char * payload, size_t length) -> void {
        this->callHandler(payload);
    };
    this->_client->on("id_message", setSID);
    this->_client->on("server_directives", onCall);
    this->_client->begin(this->_conduit_server, 8000, "/socket.io/?transport=websocket");

    while (this->_sid == nullptr) {
        webSocket.loop();
        delay(200);
    }

    strcpy(api_key_topic, this->_sid);
    strcat(api_key_topic, "_api_key");

    this->_client->emit(api_key_topic, this->_prefixed_name);
    webSocket.loop();
    return *this;
}

void Conduit::handle() {
  this->_client->loop();
}

void Conduit::startWIFI(const char* ssid, const char* password){
    WiFiMulti.addAP(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFiMulti.run() != WL_CONNECTED) {
	delay(500);
	Serial.print(".");
  }

  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");

}


void removeSpace(char* s) {
	for (char* s2 = s; *s2; ++s2) {
		if (*s2 != ' ')
			*s++ = *s2;
	}
	*s = 0;
}

