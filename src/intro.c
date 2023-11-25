#include <windows.h>
#include <GL/gl.h>
#include "glext.h" // contains type definitions for all modern OpenGL functions
#include "shader.inl"
#include "config.h"

// Define the modern OpenGL functions to load from the driver

static const char* glFuncNames[] = {
    "glCreateShaderProgramv",
    "glUseProgram",
    "glProgramUniform4fv",
    #ifdef DEBUG
    "glGetProgramiv",
    "glGetProgramInfoLog"
    #endif
};

#define N_GL_FUNCS (sizeof(glFuncNames)/sizeof(char*))

static void* glFuncs[N_GL_FUNCS];

#define glCreateShaderProgramv ((PFNGLCREATESHADERPROGRAMVPROC)glFuncs[0])
#define glUseProgram ((PFNGLUSEPROGRAMPROC)glFuncs[1])
#define glProgramUniform4fv ((PFNGLPROGRAMUNIFORM4FVPROC)glFuncs[2])

#ifdef DEBUG
#define glGetProgramiv ((PFNGLGETPROGRAMIVPROC)glFuncs[3])
#define glGetProgramInfoLog ((PFNGLGETPROGRAMINFOLOGPROC)glFuncs[4])
#endif


static GLuint fragShader;

void intro_init(HWND hwnd) {
    // Load the required OpenGL functions
    for(int i = 0; i < N_GL_FUNCS; i++){
        glFuncs[i] = wglGetProcAddress(glFuncNames[i]);
    }

    // Create a fragment shader program, the default vertex shader will
    // be used (?)
    fragShader = glCreateShaderProgramv(GL_FRAGMENT_SHADER, 1, &fragShaderSource);
    glUseProgram(fragShader);

    #ifdef DEBUG
    GLuint result;
    GLchar info[1536];
    glGetProgramiv(fragShader, GL_LINK_STATUS, &result);
    glGetProgramInfoLog(fragShader, 1024, NULL, info);
    if(!result) {
        MessageBox(hwnd, info, "FS error", MB_OK);
        ExitProcess(0);
    }
    #endif
}


static GLfloat params[4*1] = {(float)X_RES, (float)Y_RES, 0.f, 0.f};

void intro_do(GLfloat time) {
    params[2] = time;
    glProgramUniform4fv(fragShader, 0, 1, params);
    glRects(-1, -1, 1, 1);
}