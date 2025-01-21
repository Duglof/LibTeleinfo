// **********************************************************************************
// Arduino Teleinfo sample, display information on teleinfo values received
// **********************************************************************************
// Creative Commons Attrib Share-Alike License
// You are free to use/extend this library but please abide with the CC-BY-SA license:
// http://creativecommons.org/licenses/by-sa/4.0/
//
// for detailled explanation of this library see dedicated article
// https://hallard.me/libteleinfo/
//
// For any explanation about teleinfo or use, see my blog
// https://hallard.me/category/tinfo
//
// connect Teleinfo sur GPIO indiqué par le Setup()
// 
// see schematic here https://hallard.me/demystifier-la-teleinfo/
// and dedicated article here 
//
// Written by Charles-Henri Hallard (https://hallard.me)
//
// History : V1.00 2015-06-14 - First release
//
// All text above must be included in any redistribution.
//
// V3.0.0 Dugolf Adaptation ESP32

// **********************************************************************************
#include <Arduino.h>

#include <HardwareSerial.h>

#include <LibTeleinfo.h>

#ifndef ESP32
  #error Ne fonctionne que sur ESP32
#endif

// Décommenter pour choisir le mode de votre compteur linky
#define  TELEINFO_MODE    TINFO_MODE_HISTORIQUE
// #define  TELEINFO_MODE    TINFO_MODE_STANDARD


TInfo          tinfo; // Teleinfo object

/* ======================================================================
Function: printUptime 
Purpose : print pseudo uptime value
Input   : -
Output  : - 
Comments: compteur de secondes basique sans controle de dépassement
          En plus SoftwareSerial rend le compteur de millis() totalement
          A la rue, donc la precision de ce compteur de seconde n'est
          pas fiable du tout, dont acte !!!
====================================================================== */
void printUptime(void)
{
  Serial.print(millis()/1000);
  Serial.print(F("s\t"));
}

/* ======================================================================
Function: DataCallback 
Purpose : callback when we detected new or modified data received
Input   : linked list pointer on the concerned data
          current flags value
Output  : - 
Comments: -
====================================================================== */
void DataCallback(ValueList * me, uint8_t  flags)
{
  // Show our not accurate second counter
  printUptime();

  if (flags & TINFO_FLAGS_ADDED) 
    Serial.print(F("NEW -> "));

  if (flags & TINFO_FLAGS_UPDATED)
    Serial.print(F("MAJ -> "));

  // Display values
  Serial.print(String(me->name));
  Serial.print("=");
  Serial.println(String(me->value));
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
  // Serial, pour le debug
  Serial.begin(115200);

  Serial.println(F("========================================"));
  Serial.println(F(__FILE__));
  Serial.println(F(__DATE__ " " __TIME__));
  Serial.println();

  Serial.println("Sélection du mode du Linky en entête du programme");
  Serial.println("  #define  TELEINFO_MODE    TINFO_MODE_HISTORIQUE ou");
  Serial.println("  #define  TELEINFO_MODE    TINFO_MODE_STANDARD");

#if TELEINFO_MODE == TINFO_MODE_HISTORIQUE
  Serial.println("Linky mode Historique 1200 bauds");
  Serial1.begin(1200, SERIAL_7E1);
#else
  Serial.println("Linky mode Standard 9600 bauds");
  Serial1.begin(9600, SERIAL_7E1);
#endif

  Serial.print("Interface TELEINFO connectée sur GPIO");
  Serial.println(RX1);
  
  // Init teleinfo
  tinfo.init(TELEINFO_MODE);

  // Attacher les callback dont nous avons besoin
  // pour cette demo, ici attach data
  tinfo.attachData(DataCallback);

  printUptime();
  Serial.println(F("Teleinfo started"));
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
  // Teleinformation processing
  if ( Serial1.available() ) {
    tinfo.process(Serial1.read());
  }
}
