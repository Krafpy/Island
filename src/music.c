#include "music.h"
#include "config.h"
#include "synth.h"
#include "fp.h"
#include <math.h>

#define N_NOTES(notes) (sizeof(notes)/sizeof(char))

#define MUSIC_BPM (60*20/INTRO_DURATION)
#define BEAT_DURATION (60.f/(float)MUSIC_BPM)
#define MAX_AMPLITUDE (32767.0f/4.f) // must be less or equal than 32767

static char notes[] = {C(3), G(3), F(3), G(3)};

void music_init(short* buffer) {
    // Generate the music's audio signal
    for(unsigned int i = 0; i < NUM_SAMPLES; i++) {
        float t = (float)i/(float)SAMPLE_RATE;
        float nt = fmodf(t, BEAT_DURATION);

        float fc = sequence(t, notes, N_NOTES(notes), BEAT_DURATION);
        float sig = tri(fc*t);
        sig *= envelope(nt, 0.1f*BEAT_DURATION, 0.1f*BEAT_DURATION, 0.5f*BEAT_DURATION);

        short amp = (short)(sig*MAX_AMPLITUDE);
        buffer[2*i+0] = amp; // left channel
        buffer[2*i+1] = amp; // right channel
    }
}