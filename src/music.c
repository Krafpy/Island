#include "music.h"
#include "config.h"
#include "synth.h"
#include "fp.h"
#include <math.h>

// #define N_NOTES(notes) (sizeof(notes)/sizeof(char))

#define BPM 10
#define BDUR 5.f //(60.f/(float)BPM)
#define MAX_AMPLITUDE (32767.0f/4.f) // must be less or equal than 32767


static char background[] = {D(3), F(3), C(3), A(2),   D(3), F(3), C(3), E(3)};

static char pad[] = {
    C(2), E(2), G(2), D(2), F(2), A(2)
};

static char drums[] = {
    C(4), E(4), G(4), D(4), F(4), A(4)
};

static char chord(char n){
    static int incr[] = {
     // 0, 1, 2, 3, 4, 5, 6,  7,  8,  9,  10,  11
        4, 4, 3, 3, 3, 4, 4,  4,  5,  3,  5,   3
    };
    return (char)(n + incr[n % 12]);
}

static void triad(char n, float* f) {
    for(int i = 0; i < 3; i++) {
        f[i] = freq(n); n = chord(n);
    }
}

static float FMsin(float t, float fc, float fm, float iom) {
    return sinf(fc*t + iom*sinf(fm*t));
}

static float FMtri(float t, float fc, float fm, float iom) {
    return tri(fc*t + iom*sinf(fm*t));
}

static void wave(float t, float* left, float* right) {
    float f[3];
    char n;
    float L = 0.f, R = 0.f;

    #define env(a,s,r,d) envelope(fmodf(t/d, 1.f),a,s,r)

    #define D1 2.5f

    n = note(t, background, sizeof(background), D1);
    triad(n, f);
    for(int i = 0; i < 3; i++){
        if(t < 20.f || t > 40.f) {
            L += FMsin(t, f[i], f[i]+1.f, 1.f);
            R += FMsin(t, f[i], f[i]-1.f, 1.f);
        } else {
            L += FMtri(t, f[i], f[i]+1.f, 1.f);
            R += FMtri(t, f[i], f[i]-1.f, 1.f);
        }
    }
    float e = env(0.3f,0.65f,0.05f,D1);
    L *= e; R *= e;

    if(t > 40.f) {
        L *= 0.5f;
        R *= 0.5f;

        #define D2 1.25f
        n = note(t, pad, sizeof(pad), D2);
        triad(n, f);
        float p = 0.f;
        p += 1.f*tri(f[0]*t);
        p += 0.5f*tri(f[1]*t);
        p += 0.5f*tri(f[2]*t);
        e = env(0.05f, 0.9f, 0.05f, D2);
        L += p * e;
        R += p * e;
    }

    /*if(t > 65.f) {
        #define D3 0.5f
        n = note(t, drums, sizeof(drums), D3);
        triad(n, f);
        float p = 0.f;
        p += 0.2f*saw(f[0]*t);
        p += 0.1f*saw(f[1]*t);
        p += 0.1f*saw(f[2]*t);
        e = env(0.01f, 0.1f, 0.3f, D3);
        L += p * e;
        R += p * e;
    }*/

    *left = L; *right = R;
}

#define N_NOTES (sizeof(notes)/sizeof(char))

void music_init(short* buffer) {
    // Generate the music's audio signal
    for(unsigned int i = 0; i < NUM_SAMPLES; i++) {
        float t = (float)i/(float)SAMPLE_RATE;

        float left, right;
        wave(t, &left, &right);
        left  *= 0.1f;
        right *= 0.1f;
        buffer[2*i+0] = (short)(left*MAX_AMPLITUDE );
        buffer[2*i+1] = (short)(right*MAX_AMPLITUDE);
        
        // float sig = 0.f;
        // sig += wave(t);
        // sig += 0.25f*wave(t-0.25f);
        // sig *= 0.1f;

        // short amp = (short)(sig*MAX_AMPLITUDE);
        // buffer[2*i+0] = amp; // left channel
        // buffer[2*i+1] = amp; // right channel
    }
}