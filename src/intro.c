#include <windows.h>
#include <GL/gl.h>
#include "glext.h" // contains type definitions for all modern OpenGL functions
#include "shaders.inl"
#include "config.h"

// Define the modern OpenGL functions to load from the driver

#define glCreateShaderProgramv ((PFNGLCREATESHADERPROGRAMVPROC)wglGetProcAddress("glCreateShaderProgramv"))
#define glUseProgram ((PFNGLUSEPROGRAMPROC)wglGetProcAddress("glUseProgram"))
#define glProgramUniform4fv ((PFNGLPROGRAMUNIFORM4FVPROC)wglGetProcAddress("glProgramUniform4fv"))
#define glGetProgramiv ((PFNGLGETPROGRAMIVPROC)wglGetProcAddress("glGetProgramiv"))
#define glGetProgramInfoLog ((PFNGLGETPROGRAMINFOLOGPROC)wglGetProcAddress("glGetProgramInfoLog"))


static GLuint fragShader;

void intro_init(HWND hwnd) {
    // Create a fragment shader program, the default vertex shader will
    // be used (?)
    fragShader = glCreateShaderProgramv(GL_FRAGMENT_SHADER, 1, &shader_frag);
    glUseProgram(fragShader);

    #ifdef DEBUG
    GLuint result;
    GLchar info[2048];
    glGetProgramiv(fragShader, GL_LINK_STATUS, &result);
    glGetProgramInfoLog(fragShader, 2048, NULL, info);
    if(!result) {
        MessageBox(hwnd, info, "FS error", MB_OK);
        ExitProcess(0);
    }
    #endif
}


// Paramaters to pass to the fragment shader at each frame as an array of vec4s
static GLfloat params[4*1] = {(float)XRES, (float)YRES, 0.f, 0.f};

void intro_do(GLfloat time) {
    params[2] = time;
    glProgramUniform4fv(fragShader, 0, 1, params);
    glRects(-1, -1, 1, 1);
}