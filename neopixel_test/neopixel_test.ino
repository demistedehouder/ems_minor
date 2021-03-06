#include <Adafruit_NeoPixel.h>

#ifdef __AVR__

// Required for 16MHz Adafruit Trinket
#include <avr/power.h>
#endif

#define NEOPIXEL_PIN 6

// Amount of leds on the neopixel, starts at 0
#define LED_COUNT 6

// Variables for neopixel
float neoSpeed 0.008;
float maxBrightness = 255;

//Colors
uint32_t red = strip.Color(255, 0, 0);
uint32_t orange = strip.Color(255, 128, 0);
uint32_t yellow = strip.Color(255, 255, 0);
uint32_t green = strip.Color(0, 255, 0);
uint32_t light_blue = strip.Color(0, 255, 255);

// Neopixel declaration
// (led amount, led pin, pixel type flags)
Adafruit_Neopixel jewel(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
// Other led flags:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)

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

}

void flowyNeopixel(int wait)
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
}

void flashingNeopixel(int wait)
{
  
}
