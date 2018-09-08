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

#include <Arduino.h>
#include <WiFiClient.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <SocketIoClient.h>
#include <Hash.h>
#include <RequestParams.h>
#define default_fingerprint "62:78:3B:79:74:10:4B:EE:5C:DD:E6:0A:BA:59:4F:DC:FF:00:18:C3"

typedef std::function<int (RequestParams* rp)> handler;
void removeSpace(char* s);


class Conduit {
private:
	SocketIoClient* _client;
	const char* _conduit_server;
	const char* _name;
	const char* _prefixed_name;
	const char* _firmware_key;
	std::map<String, handler> _f_map;
    const char* _sid;
	const char* _fingerprint;
public: 
	Conduit(const char* name, const char* server, const char* firmware_key, const char* fingerprint = default_fingerprint); 
	Conduit& init();
    void initConnection();
	void addHandler(const char* name, handler f);
    void callHandler(RequestParams *rp);
    void handle();
    void reconnect();
    void msgCallback(char* topic, byte* payload, unsigned int length);
    void publishMessage(const char* message);
    void publishData(const char* message, const char* dataStream);
    void startWIFI(const char* ssid, const char* password);
	void sendResponse(RequestParams *rp, const char* response);
};
