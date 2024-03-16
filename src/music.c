#include "music.h"
#include "config.h"
#include "synth.h"
#include "fp.h"
#include <math.h>

#define N_NOTES(notes) (sizeof(notes)/sizeof(char))

#define BPM 10
#define BDUR 5.f //(60.f/(float)BPM)
#define MAX_AMPLITUDE (32767.0f/4.f) // must be less or equal than 32767

static char notes[] = {C(2), G(2), F(2), G(2)};

static float music_wave(float t) {
    float sig = 0.f;

    float fc = sequence(t, notes, N_NOTES(notes), BDUR);
    sig += 0.5*osc_tri(fc*t + 1.f*sinf((fc-1.f)*t));
    sig += sinf((fc+2.f)*t + 1.5f*sinf((fc+1.f)*t));
    sig *= 0.25f;

    float nt = fmodf(t, BDUR);
    sig *= envelope(nt, 0.1f*BDUR, 0.85f*BDUR, 0.05f*BDUR);

    return sig;
}

#define N_NOTES (sizeof(notes)/sizeof(char))

void music_init(short* buffer) {
    // Generate the music's audio signal
    for(unsigned int i = 0; i < NUM_SAMPLES; i++) {
        float t = (float)i/(float)SAMPLE_RATE;
        
        float sig = music_wave(t);
        float rev = music_wave(t-0.5f);
        sig += 0.25f*rev;
        sig *= 0.3f;

        float out = tri(f*t);
        out *= envelope(v, 0.1f, 0.1f, 0.5f);

        short amp = (short)(out*MAX_AMPLITUDE);
        buffer[2*i+0] = amp; // left channel
        buffer[2*i+1] = amp; // right channel
    }
}