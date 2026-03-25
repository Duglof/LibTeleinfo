echo "esp-backup.sh V1.0.2"
echo "backup entire flash memory for esp8266 and esp32 "
echo "supported flash size 1M ou 4M"
echo ""
echo "Tested on linux with python3 and esptool V4.7.0"

echo "install esptool ..."
pip install esptool > /dev/null
echo ""
echo -n "esptool version: "
esptool.py version

serialport=$(esptool.py flash_id 2>&1 | grep 'Serial port' | cut -c12-)
echo "Serial port is ${serialport}"

chip=$(esptool.py --port $serialport flash_id 2>&1 | grep 'Chip is' | cut -c9- | tr -d '() ')
echo "Chip is ${chip}"

flashsize=$(esptool.py --port $serialport flash_id 2>&1 | grep 'flash size' | cut -c22-)
echo "Flash size ${flashsize}"
echo "Backup flash ..."

case $flashsize in
   "1MB")
     esptool.py --port $serialport read_flash 0 0x100000 "Wifinfo-${chip}-${flashsize}-fullflash.bin";;
   "4MB")
     esptool.py --port $serialport read_flash 0 0x400000 "Wifinfo-${chip}-${flashsize}-fullflash.bin";;
   *)
      echo "unspported flash size: $flashsize" ;;
esac

