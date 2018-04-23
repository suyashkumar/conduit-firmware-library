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
#define PORT 443

char prefixed_name[45];
char api_key_topic[100];
char sid[30];
char response_with_quotes[30];
WiFiClient client;
SocketIoClient webSocket;
ESP8266WiFiMulti WiFiMulti;
const char* fingerprint = "BC:E5:91:A5:68:B6:EF:93:A6:82:5A:23:85:45:F8:70:3C:21:F3:AE";

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

void Conduit::callHandler(RequestParams* rp){
    this->_f_map[rp->function_name](rp);
}

Conduit& Conduit::init(){
    this->_client = &webSocket;

    std::function<void (const char*, size_t)> onCall = [this](const char * payload, size_t length) -> void {
        char *pch;
        char* elements[20];
        pch = strtok((char*)payload, ",");
        int i = 0;
        while (pch != NULL) {
            elements[i] = pch;
            pch = strtok(NULL, ",");
            i++;
        }

        RequestParams *rq = new RequestParams;
        rq->function_name = elements[0];
        rq->request_uuid = elements[1];
        this->callHandler(rq);
    };

    std::function<void (const char*, size_t)> onConnect = [this](const char * payload, size_t length) -> void {
            this->initConnection();
    };

    this->_client->on("server_directives", onCall);
    this->_client->on("connect", onConnect);
    //this->_client->begin(this->_conduit_server, 8000, "/socket.io/?transport=websocket");
    this->_client->beginSSL(this->_conduit_server, PORT, "/socket.io/?transport=websocket", fingerprint);
    for (int i=0; i < 5; i++) {
        webSocket.loop();
    }
    return *this;
}

void Conduit::initConnection() {
    this->_client->emit("api_key", this->_prefixed_name);
}

void Conduit::sendResponse(RequestParams *rp, const char* response) {
    // Response message must be send with escaped quotes
    strcpy(response_with_quotes, "\"");
    strcat(response_with_quotes, response);
    strcat(response_with_quotes, "\"");
    this->_client->emit(rp->request_uuid, response_with_quotes);
}

void Conduit::handle() {
  this->_client->loop();
}

void Conduit::startWIFI(const char* ssid, const char* password){
    WiFiMulti.addAP(ssid, password);
  Serial.println("Starting");

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

