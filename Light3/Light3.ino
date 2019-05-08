
#include "WS2812Serial.h"
#define USE_WS2812SERIAL
#include "FastLED.h"

#define DATA_STRAND1 6   // strap
#define LED_PIN 13

#define N_STRAND1 120

CRGB leds[N_STRAND1];

void setup() {
  Serial.begin(115200);
  Serial.println("Pyramid starting");
  
	// sanity check delay - allows reprogramming if accidently blowing power w/leds
  delay(2000);

  FastLED.addLeds<WS2812SERIAL, DATA_STRAND1, GRB>(leds, N_STRAND1);
  /* FastLED.addLeds<WS2812B, DATA_STRAND1, GRB>(leds, N_STRAND1); */
  /* FastLED.addLeds<WS2812SERIAL, DATA_STRAND1>(leds, N_STRAND1); */
  FastLED.setMaxPowerInVoltsAndMilliamps(5, 3000);

  //fill_solid(leds, N_STRAND1, CRGB::Red);

  pinMode(LED_PIN, OUTPUT);
}

void loop() {
  static bool x = false;
  digitalWrite(LED_PIN, x ? LOW : HIGH);
  x = !x;

  for(int whiteLed = 0; whiteLed < N_STRAND1; whiteLed = whiteLed + 1) {
    leds[whiteLed] = CRGB::Black;
  }
  
  for(int whiteLed = 0; whiteLed < N_STRAND1; whiteLed = whiteLed + 1) {
    leds[whiteLed] = CRGB::White;
    FastLED.show();
    delay(3);
  }

  for(int whiteLed = 0; whiteLed < N_STRAND1; whiteLed = whiteLed + 1) {
    leds[whiteLed] = CRGB::Black;
    FastLED.show();
    delay(3);
  }
}
