#include "music.h"
#include "config.h"
#include "synth.h"
#include "fp.h"
#include <math.h>


#define MAX_AMPLITUDE (32767.0f/32.f) // must be less or equal than 32767

#define BASE_DURATION 5.f

#define NUMARGS(...) (sizeof((char[]){__VA_ARGS__})/sizeof(char))

// dur_ratio: the duration of the note will be BASE_DURATION/dur_ratio
// num_repeat: how many times to repeat the sequence, 0 for inifinite loop
// delay: when to start playing the sequence, as a multiple of BASE_DURATION
#define SEQUENCE(delay,dur_ratio,num_repeat,...) \
    NUMARGS(__VA_ARGS__),delay,dur_ratio,num_repeat,__VA_ARGS__

#define NONE -1 // special note indicating no note playing

static char sequences[] = {
    SEQUENCE(0, 2, 0, D(3), F(3), C(3), A(2)),
    SEQUENCE(0, 2, 1, F(3), A(3), E(3), C(3)),
    SEQUENCE(1, 20, 0,
        C(4), E(4), G(4), B(4),
        C(4), E(4), G(4), B(4),
        C(4), E(4), G(4), B(4),
        C(4), E(4), G(4), B(4),

        E(4), G(4), B(4), D(5),
        E(4), G(4), B(4), D(5),
        E(4), G(4), B(4), D(5),
        E(4), G(4), B(4), D(5),

        A(3), C(4), E(4), G(4),
        A(3), C(4), E(4), G(4),
        A(3), C(4), E(4), G(4),
        A(3), C(4), E(4), G(4)
    )
};

#define SEQS_SIZE sizeof(sequences)/sizeof(char)


inline float wave(float t) {
    float out = 0.f;

    int id = 0;
    for(char* seq = sequences; seq < sequences + SEQS_SIZE; seq += 4 + *seq, id++) {

        int num_notes  = (int)seq[0];
        float delay = ((float)seq[1]);
        int dur_ratio  = (int)seq[2];
        int num_repeat = (int)seq[3];
        char* notes = &seq[4];
        
        t -= delay * BASE_DURATION;
        // ignore note if sequence has not started yet
        if(t < 0.f) {
            continue;
        }

        float d = BASE_DURATION / (float)dur_ratio;
        float u = t/d;
        char n = notes[((int)u) % num_notes];

        // ignore note if outside of repetition range, or
        // no note is played, or before the start of the
        // sequence
        if(
            (num_repeat > 0
                && (int)(u/(float)num_notes) >= num_repeat)
            || n < 0
        ) {
            continue;
        }

        float f = freq(n);
        float v = fmodf(u, 1.f);
        
        switch(id) {
        case 0:
        case 1:
            out += sinf(f*t) * envelope(v, 0.1f, 0.8f, 0.1f);
            break;
        case 2:
            out += tri(f*t) * envelope(v, 0.1f, 0.8f, 0.1f);
            break;
        default:
            break;
        }
    }
    return out;
}

void music_init(short* buffer) {
    // Generate the music's audio signal
    for(unsigned int i = 0; i < NUM_SAMPLES; i++) {
        float t = (float)i/(float)SAMPLE_RATE;

        float out = wave(t);
        buffer[2*i+0] = (short)(out*MAX_AMPLITUDE);
        buffer[2*i+1] = (short)(out*MAX_AMPLITUDE);
    }
}