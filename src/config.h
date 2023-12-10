#pragma once

// set via compilation options (/DDEBUG)
// #define DEBUG

#define INTRO_DURATION 5 // seconds

#ifdef DEBUG
#define X_RES 640
#define Y_RES 480
#else
#define X_RES 1280
#define Y_RES 720
#endif

#define MUSIC_BPM (60*20/INTRO_DURATION)