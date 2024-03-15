#include <windows.h>
#include <GL/gl.h>
#include "glext.h"
#include "config.h"
#include "stb_image_write.h"

#define _STRIDE (3*XRES)
#define STRIDE (_STRIDE + ((_STRIDE % 4) ? (4 - _STRIDE % 4) : 0))
#define BUF_SIZE (STRIDE*YRES)

static unsigned char* pixels[BUF_SIZE];

void init_capture(HWND hwnd) {
    CreateDirectory("capture", NULL);
}

void save_frame(int frameNumber) {
    glReadBuffer(GL_FRONT);
    glPixelStorei(GL_PACK_ALIGNMENT, 4);
    glReadPixels(0, 0, XRES, YRES, GL_RGB, GL_UNSIGNED_BYTE, pixels);

    char fileName[1024];
    wsprintf(fileName, ".\\capture\\frame_%06d.png", frameNumber);
    stbi_flip_vertically_on_write(1);
    stbi_write_png(fileName, XRES, YRES, 3, pixels, STRIDE);
}

void save_audio(short* waveBuffer) {
    // TODO
}
