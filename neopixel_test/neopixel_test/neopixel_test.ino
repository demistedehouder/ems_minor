#include <Adafruit_NeoPixel.h>

#define NEOPIXEL_PIN 6

// Amount of leds on the neopixel, starts at 0
#define LED_COUNT 6

// Variables for neopixel
float neoSpeed = 0.008;
float maxBrightness = 255;

// Neopixel declaration
// (led amount, led pin, pixel type flags)
Adafruit_NeoPixel jewel(LED_COUNT, NEOPIXEL_PIN, NEO_RGBW + NEO_KHZ800);
// Other led flags:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)

//Colors
uint32_t green = jewel.Color(255, 0, 0);
uint32_t white = jewel.Color(255, 255, 255);
uint32_t dark_white = jewel.Color(23, 23, 23);
uint32_t orange = jewel.Color(255, 128, 0);
uint32_t yellow = jewel.Color(255, 255, 0);
uint32_t red = jewel.Color(0, 255, 0);
uint32_t light_blue = jewel.Color(0, 255, 255);
uint32_t off = jewel.Color(0, 0, 0);

void setup() {
  // put your setup code here, to run once:
  //below is trinket specific code, uncomment if stuff doesn't run
  //  #if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
  //    clock_prescale_set(clock_div_1);
  //  #endif

  // Initialize neopixel object
  jewel.begin();

  // Turn off pixels immediately 
  jewel.show();
}

void loop() {
  // put your main code here, to run repeatedly:
  flashingNeopixel();
}

void flowyNeopixel()
{
}

// For orange lighting
void pulsatingNeopixel(int wait)
{
  // Make the lights breathe
  for (int i = 0; i < 65535; i++) {
    // Intensity will go from 10 - maxBrightness in a "breathing" manner
    float intensity = maxBrightness /2.0 * (1.0 + sin(neoSpeed * i));
    jewel.setBrightness(intensity);
    
    // Now set every LED to that color
    for (int ledNumber=0; ledNumber<LED_COUNT; ledNumber++) {
      jewel.setPixelColor(ledNumber,orange);
    }
  }

  delay(wait);
  
}

// For red lighting
void flashingNeopixel()
{
    uint32_t low = jewel.Color(0, 0, 0); 
    uint32_t high = jewel.Color(255, 255, 255);
    jewel.setPixelColor(0,white);
    // Turn them off
    
    jewel.fill(off, 1, 7);
    jewel.show();
    delay(1000);

    jewel.setPixelColor(0, dark_white);
    jewel.fill(red, 1, 7);
    jewel.show();
    delay(1000);
}
