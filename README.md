# Teleinfo Universal Library
This is a fork of Teleinfo Universal Library for the ESP8266 MCU
This is a generic Teleinfo French Meter Measure Library
- Initial Github source : <https://github.com/hallard/LibTeleinfo>
- Modified Github source : <https://github.com/Doume/LibTeleinfo>
- Modified Github source : <https://github.com/theGressier/LibTeleinfo/tree/syslog>
- Modified Github source : <https://github.com/Duglof/LibTeleinfo>

Linky mode Historique (1200 bauds)

exemples/Wifinfo
  - Compatible jeedom v4 / clé API plugin Teleinfo 64 caractères

Wifinfo :  Configuration Wifi

Depuis votre téléphone portable :
- Se connecter au réseau Wifinfo-XXXXXX
- Navigateur (chrome ou autre) url : http://192.168.4.1
- Sélectionner l'onglet Configuration
- Réseau Wifi : Mettre le SSID
- Clé Wifi    : Mettre le mot de passe de votre réseau Wifi
- Clicker sur Enregistrer
- Déployer Avancée
- Cliquer sur Redémarrer Wifinfo
- Wifinfo se connectera à votre réseau Wifi
- Accéder à votre box pour connaître sa nouvelle adresse IP

# Modifications par dugolf (version 1.0.8)
- Wifinfo.h : #define WIFINFO_VERSION "1.0.8"
- library.json : "version": "1.0.8"
- library.properties : version=1.0.8
- Teleinfo clé api 64 caractères
  - config.h : #define CFG_JDOM_APIKEY_SIZE  64
  - data : index.htm.gz : index.htm : id="jdom_apikey" : maxlength="64"
  - Wifinfo.ino : char buff[300] //To format debug strings but also use to format jeedom request
- Correction erreurs de compilation (LibTeleinfo.h / LibTeleinfo.cpp)
  - void TInfo::process (char c); : retourne void
  - void TInfo::clearBuffer(); : retourne void
  - ValueList * TInfo::valueAdd; : return NULL à la fin de la méthode
- Warning C++ conversion const char * en char * (Wifinfo.ino)
  - add void Myprint(const char *msg)

# Modifications par theGressier (version 1.0.7)
- Modification de l'envoi des données à Jeedom suite à la mise à jour du plugin Teleinfo en v4 (changement API, POST JSON)
- Reconnect WiFi automatically after incident

# Modifications par Doume (version 1.0.6) branche 'syslog' :

- Permettre l'envoi des messages de debugging à un serveur rsyslog du réseau local

   les paramètres peuvent être configurés via l'interface Web, onglet 'Configuration'
   panel 'Avancée'
   
   Il suffit de laisser le paramètre 'Syslog host' vide, pour désactiver cette fonction.
   Les envois sur le réseau utilisent le protocole UDP pour alimenter le serveur
   distant
   
   Pour compiler avec l'option SYSLOG, vous devrez installer la librairie Syslog-master.zip
   qui se trouve dans le répertoire 'librairie', dans votre environnement Arduino IDE
			
# Modifications par Doume (version 1.0.5a) branche 'static' :

- Ajout de la gestion d'un contact sec, dont l'état peut être remonté vers Domoticz
     exemple : le compteur EDF est souvent près du portail, donc on peut notifier
     			l'état ouvert/fermé du portail en utilisant Wifinfo
			
# Modifications par Doume (version 1.0.5) branche 'static' :

- Add support for request /emoncms.json

        return a json list formated to interface emoncms server from a 3rd party 
        That allow to don't activate emoncms http client inside Wifinfo module
	
- Add possibility to compile a version of sketch for module not connected to EDF counter

        Activate #define SIMU into Wifinfo.h, to obtain a version which will create
         2 variables and update one of them each second, to verify package
	 
- Add a check on variable names, and force a reinit of LibTeleinfo interface if an
  alteration is detected (each restart is counted, and displayed in 'system' page )
  
- Change library LibTeleinfo.cpp, to don't use anymore malloc/free system APIs

		Variables are now stored in static table, allocated on start
		
			50 entries max, name length 16 bytes max, value length 16 bytes max
			
		To use this library version :
		
			First, backup your old version of LibTeleinfo....
			
			copy files src/LibTeleinfo.cpp and src/LibTeleinfo.h into your Arduino
			environment (generally ~/Arduino/libraries/LibTeleinfo-master/src )
			before to compile sketch

# Added features :
- Add possibility to configure HttpRequest to send parameters/values to Domoticz
- Add all possible variable as listed below : 
- Add some informations to 'System' page, like Wifi link quality, Wifi network name, and MAC address

Ces différents messages donnent les indications suivantes en fonction de l’abonnement souscrit :
- N° d’identification du compteur : ADCO (12 caractères)
- Option tarifaire (type d’abonnement) : OPTARIF (4 car.)
- Intensité souscrite : ISOUSC ( 2 car. unité = ampères)
- Index si option = base : BASE ( 9 car. unité = Wh)
- Index heures creuses si option = heures creuses : HCHC ( 9 car. unité = Wh)
- Index heures pleines si option = heures creuses : HCHP ( 9 car. unité = Wh)
- Index heures normales si option = EJP : EJP HN ( 9 car. unité = Wh)
- Index heures de pointe mobile si option = EJP : EJP HPM ( 9 car. unité = Wh)
- Index heures creuses jours bleus si option = tempo : BBR HC JB ( 9 car. unité = Wh)
- Index heures pleines jours bleus si option = tempo : BBR HP JB ( 9 car. unité = Wh)
- Index heures creuses jours blancs si option = tempo : BBR HC JW ( 9 car. unité = Wh)
- Index heures pleines jours blancs si option = tempo : BBR HP JW ( 9 car. unité = Wh)
- Index heures creuses jours rouges si option = tempo : BBR HC JR ( 9 car. unité = Wh)
- Index heures pleines jours rouges si option = tempo : BBR HP JR ( 9 car. unité = Wh)
- Préavis EJP si option = EJP : PEJP ( 2 car.) 30mn avant période EJP
- Période tarifaire en cours : PTEC ( 4 car.)
- Couleur du lendemain si option = tempo : DEMAIN
- Intensité instantanée : IINST ( 3 car. unité = ampères)
- Avertissement de dépassement de puissance souscrite : ADPS ( 3 car. unité = ampères) (message émis uniquement en cas de dépassement effectif, dans ce cas il est immédiat)
- Intensité maximale : IMAX ( 3 car. unité = ampères)
- Puissance apparente : PAPP ( 5 car. unité = Volt.ampères)
- Groupe horaire si option = heures creuses ou tempo : HHPHC (1 car.)
- Mot d’état (autocontrôle) : MOTDETAT (6 car.)

# Hardware
ESP-12E Dev Kit (type NodeMCU 1.0) avec 4 Mo de flash (30 broches)
![esp12e ch340](docs/esp12e.png) 
![esp12e gpio](docs/esp12e-gpio.png)
Alimentation de l'ESP : 5 Volt 500mA (200mA mini)
- VIN de ESP au +
- G de ESP au -
Interface compteur Linky by hallard 
![interface linky](docs/schema-interface-linky.jpg)
Connexions au compteur Linky (il n'y a pas de sens, on peut inverser)
- Connecter Teleinfo 1 et sur I1 du compteur
- Connecter Teleinfo 2 et sur I2 du compteur
Connexions à ESP-12E
- GND : G  de ESP (GND) 
- +V  : 3V de ESP (3.3V)
- RXD : RX de ESP (RXD0 GPIO3)
# Compilation / Téléversement
Utiliser Arduino IDE V1.8.18

# Tests
Jeedom V4.4.19 : Plugin Teleinfo by Noyax37 V4.8.7
