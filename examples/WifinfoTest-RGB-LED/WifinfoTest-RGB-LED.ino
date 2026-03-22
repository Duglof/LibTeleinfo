// ===========================================================
// WifinfoTest-RGB-LED
// ===========================================================

// -----------------------------------------------------------
// Programme de Test de la LED RGB
// LED : Clignote 2 fois 500 miliisecondes toutes les 3 secondes
//
// Hardware de test
//  - Lolin wemos Mini D1 + interface teleinfo by Hallard
//  - Wemos ESP32 + interface teleinfo by Hallard
// -----------------------------------------------------------
//
// 22 Mars 2026 : V1.0.0

#include <Arduino.h>
#include <Ticker.h>

// Wemos D1 Mini + interface teleinfo by Hallard => RGB LED connecté à GPIO 14 (D5)
// Wemos ESP32 + interface teleinfo by Hallard => RGB LED connecté à GPIO 18)
#ifdef ESP8266
  #define RGB_LED_PIN    14
#elif defined(ESP32)
  #define RGB_LED_PIN    18
#else
  #error "ce n'est ni un ESP8266 ni un ESP32"
#endif

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
#define COLOR_MAGENTA       300
#define COLOR_PINK          350


// NeoPixelBus by Makuna : Test with version 2.8.4
#include <NeoPixelBus.h>


#ifdef ESP8266
  NeoPixelBus<NeoGrbFeature, NeoEsp8266BitBang400KbpsMethod> rgb_led(1, RGB_LED_PIN);
#else
  NeoPixelBus<NeoGrbFeature, NeoEsp32Rmt0800KbpsMethod> rgb_led(1, RGB_LED_PIN);
#endif

Ticker tickerCallback;
Ticker rgb_ticker;

// Flags
volatile bool task_rgbled = false;

// RGB LED parameters
volatile int     rgbFlashCount = 0;
volatile bool    rgbAction = false;
volatile int     rgbHslColor = COLOR_RED;
volatile uint8_t rgb_brightness = 50;  // define whole brigtness level for RGBLED (50%)

// Callbask to simulate an RB action
void callbackStart() {
  rgbFlashCount = 3;            // = 1 : 1 flash ; = 3 : 2 flash ; = 5 : 3 flash
  rgbHslColor = COLOR_MAGENTA;
  rgb_brightness = 50;          // rgb_brightness à 50%
  rgbAction = true;             // Execution de l'action
}

void rgb_ledOn()
{
  Serial.println("rgb_ledOn");

  // Convert to neoPixel API values
  // H (is color from 0..360) should be between 0.0 and 1.0
  // L (is brightness from 0..100) should be between 0.0 and 0.5
  RgbColor target = HslColor( rgbHslColor / 360.0f, 1.0f, rgb_brightness * 0.005f );    

  // Set RGB Led
  rgb_led.SetPixelColor(0, target); 
  rgb_led.Show();
}

void rgb_ledOff()
{
 Serial.println("rgb_ledOff");
 rgb_led.SetPixelColor(0, RgbColor(0,0,0)); 
 rgb_led.Show();
}

void Task_rgbled()
{
  task_rgbled = true;
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

  // Appel de Task_rgbled toutes les 500ms
  rgb_ticker.attach_ms(500, Task_rgbled);

  // Init the RGB Led, test R,G,B and set it off
  rgb_led.Begin();

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

  //---------------------------------------- 
  // Clignotement de la LED RGB
  //---------------------------------------
  if (task_rgbled) {      // task_rgbled est mis à true toutes les 500ms 
    task_rgbled = false;
    
    if (rgbAction) {      // Test si action en cours ...
      if (rgbFlashCount & 1) {   //Impair
        rgb_ledOn();      // rgbFlashCount impair => on allume la led
      } else {
        rgb_ledOff();     // rgbFlashCount pair => on eteint la led 
      }
      if (rgbFlashCount)
        rgbFlashCount--;    // décrément de rgbFlashCount 
      else
        rgbAction= false;   // Arret de l'action si rgbFlashCount est arrivé à zéro
    }
  }

  // Exemple : déclenchement callback toutes les 3 secondes
  static unsigned long last = 0;
  if (millis() - last > 5000) {
    last = millis();
    tickerCallback.once_ms(1, callbackStart);
  }

}
