#pragma once

#include <windows.h>

void init_capture(HWND hwnd);
void save_frame(int frame);
void save_audio(short* buffer, int bytes, HWND hwnd);