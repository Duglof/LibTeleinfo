// **********************************************************************************
// ESP8266 Teleinfo WEB Server, route web function
// **********************************************************************************
// Creative Commons Attrib Share-Alike License
// You are free to use/extend this library but please abide with the CC-BY-SA license:
// Attribution-NonCommercial-ShareAlike 4.0 International License
// http://creativecommons.org/licenses/by-nc-sa/4.0/
//
// For any explanation about teleinfo ou use, see my blog
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

// Include Arduino header
#include "webserver.h"

// Optimize string space in flash, avoid duplication
const char FP_JSON_START[] PROGMEM = "{\r\n";
const char FP_JSON_END[] PROGMEM = "\r\n}\r\n";
const char FP_QCQ[] PROGMEM = "\":\"";
const char FP_QCNL[] PROGMEM = "\",\r\n\"";
const char FP_RESTART[] PROGMEM = "OK, Redémarrage en cours\r\n";
const char FP_NL[] PROGMEM = "\r\n";

/* ======================================================================
Function: formatSize 
Purpose : format a asize to human readable format
Input   : size 
Output  : formated string
Comments: -
====================================================================== */
String formatSize(size_t bytes)
{
  if (bytes < 1024){
    return String(bytes) + F(" Byte");
  } else if(bytes < (1024 * 1024)){
    return String(bytes/1024.0) + F(" KB");
  } else if(bytes < (1024 * 1024 * 1024)){
    return String(bytes/1024.0/1024.0) + F(" MB");
  } else {
    return String(bytes/1024.0/1024.0/1024.0) + F(" GB");
  }
}

/* ======================================================================
Function: getContentType 
Purpose : return correct mime content type depending on file extension
Input   : -
Output  : Mime content type 
Comments: -
====================================================================== */
String getContentType(String filename) {
  if(filename.endsWith(".htm")) return F("text/html");
  else if(filename.endsWith(".html")) return F("text/html");
  else if(filename.endsWith(".css")) return F("text/css");
  else if(filename.endsWith(".json")) return F("text/json");
  else if(filename.endsWith(".js")) return F("application/javascript");
  else if(filename.endsWith(".png")) return F("image/png");
  else if(filename.endsWith(".gif")) return F("image/gif");
  else if(filename.endsWith(".jpg")) return F("image/jpeg");
  else if(filename.endsWith(".ico")) return F("image/x-icon");
  else if(filename.endsWith(".xml")) return F("text/xml");
  else if(filename.endsWith(".pdf")) return F("application/x-pdf");
  else if(filename.endsWith(".zip")) return F("application/x-zip");
  else if(filename.endsWith(".gz")) return F("application/x-gzip");
  else if(filename.endsWith(".otf")) return F("application/x-font-opentype");
  else if(filename.endsWith(".eot")) return F("application/vnd.ms-fontobject");
  else if(filename.endsWith(".svg")) return F("image/svg+xml");
  else if(filename.endsWith(".woff")) return F("application/x-font-woff");
  else if(filename.endsWith(".woff2")) return F("application/x-font-woff2");
  else if(filename.endsWith(".ttf")) return F("application/x-font-ttf");
  return "text/plain";
}

/* ======================================================================
Function: handleFileRead 
Purpose : return content of a file stored on SPIFFS file system
Input   : file path
Output  : true if file found and sent
Comments: -
====================================================================== */
bool handleFileRead(String path) {
  if ( path.endsWith("/") ) 
    path += "index.htm";
  
  String contentType = getContentType(path);
  String pathWithGz = path + ".gz";

  DebugF("handleFileRead ");
  Debug(path);

  if(SPIFFS.exists(pathWithGz) || SPIFFS.exists(path)) {
    if( SPIFFS.exists(pathWithGz) ){
      path += ".gz";
      DebugF(".gz");
    }

    DebuglnF(" found on FS");
 
    File file = SPIFFS.open(path, "r");
    server.streamFile(file, contentType);
    file.close();
    return true;
  }

  Debugln("");

  server.send(404, "text/plain", "File Not Found");
  return false;
}

/* ======================================================================
Function: handleFormConfig 
Purpose : handle main configuration page
Input   : -
Output  : - 
Comments: -
====================================================================== */
void handleFormConfig(void) 
{
  String response="";
  int ret ;

  LedBluON();

  // We validated config ?
  if (server.hasArg("save"))
  {
    int itemp;
    DebuglnF("===== Posted configuration"); 

    // WifInfo
    strncpy(config.ssid ,   server.arg("ssid").c_str(),     CFG_SSID_SIZE );
    strncpy(config.psk ,    server.arg("psk").c_str(),      CFG_PSK_SIZE );
    strncpy(config.host ,   server.arg("host").c_str(),     CFG_HOSTNAME_SIZE );
    strncpy(config.ap_psk , server.arg("ap_psk").c_str(),   CFG_PSK_SIZE );
    strncpy(config.ota_auth,server.arg("ota_auth").c_str(), CFG_PSK_SIZE );
    itemp = server.arg("ota_port").toInt();
    config.ota_port = (itemp>=0 && itemp<=65535) ? itemp : DEFAULT_OTA_PORT ;
    strncpy(config.syslog_host ,   server.arg("syslog_host").c_str(),     64 );
    itemp = server.arg("syslog_port").toInt();
    config.syslog_port = (itemp>=0 && itemp<=65535) ? itemp : DEFAULT_SYSLOG_PORT ;

    // mqtt
    strncpy(config.mqtt.host,   server.arg("mqtt_host").c_str(),  CFG_MQTT_HOST_SIZE );
    strncpy(config.mqtt.user,    server.arg("mqtt_user").c_str(),   CFG_MQTT_USER_SIZE );
    strncpy(config.mqtt.pswd, server.arg("mqtt_pswd").c_str(),CFG_MQTT_PSWD_SIZE );
    strncpy(config.mqtt.topic, server.arg("mqtt_topic").c_str(),CFG_MQTT_TOPIC_SIZE );
    itemp = server.arg("mqtt_port").toInt();
    config.mqtt.port = (itemp>=0 && itemp<=65535) ? itemp : CFG_MQTT_DEFAULT_PORT ; 
    itemp = server.arg("mqtt_freq").toInt();
    if (itemp>0 && itemp<=86400){
      // Mqtt Update if needed
      Tick_mqtt.detach();
      Tick_mqtt.attach(itemp, Task_mqtt);
    } else {
      itemp = 0 ; 
    }
    config.mqtt.freq = itemp;
    
    // Emoncms
    strncpy(config.emoncms.host,   server.arg("emon_host").c_str(),  CFG_EMON_HOST_SIZE );
    strncpy(config.emoncms.url,    server.arg("emon_url").c_str(),   CFG_EMON_URL_SIZE );
    strncpy(config.emoncms.apikey, server.arg("emon_apikey").c_str(),CFG_EMON_APIKEY_SIZE );
    itemp = server.arg("emon_node").toInt();
    config.emoncms.node = (itemp>=0 && itemp<=255) ? itemp : 0 ;
    itemp = server.arg("emon_port").toInt();
    config.emoncms.port = (itemp>=0 && itemp<=65535) ? itemp : CFG_EMON_DEFAULT_PORT ; 
    itemp = server.arg("emon_freq").toInt();
    if (itemp>0 && itemp<=86400){
      // Emoncms Update if needed
      Tick_emoncms.detach();
      Tick_emoncms.attach(itemp, Task_emoncms);
    } else {
      itemp = 0 ; 
    }
    config.emoncms.freq = itemp;

    // jeedom
    strncpy(config.jeedom.host,   server.arg("jdom_host").c_str(),  CFG_JDOM_HOST_SIZE );
    strncpy(config.jeedom.url,    server.arg("jdom_url").c_str(),   CFG_JDOM_URL_SIZE );
    strncpy(config.jeedom.apikey, server.arg("jdom_apikey").c_str(),CFG_JDOM_APIKEY_SIZE );
    strncpy(config.jeedom.adco,   server.arg("jdom_adco").c_str(),CFG_JDOM_ADCO_SIZE );
    itemp = server.arg("jdom_port").toInt();
    config.jeedom.port = (itemp>=0 && itemp<=65535) ? itemp : CFG_JDOM_DEFAULT_PORT ; 
    itemp = server.arg("jdom_freq").toInt();
    if (itemp>0 && itemp<=86400){
      // Emoncms Update if needed
      Tick_jeedom.detach();
      Tick_jeedom.attach(itemp, Task_jeedom);
    } else {
      itemp = 0 ; 
    }
    config.jeedom.freq = itemp;

    // HTTP Request
    strncpy(config.httpReq.host, server.arg("httpreq_host").c_str(), CFG_HTTPREQ_HOST_SIZE );
    strncpy(config.httpReq.path, server.arg("httpreq_path").c_str(), CFG_HTTPREQ_PATH_SIZE );
    itemp = server.arg("httpreq_port").toInt();
    config.httpReq.port = (itemp>=0 && itemp<=65535) ? itemp : CFG_HTTPREQ_DEFAULT_PORT ; 
    itemp = server.arg("httpreq_freq").toInt();
    if (itemp>0 && itemp<=86400)
    {
      Tick_httpRequest.detach();
      Tick_httpRequest.attach(itemp, Task_httpRequest);
    } else {
      itemp = 0 ; 
    }
    config.httpReq.freq = itemp;

    itemp = server.arg("httpreq_swidx").toInt();
    if (itemp > 0 && itemp <= 65535)
      config.httpReq.swidx = itemp;
    else
      config.httpReq.swidx = 0;

    if ( saveConfig() ) {
      ret = 200;
      response = "OK";
    } else {
      ret = 412;
      response = "Unable to save configuration";
    }

    showConfig();
  }
  else
  {
    ret = 400;
    response = "Missing Form Field";
  }

  DebugF("Sending response "); 
  Debug(ret); 
  Debug(":"); 
  Debugln(response); 
  server.send ( ret, "text/plain", response);
  LedBluOFF();
}

/* ======================================================================
Function: handleRoot 
Purpose : handle main page /
Input   : -
Output  : - 
Comments: -
====================================================================== */
void handleRoot(void) 
{
  LedBluON();
  handleFileRead("/");
  LedBluOFF();
}

/* ======================================================================
Function: formatNumberJSON 
Purpose : check if data value is full number and send correct JSON format
Input   : String where to add response
          char * value to check 
Output  : - 
Comments: 00150 => 150
          ADCO  => "ADCO"
          1     => 1
====================================================================== */
void formatNumberJSON( String &response, char * value)
{
  // we have at least something ?
  if (value && strlen(value))
  {
    boolean isNumber = true;
    char * p = value;

    // just to be sure
    if (strlen(p)<=16) {
      // check if value is number
      while (*p && isNumber) {
        if ( *p < '0' || *p > '9' )
          isNumber = false;
        p++;
      }

      // this will add "" on not number values
      if (!isNumber) {
        response += '\"' ;
        response += value ;
        response += F("\"") ;
      } else {
        // this will remove leading zero on numbers
        p = value;
        while (*p=='0' && *(p+1) )
          p++;
        response += p ;
      }
    } else {
      Debugln(F("formatNumberJSON error!"));
    }
  }
}


/* ======================================================================
Function: tinfoJSONTable 
Purpose : dump all teleinfo values in JSON table format for browser
Input   : linked list pointer on the concerned data
          true to dump all values, false for only modified ones
Output  : - 
Comments: -
====================================================================== */
void tinfoJSONTable(void)
{
String response = "";
boolean first_item = true;
  
  ValueList * me = tinfo.getList();
  
  // Got at least one ?
  if (me && me->next) {

    // Json start
    response += F("[\r\n");

    // Loop thru the node
    while (me->next) {
      // go to next node
      me = me->next;

      if (first_item)
        first_item = false;
      else 
        response += F(",\r\n");
      
      response += F("{\"na\":\"");
      response +=  String(me->name);
      response += F("\", \"va\":\"") ;
      response += String(me->value);
      response += F("\", \"ck\":\"") ;
      if (me->checksum == '"' || me->checksum == '\\' || me->checksum == '/')
        response += '\\';
      response += (char) me->checksum;
      response += F("\", \"fl\":");
      response += me->flags ;
      response += F("}");
    }
    // Json end
    response += F("\r\n]");

    Debug(F("sending... "));Debugln(response);
    
    server.send ( 200, "text/json", response );

  } else {
    Debugln(F("sending 404..."));
    server.send ( 404, "text/plain", "No data" );
  }
  Debugln(F("OK!"));
  
}

/* ======================================================================
Function: getSysJSONData 
Purpose : Return JSON string containing system data
Input   : Response String
Output  : - 
Comments: -
====================================================================== */
void getSysJSONData(String & response)
{
  response = "";
  char buffer[32];
  int32_t adc = ( 1000 * analogRead(A0) / 1024 );

  // Json start
  response += F("[\r\n");

  response += "{\"na\":\"Uptime\",\"va\":\"";
  response += sysinfo.sys_uptime;
  response += "\"},\r\n";

  response += "{\"na\":\"Adresse IP\",\"va\":\"";
  response += WiFi.localIP().toString();
  response += "\"},\r\n";

  response += "{\"na\":\"Adresse MAC station\",\"va\":\"";
  response += WiFi.macAddress();
  response += "\"},\r\n";
  
  if (WiFi.status() == WL_CONNECTED)
  {
      response += "{\"na\":\"Wifi network\",\"va\":\"";
      response += config.ssid;
      response += "\"},\r\n";
      response += "{\"na\":\"Wifi RSSI\",\"va\":\"";
      response += WiFi.RSSI();
      response += " dB\"},\r\n";
  } else {
      response += "{\"na\":\"Wifi network\",\"va\":\"";
      response += "Not connected";
      response += "\"},\r\n";
  }

  response += "{\"na\":\"WifInfo Version\",\"va\":\"" WIFINFO_VERSION "\"},\r\n";

  response += "{\"na\":\"Compile le\",\"va\":\"" __DATE__ " " __TIME__ "\"},\r\n";
  
  response += "{\"na\":\"Options de compilation\",\"va\":\"";
  response += optval;
  response += "\"},\r\n";

  response += "{\"na\":\"SDK Version\",\"va\":\"";
  response += system_get_sdk_version() ;
  response += "\"},\r\n";

  response += "{\"na\":\"Chip ID\",\"va\":\"";
  sprintf_P(buffer, "0x%0X",system_get_chip_id() );
  response += buffer ;
  response += "\"},\r\n";

  response += "{\"na\":\"Boot Version\",\"va\":\"";
  sprintf_P(buffer, "0x%0X",system_get_boot_version() );
  response += buffer ;
  response += "\"},\r\n";

  response += "{\"na\":\"Flash Real Size\",\"va\":\"";
  response += formatSize(ESP.getFlashChipRealSize()) ;
  response += "\"},\r\n";

  response += "{\"na\":\"Firmware Size\",\"va\":\"";
  response += formatSize(ESP.getSketchSize()) ;
  response += "\"},\r\n";

  response += "{\"na\":\"Free Size\",\"va\":\"";
  response += formatSize(ESP.getFreeSketchSpace()) ;
  response += "\"},\r\n";

  response += "{\"na\":\"Analog\",\"va\":\"";
  adc = ( (1000 * analogRead(A0)) / 1024);
  sprintf_P( buffer, PSTR("%d mV"), adc);
  response += buffer ;
  response += "\"},\r\n";

  FSInfo info;
  SPIFFS.info(info);

  response += "{\"na\":\"SPIFFS Total\",\"va\":\"";
  response += formatSize(info.totalBytes) ;
  response += "\"},\r\n";

  response += "{\"na\":\"SPIFFS Used\",\"va\":\"";
  response += formatSize(info.usedBytes) ;
  response += "\"},\r\n";

  response += "{\"na\":\"SPIFFS Occupation\",\"va\":\"";
  sprintf_P(buffer, "%d%%",100*info.usedBytes/info.totalBytes);
  response += buffer ;
  response += "\"},\r\n"; 

  // Free mem
  response += "{\"na\":\"Free Ram\",\"va\":\"";
  response += formatSize(system_get_free_heap_size()) ;
  response += "\"},\r\n";

  // LibTeleinfo Checksum Error Count
  response += "{\"na\":\"Teleinfo Erreur Checksum\",\"va\":\"";
  response += String(tinfo.getChecksumErrorCount());
  response += "\"},\r\n";

  // LibTeleinfo Frame Size Error Count
  response += "{\"na\":\"Teleinfo Erreur Taille Frame\",\"va\":\"";
  response += String(tinfo.getFrameSizeErrorCount());
  response += "\"},\r\n";

  // LibTeleinfo Frame Format Error Count
  response += "{\"na\":\"Teleinfo Erreur Format Frame\",\"va\":\"";
  response += String(tinfo.getFrameFormatErrorCount());
  response += "\"},\r\n";

  // LibTeleinfo Frame Interrupted Count
  response += "{\"na\":\"Teleinfo Erreur Frame Interrompues\",\"va\":\"";
  response += String(tinfo.getFrameInterruptedCount());
  response += "\"}\r\n"; // Last don't have comma at end
 
  // Json end
  response += F("]\r\n");
}

/* ======================================================================
Function: sysJSONTable 
Purpose : dump all sysinfo values in JSON table format for browser
Input   : -
Output  : - 
Comments: -
====================================================================== */
void sysJSONTable()
{
  String response = "";
  
  getSysJSONData(response);

  // Just to debug where we are
  Debug(F("Serving /system page... "));Debugln(response);
  server.send ( 200, "text/json", response );
  Debugln(F("Ok!"));
}



/* ======================================================================
Function: getConfigJSONData 
Purpose : Return JSON string containing configuration data
Input   : Response String
Output  : - 
Comments: -
====================================================================== */
void getConfJSONData(String & r)
{
  // Json start
  r = FPSTR(FP_JSON_START); 

  r+="\"";
  r+=CFG_FORM_SSID;      r+=FPSTR(FP_QCQ); r+=config.ssid;           r+= FPSTR(FP_QCNL); 
  r+=CFG_FORM_PSK;       r+=FPSTR(FP_QCQ); r+=config.psk;            r+= FPSTR(FP_QCNL); 
  r+=CFG_FORM_HOST;      r+=FPSTR(FP_QCQ); r+=config.host;           r+= FPSTR(FP_QCNL); 
  r+=CFG_FORM_AP_PSK;    r+=FPSTR(FP_QCQ); r+=config.ap_psk;         r+= FPSTR(FP_QCNL); 
  r+=CFG_FORM_MQTT_HOST; r+=FPSTR(FP_QCQ); r+=config.mqtt.host;      r+= FPSTR(FP_QCNL); 
  r+=CFG_FORM_MQTT_PORT; r+=FPSTR(FP_QCQ); r+=config.mqtt.port;      r+= FPSTR(FP_QCNL); 
  r+=CFG_FORM_MQTT_USER; r+=FPSTR(FP_QCQ); r+=config.mqtt.user;      r+= FPSTR(FP_QCNL); 
  r+=CFG_FORM_MQTT_PSWD; r+=FPSTR(FP_QCQ); r+=config.mqtt.pswd;      r+= FPSTR(FP_QCNL); 
  r+=CFG_FORM_MQTT_TOPIC;r+=FPSTR(FP_QCQ); r+=config.mqtt.topic;     r+= FPSTR(FP_QCNL); 
  r+=CFG_FORM_MQTT_FREQ; r+=FPSTR(FP_QCQ); r+=config.mqtt.freq;      r+= FPSTR(FP_QCNL); 
  r+=CFG_FORM_EMON_HOST; r+=FPSTR(FP_QCQ); r+=config.emoncms.host;   r+= FPSTR(FP_QCNL); 
  r+=CFG_FORM_EMON_PORT; r+=FPSTR(FP_QCQ); r+=config.emoncms.port;   r+= FPSTR(FP_QCNL); 
  r+=CFG_FORM_EMON_URL;  r+=FPSTR(FP_QCQ); r+=config.emoncms.url;    r+= FPSTR(FP_QCNL); 
  r+=CFG_FORM_EMON_KEY;  r+=FPSTR(FP_QCQ); r+=config.emoncms.apikey; r+= FPSTR(FP_QCNL); 
  r+=CFG_FORM_EMON_NODE; r+=FPSTR(FP_QCQ); r+=config.emoncms.node;   r+= FPSTR(FP_QCNL); 
  r+=CFG_FORM_EMON_FREQ; r+=FPSTR(FP_QCQ); r+=config.emoncms.freq;   r+= FPSTR(FP_QCNL); 
  r+=CFG_FORM_OTA_AUTH;  r+=FPSTR(FP_QCQ); r+=config.ota_auth;       r+= FPSTR(FP_QCNL); 
  r+=CFG_FORM_OTA_PORT;  r+=FPSTR(FP_QCQ); r+=config.ota_port;       r+= FPSTR(FP_QCNL);
  r+=CFG_FORM_SYSLOG_HOST; r+=FPSTR(FP_QCQ); r+=config.syslog_host;  r+= FPSTR(FP_QCNL); 
  r+=CFG_FORM_SYSLOG_PORT; r+=FPSTR(FP_QCQ); r+=config.syslog_port;  r+= FPSTR(FP_QCNL);
  r+=CFG_FORM_JDOM_HOST; r+=FPSTR(FP_QCQ); r+=config.jeedom.host;   r+= FPSTR(FP_QCNL); 
  r+=CFG_FORM_JDOM_PORT; r+=FPSTR(FP_QCQ); r+=config.jeedom.port;   r+= FPSTR(FP_QCNL); 
  r+=CFG_FORM_JDOM_URL;  r+=FPSTR(FP_QCQ); r+=config.jeedom.url;    r+= FPSTR(FP_QCNL); 
  r+=CFG_FORM_JDOM_KEY;  r+=FPSTR(FP_QCQ); r+=config.jeedom.apikey; r+= FPSTR(FP_QCNL); 
  r+=CFG_FORM_JDOM_ADCO; r+=FPSTR(FP_QCQ); r+=config.jeedom.adco;   r+= FPSTR(FP_QCNL); 
  r+=CFG_FORM_JDOM_FREQ; r+=FPSTR(FP_QCQ); r+=config.jeedom.freq;   r+= FPSTR(FP_QCNL); 
  r+=CFG_FORM_HTTPREQ_HOST; r+=FPSTR(FP_QCQ); r+=config.httpReq.host;   r+= FPSTR(FP_QCNL); 
  r+=CFG_FORM_HTTPREQ_PORT; r+=FPSTR(FP_QCQ); r+=config.httpReq.port;   r+= FPSTR(FP_QCNL); 
  r+=CFG_FORM_HTTPREQ_PATH; r+=FPSTR(FP_QCQ); r+=config.httpReq.path;   r+= FPSTR(FP_QCNL);  
  r+=CFG_FORM_HTTPREQ_FREQ; r+=FPSTR(FP_QCQ); r+=config.httpReq.freq;   r+= FPSTR(FP_QCNL);   
  r+=CFG_FORM_HTTPREQ_SWIDX; r+=FPSTR(FP_QCQ); r+=config.httpReq.swidx;  
  

  r+= F("\""); 
  // Json end
  r += FPSTR(FP_JSON_END);

}

/* ======================================================================
Function: confJSONTable 
Purpose : dump all config values in JSON table format for browser
Input   : -
Output  : - 
Comments: -
====================================================================== */
void confJSONTable()
{
  String response = "";
  getConfJSONData(response);
  // Just to debug where we are
  Debug(F("Serving /config page..."));
  server.send ( 200, "text/json", response );
  Debugln(F("Ok!"));
}

/* ======================================================================
Function: getSpiffsJSONData 
Purpose : Return JSON string containing list of SPIFFS files
Input   : Response String
Output  : - 
Comments: -
====================================================================== */
void getSpiffsJSONData(String & response)
{
  bool first_item = true;

  // Json start
  response = FPSTR(FP_JSON_START);

  // Files Array  
  response += F("\"files\":[\r\n");

  // Loop trough all files
  Dir dir = SPIFFS.openDir("/");
  while (dir.next()) {    
    String fileName = dir.fileName();
    size_t fileSize = dir.fileSize();
    if (first_item)  
      first_item=false;
    else
      response += ",";

    response += F("{\"na\":\"");
    response += fileName.c_str();
    response += F("\",\"va\":\"");
    response += fileSize;
    response += F("\"}\r\n");
  }
  response += F("],\r\n");


  // SPIFFS File system array
  response += F("\"spiffs\":[\r\n{");
  
  // Get SPIFFS File system informations
  FSInfo info;
  SPIFFS.info(info);
  response += F("\"Total\":");
  response += info.totalBytes ;
  response += F(", \"Used\":");
  response += info.usedBytes ;
  response += F(", \"ram\":");
  response += system_get_free_heap_size() ;
  response += F("}\r\n]"); 

  // Json end
  response += FPSTR(FP_JSON_END);
}

/* ======================================================================
Function: spiffsJSONTable 
Purpose : dump all spiffs system in JSON table format for browser
Input   : -
Output  : - 
Comments: -
====================================================================== */
void spiffsJSONTable()
{
  String response = "";
  getSpiffsJSONData(response);
  server.send ( 200, "text/json", response );
}

/* ======================================================================
Function: sendJSON 
Purpose : dump all values in JSON
Input   : linked list pointer on the concerned data
          true to dump all values, false for only modified ones
Output  : - 
Comments: -
====================================================================== */
void sendJSON(void)
{
  ValueList * me = tinfo.getList();
  String response = "";
  
  // Got at least one ?
  if (me) {
    // Json start
    response += FPSTR(FP_JSON_START);
    response += F("\"_UPTIME\":");
    response += seconds;

    // Loop thru the node
    while (me->next) {
      // go to next node
      me = me->next;
      response += F(",\"") ;
      response += String(me->name);
      response += F("\":") ;
      formatNumberJSON(response, (char *)(String(me->value).c_str()));
    }
   // Json end
   response += FPSTR(FP_JSON_END) ;

  } else {
    server.send ( 404, "text/plain", "No data" );
  }
  server.send ( 200, "text/json", response );
}


/* ======================================================================
Function: wifiScanJSON 
Purpose : scan Wifi Access Point and return JSON code
Input   : -
Output  : - 
Comments: -
====================================================================== */
void wifiScanJSON(void)
{
  String response = "";
  bool first = true;

  // Just to debug where we are
  Debug(F("Serving /wifiscan page..."));

  int n = WiFi.scanNetworks();

  // Json start
  response += F("[\r\n");

  for (uint8_t i = 0; i < n; ++i)
  {
    int8_t rssi = WiFi.RSSI(i);
    
    // uint8_t percent;

    // dBm to Quality
    // if(rssi<=-100)      percent = 0;
    // else if (rssi>=-50) percent = 100;
    // else                percent = 2 * (rssi + 100);

    if (first) 
      first = false;
    else
      response += F(",");

    response += F("{\"ssid\":\"");
    response += WiFi.SSID(i);
    response += F("\",\"rssi\":") ;
    response += rssi;
    response += FPSTR(FP_JSON_END);
  }

  // Json end
  response += FPSTR("]\r\n");

  Debug(F("sending..."));
  server.send ( 200, "text/json", response );
  Debugln(F("Ok!"));
}


/* ======================================================================
Function: handleFactoryReset 
Purpose : reset the module to factory settingd
Input   : -
Output  : - 
Comments: -
====================================================================== */
void handleFactoryReset(void)
{
  // Just to debug where we are
  Debug(F("Serving /factory_reset page..."));
  ResetConfig();
  ESP.eraseConfig();
  Debug(F("sending..."));
  server.send ( 200, "text/plain", FPSTR(FP_RESTART) );
  Debugln(F("Ok!"));
  delay(1000);
  ESP.restart();
  while (true)
    delay(1);
}

/* ======================================================================
Function: handleReset 
Purpose : reset the module
Input   : -
Output  : - 
Comments: -
====================================================================== */
void handleReset(void)
{
  // Just to debug where we are
  Debug(F("Serving /reset page..."));
  Debug(F("sending..."));
  server.send ( 200, "text/plain", FPSTR(FP_RESTART) );
  Debugln(F("Ok!"));
  delay(1000);
  ESP.restart();
  while (true)
    delay(1);

}


/* ======================================================================
Function: handleNotFound 
Purpose : default WEB routing when URI is not found
Input   : -
Output  : - 
Comments: -
====================================================================== */
void handleNotFound(void) 
{
  String response = "";
  boolean found = false;  

  // Led on
  LedBluON();

  // try to return SPIFFS file
  found = handleFileRead(server.uri());

  // Try Teleinfo ETIQUETTE
  if (!found) {
    // We check for an known label
    ValueList * me = tinfo.getList();
    const char * uri;
    // convert uri to char * for compare
    uri = server.uri().c_str();

    Debugf("handleNotFound(%s)\r\n", uri);

    // Got at least one and consistent URI ?
    if (me && uri && *uri=='/' && *++uri ) {
      
      // Loop thru the linked list of values
      while (me->next && !found) {

        // go to next node
        me = me->next;

        //Debugf("compare to '%s' ", me->name);
        // Do we have this one ?
        if (strcmp (me->name, uri) == 0 )
        {
          // no need to continue
          found = true;

          // Add to respone
          response += F("{\"") ;
          response += me->name ;
          response += F("\":") ;
          formatNumberJSON(response, me->value);
          response += F("}\r\n");
        }
      }
    }

    // Got it, send json
    if (found) 
      server.send ( 200, "text/json", response );
  }

  // All trys failed
  if (!found) {
    // send error message in plain text
    String message = F("File Not Found\n\n");
    message += F("URI: ");
    message += server.uri();
    message += F("\nMethod: ");
    message += ( server.method() == HTTP_GET ) ? "GET" : "POST";
    message += F("\nArguments: ");
    message += server.args();
    message += FPSTR(FP_NL);

    for ( uint8_t i = 0; i < server.args(); i++ ) {
      message += " " + server.argName ( i ) + ": " + server.arg ( i ) + FPSTR(FP_NL);
    }

    server.send ( 404, "text/plain", message );
  }

  // Led off
  LedBluOFF();
}
