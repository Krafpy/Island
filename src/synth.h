#pragma once

// if defined, supposes all frequencies are premultiplied
// by 2pi. This reduces the size of the code, a bit.
#define FREQ_AS_PULSE

#define C(oct)  ((char)(12*(oct) + 0))
#define Cd(oct) ((char)(12*(oct) + 1))
#define D(oct)  ((char)(12*(oct) + 2))
#define Dd(oct) ((char)(12*(oct) + 3))
#define E(oct)  ((char)(12*(oct) + 4))
#define F(oct)  ((char)(12*(oct) + 5))
#define Fd(oct) ((char)(12*(oct) + 6))
#define G(oct)  ((char)(12*(oct) + 7))
#define Gd(oct) ((char)(12*(oct) + 8))
#define A(oct)  ((char)(12*(oct) + 9))
#define Ad(oct) ((char)(12*(oct) + 10))
#define B(oct)  ((char)(12*(oct) + 11))


#define TWO_PI 6.28318530718f
#define NEXT_NOTE 1.05946309436f // 2^(1/12)
#define FREQ_C0 32.7f
#define PULSE_C0 (TWO_PI*FREQ_C0)


float envelope(float t, float a, float s, float r);
float tri(float t);
float saw(float t);
float sqr(float t);
float plerp(float t, float f0, float df, float d);
char note(float t, char* notes, int l, float d);
float freq(char n);
float fseq(float t, char* notes, int l, float d);