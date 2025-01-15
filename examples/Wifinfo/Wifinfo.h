// **********************************************************************************
// ESP8266 WifInfo WEB Server global Include file
// **********************************************************************************
// Creative Commons Attrib Share-Alike License
// You are free to use/extend this library but please abide with the CC-BY-SA license:
// Attribution-NonCommercial-ShareAlike 4.0 International License
// http://creativecommons.org/licenses/by-nc-sa/4.0/
//
// For any explanation about teleinfo ou use , see my blog
// http://hallard.me/category/tinfo
//
// This program works with the Wifinfo board
// see schematic here https://github.com/hallard/teleinfo/tree/master/Wifinfo
//
// Written by Charles-Henri Hallard (http://hallard.me)
//
// History : V1.00 2015-06-14 - First release
//
// All text above must be included in any redistribution.
//
// **********************************************************************************
#ifndef WIFINFO_H
#define WIFINFO_H

// Include Arduino header
#include <Arduino.h>
#ifdef ESP8266
  // ESP8266
  #include <ESP8266WiFi.h>
  #include <ESP8266WebServer.h>
  #include <ESP8266HTTPClient.h>
  #include <ESP8266mDNS.h>
#elif defined(ESP32)
  // ESP32
  #include <WiFi.h>
  #include <ESP32WebServer.h>    // https://github.com/Pedroalbuquerque/ESP32WebServer download and place in your Libraries folder
  #include <HTTPClient.h>
  #include <ESPmDNS.h>
  #include <esp_wifi.h>
  #include <SPIFFS.h>
#else
  #error "ce n'est ni un ESP8266 ni un ESP32"
#endif

#include <WiFiUdp.h>
#include <EEPROM.h>
#include <Ticker.h>
//#include <WebSocketsServer.h>
//#include <Hash.h>
#include <NeoPixelBus.h>
#include <LibTeleinfo.h>
#include <FS.h>

#ifdef ESP8266
// ESP8266
extern "C" {
  #include "user_interface.h"
}
#else

// ESP32 : Vérifier si tous nécessaires !!!
extern "C" {
  #include "esp_system.h"
  // #include "esp_spi_flash.h"  deprecated
  #include "spi_flash_mmap.h"
}
#endif

#include "webserver.h"
#include "webclient.h"
#include "config.h"

// Décommenter SIMU pour compiler une version de test
//  pour un module non connecté au compteur EDF (simule un ADCO et une valeur HCHC)
// Le port Serial sera alors utilisé pour le DEBUG (accessible via USB pour l'IDE)
// #define SIMU

// Décommenter DEBUG pour une version capable d'afficher du Debug
//  soit sur Serial, soit sur Serial1 si compteur EDF raccordé sur Serial
// Attention : si SIMU n'est pas déclaré, le debug est envoyé sur Serial1
//  donc n'est pas visible au travers du port USB pour Arduino IDE !
#define DEBUG

// Décommenter SYSLOG pour une version capable d'envoyer du Debug
//  vers un serveur rsyslog du réseau
// #define SYSLOG

// Linky mode Historique ou Linky en mode Standard
// A définir dans l'onglet Configuration puis déployer Avancée
// Il faut redémarrer Wifinfo pour que le changement soit pris en compte

#ifdef ESP8266
  #ifdef SIMU
    #define DEBUG_SERIAL  Serial
  #else
    #define DEBUG_SERIAL  Serial1
  #endif  //SIMU
#else
  //ESP32
  #define DEBUG_SERIAL  Serial
#endif

#define WIFINFO_VERSION "3.0.0"

// I prefix debug macro to be sure to use specific for THIS library
// debugging, this should not interfere with main sketch or other 
// libraries
#ifdef SYSLOG
  #include <Syslog.h>

  #define APP_NAME "Wifinfo"

  #define Debug(x)     Myprint(x)
  #define Debugln(x)   Myprintln(x)
  #define DebugF(x)    Myprint(F(x))
  #define DebuglnF(x)  Myprintln(F(x))

  // Ceci compile et transmet les arguments
  #define Debugf(...)    Myprintf(__VA_ARGS__)

  #define Debugflush() Myflush()
#else
  #ifdef DEBUG
    #define Debug(x)     DEBUG_SERIAL.print(x)
    #define Debugln(x)   DEBUG_SERIAL.println(x)
    #define DebugF(x)    DEBUG_SERIAL.print(F(x))
    #define DebuglnF(x)  DEBUG_SERIAL.println(F(x))
    #define Debugf(...)  DEBUG_SERIAL.printf(__VA_ARGS__)
    #define Debugflush() DEBUG_SERIAL.flush()
  #else
    #define Debug(x) 
    #define Debugln(x)
    #define DebugF(x) 
    #define DebuglnF(x) 
    #define Debugf(...)
    #define Debugflush()
  #endif
#endif // SYSLOG

#define BLINK_LED_MS   50 // 50 ms blink
#define RGB_LED_PIN    14 
#define RED_LED_PIN    12

// value for HSL color
// see http://www.workwithcolor.com/blue-color-hue-range-01.htm
#define COLOR_RED             0
#define COLOR_ORANGE         30
#define COLOR_ORANGE_YELLOW  45
#define COLOR_YELLOW         60
#define COLOR_YELLOW_GREEN   90
#define COLOR_GREEN         120
#define COLOR_GREEN_CYAN    165
#define COLOR_CYAN          180
#define COLOR_CYAN_BLUE     210
#define COLOR_BLUE          240
#define COLOR_BLUE_MAGENTA  275
#define COLOR_MAGENTA	      300
#define COLOR_PINK		      350

// GPIO 1 TX on board blue led
#ifdef BLU_LED_PIN
#define LedBluON()  {digitalWrite(BLU_LED_PIN, 0);}
#define LedBluOFF() {digitalWrite(BLU_LED_PIN, 1);}
#else
#define LedBluON()  {}
#define LedBluOFF() {}
#endif
// GPIO 12 red led
#define LedRedON()  {digitalWrite(RED_LED_PIN, 1);}
#define LedRedOFF() {digitalWrite(RED_LED_PIN, 0);}

// Light off the RGB LED
#ifndef RGB_LED_PIN
#define LedRGBOFF() {}
#define LedRGBON(x) {}
#endif
// sysinfo informations
typedef struct 
{
  String sys_uptime;
} _sysinfo;

// Exported variables/object instancied in main sketch
// ===================================================
#ifdef ESP8266
  extern ESP8266WebServer server;
#else
  extern ESP32WebServer server;
#endif

extern WiFiUDP OTA;
extern TInfo tinfo;
extern uint8_t rgb_brightness;
extern unsigned long seconds;
extern _sysinfo sysinfo;
extern Ticker Tick_mqtt;
extern Ticker Tick_emoncms;
extern Ticker Tick_jeedom;
extern Ticker Tick_httpRequest;
extern String optval;     // On conserve le même nom


// Exported function located in main sketch
// ===================================================
void ResetConfig(void);
void Task_mqtt();
void Task_emoncms();
void Task_jeedom();
void Task_httpRequest();

#ifdef SYSLOG
void Myprint(void);
void Myprint(char *msg);
void Myprint(const char *msg);
void Myprint(String msg);
void Myprint(int i);
void Myprint(unsigned int i);
void Myprintf(const char * format, ...);
void Myprintln(void);
void Myprintln(char *msg);
void Myprintln(const char *msg);
void Myprintln(String msg);
void Myprintln(const __FlashStringHelper *msg);
void Myprintln(int i);
void Myprintln(unsigned int i);
void Myprintln(uint32_t i);
void Myflush(void);
#endif  // SYSLOG

#endif
