#ifndef WEB_HANDLER_H
#define WEB_HANDLER_H

#define WEB_UPDATE_COOLDOWN 50
#define WEB_WIFI_STOP_RETRY_AFTER 60000

#ifdef ESP32
#include <WiFi.h>
#include <WebServer.h>
using WebServerT = WebServer;
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
using WebServerT = ESP8266WebServer;
#endif

extern WebServerT web_server;

void web_setup();
void web_loop();

#endif