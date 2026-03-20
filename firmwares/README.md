# Backup / Restore

## Backup
- **esp-backup.sh**
- Outils pour faire un backup de la memoire flash de l'esp8266 ou ESP32
- Testé sur linux avec esptool V4.7.0
- Utilise esptool.py (installation automatique)
- Détecte automatiquement l'esp connecté sur votre ordinateur
- Exécuter ./esp-backup.sh
- Affiche:
  - Le port de connexion
  - Le modèle
  - La taille de la memoire flash
- Effectue une lecture complète de la mémoire et créé un fichier
  - Exemple pour un ESP8266 Wemos D1 Mini : **wifinfo-ESP8266EX-4MB-firmware.bin**

## Program / Restore
- Ce fichier pourra être uilisé pour programmer un autre module:
  - Connecter l'esp à programmer sur votre ordinateur (et seulement celui là !!!)
  - Choisissez le bon fichier pour votre ESP
  - Exécuter la commande:
    - **esptool.py write_flash 0x00000 wifinfo-ESP8266EX-4MB-firmware.bin**

