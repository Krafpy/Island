#pragma once

#include <windows.h>
#include "config.h"

#define MUSIC_DURATION INTRO_DURATION // seconds
#define SAMPLE_RATE 44100 // 44.1 kHz (typical for audio)
#define NUM_CHANNELS 2    // Stereo
#define BIT_DEPTH 16      // 16 bits per sample (typical for audio)

#define BYTE_RATE (SAMPLE_RATE * NUM_CHANNELS * BIT_DEPTH / 8)
#define SAMPLE_ALIGNMENT (NUM_CHANNELS * BIT_DEPTH / 8)
#define DATA_BYTES (MUSIC_DURATION * SAMPLE_RATE * NUM_CHANNELS * BIT_DEPTH / 8)


typedef struct WAVHeader {
    // RIFF Header
    DWORD riffHeader;       // Contains "RIFF"
    DWORD wavSize;          // Size of the wav portion of the file
    DWORD waveHeader;       // Contains "WAVE"
    
    // Format Header
    DWORD fmtHeader;        // Contains "fmt " (includes trailing space)
    DWORD fmtChunkSize;     // Should be 16 for PCM
    WORD audioFormat;       // Should be 1 for PCM, 3 for IEEE Float
    WORD numChannels;       // 1 to 6
    DWORD sampleRate;       // in Hz
    DWORD byteRate;         // Number of bytes per second
    WORD sampleAlignment;   // numChannels * Bytes Per Sample
    WORD bitDepth;          // Number of bits per sample
    
    // Data
    DWORD dataHeader;       // Contains "data"
    DWORD dataBytes;        // Number of bytes in data
} WAVHeader;

typedef struct WAVFile {
    WAVHeader header;
    char buffer[DATA_BYTES]; // The actual audio data
} WAVFile;


void music_init(short* buffer);