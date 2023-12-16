#pragma once

// set via compilation options (/DDEBUG)
// #define DEBUG

#define INTRO_DURATION 5 // seconds

#ifdef DEBUG
#define XRES 640
#define YRES 480
#else
#define XRES 1280
#define YRES 720
#endif

#define MUSIC_BPM (60*20/INTRO_DURATION)