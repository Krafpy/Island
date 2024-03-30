#include "music.h"
#include "config.h"
#include "synth.h"
#include "fp.h"
#include <math.h>


#define MAX_AMPLITUDE (32767.0f/4.f) // must be less or equal than 32767

#define BASE_DURATION 2.5f

#define NUMARGS(...) (sizeof((char[]){__VA_ARGS__})/sizeof(char))

// dur_ratio: the duration of the note will be BASE_DURATION/dur_ratio
// num_repeat: how many times to repeat the sequence, 0 for inifinite loop
// delay: when to start playing the sequence, as a multiple of BASE_DURATION
#define SEQUENCE(delay,dur_ratio,num_repeat,env_id,...) \
    NUMARGS(__VA_ARGS__),delay,dur_ratio,num_repeat,env_id,__VA_ARGS__

#define NONE -1
#define HOLD -2

typedef struct {
    float a;
    float s;
    float r;
} env_t;

static env_t envs[] = {
    {0.01f, 0.98f, 0.01f}, {0.1f, 0.8f, 0.1f}, {0.1f, 0.3f, 0.6f}
};

static char sequences[] = {
    SEQUENCE(0, 1, 4, 0, A(2), HOLD, B(2), G(2), A(2), HOLD, B(2), D(3)), // 0
    SEQUENCE(0, 1, 4, 0, C(3), HOLD, D(3), B(2), C(3), HOLD, D(3), F(3)), // 1
    SEQUENCE(0, 1, 4, 0, E(3), HOLD, G(3), D(3), E(3), HOLD, G(3), A(3)), // 2

    // fast drums
    SEQUENCE(8, 16, 0, 0, // 3
        E(2), E(2), E(2), E(2),
        E(2), E(2), E(2), E(2),

        D(2), D(2), D(2), D(2),
        D(2), D(2), D(2), D(2),

        E(2), E(2), E(2), E(2),
        E(2), E(2), E(2), E(2),

        D(2), D(2), D(2), D(2),
        D(2), D(2), D(2), D(2),

        F(2), F(2), F(2), F(2),
        F(2), F(2), F(2), F(2),
        F(2), F(2), F(2), F(2),
        F(2), F(2), F(2), F(2)
    ),

    // higher pitched drums
    SEQUENCE(8, 16, 3, 2, // 4
        E(3), D(3), C(3), A(2),
        E(3), D(3), C(3), A(2),
        E(3), D(3), C(3), A(2),
        E(3), D(3), C(3), A(2),

        G(3), F(3), D(3), C(3),
        G(3), F(3), D(3), C(3),
        G(3), F(3), D(3), C(3),
        G(3), F(3), D(3), C(3),

        A(3), G(3), D(3), B(2),
        A(3), G(3), D(3), B(2),
        A(3), G(3), D(3), B(2),
        A(3), G(3), D(3), B(2),

        G(3), F(3), D(3), C(3),
        G(3), F(3), D(3), C(3),
        G(3), F(3), D(3), C(3),
        G(3), F(3), D(3), C(3)
    ),

    // epic progression
    SEQUENCE(8, 1, 5, 1, C(3), G(2), A(2), F(2)), // 5
    SEQUENCE(0, 1, 5, 1, G(3), G(3), A(3), A(3)), // 6
    SEQUENCE(0, 1, 5, 1, C(4), B(3), C(4), C(4)), // 7
    SEQUENCE(0, 1, 5, 1, E(4), D(4), E(4), F(4)), // 8

    // noisy stuff?
    // 9
    SEQUENCE(8, 8, 10, 0, A(2), HOLD, HOLD, HOLD, B(2), HOLD, G(2), HOLD, A(2), HOLD, HOLD, HOLD, B(2), HOLD, D(3), B(2)),
    // 10
    SEQUENCE(0, 8, 10, 0, C(3), HOLD, HOLD, HOLD, D(3), HOLD, B(2), HOLD, C(3), HOLD, HOLD, HOLD, D(3), HOLD, F(3), D(3)),
    // 11
    SEQUENCE(0, 8, 10, 0, E(3), HOLD, HOLD, HOLD, G(3), HOLD, D(3), HOLD, E(3), HOLD, HOLD, HOLD, G(3), HOLD, A(3), G(3)),

    // anoter epic progression
    SEQUENCE(12, 1, 1, 1, A(3), F(2), G(2), E(3),  D(3), F(2), A(3), G(2), HOLD), // 12
    SEQUENCE( 0, 1, 1, 1, A(4), F(3), G(3), G(3),  A(3), C(4), A(4), B(3), HOLD), // 13
    SEQUENCE( 0, 1, 1, 1, C(4), A(3), B(3), B(3),  D(4), F(4), C(4), D(4), HOLD), // 14
    SEQUENCE( 0, 1, 1, 1, E(4), C(4), D(4), E(4),  F(4), A(4), E(4), G(4), HOLD), // 15
};

#define SEQS_SIZE sizeof(sequences)/sizeof(char)


float impulse(float x, float k) {
    float h = x/k;
    return h*expf(1.f-h);
}

float sat(float x) {
    return x < 0.f ? 0.f : x > 1.f ? 1.f : x;
}


inline float wave(float t) {
    float out = 0.f;

    // t += 50.f;
    
    float t0 = t;
    int id = 0;
    for(char* seq = sequences; seq < sequences + SEQS_SIZE; seq += 5 + *seq, id++) {

        int num_notes  = (int)seq[0];
        float delay =  (float)seq[1];
        int dur_ratio  = (int)seq[2];
        int num_repeat = (int)seq[3];
        int env_id =     (int)seq[4];
        char* notes = &seq[5];
        
        t -= delay * BASE_DURATION;
        // ignore note if sequence has not started yet
        if(t < 0.f) {
            continue;
        }

        float d = BASE_DURATION / (float)dur_ratio;
        float u = t/d;
        int i = ((int)u) % num_notes;

        // ignore note if outside of repetition range, or
        // no note is played
        if(
            (num_repeat > 0
                && (int)(u/(float)num_notes) >= num_repeat)
            || notes[i] == NONE
        ) {
            continue;
        }

        int j = i;
        while(notes[j] == HOLD) {
            j--;
        }
        char n = notes[j];
        float f = freq(n);
        float v = fmodf(u, 1.f) + (float)(i - j);

        int h = 0;
        j++;
        while(j < num_notes && notes[j] == HOLD){
            h++;
            j++;
        }

        // #define env(a,s,r) envelope(v, a, s + h, r)
        env_t e = envs[env_id];
        float env = envelope(v, e.a, e.s + h, e.r);

        float alpha = 0.f;
        if(t0 >= 60.f && t0 <= 65.f) {
            alpha = (t0 - 60.f)/5.f;
        } else if(t0 > 65.f){
            alpha = 1.f;
        }
        // float beta = 0.f;
        // if(t0 >= 105.f && t0 <= 107.f) {
        //     beta = (t0 - 105.f) / 2.f;
        // } else if(t0 > 107.f) {
        //     beta = 1.f;
        // }

        switch(id) {
        case 0:
        case 1:
        case 2: {
            float e = env * 0.1f * (1.f - alpha);
            out += sinf(f*t + (4000.f/f)*(1.f + 0.5f*sinf(2.f*t))*sinf((f+1.6f)*t)) * e;
            break;
        }
        case 3: {
            float e = env * 0.2f * impulse(v, 0.1f);
            float o1 = sinf(f*t + 0.2f*saw(f*t)) * e;
            float o2 = sinf(f*t + 2.f*tri(f*t)) * e * 3.f;
            out += (1.f-alpha)*o1 + alpha*o2;
            break;
        }
        case 4: {
            float e = env * 0.3f * impulse(v, 0.5f) * (1.f - alpha);
            out += sinf(f*t + 0.1f*sqr(f*t)) * e;
            break;
        }
        case 5:
        case 6:
        case 7:
        case 8:
        
        case 12:
        case 13:
        case 14:
        case 15: {
            float e = env * 0.1f;
            out += sinf(f*t + (4000.f/f)*(1.f + 0.5f*sinf(2.f*t))*sinf((f+1.6f)*t)) * e;
            break;
        }
        case 9:
        case 10:
        case 11: {
            float e = env * 0.1f;
            float o = 0.f;
            if(t0 <= 90.f) {
            o = sinf(f*t + (500.f/f)*(1.f + 0.5f*sinf(2.f*t+4.f))*sinf((f+1.6f)*t)) * e * 2.f;
            } else {
            o = sinf(f*t + (400.f/f)*(1.f + 0.5f*sinf(2.f*t+4.f))*saw((f+1.6f)*t)) * e;
            }
            out += o;
            break;
        }
        }
    }

    if(t0 > 133.f) {
        out *= 1.f - ((t0 - 133.f) / ((float)INTRO_DURATION - 133.f));
    }

    out *= 0.5f; // amplitude regularization
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