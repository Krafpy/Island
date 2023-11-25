#include "synth.h"

#define _USE_MATH_DEFINES
#include <math.h>

// All modern x86-64 CPUs *should* support FCMOVcc instructions
#define SUPPORTS_FCMOVCC

// Creates an enveloppe with attack, sustain and release durations.
// Output in range [0, 1].
float envelope(float t, float a, float s, float r) {
    // float t1 = t/a;
    // float t2 = 1.f - (t-a-s)/r;
    // t = t1 < t2 ? t1 : t2;
    // t = t > 1.f ? 1.f : t;
    // t = t < 0.f ? 0.f : t;
    // return t;

    #ifdef SUPPORTS_FCMOVCC
    __asm {
        fld dword ptr [t]   // t
        fdiv dword ptr [a]  // t/a

        fld1                // 1, t/a
        fld dword ptr [t]   // t, 1, t/a
        fsub dword ptr [a]  // t-a, 1, t/a
        fsub dword ptr [s]  // t-a-s, 1, t/a
        fdiv dword ptr [r]  // (t-a-s)/r, 1, t/a
        fsubp st(1), st     // t2 = 1-(t-a-s)/r, t1 = t/a

        fcomi st, st(1)
        fcmovnb st, st(1)   // x = min(t1, t2), t1
        fstp st(1)          // x
        fld1                // 1, x
        fcomi st, st(1)
        fcmovnb st, st(1)   // min(1, x), x
        fstp st(1)          // min(1, x)
        fldz                // 0, min(1, x)
        fcomi st, st(1)
        fcmovb st, st(1)    // max(0, min(1, x)), min(1, x)
        fstp st(1)          // max(0, min(1, x))
    };
    #else
    // compute clamp(min(t/a, 1-(t-a-s)/r), 0, 1)
    // knowing that:
    // clamp(x, a, b) = max(a, min(b, x))
    // min(a, b) = (a + b - |a - b|) / 2
    // max(a, b) = (a + b + |a - b|) / 2
    // this is somehow more compressible than the
    // above comparisons
    __asm {
        fld1                // 1
        fadd st, st         // 2

        fld dword ptr [t]   // t, 2
        fdiv dword ptr [a]  // t/a, 2

        fld1                // 1, t/a, 2
        fld dword ptr [t]   // t, 1, t/a, 2
        fsub dword ptr [a]  // t-a, 1, t/a, 2
        fsub dword ptr [s]  // t-a-s, 1, t/a, 2
        fdiv dword ptr [r]  // (t-a-s)/r, 1, t/a, 2
        fsubp st(1), st     // 1-(t-a-s)/r, t/a, 2
        fld st(1)           // t/a, 1-(t-a-s)/r, t/a, 2
        fsub st, st(1)      // t/a - (1-(t-a-s)/r), 1-(t-a-s)/r, t/a, 2
        fabs                // |t/a - (1-(t-a-s)/r)|, 1-(t-a-s)/r, t/a, 2
        fsubp st(1), st     // 1-(t-a-s)/r - |t/a - (1-(t-a-s)/r)|, t/a, 2
        faddp st(1), st     // t/a + 1-(t-a-s)/r - |t/a - (1-(t-a-s)/r)|, 2
        fdiv st, st(1)      // (t/a + 1-(t-a-s)/r - |t/a - (1-(t-a-s)/r)|) / 2, 2
                            // x = min(t/a, 1-(t-a-s)/r), 2
        fld1                // 1, x, 2
        fld st(1)           // x, 1, x, 2
        fsub st, st(1)      // x-1, 1, x, 2
        fabs                // |x-1|, 1, x, 2
        fsubp st(1), st     // 1-|x-1|, x, 2
        faddp st(1), st     // 1+x-|x-1|, 2
        fld st              // 1+x-|x-1|, 1+x-|x-1|, 2
        fabs                // |1+x-|x-1||, 1+x-|x-1|, 2
        faddp st(1), st     // 1+x-|x-1| + |1+x-|x-1||, 2
        fdiv st, st(1)      // (1+x-|x-1| + |1+x-|x-1||)/2, 2
        fdivrp st(1), st    // (1+x-|x-1| + |1+x-|x-1||)/4
                            // max(0, min(1, x))
    };
    #endif
}

// Triangle wave oscillator.
float osc_tri(float t) {
    // return 1.f - 2.f*fabs(fmodf(t/M_PI, 2.f) - 1.f);
    __asm {
        fld1                // 1
        fld1                // 1, 1
        fadd st, st         // 2, 1
        fld dword ptr [t]   // t, 2, 1
        fldpi               // pi, t, 2, 1
        fdivp st(1), st     // t/pi, 2, 1
        fprem               // mod(t/pi,2), 2, 1
        fsub st, st(2)      // mod(t/pi,2)-1, 2, 1
        fabs                // |mod(t/pi,2)-1|, 2, 1
        fmulp st(1), st     // 2|mod(t/pi,2)-1|, 1
        fsubp st(1), st     // 1-2|mod(t/pi,2)-1|
    };
}

// Saw tooth wave oscillator.
float osc_saw(float t) {
    __asm {
        fld1                // 1
        fadd st, st         // 2
        fld dword ptr [t]   // t, 2
        fldpi               // pi, t, 2
        fdivp st(1), st     // t/pi, 2
        fprem               // mod(t/pi,2), 2
        fstp st(1)          // mod(t/pi,2)
        fld1                // 1, mod(t/pi,2)
        fsubp st(1), st     // mod(t/pi,2)-1
    };
}

// Square wave oscillator
float osc_sqr(float t) {
    float y = sinf(t);
    int i = *(int*)&y;
    // int s = (i >> 31)*2 - 1;
    int s = ((i >> 30) & 2) - 1;
    return (float)s;
}

// Computes a phase that switches from frequency f0 to f0 + df
// over the duration d, and maintains the final frequency.
float pha_lerp(float t, float f0, float df, float d) {
    float m = t < d ? t : d;
    #ifdef FREQ_AS_PULSE
    return f0*t + df*(m*m/(2.f*d) + t - m);
    #else
    return TWO_PI*f0*t + TWO_PI*df*(m*m/(2.f*d) + t - m);
    #endif
}

// Returns the frequency of the note played at the given
// time given notes the sequences of l notes, each of
// duration d.
float sequence(float t, char* notes, int l, float d) {
    int i = ((int)(t/d)) % l;
    float n = (float)notes[i];
    #ifdef FREQ_AS_PULSE
    return PULSE_C0 * powf(NEXT_NOTE, n);
    #else
    return FREQ_C0 * powf(NEXT_NOTE, n);
    #endif
}
