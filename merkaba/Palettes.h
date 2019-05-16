#pragma once

#include <FastLED.h>

DEFINE_GRADIENT_PALETTE( fire_gp ) {
    0x00,     0,  0,  0,   //black
    0x55,   255,  0,  0,   //red
    0xAA,   255,255,  0,   //bright yellow
    0xFF,   255,255,255 }; //full white

// Gradient palette "bhw1_03_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/bhw/bhw1/tn/bhw1_03.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 16 bytes of program space.
DEFINE_GRADIENT_PALETTE( bhw1_03_gp ) {
    0,   0,  0,  0,
    0x55,  0x00,0x20,0x90,
    0xAA,  40,219,105,
    0xFF, 255,255,255};

// Gradient palette "es_emerald_dragon_11_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/es/emerald_dragon/tn/es_emerald_dragon_11.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 16 bytes of program space.
DEFINE_GRADIENT_PALETTE( es_emerald_dragon_11_gp ) {
    0,   1,  8,  1,
    170,  78,156,  2,
    220, 197,250, 78,
    255, 229,244,192};


// Gradient palette "bhw1_w00t_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/bhw/bhw1/tn/bhw1_w00t.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 16 bytes of program space.

DEFINE_GRADIENT_PALETTE( bhw1_w00t_gp ) {
    0,   3, 13, 43,
    104,  78,141,240,
    188, 255,  0,  0,
    255,  28,  1,  1};

// Gradient palette "bhw1_bluesteel_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/bhw/bhw1/tn/bhw1_bluesteel.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 28 bytes of program space.

DEFINE_GRADIENT_PALETTE( bhw1_bluesteel_gp ) {
    0,   1,  2,  1,
    51,  42, 55, 45,
    89, 144,178,170,
    130, 255,255,255,
    146, 194,215,210,
    175, 144,178,170,
    255,   1,  2,  1};

// Gradient palette "es_platinum_11_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/es/platinum/tn/es_platinum_11.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 8 bytes of program space.

DEFINE_GRADIENT_PALETTE( es_platinum_11_gp ) {
    0,   1,  1,  1,
    255, 227,241,255};


// Gradient palette "bhw1_07_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/bhw/bhw1/tn/bhw1_07.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 8 bytes of program space.

DEFINE_GRADIENT_PALETTE( bhw1_07_gp ) {
    0, 232, 65,  1,
    255, 229,227,  1};

// Gradient palette "bhw1_sunconure_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/bhw/bhw1/tn/bhw1_sunconure.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 16 bytes of program space.

DEFINE_GRADIENT_PALETTE( bhw1_sunconure_gp ) {
    0,  20,223, 13,
    160, 232, 65,  1,
    252, 232,  5,  1,
    255, 232,  5,  1};

// Gradient palette "bhw1_purplered_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/bhw/bhw1/tn/bhw1_purplered.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 8 bytes of program space.

DEFINE_GRADIENT_PALETTE( bhw1_purplered_gp ) {
    0, 255,  0,  0,
    255, 107,  1,205};

// Gradient palette "bhw1_sunset1_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/bhw/bhw1/tn/bhw1_sunset1.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 24 bytes of program space.

DEFINE_GRADIENT_PALETTE( bhw1_sunset1_gp ) {
    0,  33, 21, 25,
    38, 125, 29, 20,
    71, 222, 59, 30,
    145, 190,147,127,
    178,  88,136,203,
    255,   3, 24, 78};

static CRGBPalette16 gPalettes[] = {
    fire_gp,

    bhw1_03_gp,
    es_emerald_dragon_11_gp,

    bhw1_bluesteel_gp,
    bhw1_w00t_gp,
    es_platinum_11_gp,

    bhw1_07_gp,

    bhw1_sunconure_gp,
    bhw1_purplered_gp,
    bhw1_sunset1_gp,
};
static constexpr int gNumPalettes = sizeof(gPalettes) / sizeof(gPalettes[0]);
