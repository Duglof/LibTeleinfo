// **********************************************************************************
// ESP8266 Teleinfo WEB Server
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
// ===================================================================
// Wifinfo : Connection à votre réseau Wifi
// ===================================================================
//  A faire une seule fois ou après des changements dans le répertoire data
//
// Téléverser Wifinfo et le data sur votre ESP12E
// Alimenter votre module ESP12E par le cable USB (ou autre) 
// Avec votre téléphone
//   Se connecter au réseau Wifi Wifinfo-xxxxxx
//   Ouvrir votre navigateur préféré (Chrome ou autre)
//   Accéder à l'url http://192.168.4.1 (la page web de Wifinfo doit appaître)
//   Sélectionner l'onglet 'Configuration'
//   Renseigner
//     Réseau Wifi
//     Clé Wifi
//   Cliquer sur 'Enregistrer'
//   Enfin dans la partie 'Avancée' cliquer sur Redémarrer Wifinfo
//   Wifinfo se connectera à votre réseau Wifi
//   Si ce n'est pas le cas c'est que ne nom du réseau ou la clé sont erronés
// ===================================================================
//
// Modifié par Dominique DAMBRAIN 2017-07-10 (http://www.dambrain.fr)
//       Version 1.0.5
//       Librairie LibTeleInfo : Allocation statique d'un tableau de stockage 
//           des variables (50 entrées) afin de proscrire les malloc/free
//           pour éviter les altérations des noms & valeurs
//       Modification en conséquence des séquences de scanning du tableau
//       ATTENTION : Nécessite probablement un ESP-8266 type Wemos D1,
//        car les variables globales occupent 42.284 octets
//
//       Version 1.0.5a (11/01/2018)
//       Permettre la mise à jour OTA à partir de fichiers .ino.bin (Auduino IDE 1.8.3)
//       Ajout de la gestion d'un switch (Contact sec) relié à GND et D5 (GPIO-14)
//          Décommenter le #define SENSOR dans Wifinfo.h
//          Pour être utilisable avec Domoticz, au moins l'URL du serveur et le port
//          doivent être renseignés dans la configuration HTTP Request, ainsi que 
//          l'index du switch (déclaré dans Domoticz)
//          L'état du switch (On/Off) est envoyé à Domoticz au boot, et à chaque
//            changement d'état
//       Note : Nécessité de flasher le SPIFFS pour pouvoir configurer l'IDX du switch
//              et flasher le sketch winfinfo.ino.bin via interface Web
//       Rendre possible la compilation si define SENSOR en commentaire
//              et DEFINE_DEBUG en commentaire (aucun debug, version Production...)
//
//       Version 1.0.6 (04/02/2018) Branche 'syslog' du github
//		      Ajout de la fonctionnalité 'Remote Syslog'
//		        Pour utiliser un serveur du réseau comme collecteur des messages Debug
//            Note : Nécessité de flasher le SPIFFS pour pouvoir configurer le remote syslog
//          Affichage des options de compilation sélectionnées dans l'onglet 'Système'
//            et au début du Debug + syslog éventuels
//
//       Modifié par theGressier
//       Version 1.0.7 (02/09/2019) Branche 'syslog' du github 
//          Changement fonction jeedomPost et httpPost
//          Compatible jeedom V4
//
//        Version 1.0.7 (10/12/2019) Use new WiFi library feature
//           Reconnect WiFi automatically after incident
//
//        Version 1.0.8 (29/12/2024) Update for jeedom API key 64 characters
//           Teleinfo clé API 64 caractères
//             Wifinfo.h : #define WIFINFO_VERSION "1.0.8"
//             config.h : #define CFG_JDOM_APIKEY_SIZE  64
//             data : index.htm.gz : index.htm : id="jdom_apikey" : maxlength="64"
//             Wifinfo.ino : char buff[300] //To format debug strings but also use to format jeedom request
//           Warning C++ conversion const char * to char *
//             Wifinfo.ino : add void Myprint(const char *msg) to remove C++ warning on Debug(".");
//           LibTeleinfo.h / LibTeleinfo.cpp : correction erreurs de compilation
//
//        Version 2.0.0 (15/01/2025)
//          Merge avec https://github.com/hallard/LibTeleinfo/tree/master
//            Integration nouvelle LibTeleinfo compatible mode Historique et mode Standard
//              Presque identique à https://github.com/arendst/Tasmota/tree/development/lib/lib_div/LibTeleinfo
//          Compile en Linky mode Historique et Linky mode Standard
//            Configuration dans l'interface Web Onglet Configuration / Avancée
//              Sélectionner le mode du Linky et cliquer sur enregistrer
//              Cliquer ensuite sur Redémarrer Wifinfo pour que la configuration soit prise en compte
//          Les options de comilation sont affichées sur la page web dans l'onglet Système
//          Ajout de Mqtt : From https://github.com/Davcail/LibTeleinfo-syslog-mqtt
//
//          Environment
//           Arduino IDE 1.8.18
//             Préférences : https://arduino.esp8266.com/stable/package_esp8266com_index.json
//             Folder Arduino/tools : https://github.com/esp8266/arduino-esp8266fs-plugin/releases/download/0.5.0/ESP8266FS-0.5.0.zip
//               (Arduino/tools/ESP8266FS/tool/esp8266fs.jar)
//             Folder Arduino/libraries : Wifinfo/librairie/Syslog-master.zip : uncompress
//             Folder Arduino/libraries : NeoPixelBus_by_Makuna V2.8.3 : Install from Arduino IDE
//             Type de carte : NodeMCU 1.0 (ESP-12E Module)
//             Compilation / Téléversement
//               Options de compilation dans Wifinfo.h (SIMU, DEBUG, SYSLOG)
//               Croquis/Compiler (sans erreur !!!)
//               Tools/ESP8266 Data Upload (Arduino/tools/ESP8266FS doit être installé)
//               Croquis/Téléverser
//           Jeedom V4.4.19 : Plugin Teleinfo by Noyax37 V4.8.7
//             Re-Installer les dépendances
//             Désactiver la gestion automatique du démon (il doit être NOK)
//             Bloquer la création automatique des compteurs : Ne pas cocher
//             Utilisation d’un modem de téléinformation : Ne pas cocher
//             Activer le MQTT : Ne pas cocher
//             Cliquer sur sauvegarder
//             Au premier message reçu, le compteur est créé automatiquement
//          Hardware : Node MCU Kit de développement V3 CH340 NodeMCU + Motor Shield Wifi Esp8266 Esp-12e
//            - RGB LED connected on pin 14 (D5 /GPIO14) : WS2812B : https://wiki.mchobby.be/index.php?title=NeoPixel-UserGuide
//            - Red LED connected on pin 12
//            - Teleinfo connected to RXD2 (GPIO13) Mode historique 1200 bauds
// **********************************************************************************
// Global project file
#include "Wifinfo.h"


// PubSubClient V2.8.0 : The maximum message size, including header, is 256 bytes by default. This is configurable via MQTT_MAX_PACKET_SIZE in PubSubClient.h
// Better define : client.setBufferSize(512);
#include <PubSubClient.h> //attention mettre #define MQTT_MAX_PACKET_SIZE 512, sinon le payload data ne se raffraichit pas.
#include <ArduinoOTA.h>
#include <EEPROM.h>
#include <Ticker.h>


#ifdef ESP8266
  ESP8266WebServer server(80);
#else
  ESP32WebServer server(80);
#endif

bool ota_blink;
String optval;    // Options de compilation

// Teleinfo
TInfo tinfo;

// RGB Led
#ifdef RGB_LED_PIN
//NeoPixelBus rgb_led = NeoPixelBus(1, RGB_LED_PIN, NEO_RGB | NEO_KHZ800);
//NeoPixelBus<NeoGrbFeature, NeoEsp8266BitBang800KbpsMethod> rgb_led(1, RGB_LED_PIN);

#ifdef ESP8266
  // ESP8266
  NeoPixelBus<NeoRgbFeature, NeoEsp8266BitBang800KbpsMethod> rgb_led(1, RGB_LED_PIN);
#else
  // ESP32
  NeoPixelBus<NeoRgbFeature, Neo800KbpsMethod> rgb_led(1, RGB_LED_PIN);
#endif

#endif


// define whole brigtness level for RGBLED (50%)
uint8_t rgb_brightness = 50;
// LED Blink timers
Ticker rgb_ticker;
Ticker blu_ticker;
Ticker red_ticker;
Ticker Every_1_Sec;
Ticker Tick_mqtt;
Ticker Tick_emoncms;
Ticker Tick_jeedom;
Ticker Tick_httpRequest;

volatile boolean task_1_sec = false;
volatile boolean task_mqtt = false;
volatile boolean task_emoncms = false;
volatile boolean task_jeedom = false;
volatile boolean task_httpRequest = false;
volatile boolean task_updsw = false;
unsigned long seconds = 0;

// sysinfo data
_sysinfo sysinfo;

#ifdef SIMU
//for tests
uint8_t flags = 8;
int loop_cpt = 60000;
String name2 = "HCHC";
char * s2 = (char *)name2.c_str();
String value2 = "000060000";
char * v2 = (char *) value2.c_str();
#endif

//Client Mqtt
WiFiClient MqttClient;
PubSubClient MQTTclient(MqttClient);
bool Mqtt_Init=0;
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 0;
const int   daylightOffset_sec = 3600;

#ifdef SYSLOG
WiFiUDP udpClient;
Syslog syslog(udpClient, SYSLOG_PROTO_IETF);

char logbuffer[1024];
char waitbuffer[1024];

char *lines[50];
int in=-1;
int out=-1;

unsigned int pending = 0 ;
volatile boolean SYSLOGusable=false;
volatile boolean SYSLOGselected=false;
int plog=0;

void convert(const __FlashStringHelper *ifsh)
{
  PGM_P p = reinterpret_cast<PGM_P>(ifsh);
  plog=0;
  while (1) {
    unsigned char c = pgm_read_byte(p++);
    if (c == 0) {
      logbuffer[plog]=0;
      break;
    }
    logbuffer[plog]=c;
    plog++;
  }
}

void process_line(char *msg) {
    strcat(waitbuffer,msg);
    pending=strlen(waitbuffer);
    if( waitbuffer[pending-1] == 0x0D || waitbuffer[pending-1] == 0x0A) {
      //Cette ligne est complete : l'envoyer !
      for(int i=0; i < pending-1; i++) {
        if(waitbuffer[i] <= 0x20)
          waitbuffer[i] = 0x20;
      }
      syslog.log(LOG_INFO,waitbuffer);
      delay(2*pending);
      memset(waitbuffer,0,255);
      pending=0;
      
    }
}


// Toutes les fonctions aboutissent sur la suivante :
void Myprint(char *msg) {
  
#ifdef DEBUG
  DEBUG_SERIAL.print(msg);
#endif

  if( SYSLOGusable ) {
    process_line(msg);   
  } else if ( SYSLOGselected) {
    //syslog non encore disponible
    //stocker les messages à envoyer plus tard
    in++;
    if(in >= 50) {
      //table saturée !
      in=0;
    }
    if(lines[in]) {
      //entrée occupée : l'écraser, tant pis !
      free(lines[in]);
    }
    lines[in]=(char *)malloc(strlen(msg)+2);
    memset(lines[in],0,strlen(msg+1));
    strcpy(lines[in],msg);   
  }
}

void Myprint() {
  logbuffer[0] = 0;
  Myprint(logbuffer);
}

void Myprint(const char *msg)
{
  strcpy(logbuffer, msg);
  Myprint(logbuffer);
}

void Myprint(String msg) {
  sprintf(logbuffer,"%s",msg.c_str());
  Myprint(logbuffer);
}

void Myprint(int i) {
  sprintf(logbuffer,"%d", i);
  Myprint(logbuffer);
}

void Myprint(unsigned int i) {
  sprintf(logbuffer,"%u", i);
  Myprint(logbuffer);
}

// void Myprintf(...)
void Myprintf(const char * format, ...)
{

  va_list args;
  va_start (args, format);
  vsnprintf (logbuffer,sizeof(logbuffer),format, args);
  Myprint(logbuffer);
  va_end (args);

}

void Myprintln() {
  sprintf(logbuffer,"\n");
  Myprint(logbuffer);
}

void Myprintln(char *msg)
{
  sprintf(logbuffer,"%s\n",msg);
  Myprint(logbuffer);
}

void Myprintln(const char *msg)
{
  sprintf(logbuffer,"%s\n",msg);
  Myprint(logbuffer);
}

void Myprintln(String msg) {
  sprintf(logbuffer,"%s\n",msg.c_str());
  Myprint(logbuffer);
}

void Myprintln(const __FlashStringHelper *msg) {
  convert(msg);
  logbuffer[plog]=(char)'\n';
  logbuffer[plog+1]=0;
  Myprint(logbuffer);
}

void Myprintln(int i) {
  sprintf((char *)logbuffer,"%d\n", i);
  Myprint(logbuffer);
}

void Myprintln(unsigned int i) {
  sprintf((char *)logbuffer,"%u\n", i);
  Myprint(logbuffer);
}

void Myflush() {
#ifdef DEBUG
  DEBUG_SERIAL.flush();
#endif
}

#endif // SYSLOG

/* ======================================================================
Function: UpdateSysinfo 
Purpose : update sysinfo variables
Input   : true if first call
          true if needed to print on serial debug
Output  : - 
Comments: -
====================================================================== */
void UpdateSysinfo(boolean first_call, boolean show_debug)
{
  char buff[64];

  int sec = seconds;
  int min = sec / 60;
  int hr = min / 60;
  long day = hr / 24;

  sprintf_P( buff, PSTR("%ld days %02d h %02d m %02d sec"),day, hr % 24, min % 60, sec % 60);
  sysinfo.sys_uptime = buff;
}

/* ======================================================================
Function: Task_1_Sec 
Purpose : update our second ticker
Input   : -
Output  : - 
Comments: -
====================================================================== */
void Task_1_Sec()
{
  task_1_sec = true;
  seconds++;
}

/* ======================================================================
Function: Task_mqtt
Purpose : callback of mqtt ticker
Input   : 
Output  : -
Comments: Like an Interrupt, need to be short, we set flag for main loop
====================================================================== */
void Task_mqtt()
{
  task_mqtt = true;
}

/* ======================================================================
Function: Task_emoncms
Purpose : callback of emoncms ticker
Input   : 
Output  : -
Comments: Like an Interrupt, need to be short, we set flag for main loop
====================================================================== */
void Task_emoncms()
{
  task_emoncms = true;
}

/* ======================================================================
Function: Task_jeedom
Purpose : callback of jeedom ticker
Input   : 
Output  : -
Comments: Like an Interrupt, need to be short, we set flag for main loop
====================================================================== */
void Task_jeedom()
{
  task_jeedom = true;
}

/* ======================================================================
Function: Task_httpRequest
Purpose : callback of http request ticker
Input   : 
Output  : -
Comments: Like an Interrupt, need to be short, we set flag for main loop
====================================================================== */
void Task_httpRequest()
{
  task_httpRequest = true;
}

/* ======================================================================
Function: LedOff 
Purpose : callback called after led blink delay
Input   : led (defined in term of PIN)
Output  : - 
Comments: -
====================================================================== */
void LedOff(int led)
{
  #ifdef BLU_LED_PIN
  if (led==BLU_LED_PIN)
    LedBluOFF();
  #endif
  if (led==RED_LED_PIN)
    LedRedOFF();
  if (led==RGB_LED_PIN)
    LedRGBOFF();
}


// Light off the RGB LED
#ifdef RGB_LED_PIN
/* ======================================================================
Function: LedRGBON
Purpose : Light RGB Led with HSB value
Input   : Hue (0..255)
          Saturation (0..255)
          Brightness (0..255)
Output  : - 
Comments: 
====================================================================== */
void LedRGBON (uint16_t hue)
{
  if (config.config & CFG_RGB_LED) {
    // Convert to neoPixel API values
    // H (is color from 0..360) should be between 0.0 and 1.0
    // L (is brightness from 0..100) should be between 0.0 and 0.5
    RgbColor target = HslColor( hue / 360.0f, 1.0f, rgb_brightness * 0.005f );    

      // Set RGB Led
    rgb_led.SetPixelColor(0, target); 
    rgb_led.Show();
  }
}

/* ======================================================================
Function: LedRGBOFF 
Purpose : light off the RGN LED
Input   : -
Output  : - 
Comments: -
====================================================================== */
//void LedOff(int led)
void LedRGBOFF(void)
{
  if (config.config & CFG_RGB_LED) {
    rgb_led.SetPixelColor(0,RgbColor(0)); 
    rgb_led.Show();
  }
}

#endif


/* ======================================================================
Function: ADPSCallback 
Purpose : called by library when we detected a ADPS on any phased
Input   : phase number 
            0 for ADPS (monophase)
            1 for ADIR1 triphase
            2 for ADIR2 triphase
            3 for ADIR3 triphase
Output  : - 
Comments: should have been initialised in the main sketch with a
          tinfo.attachADPSCallback(ADPSCallback())
====================================================================== */
void ADPSCallback(uint8_t phase)
{
  // Monophasé
  if (phase == 0 ) {
    Debugln(F("ADPS"));
  } else {
    Debug(F("ADPS Phase "));
    Debugln('0' + phase);
  }
}

/* ======================================================================
Function: DataCallback 
Purpose : callback when we detected new or modified data received
Input   : linked list pointer on the concerned data
          value current state being TINFO_VALUE_ADDED/TINFO_VALUE_UPDATED
Output  : - 
Comments: -
====================================================================== */
void DataCallback(ValueList * me, uint8_t flags)
{
 boolean ret = false;
 String topic,payload;
 
  // Envoie donnée mqtt si freq != 0
  if (*config.mqtt.host &&  config.mqtt.freq) {

    // Si la données a été 
    // - ajouté (TINFO_FLAGS_ADDED),
    // - mise à jour (TINFO_FLAGS_UPDATED),
    // - ou que c'est un alerte de depassement de consommation ADPS, ADIR1, ADIR2, ADIR3 (TINFO_FLAGS_ALERT)
    // Alors on l'envoie via mqtt
    if ((flags & TINFO_FLAGS_ADDED) || (flags & TINFO_FLAGS_UPDATED) || (flags & TINFO_FLAGS_ALERT)) {
      
      mqttConnect();
      
      if (*config.mqtt.topic) {
        // Publish 
        topic = String(config.mqtt.topic);
        topic += "/data/";
        topic += String(me->name);
        payload = String(me->value);
        // And submit all to mqtt
        Debug("mqtt publish "); Debug(topic);Debug(" ");Debug(payload);
        ret=MQTTclient.publish(String(topic).c_str(), String(payload).c_str() , true);      
        if (ret)
          Debugln(" OK");
        else
          Debugln(" KO");
      }
    }
  }

  // This is for simulating ADPS during my tests
  // ===========================================
  /*
  static uint8_t test = 0;
  // Each new/updated values
  if (++test >= 20) {
    test=0;
    uint8_t anotherflag = TINFO_FLAGS_NONE;
    ValueList * anotherme = tinfo.addCustomValue("ADPS", "46", &anotherflag);

    // Do our job (mainly debug)
    DataCallback(anotherme, anotherflag);
  }
  Debugf("%02d:",test);
  */
  // ===========================================
  
/*
  // Do whatever you want there
  Debug(me->name);
  Debug('=');
  Debug(me->value);
  
  if ( flags & TINFO_FLAGS_NOTHING ) Debug(F(" Nothing"));
  if ( flags & TINFO_FLAGS_ADDED )   Debug(F(" Added"));
  if ( flags & TINFO_FLAGS_UPDATED ) Debug(F(" Updated"));
  if ( flags & TINFO_FLAGS_EXIST )   Debug(F(" Exist"));
  if ( flags & TINFO_FLAGS_ALERT )   Debug(F(" Alert"));

  Debugln();
*/
}

/* ======================================================================
Function: NewFrame 
Purpose : callback when we received a complete teleinfo frame
Input   : linked list pointer on the concerned data
Output  : - 
Comments: -
====================================================================== */
void NewFrame(ValueList * me) 
{
  char buff[32];

  // Light the RGB LED 
  if ( config.config & CFG_RGB_LED) {
    LedRGBON(COLOR_GREEN);
    
    // led off after delay
    rgb_ticker.once_ms( (uint32_t) BLINK_LED_MS, LedOff, (int) RGB_LED_PIN);
  }

  sprintf_P( buff, PSTR("New Frame (%ld Bytes free)"), ESP.getFreeHeap() );
  Debugln(buff);
}

/* ======================================================================
Function: NewFrame 
Purpose : callback when we received a complete teleinfo frame
Input   : linked list pointer on the concerned data
Output  : - 
Comments: it's called only if one data in the frame is different than
          the previous frame
====================================================================== */
void UpdatedFrame(ValueList * me)
{
  char buff[32];
  
  // Light the RGB LED (purple)
  if ( config.config & CFG_RGB_LED) {
    LedRGBON(COLOR_MAGENTA);

    // led off after delay
    rgb_ticker.once_ms(BLINK_LED_MS, LedOff, RGB_LED_PIN);
  }

  sprintf_P( buff, PSTR("Updated Frame (%ld Bytes free)"), ESP.getFreeHeap() );
  Debugln(buff);

/*
  // Got at least one ?
  if (me) {
    WiFiUDP myudp;
    IPAddress ip = WiFi.localIP();

    // start UDP server
    myudp.begin(1201);
    ip[3] = 255;

    // transmit broadcast package
    myudp.beginPacket(ip, 1201);

    // start of frame
    myudp.write(TINFO_STX);

    // Loop thru the node
    while (me->next) {
      me = me->next;
      // prepare line and write it
      sprintf_P( buff, PSTR("%s %s %c\n"),me->name, me->value, me->checksum );
      myudp.write( buff);
    }

    // End of frame
    myudp.write(TINFO_ETX);
    myudp.endPacket();
    myudp.flush();

  }
*/
}


/* ======================================================================
Function: ResetConfig
Purpose : Set configuration to default values
Input   : -
Output  : -
Comments: -
====================================================================== */
void ResetConfig(void) 
{
  // Start cleaning all that stuff
  memset(&config, 0, sizeof(_Config));

  // Set default Hostname
#ifdef ESP8266
  // ESP8266
  sprintf_P(config.host, PSTR("WifInfo-%06X"), ESP.getChipId());
#else
  //ESP32
  int ChipId;
  uint64_t macAddress = ESP.getEfuseMac();
  uint64_t macAddressTrunc = macAddress << 40;
  ChipId = macAddressTrunc >> 40;
  sprintf_P(config.host, PSTR("WifInfo-%06X"), ChipId);
#endif

  strcpy_P(config.ota_auth, PSTR(DEFAULT_OTA_AUTH));
  config.ota_port = DEFAULT_OTA_PORT ;

  // Add other init default config here

  // Mqtt
  strcpy_P(config.mqtt.host, CFG_MQTT_DEFAULT_HOST);
  config.mqtt.port = CFG_MQTT_DEFAULT_PORT;
  strcpy_P(config.mqtt.pswd, CFG_MQTT_DEFAULT_PSWD);
  strcpy_P(config.mqtt.user, CFG_MQTT_DEFAULT_USER);
  strcpy_P(config.mqtt.topic, CFG_MQTT_DEFAULT_TOPIC);

  // Emoncms
  strcpy_P(config.emoncms.host, CFG_EMON_DEFAULT_HOST);
  config.emoncms.port = CFG_EMON_DEFAULT_PORT;
  strcpy_P(config.emoncms.url, CFG_EMON_DEFAULT_URL);

  // Jeedom
  strcpy_P(config.jeedom.host, CFG_JDOM_DEFAULT_HOST);
  config.jeedom.port = CFG_JDOM_DEFAULT_PORT;
  strcpy_P(config.jeedom.url, CFG_JDOM_DEFAULT_URL);
  //strcpy_P(config.jeedom.adco, CFG_JDOM_DEFAULT_ADCO);

  // HTTP Request
  strcpy_P(config.httpReq.host, CFG_HTTPREQ_DEFAULT_HOST);
  config.httpReq.port = CFG_HTTPREQ_DEFAULT_PORT;
  strcpy_P(config.httpReq.path, CFG_HTTPREQ_DEFAULT_PATH);

  config.syslog_port = DEFAULT_SYSLOG_PORT;
  
  config.config |= CFG_RGB_LED;

  // save back
  saveConfig();
}

/* ======================================================================
Function: WifiHandleConn
Purpose : Handle Wifi connection / reconnection and OTA updates
Input   : setup true if we're called 1st Time from setup
Output  : state of the wifi status
Comments: -
====================================================================== */
int WifiHandleConn(boolean setup = false) 
{
  int ret = WiFi.status();

  if (setup) {
    // Pourquoi ce n'était pas appelé avant V1.0.9 et précédente
    WiFi.mode(WIFI_STA);

    DebuglnF("========== WiFi.printDiag Start"); 
    WiFi.printDiag(DEBUG_SERIAL);
    DebuglnF("========== WiFi.printDiag End"); 
    Debugflush();

    // no correct SSID
    if (!*config.ssid) {
      DebugF("no Wifi SSID in config, trying to get SDK ones..."); 

      // Let's see of SDK one is okay
      if ( WiFi.SSID() == "" ) {
        DebuglnF("Not found may be blank chip!"); 
      } else {
        *config.psk = '\0';

        // Copy SDK SSID
        strcpy(config.ssid, WiFi.SSID().c_str());

        // Copy SDK password if any
        if (WiFi.psk() != "")
          strcpy(config.psk, WiFi.psk().c_str());

        DebuglnF("found one!"); 

        // save back new config
        saveConfig();
      }
    }

    // correct SSID
    if (*config.ssid) {
      uint8_t timeout ;

      DebugF("Connecting to: "); 
      Debug(config.ssid);
      Debugflush();

      // Do wa have a PSK ?
      if (*config.psk) {
        // protected network
        Debug(F(" with key '"));
        Debug(config.psk);
        Debug(F("'..."));
        Debugflush();
        WiFi.begin(config.ssid, config.psk);
      } else {
        // Open network
        Debug(F("unsecure AP"));
        Debugflush();
        WiFi.begin(config.ssid);
      }

      timeout = 25; // 25 * 200 ms = 5 sec time out
      // 200 ms loop
      while ( ((ret = WiFi.status()) != WL_CONNECTED) && timeout )
      {
        // Orange LED
        LedRGBON(COLOR_ORANGE);
        delay(50);
        LedRGBOFF();
        delay(150);
        --timeout;
      }
    }

    // delay(2000);
    // return(ret);
    
    // connected ? disable AP, client mode only
    if (ret == WL_CONNECTED)
    {
      DebuglnF("connected!");
      // WiFi.mode(WIFI_STA); Ca ne sert à rien, c'est fait au début

      DebugF("IP address   : "); Debugln(WiFi.localIP().toString());
      DebugF("MAC address  : "); Debugln(WiFi.macAddress());
 #ifdef SYSLOG
    if (*config.syslog_host) {
      SYSLOGselected=true;
      // Create a new syslog instance with LOG_KERN facility
      syslog.server(config.syslog_host, config.syslog_port);
      syslog.deviceHostname(config.host);
      syslog.appName(APP_NAME);
      syslog.defaultPriority(LOG_KERN);
      memset(waitbuffer,0,sizeof(waitbuffer));
      pending=0;
      SYSLOGusable=true;
    } else {
      SYSLOGusable=false;
      SYSLOGselected=false;
    }
#endif
   
    // not connected ? start AP
    } else {
      char ap_ssid[32];
      DebuglnF("Error!");
      Debugflush();

      // STA+AP Mode without connected to STA, autoconnect will search
      // other frequencies while trying to connect, this is causing issue
      // to AP mode, so disconnect will avoid this

      // Disable auto retry search channel
      WiFi.disconnect(); 

      // SSID = hostname
      strcpy(ap_ssid, config.host );
      DebugF("Switching to AP ");
      Debugln(ap_ssid);
      Debugflush();
      
      // protected network
      if (*config.ap_psk) {
        DebugF(" with key '");
        Debug(config.ap_psk);
        DebuglnF("'");
        WiFi.softAP(ap_ssid, config.ap_psk);
      // Open network
      } else {
        DebuglnF(" with no password");
        WiFi.softAP(ap_ssid);
      }
      WiFi.mode(WIFI_AP_STA);

      DebugF("IP address   : "); Debugln(WiFi.softAPIP().toString());
      DebugF("MAC address  : "); Debugln(WiFi.softAPmacAddress());
    }
    
    // Version 1.0.7 : Use auto reconnect Wifi
#ifdef ESP8266
    // Ne sebme pas exister pour ESP32
    WiFi.setAutoConnect(true);
#endif
    WiFi.setAutoReconnect(true);
    DebuglnF("auto-reconnect armed !");
      
	  
    // Set OTA parameters
    ArduinoOTA.setPort(config.ota_port);
    ArduinoOTA.setHostname(config.host);
    ArduinoOTA.setPassword(config.ota_auth);
    ArduinoOTA.begin();

    // just in case your sketch sucks, keep update OTA Available
    // Trust me, when coding and testing it happens, this could save
    // the need to connect FTDI to reflash
    // Usefull just after 1st connexion when called from setup() before
    // launching potentially buggy main()
    for (uint8_t i=0; i<= 10; i++) {
      LedRGBON(COLOR_MAGENTA);
      delay(100);
      LedRGBOFF();
      delay(200);
      ArduinoOTA.handle();
    }

  } // if setup

  return WiFi.status();
}

boolean mqttConnect() {
  boolean ret = false;

    DebugF("Connexion au serveur MQTT... ");

    if ( WiFi.status() == WL_CONNECTED){
      //DebugF ("execution tache MQTT / wifi connecté Init mqtt=");
      //Debugln (Mqtt_Init);
      if (!Mqtt_Init){
        MQTTclient.setServer(config.mqtt.host, config.mqtt.port);    //Configuration de la connexion au serveur MQTT
        MQTTclient.setCallback(Mqttcallback);  //La fonction de callback qui est executée à chaque réception de message  
        Mqtt_Init=1; 
      }
      ret = MQTTclient.connected();
      if (!ret) {
        //Debugln ("demande de connexion MQTT");
        ret = MQTTclient.connect(config.mqtt.topic, config.mqtt.user, config.mqtt.pswd);
      }
    }

    if (ret) {
      DebuglnF("OK");
    } else {
      DebuglnF("KO");
    }
    return(ret);
}

/* ======================================================================
Function: mqttStartupLogs (called one at startup, if mqtt activated)
Purpose : Send logs to mqtt ()
Input   : 
Output  : true if post returned OK
Comments: -
====================================================================== */
boolean mqttStartupLogs()
{
String topic,payload;
boolean ret = false;

  if (*config.mqtt.host && config.mqtt.freq != 0) {
    if (*config.mqtt.topic) {
        // Publish Startup
        topic= String(config.mqtt.topic);
        topic+= "/log";
        String payload = "Wifinfo Startup V";
        payload += WIFINFO_VERSION;
        // And submit all to mqtt
        Debug("mqtt ");
        Debug(topic);
        ret = MQTTclient.publish(String(topic).c_str(), String(payload).c_str() , true);      
        if (ret)
          Debugln(" OK");
        else
          Debugln(" KO");

        // Publish IP
        topic= String(config.mqtt.topic);
        topic+= "/log";
        payload = WiFi.localIP().toString();
        // And submit all to mqtt
        Debug("mqtt ");
        Debug(topic);
        ret &= MQTTclient.publish(String(topic).c_str(), String(payload).c_str() , true);      
        if (ret)
          Debugln(" OK");
        else
          Debugln(" KO");
      
        //Publish Date Heure
        topic= String(config.mqtt.topic);
        topic+= "/log";
        struct tm timeinfo;
        if(getLocalTime(&timeinfo)){
          char buf[20];
          strftime(buf,sizeof(buf),"%FT%H:%M:%S",&timeinfo);
          payload = buf;
          // And submit all to mqtt
          Debug("mqtt ");
          Debug(topic);
          Debug( " value ");
          Debug(payload);
          ret &= MQTTclient.publish(String(topic).c_str(), String(payload).c_str() , true);      
          if (ret)
            Debugln(" OK");
          else
            Debugln(" KO");
        }
    } else {
      Debugln("mqtt TOPIC non configuré");
    }
  }
  return(ret);  
}
/* ======================================================================
Function: mqttPost (called by main sketch on timer, if activated)
Purpose : Do a http post to mqtt
Input   : 
Output  : true if post returned OK
Comments: -
====================================================================== */
// Non utilisé : mqtt data sent by DataCallback()
/*
boolean mqttPost(void)
{
String topic,payload;
boolean ret = false;

  // Some basic checking
  if (*config.mqtt.host) {
    ValueList * me = tinfo.getList();
    // Got at least one ?
    if (me && me->next) {
      String topic;
      Debugln("mqttPost publish");
      
      if (*config.mqtt.topic) {
          
        //Publish DATA
        topic= String(config.mqtt.topic);
        topic+= "/data";
         Debug(topic);
         Debug( " value ");
         payload = build_mqtt_json();
        // And submit all to mqtt
        ret = MQTTclient.publish(String(topic).c_str(), String(payload).c_str() , true);             
          if (ret)
            Debugln(" OK");
          else
            Debugln(" KO");
      } 

    } else {
      Debugln("mqttPost : list is empty");
    } // if me
  } // if host
  return ret;
}
*/

/* ======================================================================
Function: Mqttcallback
Purpose : Déclenche les actions à la réception d'un message mqtt
          D'après http://m2mio.tumblr.com/post/30048662088/a-simple-example-arduino-mqtt-m2mio
Input   : topic
          payload
          length
Output  : - 
Comments: -
====================================================================== */
void Mqttcallback(char* topic, byte* payload, unsigned int length) {

  #ifdef DEBUG 
    DebugF("Message recu =>  topic: ");
    Debug(String(topic));
    DebugF(" | longueur: ");
    Debugln(String(length,DEC));
  #endif

  /*
  // create character buffer with ending null terminator (string)
  for(i=0; i<length; i++) {
    buff[i] = payload[i];
  }
  buff[i] = '\0';
  
  String msgString = String(buff);
  */


  #ifdef DEBUG 
  // String 3 paramètres n'existe pas xxxxxx
  // String msgString = String(payload, 0 , length);
  //  DebuglnF("Payload: ");
  //  Debugln(msgString);
  #endif

}

/* ======================================================================
Function: setup
Purpose : Setup I/O and other one time startup stuff
Input   : -
Output  : - 
Comments: -
====================================================================== */
void setup()
{
  // Set CPU speed to 160MHz
#ifdef ESP8266
  // ESP8266
  system_update_cpu_freq(160);
#else
  //ESP32
  setCpuFrequencyMhz(160);
#endif

#ifdef SYSLOG
  for(int i=0; i<50; i++)
    lines[i]=0;
  in=-1;
  out=-1;

  SYSLOGselected=true;  //Par défaut, au moins stocker les premiers msg debug
  SYSLOGusable=false;   //Tant que non connecté, ne pas émettre sur réseau
#endif

  optval = "";

#ifdef SIMU
  optval += "SIMU, ";
#else
  optval += "No SIMU, ";
#endif

#ifdef DEBUG
  optval += "DEBUG, ";
#else
  optval += "No DEBUG, ";
#endif

#ifdef SYSLOG
  optval += "SYSLOG";
#else
  optval += "No SYSLOG";
#endif

  // Init the RGB Led, and set it off
  rgb_led.Begin();
  LedRGBOFF();

  // Init the serial 1, Our Debug Serial TXD0
  // note this serial can only transmit, just 
  // enough for debugging purpose
  DEBUG_SERIAL.begin(115200);
  Debugln(F("\r\n\r\n=============="));
  Debug(F("WifInfo V"));
  Debugln(F(WIFINFO_VERSION));
  Debugln();
  Debugflush();

  // Clear our global flags
  config.config = 0;

  // Our configuration is stored into EEPROM
  //EEPROM.begin(sizeof(_Config));
  EEPROM.begin(1024);

  DebugF("Config size="); Debug(sizeof(_Config));
  DebugF("  (mqtt=");   Debug(sizeof(_mqtt));
  DebugF("  emoncms=");   Debug(sizeof(_emoncms));
  DebugF("  jeedom=");   Debug(sizeof(_jeedom));
  DebugF("  httpRequest=");   Debug(sizeof(_httpRequest));
  Debugln(')');
  Debugflush();

  // Check File system init 
  if (!SPIFFS.begin())
  {
    // Serious problem
    DebuglnF("SPIFFS Mount failed");
  } else {
   
    DebuglnF("SPIFFS Mount succesfull");

#ifdef ESP8266
    Dir dir = SPIFFS.openDir("/");
    while (dir.next()) {    
      String fileName = dir.fileName();
      size_t fileSize = dir.fileSize();
      Debugf("FS File: %s, size: %d\n", fileName.c_str(), fileSize);
    }
#else
    // ESP32
    File root = SPIFFS.open("/");
    File file;
    while ((file = root.openNextFile())) {
      String fileName = file.name();
      size_t fileSize = file.size();
      Debugf("FS File: %s, size: %d\n", fileName.c_str(), fileSize);
    }
#endif
    DebuglnF("");
  }

  // Read Configuration from EEP
  if (readConfig()) {
      DebuglnF("Good CRC, not set!");
  } else {
    // Reset Configuration
    ResetConfig();

    // save back
    saveConfig();

    // Indicate the error in global flags
    config.config |= CFG_BAD_CRC;

    DebuglnF("Reset to default");
  }

  // We'll drive our onboard LED
  // On ESP8266 old TXD1, not used anymore, has been swapped
  pinMode(RED_LED_PIN, OUTPUT); 
  LedRedOFF();
 
  // start Wifi connect or soft AP
  WifiHandleConn(true);

#ifdef SYSLOG
  //purge previous debug message,
  if(SYSLOGselected) {
    if(in != out && in != -1) {
        //Il y a des messages en attente d'envoi
        out++;
        while( out <= in ) {
          process_line(lines[out]);
          free(lines[out]);
          lines[out]=0;
          out++;
        }
        DebuglnF("syslog buffer empty");
    }
  } else {
    DebuglnF("syslog not activated !");
  }
#endif

  // OTA callbacks
  ArduinoOTA.onStart([]() { 
    LedRGBON(COLOR_MAGENTA);
    DebuglnF("Update Started");
    ota_blink = true;
  });

  ArduinoOTA.onEnd([]() { 
    LedRGBOFF();
    DebuglnF("Update finished restarting");
  });

  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    if (ota_blink) {
      LedRGBON(COLOR_MAGENTA);
    } else {
      LedRGBOFF();
    }
    ota_blink = !ota_blink;
    //Serial.printf("Progress: %u%%\n", (progress / (total / 100)));
  });

  ArduinoOTA.onError([](ota_error_t error) {
    LedRGBON(COLOR_RED);
    Debugf("Update Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) DebuglnF("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) DebuglnF("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) DebuglnF("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) DebuglnF("Receive Failed");
    else if (error == OTA_END_ERROR) DebuglnF("End Failed");
    ESP.restart(); 
  });

  // Update sysinfo variable and print them
  UpdateSysinfo(true, true);

  server.on("/", handleRoot);
  server.on("/config_form.json", handleFormConfig);
  server.on("/json", sendJSON);
  server.on("/tinfo.json", tinfoJSONTable);
  server.on("/system.json", sysJSONTable);
  server.on("/config.json", confJSONTable);
  server.on("/spiffs.json", spiffsJSONTable);
  server.on("/wifiscan.json", wifiScanJSON);
  server.on("/factory_reset", handleFactoryReset);
  server.on("/reset", handleReset);

  // handler for the hearbeat
  server.on("/hb.htm", HTTP_GET, [&](){
      server.sendHeader("Connection", "close");
      server.sendHeader("Access-Control-Allow-Origin", "*");
      server.send(200, "text/html", R"(OK)");
  });

  // handler for the /update form POST (once file upload finishes)
  server.on("/update", HTTP_POST, 
    // handler once file upload finishes
    [&]() {
      server.sendHeader("Connection", "close");
      server.sendHeader("Access-Control-Allow-Origin", "*");
      server.send(200, "text/plain", (Update.hasError())?"FAIL":"OK");
      ESP.restart();
    },
    // handler for upload, get's the sketch bytes, 
    // and writes them through the Update object
    [&]() {
      HTTPUpload& upload = server.upload();

      if(upload.status == UPLOAD_FILE_START) {
        uint32_t maxSketchSpace = (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
#ifdef ESP8266
        WiFiUDP::stopAll();
#else
        // xxxxxxxx ESP32 : je ne sais pas
      #pragma message "xxxxxxx WiFiUDP::stopAll() n'existe pas pour ESP32 : que mettre ?"
#endif
        Debugf("Update: %s\n", upload.filename.c_str());
        LedRGBON(COLOR_MAGENTA);
        ota_blink = true;

        //start with max available size
        if(!Update.begin(maxSketchSpace)) 
          Update.printError(Serial1);

      } else if(upload.status == UPLOAD_FILE_WRITE) {
        if (ota_blink) {
          LedRGBON(COLOR_MAGENTA);
        } else {
          LedRGBOFF();
        }
        ota_blink = !ota_blink;
        Debug(".");
        if(Update.write(upload.buf, upload.currentSize) != upload.currentSize) 
          Update.printError(Serial1);

      } else if(upload.status == UPLOAD_FILE_END) {
        //true to set the size to the current progress
        if(Update.end(true)) 
          Debugf("Update Success: %u\nRebooting...\n", upload.totalSize);
        else 
          Update.printError(Serial1);

        LedRGBOFF();

      } else if(upload.status == UPLOAD_FILE_ABORTED) {
        Update.end();
        LedRGBOFF();
        DebuglnF("Update was aborted");
      }
      delay(0);
    }
  );

  // All other not known 
  server.onNotFound(handleNotFound);
  
  // serves all SPIFFS Web file with 24hr max-age control
  // to avoid multiple requests to ESP
  server.serveStatic("/font", SPIFFS, "/font","max-age=86400"); 
  server.serveStatic("/js",   SPIFFS, "/js"  ,"max-age=86400"); 
  server.serveStatic("/css",  SPIFFS, "/css" ,"max-age=86400"); 
  server.begin();

  // Display configuration
  showConfig();

  Debugln(F("HTTP server started"));

  // Teleinfo is connected to RXD2 (GPIO13) to 
  // avoid conflict when flashing, this is why
  // we swap RXD1/RXD1 to RXD2/TXD2 
  // Note that TXD2 is not used teleinfo is receive only
  //
  // From : https://arduino.esp8266.com/Arduino/versions/2.1.0-rc2/doc/reference.html
  // Serial uses UART0, which is mapped to pins GPIO1 (TX) and GPIO3 (RX).
  // Serial may be remapped to GPIO15 (TX) and GPIO13 (RX) by calling Serial.swap() after Serial.begin
  // Calling swap again maps UART0 back to GPIO1 and GPIO3.
#ifndef SIMU
  #ifdef ESP8266
    if (config.linky_mode_standard)
      Serial.begin(9600, SERIAL_7E1);
    else
      Serial.begin(1200, SERIAL_7E1);

      Serial.swap();
  #else
  // ESP32
    if (config.linky_mode_standard)
      Serial2.begin(9600, SERIAL_7E1);
    else
      Serial2.begin(1200, SERIAL_7E1);
  #endif

#endif

  // Init teleinfo
  if (config.linky_mode_standard)
    tinfo.init(TINFO_MODE_STANDARD);
  else
    tinfo.init(TINFO_MODE_HISTORIQUE);


  // Attach the callback we need
  // set all as an example
  tinfo.attachADPS(ADPSCallback);
  tinfo.attachData(DataCallback);
  tinfo.attachNewFrame(NewFrame);
  tinfo.attachUpdatedFrame(UpdatedFrame);

  //webSocket.begin();
  //webSocket.onEvent(webSocketEvent);

  // Light off the RGB LED
  LedRGBOFF();

  // Update sysinfo every second
  Every_1_Sec.attach(1, Task_1_Sec);

  // Init and get the time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Debugln("Failed to obtain time");
  } else {
    char buf[20];
    strftime(buf,sizeof(buf),"%FT%H:%M:%S",&timeinfo);
    Debugln(buf);
  }
     
  // Mqtt Update if needed
  if (config.mqtt.freq) {
    Tick_mqtt.attach(config.mqtt.freq, Task_mqtt);
    mqttStartupLogs();  //send startup logs to mqtt
  }
  
  // Emoncms Update if needed
  if (config.emoncms.freq) 
    Tick_emoncms.attach(config.emoncms.freq, Task_emoncms);

  // Jeedom Update if needed
  if (config.jeedom.freq) 
    Tick_jeedom.attach(config.jeedom.freq, Task_jeedom);

  // HTTP Request Update if needed
  if (config.httpReq.freq) 
    Tick_httpRequest.attach(config.httpReq.freq, Task_httpRequest);

//To simulate Teleinfo on not connected module
#ifdef SIMU
    String name1 = "ADCO";
    String value1 = "01234546789012";
    
    char * s1 = (char *)name1.c_str();
    char * v1 = (char *)value1.c_str();
    ValueList * me;
    flags = TINFO_FLAGS_NONE;
    me = tinfo.addCustomValue(s1, v1, &flags); //ADCO arbitrary value
    DataCallback(me, me->flags);  // addCustomValue n'appelant pas le call back, il faut l'appeler pour mqtt
    me = tinfo.addCustomValue(s2, v2, &flags); //counter value
    DataCallback(me, me->flags);  // addCustomValue n'appelant pas le call back, il faut l'appeler pour mqtt
    tinfo.valuesDump();
#endif

}

/* ======================================================================
Function: loop
Purpose : infinite loop main code
Input   : -
Output  : - 
Comments: -
====================================================================== */
void loop()
{
  char c;

  // Do all related network stuff
  server.handleClient();
  ArduinoOTA.handle();

  //webSocket.loop();

  // Only once task per loop, let system do it own task
  if (task_1_sec) { 
    UpdateSysinfo(false, false); 
    task_1_sec = false; 

//To simulate Teleinfo on not connected module
#ifdef SIMU
    ValueList * me;
    loop_cpt++;
    if((loop_cpt % 10) == 0)
    {
      // each 10 second, try to change HCHC value
      //Increase v2 value
      sprintf(v2, "%09d", (loop_cpt) );
      // and update ListValues
      flags = TINFO_FLAGS_NONE;
      me = tinfo.addCustomValue(s2, v2, &flags); 
      DataCallback(me, me->flags);  // addCustomValue n'appelant pas le call back, il faut l'appeler pour mqtt
    }
#endif

  } else if (task_mqtt) { 
    // -------------------------------
    // Les données sont envoyées par DataCallback()
    // dès l'ajout ou la modification d'une donnée de Téléinformation
    // Cette tache ici ne fait plus rien
    // -------------------------------
    //gestion connexion Mqtt
    // mqttConnect();
    //Mqtt Publier les data; 
    // mqttPost(); 

    task_mqtt=false; 
  } else if (task_emoncms) { 
    emoncmsPost(); 
    task_emoncms=false; 
  } else if (task_jeedom) { 
    jeedomPost();  
    task_jeedom=false;
  } else if (task_httpRequest) { 
    httpRequest();  
    task_httpRequest=false;
  } 

#ifdef ESP8266
  // Handle teleinfo serial
  if ( Serial.available() ) {
    // Read Serial and process to tinfo
    c = Serial.read();
    // Debugf("Serial available '%c'", c);Debugln("");
    //Serial1.print(c);
    tinfo.process(c);
  }
#else
  // ESP32
  // Handle teleinfo serial
  if ( Serial2.available() ) {
    // Read Serial and process to tinfo
    c = Serial2.read();
    // Debugf("Serial available '%c'", c);Debugln("");
    //Serial1.print(c);
    tinfo.process(c);
  }
#endif

  //delay(10);
}
