#include "music.h"
#include "config.h"
#include "synth.h"
#include "fp.h"
#include <math.h>
#include <memory.h>

#define NUM_SAMPLES (MUSIC_DURATION * SAMPLE_RATE)
#define MAX_AMPLITUDE (32767.0f/4.f) // must be less or equal than 32767

static WAVHeader header = {
    .riffHeader = 0x46464952, // little-endian "RIFF"
    .wavSize = sizeof(WAVHeader) + DATA_BYTES - sizeof(char[8]),
    .waveHeader = 0x45564157, // little-endian "WAVE"

    .fmtHeader = 0x20746D66, // little-endian "fmt "
    .fmtChunkSize = 16,
    .audioFormat = 1,
    .numChannels = NUM_CHANNELS,
    .sampleRate = SAMPLE_RATE,
    .byteRate = BYTE_RATE,
    .sampleAlignment = SAMPLE_ALIGNMENT,
    .bitDepth = BIT_DEPTH,
    
    .dataHeader = 0x61746164, // little-endian "data"
    .dataBytes = DATA_BYTES
};

// Force the complete wav file to be stored in .bss
static WAVFile music;


#define N_NOTES(notes) (sizeof(notes)/sizeof(char))

static char notes[] = {
    C(3), G(3), F(3), G(3)
};

WAVFile* music_init() {
    memcpy(&music, &header, sizeof(WAVHeader));

    short* buffer = (short*)music.buffer;
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

    return &music;
}