// **********************************************************************************
// ESP8266 Teleinfo WEB Client, web server function
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
// History : V1.00 2015-12-04 - First release
//
// All text above must be included in any redistribution.
//
// **********************************************************************************

#include "webclient.h"

/* ======================================================================
Function: httpPost
Purpose : Do a http post
Input   : hostname
          port
          url
          data
Output  : true if received 200 OK
Comments: -
====================================================================== */
boolean httpPost(char * host, uint16_t port, char * url, char * data)
{
  WiFiClient client;
  HTTPClient http;
  bool ret = false;
  int httpCode=0;

  unsigned long start = millis();

  // configure traged server and url
  http.begin(client, host, port, url); 
  //http.begin("http://emoncms.org/input/post.json?node=20&apikey=2f13e4608d411d20354485f72747de7b&json={PAPP:100}");
  //http.begin("emoncms.org", 80, "/input/post.json?node=20&apikey=2f13e4608d411d20354485f72747de7b&json={}"); //HTTP

  Debugf("http%s://%s:%d%s => ", port==443?"s":"", host, port, url);

  // start connection and send HTTP header
  if ( data != NULL) { // data string is not null, use POST instead of GET
    http.addHeader("Content-Type", "application/json");  //Specify content-type header
    httpCode = http.POST(data);
  } else {
    httpCode = http.GET();
  }
  if(httpCode) {
      // HTTP header has been send and Server response header has been handled
      Debug(httpCode);
      Debug(" ");
      // file found at server
      if(httpCode >= 200 && httpCode < 300) {
        String payload = http.getString();
        Debug(payload);
        ret = true;
      }
  } else {
      DebugF("failed!");
  }
  Debugf(" in %lu ms\r\n",millis()-start);
  return ret;
}

/* ======================================================================
Function: build_mqtt_json string (usable by webserver.cpp)
Purpose : construct the json part of mqtt url
Input   : -
Output  : String if some Teleinfo data available
Comments: -
====================================================================== */
/* exemple data
 *  From https://forum.hacf.fr/t/mqtt-instable-linky/21008
 * {« TIC »:{« ADSC »:« 0222761xxxx »,« VTIC »:2,« NGTF »:« TEMPO »,« LTARF »:« HP BLEU »,« EAST »:999935,« EASF01 »:834119,« EASF02 »:30571,« EASF03 »:60434,« EASF04 »:58796,« EASF05 »:8101,« EASF06 »:7914,« EASF07 »:0,« EASF08 »:0,« EASF09 »:0,« EASF10 »:0,« EASD01 »:902654,« EASD02 »:97281,« EASD03 »:0,« EASD04 »:0,« IRMS1 »:2,« IRMS2 »:1,« IRMS3 »:1,« URMS1 »:239,« URMS2 »:240,« URMS3 »:238,« PREF »:12,« PCOUP »:12,« SINSTS »:659,« SINSTS1 »:378,« SINSTS2 »:125,« SINSTS3 »:154,« SMAXSN »:7270,« SMAXSN1 »:2850,« SMAXSN2 »:2110,« SMAXSN3 »:2300,« SMAXSN-1 »:7190,« SMAXSN1-1 »:2830,« SMAXSN2-1 »:2090,« SMAXSN3-1 »:2300,« CCASN »:528,« CCASN-1 »:522,« UMOY1 »:239,« UMOY2 »:240,« UMOY3 »:238,« STGE »:« 013A4401 »,« DPM1 »:0,« FPM1 »:0,« PRM »:2147483647,« RELAIS »:0,« NTARF »:2,« NJOURF »:0,« NJOURF+1 »:0,« PPOINTE »:« 00004003 06004004 16004003 NONUTILE NONUTILE NONUTILE NONUTILE NONUTILE NONUTILE NONUTILE NONUTILE »,« SINTS3 »:180,« UMS1 »:241,« ESD02 »:88269}}
*/
String build_mqtt_json(void)
{
  boolean first_item = true;
  boolean skip_item;

  String mqtt_data = "{\"TIC\":{" ;

  ValueList * me = tinfo.getList();

      // Loop thru the node
      while (me->next) {
        // go to next node
        me = me->next;
        skip_item = false;

        // Si Item virtuel, on le met pas
        if (*me->name =='_')
          skip_item = true;

        // On doit ajouter l'item ?
        if (!skip_item) {
          if (first_item) {
            first_item = false; 
            mqtt_data += "\"";
          } else {
            mqtt_data += ",\"";
          }
          mqtt_data += String(me->name);
          mqtt_data += "\":\"";
          mqtt_data += String(me->value);
          mqtt_data += "\"";
        }
      } // While me
      // Json end
      mqtt_data += "}}";
      Debugln(mqtt_data);
      
  return mqtt_data;
}


/* ======================================================================
Function: emoncmsPost
Purpose : Do a http post to emoncms
Input   : 
Output  : true if post returned 200 OK
Comments: -
====================================================================== */
boolean emoncmsPost(void)
{
  boolean ret = false;

  // Some basic checking
  if (*config.emoncms.host) {
    ValueList * me = tinfo.getList();
    // Got at least one ?
    if (me && me->next) {
      String url ; 
      boolean first_item;

      url = *config.emoncms.url ? config.emoncms.url : "/";
      url += "?";
      if (config.emoncms.node>0) {
        url+= F("node=");
        url+= String(config.emoncms.node);
        url+= "&";
      } 

      url += F("apikey=") ;
      url += config.emoncms.apikey;
      url += F("&json={") ;

      first_item = true;

      // Loop thru the node
      while (me->next) {
        // go to next node
        me = me->next;
        // First item do not add , separator
        if (first_item)
          first_item = false;
        else
          url += ",";

        url +=  String(me->name);
        url += ":" ;

        // EMONCMS ne sais traiter que des valeurs numériques, donc ici il faut faire une 
        // table de mappage, tout à fait arbitraire, mais c"est celle-ci dont je me sers 
        // depuis mes débuts avec la téléinfo
        if (!strcmp(me->name, "OPTARIF")) {
          // L'option tarifaire choisie (Groupe "OPTARIF") est codée sur 4 caractères alphanumériques 
          /* J'ai pris un nombre arbitraire codé dans l'ordre ci-dessous
          je mets le 4eme char à 0, trop de possibilités
          BASE => Option Base. 
          HC.. => Option Heures Creuses. 
          EJP. => Option EJP. 
          BBRx => Option Tempo
          */
          char * p = me->value;
            
               if (*p=='B'&&*(p+1)=='A'&&*(p+2)=='S') url += "1";
          else if (*p=='H'&&*(p+1)=='C'&&*(p+2)=='.') url += "2";
          else if (*p=='E'&&*(p+1)=='J'&&*(p+2)=='P') url += "3";
          else if (*p=='B'&&*(p+1)=='B'&&*(p+2)=='R') url += "4";
          else url +="0";
        } else if (!strcmp(me->name, "HHPHC")) {
          // L'horaire heures pleines/heures creuses (Groupe "HHPHC") est codé par un caractère A à Y 
          // J'ai choisi de prendre son code ASCII
          int code = *me->value;
          url += String(code);
        } else if (!strcmp(me->name, "PTEC")) {
          // La période tarifaire en cours (Groupe "PTEC"), est codée sur 4 caractères 
          /* J'ai pris un nombre arbitraire codé dans l'ordre ci-dessous
          TH.. => Toutes les Heures. 
          HC.. => Heures Creuses. 
          HP.. => Heures Pleines. 
          HN.. => Heures Normales. 
          PM.. => Heures de Pointe Mobile. 
          HCJB => Heures Creuses Jours Bleus. 
          HCJW => Heures Creuses Jours Blancs (White). 
          HCJR => Heures Creuses Jours Rouges. 
          HPJB => Heures Pleines Jours Bleus. 
          HPJW => Heures Pleines Jours Blancs (White). 
          HPJR => Heures Pleines Jours Rouges. 
          */
               if (!strcmp(me->value, "TH..")) url += "1";
          else if (!strcmp(me->value, "HC..")) url += "2";
          else if (!strcmp(me->value, "HP..")) url += "3";
          else if (!strcmp(me->value, "HN..")) url += "4";
          else if (!strcmp(me->value, "PM..")) url += "5";
          else if (!strcmp(me->value, "HCJB")) url += "6";
          else if (!strcmp(me->value, "HCJW")) url += "7";
          else if (!strcmp(me->value, "HCJR")) url += "8";
          else if (!strcmp(me->value, "HPJB")) url += "9";
          else if (!strcmp(me->value, "HPJW")) url += "10";
          else if (!strcmp(me->value, "HPJR")) url += "11";
          else url +="0";
        } else {
          url += String(me->value);
        }
      } // While me

      // Json end
      url += "}";

      ret = httpPost( config.emoncms.host, config.emoncms.port, (char *) url.c_str(), NULL) ;
    } // if me
  } // if host
  return ret;
}

/* ======================================================================
Function: jeedomPost
Purpose : Do a http post to jeedom server
Input   : 
Output  : true if post returned 200 OK
Comments: -
====================================================================== */
boolean jeedomPost(void)
{
  boolean ret = false;

   Debugln("jeedomPost : début");
  
  // Some basic checking
  if (*config.jeedom.host) {
    ValueList * me = tinfo.getList();
    // Got at least one ?
    if (me && me->next) {
      String url ; 
      boolean skip_item;

      url = *config.jeedom.url ? config.jeedom.url : "/";
      url += "?";


      url += F("apikey=") ;
      url += config.jeedom.apikey;

       String data = "{\"device\":{\"";

      // Config identifiant forcée ?
      if (*config.jeedom.adco) {
        data += config.jeedom.adco;
        data += "\":{\"device\":\"";
        data += config.jeedom.adco;
        data += "\"";
      } 

      // Loop thru the node
      while (me->next) {
        // go to next node
        me = me->next;
        skip_item = false;

        // Si ADCO déjà renseigné, on le remet pas
        if (!strcmp(me->name, "ADCO")) {
          if (*config.jeedom.adco)
            skip_item = true;
        }

        // Si Item virtuel, on le met pas
        if (*me->name =='_')
          skip_item = true;

        // On doit ajouter l'item ?
        if (!skip_item) {
          data += ",\"";
          data += String(me->name);
          data += "\":\"";
          data += String(me->value);
          data += "\"";
        }
      } // While me
      // Json end
      data += "}}}";
      Debugln(data);

      ret = httpPost( config.jeedom.host, config.jeedom.port, (char *) url.c_str(),(char *) data.c_str()) ;
    } else {
      Debugln("jeedomPost : la liste des données est vide");
    }
  } else {
   Debugln("jeedomPost : jeedom host non configuré");
  }
  return ret;
}

/* ======================================================================
Function: HTTP Request
Purpose : Do a http request
Input   : 
Output  : true if post returned 200 OK
Comments: -
====================================================================== */
boolean httpRequest(void)
{
  boolean ret = false;

  // Some basic checking
  if (*config.httpReq.host)
  {
    ValueList * me = tinfo.getList();
    // Got at least one ?
    if (me && me->next)
    {
      String url ; 
      boolean skip_item;

      url = *config.httpReq.path ? config.httpReq.path : "/";
      url += "?";

      // Loop thru the node
      while (me->next) {
        // go to next node
        me = me->next;
        skip_item = false;

        // Si Item virtuel, on le met pas
        if (*me->name =='_')
          skip_item = true;

        // On doit ajouter l'item ?
        if (!skip_item)
        {
          String valName = String(me->name);
          if (valName == "HCHP")
          {
            url.replace("%HCHP%", me->value);
          }
          if (valName == "HCHC")
          {
            url.replace("%HCHC%", me->value);
          }
          if (valName == "PAPP")
          {
            url.replace("%PAPP%", me->value);
          }
          if (valName == "ADCO")
          {
            url.replace("%ADCO%", me->value);
          }
          if (valName == "OPTARIF")
          {
            url.replace("%OPTARIF%", me->value);
          }
          if (valName == "ISOUC")
          {
            url.replace("%ISOUC%", me->value);
          }
          if (valName == "PTEC")
          {
            url.replace("%PTEC%", me->value);
          }
          if (valName == "IINST")
          {
            url.replace("%IINST%", me->value);
          }
          if (valName == "IMAX")
          {
            url.replace("%IMAX%", me->value);
          }
          if (valName == "HHPHC")
          {
            url.replace("%HHPHC%", me->value);
          }
          if (valName == "MOTDETAT")
          {
            url.replace("%MOTDETAT%", me->value);
          }
          if (valName == "BASE")
          {
            url.replace("%BASE%", me->value);
          }
        }
      } // While me

      ret = httpPost( config.httpReq.host, config.httpReq.port, (char *) url.c_str(), NULL) ;
    } // if me
  } // if host
  return ret;
}
