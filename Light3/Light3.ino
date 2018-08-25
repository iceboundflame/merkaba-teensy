
#include "FastLED.h"

#define DATA_STRAND1 5   // strap
#define DATA_STRAND2 6   // strap
#define DATA_STRAND3 21  // back
#define DATA_STRAND4 8   // top/sides
#define LED_PIN 13

#define N_STRAND1 59
#define N_STRAND2 60
#define N_STRAND3 41
#define N_STRAND4 102
#define N_ALL N_STRAND1 + N_STRAND2 + N_STRAND3 + N_STRAND4

CRGB leds[N_ALL + 1];
CRGB* ledsStrand1 = leds + 1;  // skip dead pixel
CRGB* ledsStrand2 = &ledsStrand1[N_STRAND1];
CRGB* ledsStrand3 = &ledsStrand2[N_STRAND2];
CRGB* ledsStrand4 = &ledsStrand3[N_STRAND3];

void setup() {
  Serial.begin(115200);
  Serial.println("Pyramid starting");
  
	// sanity check delay - allows reprogramming if accidently blowing power w/leds
  delay(2000);

  FastLED.addLeds<WS2812SERIAL, DATA_STRAND1, GRB>(ledsStrand1 - 1, N_STRAND1 + 1);
  FastLED.addLeds<WS2812SERIAL, DATA_STRAND2, GRB>(ledsStrand2, N_STRAND2);
  FastLED.addLeds<WS2812SERIAL, DATA_STRAND3, GRB>(ledsStrand3, N_STRAND3);
  FastLED.addLeds<WS2812SERIAL, DATA_STRAND4, GRB>(ledsStrand4, N_STRAND4);
  FastLED.setMaxPowerInVoltsAndMilliamps(5, 3000);

  fill_solid(leds, N_ALL, CRGB::Red);

  pinMode(LED_PIN, OUTPUT);
}

void loop() {
  static bool x = false;
  digitalWrite(LED_PIN, x ? LOW : HIGH);
  x = !x;

  for(int whiteLed = 0; whiteLed < N_ALL; whiteLed = whiteLed + 1) {
    leds[whiteLed] = CRGB::Black;
  }
  
  for(int whiteLed = 0; whiteLed < N_ALL; whiteLed = whiteLed + 1) {
    leds[whiteLed] = CRGB::White;
    FastLED.show();
    delay(3);
  }

  for(int whiteLed = 0; whiteLed < N_ALL; whiteLed = whiteLed + 1) {
    leds[whiteLed] = CRGB::Black;
    FastLED.show();
    delay(3);
  }
}
