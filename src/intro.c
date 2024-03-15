#include <windows.h>
#include <GL/gl.h>
#include "glext.h" // contains type definitions for all modern OpenGL functions
#include "shaders.inl"
#include "config.h"

// Define the modern OpenGL functions to load from the driver

#define glCreateShaderProgramv ((PFNGLCREATESHADERPROGRAMVPROC)wglGetProcAddress("glCreateShaderProgramv"))
#define glUseProgram ((PFNGLUSEPROGRAMPROC)wglGetProcAddress("glUseProgram"))
#define glProgramUniform4fv ((PFNGLPROGRAMUNIFORM4FVPROC)wglGetProcAddress("glProgramUniform4fv"))
#define glCreateTextures ((PFNGLCREATETEXTURESPROC)wglGetProcAddress("glCreateTextures"))
#define glTextureStorage2D ((PFNGLTEXTURESTORAGE2DPROC)wglGetProcAddress("glTextureStorage2D"))
#define glBindTextureUnit ((PFNGLBINDTEXTUREUNITPROC)wglGetProcAddress("glBindTextureUnit"))
#define glCreateFramebuffers ((PFNGLCREATEFRAMEBUFFERSPROC)wglGetProcAddress("glCreateFramebuffers"))
#define glNamedFramebufferTexture ((PFNGLNAMEDFRAMEBUFFERTEXTUREPROC)wglGetProcAddress("glNamedFramebufferTexture"))
#define glBindFramebuffer ((PFNGLBINDFRAMEBUFFERPROC)wglGetProcAddress("glBindFramebuffer"))
#define glBlitNamedFramebuffer ((PFNGLBLITNAMEDFRAMEBUFFERPROC)wglGetProcAddress("glBlitNamedFramebuffer"))
#define glGetProgramiv ((PFNGLGETPROGRAMIVPROC)wglGetProcAddress("glGetProgramiv"))
#define glGetProgramInfoLog ((PFNGLGETPROGRAMINFOLOGPROC)wglGetProcAddress("glGetProgramInfoLog"))

static GLuint fragShader;
static GLuint frameTex, renderFBO;

void intro_init(HWND hwnd) {
    // Create a fragment shader program, the default vertex shader will
    // be used (?)
    fragShader = glCreateShaderProgramv(GL_FRAGMENT_SHADER, 1, &main_frag);
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

    glCreateTextures(GL_TEXTURE_2D, 1, &frameTex);
    glTextureStorage2D(frameTex, 1, GL_RGBA8, XRES, YRES);
    glBindTextureUnit(0, frameTex); // immediatly bind the texture to the shader sampler
    glCreateFramebuffers(1, &renderFBO);
    glNamedFramebufferTexture(renderFBO, GL_COLOR_ATTACHMENT0, frameTex, 0);
}


// Paramaters to pass to the fragment shader at each frame as an array of vec4s
static GLfloat params[4*1] = {(float)XRES, (float)YRES, 0.f, 0.f};

void intro_do(GLfloat time) {
    params[2] = time;
    glProgramUniform4fv(fragShader, 0, 1, params);
    // draw in the framebuffer to update the texture in the sampler
    glBindFramebuffer(GL_FRAMEBUFFER, renderFBO);
    glRects(-1, -1, 1, 1);

    // copy the framebuffer to the screen
    glBlitNamedFramebuffer(renderFBO, 0, 0, 0, XRES, YRES, 0, 0, XRES, YRES, GL_COLOR_BUFFER_BIT, GL_LINEAR);
}