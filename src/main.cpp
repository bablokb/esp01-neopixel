#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <Adafruit_NeoPixel.h>
#include "NeoPatterns.h"
#include "main_html.h"

void onComplete();

//--- constants   ------------------------------------------------------------

#include "config.h"

#ifndef SSID
  #error "!!!!!!!!!! please define SSID in config.h !!!!!!!!!!"
#endif
const char* ssid = SSID;

#ifndef PASSWD
  #error "!!!!!!!!!! please define PASSWD in config.h !!!!!!!!!!"
#endif
const char* password = PASSWD;

#ifndef PIN_DATA
  #define PIN_DATA 2
#endif

#ifndef SERVER_PORT
  #define SERVER_PORT 80
#endif

#ifndef N_PIXELS
  #define N_PIXELS 7
#endif

//web server
ESP8266WebServer this_server(SERVER_PORT);

// version with Adafruit multi-tasking design
//https://learn.adafruit.com/multi-tasking-the-arduino-part-3?view=all#put-it-all-together-dot-dot-dot


/****************************************************************
 *
 *
 *
 ****************************************************************/

// Define a NeoPattern for the ring
//  as well as some completion routines
NeoPatterns neopixels(N_PIXELS, PIN_DATA, NEO_RGB + NEO_KHZ800, &onComplete);

// neopixels Completion Callback
void onComplete() {
  neopixels.reverse();
}

void handleRoot() {
  String page = main_html;
  this_server.send(200, "text/html", page);
}

void returnFail(String msg) {
  this_server.sendHeader("Connection", "close");
  this_server.sendHeader("Access-Control-Allow-Origin", "*");
  this_server.send(500, "text/plain", msg + "\r\n");
  Serial.println("RGB Color - Bad Args");
}

void handleRGB() {
  long  LEDvalueR = 255;
  long  LEDvalueG = 255;
  long  LEDvalueB = 255;

  if (!this_server.hasArg("R")) return returnFail("BAD ARGS");
  LEDvalueR = (this_server.arg("R").toInt());
  if (!this_server.hasArg("G")) return returnFail("BAD ARGS");
  LEDvalueG = (this_server.arg("G").toInt());
  if (!this_server.hasArg("B")) return returnFail("BAD ARGS");
  LEDvalueB = this_server.arg("B").toInt();

  this_server.send(200, "text/plain",  "RGB Color");
  neopixels.initColorWipe(neopixels.Color(LEDvalueR, LEDvalueG, LEDvalueB), 30); // RGB

  Serial.println("RGB Color");
}

// --- configure handler for server   ----------------------------------------

void configureServer() {
  this_server.on("/", handleRoot);
  this_server.on("/set", handleRGB);

  this_server.on("/R", []() {
    this_server.send(200);
    Serial.println("Red");
    neopixels.initColorWipe(neopixels.Color(255,0,0),50);
  });

  this_server.on("/G", []() {
    this_server.send(200);
    Serial.println("Green");
    neopixels.initColorWipe(neopixels.Color(0,255,0),50);
  });


  this_server.on("/B", []() {
    this_server.send(200);
    Serial.println("Blue");
    neopixels.initColorWipe(neopixels.Color(0,0,255),50);
  });


  this_server.on("/W", []() {
    this_server.send(200);
    Serial.println("White");
    neopixels.initColorWipe(neopixels.Color(255, 255, 255), 80);
  });

  this_server.on("/Y", []() {
    this_server.send(200);
    Serial.println("Rainbow Cycle");
    neopixels.initRainbowCycle(100);
  });
}

/****************************************************************/

void setup(void) {
  Serial.begin(115200);

  // Initialize
  neopixels.begin();
  neopixels.initColorWipe(neopixels.Color(255, 0, 0), 50); // Red
  neopixels.updatePattern();


  // Connect to WiFi network
  Serial.print("Connecting to: ");
  Serial.println(ssid);
  neopixels.initColorWipe(neopixels.Color(0, 255, 0), 50); // Green

#ifdef STATIC_IP
  IPAddress staticIP(STATIC_IP);
  IPAddress gateway(GATEWAY_IP);
  IPAddress subnet(SUBNET);
  IPAddress dns(DNS_IP);
  WiFi.config(staticIP,subnet,gateway,dns);
#endif
  WiFi.begin(ssid, password);

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  neopixels.initColorWipe(neopixels.Color(0, 0, 255), 50); // Blue

  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  neopixels.initRainbowCycle(16);

  configureServer();
  this_server.begin();
}

void loop(void) {
  neopixels.updatePattern();
  this_server.handleClient();
}
