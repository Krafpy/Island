#pragma once

// if defined, supposes all frequencies are premultiplied
// by 2pi. This reduces the size of the code, a bit.
#define FREQ_AS_PULSE

#define _OCT(oct) (12*(oct))

#define C(oct)  ((char)(_OCT(oct) + 0))
#define Cd(oct) ((char)(_OCT(oct) + 1))
#define D(oct)  ((char)(_OCT(oct) + 2))
#define Dd(oct) ((char)(_OCT(oct) + 3))
#define E(oct)  ((char)(_OCT(oct) + 4))
#define F(oct)  ((char)(_OCT(oct) + 5))
#define Fd(oct) ((char)(_OCT(oct) + 6))
#define G(oct)  ((char)(_OCT(oct) + 7))
#define Gd(oct) ((char)(_OCT(oct) + 8))
#define A(oct)  ((char)(_OCT(oct) + 9))
#define Ad(oct) ((char)(_OCT(oct) + 10))
#define B(oct)  ((char)(_OCT(oct) + 11))


#define TWO_PI 6.28318530718f
#define NEXT_NOTE 1.05946309436f // 2^(1/12)
#define FREQ_C0 32.7f
#define PULSE_C0 (TWO_PI*FREQ_C0)


// types to make it clearer what type of signal the
// following functions return and expect.
typedef float sig_t;
typedef float pha_t;

float envelope(float t, float a, float s, float r);
sig_t osc_tri(pha_t t);
sig_t osc_saw(pha_t t);
sig_t osc_sqr(pha_t t);
pha_t pha_lerp(float t, float f0, float df, float d);
float sequence(float t, char* notes, int l, float d);