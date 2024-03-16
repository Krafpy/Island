#include <windows.h>
#include <GL/gl.h>
#include "glext.h"
#include "config.h"
#include "stb_image_write.h"

static unsigned char pixels[3*XRES*YRES];

void init_capture(HWND hwnd) {
    CreateDirectory("capture", NULL);
}

void save_frame(int frame) {
    glReadBuffer(GL_FRONT);
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glReadPixels(0, 0, XRES, YRES, GL_RGB, GL_UNSIGNED_BYTE, pixels);
    
    char fileName[1024];
    wsprintf(fileName, ".\\capture\\frame_%06d.png", frame);
    stbi_flip_vertically_on_write(1);
    stbi_write_png(fileName, XRES, YRES, 3, pixels, 3*XRES);
}

void save_audio(short* buffer, int bytes, HWND hwnd) {
    HANDLE file = CreateFile(
        ".\\capture\\audio.raw",
        GENERIC_WRITE,
        0,
        NULL, 
        CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        NULL);
    
    if(file == INVALID_HANDLE_VALUE) {
        MessageBox(hwnd, "Failed to create audio file.", "Error", MB_OK);
        ExitProcess(-1);
        return;
    }

    DWORD nbWritten;
    if(!WriteFile(file, (LPCVOID)buffer, (DWORD)bytes, &nbWritten, NULL)) {
        MessageBox(hwnd, "Failed to write audio file.", "Error", MB_OK);
        ExitProcess(-1);
    }
}
