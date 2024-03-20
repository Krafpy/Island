#include "music.h"
#include "config.h"
#include "synth.h"
#include "fp.h"
#include <math.h>

#define MUSIC_BPM (60*20/INTRO_DURATION)
#define BEAT_DURATION (60.f/(float)MUSIC_BPM)
#define MAX_AMPLITUDE (32767.0f/4.f) // must be less or equal than 32767

static char notes[] = {C(3), G(3), F(3), G(3)};

#define N_NOTES (sizeof(notes)/sizeof(char))

void music_init(short* buffer) {
    // Generate the music's audio signal
    for(unsigned int i = 0; i < NUM_SAMPLES; i++) {
        float t = (float)i/(float)SAMPLE_RATE;
        float u = t/BEAT_DURATION;

        char n = notes[(int)(t/BEAT_DURATION) % N_NOTES];
        float f = freq(n);
        float v = fmodf(u, 1.f);

        float out = tri(f*t);
        out *= envelope(v, 0.1f, 0.1f, 0.5f);

        short amp = (short)(out*MAX_AMPLITUDE);
        buffer[2*i+0] = amp; // left channel
        buffer[2*i+1] = amp; // right channel
    }
}