// The vertex shader is not required because the graphics card
// will use the default one if we only provide the fragment shader

// static const char* vertShaderSource = \
// "#version 460\n"
// "layout (location=0) in vec2 inPos;"
// "out gl_PerVertex"
// "{"
//     "vec4 gl_Position;"
// "};"
// "void main()"
// "{"
//     "gl_Position = vec4(inPos,0.0,1.0);"
// "}";

static const char* fragShaderSource = \
"#version 460\n"
"layout (location=0) uniform vec4 params;"
"out vec4 outCol;"
"void main()"
"{"
    "float r = 0.5+0.5*sin(params.x*5.);"
    "outCol = vec4(r,1.-r,0.,1.);"
"}";