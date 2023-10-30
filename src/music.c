#include "music.h"
#include "config.h"
#include "synth.h"
#include "fp.h"
#include <math.h>

#define NUM_SAMPLES (MUSIC_DURATION * SAMPLE_RATE)
#define MAX_AMPLITUDE (32767.0f/4.f) // must be less or equal than 32767

#define N_NOTES(notes) (sizeof(notes)/sizeof(char))

static char notes[] = {
    C(3), G(3), F(3), G(3)
};

void music_init(WAVFile* music) {
    short* buffer = (short*)music->buffer;
    // Generate the music's audio signal
    for(unsigned int i = 0; i < NUM_SAMPLES; i++) {
        float t = (float)i/(float)SAMPLE_RATE;
        
        float nd = ((float)MUSIC_DURATION)/(4.f*(float)N_NOTES(notes));
        float tn = fmodf(t, nd);

        float fc = sequence(t, notes, N_NOTES(notes), nd);
        sig_t s = osc_tri(fc*t);
        s *= envelope(tn, 0.1f*nd, 0.1f*nd, 0.8f*nd);

        buffer[2*i+0] = (short)(s*MAX_AMPLITUDE); // left channel
        buffer[2*i+1] = (short)(s*MAX_AMPLITUDE); // right channel
    }
}