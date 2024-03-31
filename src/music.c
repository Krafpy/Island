#include "music.h"
#include "config.h"
#include "synth.h"
#include "fp.h"
#include <math.h>


#define MAX_AMPLITUDE (32767.0f/2.f) // must be less or equal than 32767

#define BASE_DURATION 2.5f

#define NUMARGS(...) (sizeof((char[]){__VA_ARGS__})/sizeof(char))

// dur_ratio: the duration of the note will be BASE_DURATION/dur_ratio
// num_repeat: how many times to repeat the sequence, 0 for inifinite loop
// delay: when to start playing the sequence, as a multiple of BASE_DURATION
#define SEQUENCE(delay,dur_ratio,num_repeat,env_id,wave_id,...) \
    NUMARGS(__VA_ARGS__),delay,dur_ratio,num_repeat,env_id,wave_id,__VA_ARGS__

#define NONE -127
#define HOLD -128

typedef struct {
    float a;
    float s;
    float r;
} env_t;

static env_t envs[] = {
    {p0d01, p0d98, p0d01},
    {p0d10, p0d80, p0d10},
    {p0d10, p0d30, p0d60},
    {p0d10, p0d10, p0d80}
};

static char sequences[] = {
    SEQUENCE(0, 1, 3, 0, 0, A(2), HOLD, B(2), G(2), A(2), HOLD, B(2), D(3)), // 0
    SEQUENCE(0, 1, 3, 0, 0, C(3), HOLD, D(3), B(2), C(3), HOLD, D(3), F(3)), // 1
    SEQUENCE(0, 1, 3, 0, 0, E(3), HOLD, G(3), D(3), E(3), HOLD, G(3), A(3)), // 2

    // fast drums
    SEQUENCE(8, 16, 4, 0, 1, // 3
        E(2), E(2), E(2), E(2),
        E(2), E(2), E(2), E(2),
        E(2), E(2), E(2), E(2),
        E(2), E(2), E(2), E(2),

        D(2), D(2), D(2), D(2),
        D(2), D(2), D(2), D(2),
        D(2), D(2), D(2), D(2),
        D(2), D(2), D(2), D(2),

        E(2), E(2), E(2), E(2),
        E(2), E(2), E(2), E(2),
        E(2), E(2), E(2), E(2),
        E(2), E(2), E(2), E(2),

        F(2), F(2), F(2), F(2),
        F(2), F(2), F(2), F(2),
        F(2), F(2), F(2), F(2),
        F(2), F(2), F(2), F(2)
    ),

    // higher pitched drums
    SEQUENCE(8, 16, 2, 2, 2, // 4
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
    SEQUENCE(8, 1, 5, 1, 4, C(3), G(2), A(2), F(2)), // 5
    SEQUENCE(0, 1, 5, 1, 4, G(3), G(3), A(3), A(3)), // 6
    SEQUENCE(0, 1, 5, 1, 4, C(4), B(3), C(4), C(4)), // 7
    SEQUENCE(0, 1, 5, 1, 4, E(4), D(4), E(4), F(4)), // 8

    SEQUENCE(0, 16, 0, 0, 5,
        C(2), C(2), C(2), C(2),
        C(2), C(2), C(2), C(2),
        C(2), C(2), C(2), C(2),
        C(2), C(2), C(2), C(2),

        G(1), G(1), G(1), G(1),
        G(1), G(1), G(1), G(1),
        G(1), G(1), G(1), G(1),
        G(1), G(1), G(1), G(1),

        A(1), A(1), A(1), A(1),
        A(1), A(1), A(1), A(1),
        A(1), A(1), A(1), A(1),
        A(1), A(1), A(1), A(1),

        F(1), F(1), F(1), F(1),
        F(1), F(1), F(1), F(1),
        F(1), F(1), F(1), F(1),
        F(1), F(1), F(1), F(1)
    ),

    SEQUENCE(8, 1, 3, 0, 3, A(0), E(0), F(0), D(0)),
    SEQUENCE(0, 1, 3, 0, 3, E(1), E(1), F(1), F(1)),
    SEQUENCE(0, 1, 3, 0, 3, A(1), G(1), A(1), A(1)),

    // another epic progression
    SEQUENCE(12, 1, 1, 1, 4, A(2), F(2), G(2), E(3),  D(3), F(2), A(3)), // 12
    SEQUENCE( 0, 1, 1, 1, 4, A(3), F(3), G(3), G(3),  A(3), C(4), A(4)), // 13
    SEQUENCE( 0, 1, 1, 1, 4, C(4), A(3), B(3), B(3),  D(4), F(4), C(4)), // 14
    SEQUENCE( 0, 1, 1, 1, 4, E(4), C(4), D(4), E(4),  F(4), A(4), E(4)), // 15

    SEQUENCE( 0, 1, 1, 0, 3, F(1), D(1), E(1), C(2),  B(1), D(1), E(1)),
    SEQUENCE( 0, 1, 1, 0, 3, F(2), D(2), E(2), E(2),  F(2), A(2), F(2)),

    SEQUENCE(7, 1, 1, 3, 4, G(2), HOLD),
    SEQUENCE(0, 1, 1, 3, 4, B(3), HOLD),
    SEQUENCE(0, 1, 1, 3, 4, D(4), HOLD),
    SEQUENCE(0, 1, 1, 3, 4, G(4), HOLD),

    SEQUENCE(0, 1, 1, 3, 3, E(1), HOLD),
    SEQUENCE(0, 1, 1, 3, 3, G(2), HOLD),
};

#define SEQS_SIZE sizeof(sequences)/sizeof(char)


typedef struct {
    char fm_instr;
    float fm_iom;
    float fm_iom_sin;
    char fm_offset;
    float env_scale;
    float imp;
} wave_t;

#define INSTR_SIN 0
#define INSTR_TRI 1
#define INSTR_SQR 2
// #define INSTR_SAW 2

static wave_t waves[] = {
    {.fm_instr = INSTR_SIN, .fm_iom = 1.f, .fm_iom_sin = p0d50, .fm_offset = -1, .env_scale = p0d05, .imp = 0.f},
    {.fm_instr = INSTR_SQR, .fm_iom = p0d20, .fm_iom_sin = 0.f, .fm_offset = 0, .env_scale = p0d05, .imp = p0d10},
    {.fm_instr = INSTR_SQR, .fm_iom = p0d03, .fm_iom_sin = 0.f, .fm_offset = 0, .env_scale = p0d20, .imp = p0d50},
    {.fm_instr = INSTR_TRI, .fm_iom = 5.f, .fm_iom_sin = 0.f, .fm_offset = 0, .env_scale = p0d05, .imp = 0.f},
    {.fm_instr = INSTR_TRI, .fm_iom = p0d20, .fm_iom_sin = p0d50, .fm_offset = -1, .env_scale = p0d05, .imp = 0.f},
    {.fm_instr = INSTR_SQR, .fm_iom = p0d10, .fm_iom_sin = 0.f, .fm_offset = 0, .env_scale = p0d05, .imp = p0d10},
};

float impulse(float x, float k) {
    float h = x/k;
    return h*expf(1.f-h);
}


inline float wave(float t) {
    float out = 0.f;
    
    float t0 = t;
    int id = 0;
    for(char* seq = sequences; seq < sequences + SEQS_SIZE; seq += 6 + *seq, id++) {
        
        int num_notes  = (int)seq[0];
        float delay =  (float)seq[1];
        int dur_ratio  = (int)seq[2];
        int num_repeat = (int)seq[3];
        int env_id =     (int)seq[4];
        int wave_id =    (int)seq[5];
        char* notes = &seq[6];
        
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

        env_t e = envs[env_id];
        wave_t w = waves[wave_id];

        float env = envelope(v, e.a, e.s + h, e.r);
        env *= w.env_scale;
        if(w.imp > 0.f) {
            env *= impulse(v, w.imp);
        }

        float fst = (f + 1.6f * (float)w.fm_offset) * t;
        float fm_sig;
        switch(w.fm_instr){
        case INSTR_SIN: fm_sig = sinf(fst); break;
        case INSTR_TRI: fm_sig = tri(fst); break;
        case INSTR_SQR: fm_sig = sqr(fst); break;
        // case INSTR_SAW: fm_sig = saw(fst); break;
        }

        out += sinf(f*t + w.fm_iom*(1.f + w.fm_iom_sin*sinf(2.f*t))*fm_sig) * env;
    }

    out *= envelope(t0, p0d01, 133.f, 2.f);

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