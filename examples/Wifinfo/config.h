// **********************************************************************************
// ESP8266 Teleinfo WEB Server configuration Include file
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
// Warning EEPROM size limited at 1024 byte by EEPROM library 
//
// All text above must be included in any redistribution.
//
// **********************************************************************************
#ifndef __CONFIG_H__
#define __CONFIG_H__

// Include main project include file
#include "Wifinfo.h"

#define CFG_SSID_SIZE 		32
#define CFG_PSK_SIZE  		64
#define CFG_HOSTNAME_SIZE 16

#define CFG_MQTT_HOST_SIZE 		  32
#define CFG_MQTT_PSWD_SIZE 	    64
#define CFG_MQTT_USER_SIZE      32
#define CFG_MQTT_TOPIC_SIZE     32
#define CFG_MQTT_DEFAULT_HOST   "monserveur.org"
#define CFG_MQTT_DEFAULT_PSWD   "password"
#define CFG_MQTT_DEFAULT_USER   "user"
#define CFG_MQTT_DEFAULT_TOPIC  "TIC"
#define CFG_MQTT_DEFAULT_PORT   1883

#define CFG_EMON_HOST_SIZE 		32
#define CFG_EMON_APIKEY_SIZE 	32
#define CFG_EMON_URL_SIZE 		32
#define CFG_EMON_DEFAULT_PORT   80
#define CFG_EMON_DEFAULT_HOST   "emoncms.org"
#define CFG_EMON_DEFAULT_URL    "/input/post.json"

#define CFG_JDOM_HOST_SIZE      32
// M.G.
#define CFG_JDOM_APIKEY_SIZE    64
#define CFG_JDOM_URL_SIZE       64
#define CFG_JDOM_ADCO_SIZE      12
#define CFG_JDOM_DEFAULT_PORT   80
#define CFG_JDOM_DEFAULT_HOST   "jeedom.local"
#define CFG_JDOM_DEFAULT_URL    "/plugins/teleinfo/core/php/jeeTeleinfo.php"
#define CFG_JDOM_DEFAULT_ADCO   "0000111122223333"

#define CFG_HTTPREQ_HOST_SIZE    32
#define CFG_HTTPREQ_PATH_SIZE    150
#define CFG_HTTPREQ_DEFAULT_PORT 80
#define CFG_HTTPREQ_DEFAULT_HOST "127.0.0.1"
#define CFG_HTTPREQ_DEFAULT_PATH  "/json.htm?type=command&param=udevice&idx=1&nvalue=0&svalue=%HCHP%;%HCHC%;0;0;%PAPP%;0"

// Port pour l'OTA
#define DEFAULT_OTA_PORT      8266
#define DEFAULT_OTA_AUTH      "OTA_WifInfo"
//#define DEFAULT_OTA_AUTH    ""
#define DEFAULT_SYSLOG_PORT   514

// Bit definition for different configuration modes
#define CFG_LCD				        0x0001	// Enable display
#define CFG_DEBUG			        0x0002	// Enable serial debug
#define CFG_RGB_LED           0x0004  // Enable RGB LED
#define CFG_BAD_CRC           0x8000  // Bad CRC when reading configuration

// Web Interface Configuration Form field names
#define CFG_FORM_SSID     FPSTR("ssid")
#define CFG_FORM_PSK      FPSTR("psk")
#define CFG_FORM_HOST     FPSTR("host")
#define CFG_FORM_AP_PSK   FPSTR("ap_psk")
#define CFG_FORM_OTA_AUTH FPSTR("ota_auth")
#define CFG_FORM_OTA_PORT FPSTR("ota_port")
#define CFG_FORM_SYSLOG_HOST FPSTR("syslog_host")
#define CFG_FORM_SYSLOG_PORT FPSTR("syslog_port")

#define CFG_FORM_LCD         FPSTR("cfg_oled")
#define CFG_FORM_CFG_RGB     FPSTR("cfg_rgb")

#define CFG_FORM_MQTT_HOST  FPSTR("mqtt_host")
#define CFG_FORM_MQTT_PORT  FPSTR("mqtt_port")
#define CFG_FORM_MQTT_USER  FPSTR("mqtt_user")
#define CFG_FORM_MQTT_PSWD  FPSTR("mqtt_pswd")
#define CFG_FORM_MQTT_TOPIC FPSTR("mqtt_topic")
#define CFG_FORM_MQTT_FREQ  FPSTR("mqtt_freq")

#define CFG_FORM_EMON_HOST  FPSTR("emon_host")
#define CFG_FORM_EMON_PORT  FPSTR("emon_port")
#define CFG_FORM_EMON_URL   FPSTR("emon_url")
#define CFG_FORM_EMON_KEY   FPSTR("emon_apikey")
#define CFG_FORM_EMON_NODE  FPSTR("emon_node")
#define CFG_FORM_EMON_FREQ  FPSTR("emon_freq")

#define CFG_FORM_JDOM_HOST  FPSTR("jdom_host")
#define CFG_FORM_JDOM_PORT  FPSTR("jdom_port")
#define CFG_FORM_JDOM_URL   FPSTR("jdom_url")
#define CFG_FORM_JDOM_KEY   FPSTR("jdom_apikey")
#define CFG_FORM_JDOM_ADCO  FPSTR("jdom_adco")
#define CFG_FORM_JDOM_FREQ  FPSTR("jdom_freq")

#define CFG_FORM_HTTPREQ_HOST  FPSTR("httpreq_host")
#define CFG_FORM_HTTPREQ_PORT  FPSTR("httpreq_port")
#define CFG_FORM_HTTPREQ_PATH  FPSTR("httpreq_path")
#define CFG_FORM_HTTPREQ_FREQ  FPSTR("httpreq_freq")
#define CFG_FORM_HTTPREQ_SWIDX FPSTR("httpreq_swidx")

#define CFG_FORM_IP  FPSTR("wifi_ip")
#define CFG_FORM_GW  FPSTR("wifi_gw")
#define CFG_FORM_MSK FPSTR("wifi_msk")

#define CFG_FORM_LINKY_MOD FPSTR("linky_mod")

#pragma pack(push)  // push current alignment to stack
#pragma pack(1)     // set alignment to 1 byte boundary

// Config for mqtt
// 256 Bytes

typedef struct 
{
  char  host[CFG_MQTT_HOST_SIZE+1]; 	  // FQDN 
  char  user[CFG_MQTT_USER_SIZE+1];     // Secret
  char  pswd[CFG_MQTT_PSWD_SIZE+1];     // Post URL
  char  topic[CFG_MQTT_TOPIC_SIZE+1];   // Post URL
  uint16_t port;    					          // Protocol port (1883)
  uint32_t freq;                        // refresh rate
//  uint8_t  filler[22];  				      // in case adding data in config avoiding loosing current conf by bad crc*/
} _mqtt;

// Config for emoncms
// 128 Bytes
typedef struct 
{
  char  host[CFG_EMON_HOST_SIZE+1]; 	  // FQDN 
  char  apikey[CFG_EMON_APIKEY_SIZE+1]; // Secret
  char  url[CFG_EMON_URL_SIZE+1];  	    // Post URL
  uint16_t port;                        // Protocol port (HTTP/HTTPS)
  uint8_t  node;     					          // optional node
  uint32_t freq;                        // refresh rate
//  uint8_t  filler[22];  				      // in case adding data in config avoiding loosing current conf by bad crc*/
} _emoncms;

// Config for jeedom
// 192 Bytes
typedef struct 
{
  char  host[CFG_JDOM_HOST_SIZE+1];     // FQDN 
  char  apikey[CFG_JDOM_APIKEY_SIZE+1]; // Secret
  char  url[CFG_JDOM_URL_SIZE+1];       // Post URL
  char  adco[CFG_JDOM_ADCO_SIZE+1];     // Identifiant compteur
  uint16_t port;                        // Protocol port (HTTP/HTTPS)
  uint32_t freq;                        // refresh rate
  uint8_t filler[10];                   // in case adding data in config avoiding loosing current conf by bad crc*/
} _jeedom;

// Config for http request
// 192 Bytes
typedef struct 
{
  char  host[CFG_HTTPREQ_HOST_SIZE+1];  // FQDN 
  char  path[CFG_HTTPREQ_PATH_SIZE+1];  // Path
  uint16_t port;                        // Protocol port (HTTP/HTTPS) 
  uint32_t freq;                        // refresh rate
  uint16_t swidx;                       // Switch index (into Domoticz)
//  uint8_t filler[22];                 // in case adding data in config avoiding loosing current conf by bad crc*/
} _httpRequest;

// Config saved into eeprom
// 1024 bytes total including CRC
// Warning La librairie semble limiter la taille à 1024 pour un ESP8266
typedef struct 
{
  char  ssid[CFG_SSID_SIZE+1]; 	        // SSID     
  char  psk[CFG_PSK_SIZE+1]; 		        // Pre shared key
  char  host[CFG_HOSTNAME_SIZE+1];      // Hostname 
  char  ap_psk[CFG_PSK_SIZE+1];         // Access Point Pre shared key
  char  ota_auth[CFG_PSK_SIZE+1];       // OTA Authentication password
  uint32_t config;           		        // Bit field register 
  uint16_t ota_port;         		        // OTA port 
  char     syslog_host[64];             // Adresse IP ou DNS du serveur rsyslog
  uint16_t syslog_port;                 // port rsyslog (generalement 514)
  _mqtt mqtt;                           // mqtt configuration
  _emoncms emoncms;                     // Emoncms configuration
  _jeedom  jeedom;                      // jeedom configuration
  _httpRequest httpReq;                 // HTTP request
  uint8_t linky_mode_standard;          // 0 si mode historique 1 si mode standard
  uint8_t  filler[44];      		        // in case adding data in config avoiding loosing current conf by bad crc
  uint16_t crc;
} _Config;


// Exported variables/object instancied in main sketch
// ===================================================
extern _Config config;

#pragma pack(pop)
 
// Declared exported function from route.cpp
// ===================================================
bool readConfig(bool clear_on_error=true);
bool saveConfig(void);
void showConfig(void);


#endif 
