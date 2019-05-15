///* Mono Peak Meter
//
//   Scrolling peak audio level meter in the Arduino Serial Monitor
//
//   Audio input needs to connect to pin 16 (A2).  The signal range is 0 to 1.2V.
//   See the documentation in the Audio System Design Tool for the recommended
//   circuit to connect an analog signal.
//
//   This example code is in the public domain
//*/
//
//#include <Audio.h>
//#include <Wire.h>
//#include <SPI.h>
//#include <SD.h>
//#include <SerialFlash.h>
//
//// GUItool: begin automatically generated code
//AudioInputAnalog         adc1(A9);           //xy=164,95
//AudioAnalyzePeak         peak1;          //xy=317,123
//AudioConnection          patchCord1(adc1, peak1);
//// GUItool: end automatically generated code
//
////void setup() {
////  AudioMemory(4);
////  Serial.begin(9600);
////}
////
////// for best effect make your terminal/monitor a minimum of 31 chars wide and as high as you can.
////
////elapsedMillis fps;
////
////void loop() {
////  if (fps > 24) {
////    if (peak1.available()) {
////      fps = 0;
////      int monoPeak = peak1.read() * 30.0;
////      Serial.print("|");
////      for (int cnt=0; cnt<monoPeak; cnt++) {
////        Serial.print(">");
////      }
////      Serial.println();
////    }
////  }
////}
//

// FastLED Cylon Example, using Non-Blocking WS2812Serial

#include <WS2812Serial.h>
#define USE_WS2812SERIAL
#include <FastLED.h>

// How many leds in your strip?
/* #define NUM_LEDS 180 */
#define NUM_LEDS 180

#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

// GUItool: begin automatically generated code
AudioInputAnalog         adc1(A9);           //xy=173,294
AudioAnalyzePeak         peak1;          //xy=313,291
AudioConnection          patchCord1(adc1, peak1);
// GUItool: end automatically generated code


// Usable pins:
//   Teensy LC:   1, 4, 5, 24
//   Teensy 3.2:  1, 5, 8, 10, 31   (overclock to 120 MHz for pin 8)
//   Teensy 3.5:  1, 5, 8, 10, 26, 32, 33, 48
//   Teensy 3.6:  1, 5, 8, 10, 26, 32, 33
#define DATA_PIN_1 1
#define DATA_PIN_2 5
#define DATA_PIN_3 8

// Define the array of leds
CRGB leds[NUM_LEDS];
CRGB leds2[NUM_LEDS];
CRGB leds3[NUM_LEDS];

void setup() {
  AudioMemory(4);

  Serial.begin(57600);
  Serial.println("resetting");
  LEDS.addLeds<WS2812SERIAL,1,RGB>(leds,NUM_LEDS);
  LEDS.addLeds<WS2812SERIAL,8,RGB>(leds2,NUM_LEDS);
  LEDS.addLeds<WS2812SERIAL,10,RGB>(leds3,NUM_LEDS);
  LEDS.setBrightness(84);
  /* pinMode(1, OUTPUT); */
  /* pinMode(5, OUTPUT); */
  /* pinMode(8, OUTPUT); */
  pinMode(13, OUTPUT);
}

void fadeall() { for(int i = 0; i < NUM_LEDS; i++) {
    leds[i].nscale8(250);
    leds2[i].nscale8(250);
    leds3[i].nscale8(250);
} }

void loop() {
/*   digitalWrite(1, HIGH); */
/*   digitalWrite(5, HIGH); */
/*   digitalWrite(8, HIGH); */
  digitalWrite(13, HIGH);
/*   delay(1000); */
/*   digitalWrite(1, LOW); */
/*   digitalWrite(5, LOW); */
/*   digitalWrite(8, LOW); */
/*   digitalWrite(13, LOW); */
/*   delay(1000); */
/*   return; */


  static uint8_t hue = 0;
  Serial.print("x");
  // First slide the led in one direction
  for(int i = 0; i < NUM_LEDS; i++) {
    // Set the i'th led to red
    leds[i] = leds2[i] = leds3[i] = CHSV(hue++, 255, 255);
    // Show the leds

    LEDS.setBrightness(255 * peak1.read());
    FastLED.show();
    // now that we've shown the leds, reset the i'th led to black
    // leds[i] = CRGB::Black;
    fadeall();
    // Wait a little bit before we loop around and do it again
    //delay(20);
  }
  Serial.print("x");

  digitalWrite(13, LOW);

  // Now go in the other direction.
  for(int i = (NUM_LEDS)-1; i >= 0; i--) {
    // Set the i'th led to red
    leds[i] = leds2[i] = leds3[i] = CHSV(hue++, 255, 255);
    // Show the leds

    LEDS.setBrightness(255 * peak1.read());
    FastLED.show();
    // now that we've shown the leds, reset the i'th led to black
    // leds[i] = CRGB::Black;
    fadeall();
    // Wait a little bit before we loop around and do it again
    //delay(20);
  }
}
