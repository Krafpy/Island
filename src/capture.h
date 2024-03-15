#pragma once

#include <windows.h>

void init_capture(HWND hwnd);
void save_frame(int frameNumber);
void save_audio(short* waveBuffer);