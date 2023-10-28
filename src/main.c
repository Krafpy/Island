#include <windows.h>
#include <GL/gl.h>
#include "intro.h"
#include "music.h"
#include "config.h"

LRESULT CALLBACK WindowProc(
    HWND hwnd, // the handle to the window that receives the message
    UINT uMsg, // the message code
    // additional data, depends on the message code
    WPARAM wParam,
    LPARAM lParam
) {
    switch(uMsg){
        case WM_CLOSE:
        case WM_KEYDOWN:
            PBOOL pDone = (PBOOL)GetWindowLongPtr(hwnd, GWLP_USERDATA);
            *pDone = TRUE;
            PostQuitMessage(0);
            break;
        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}

int WINAPI wWinMain(
    HINSTANCE hInstance, // handle to the currently loaded executable
    HINSTANCE hPrevInstance, // legacy from 16-bit Windows, always 0
    PWSTR pCmdLine, // command line arguments as Unicode string
    int nCmdShow // flag indicating if the current app's window is minimized, maximised or shown normally
) {
    #ifdef DEBUG
    // Register the window class
    // The window class defines common data and behaviour of a window, and is
    // used internally by the operating system.
    // Each created window (window insance) must be associated with a window class.
    const LPCSTR CLASS_NAME = "Sample Window Class";
    
    WNDCLASS wc = {0};
    wc.lpfnWndProc = WindowProc; // the window procedure used to handle messages
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;

    RegisterClass(&wc);
    #else
    const LPCSTR CLASS_NAME = "static";
    #endif

    HWND hwnd = CreateWindowEx(
        0, // optional window behaviour flags, 0 = default behaviour
        CLASS_NAME, // the name of the window class to use for this window
        "", // the title of the window
        // set of flags to describe the look and feel of the window,
        // this is the default combination of flags that set a title bar, border,
        // menu, minimize and maximize buttons
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        // xy-position, height and width of the window, set to default
        100, 100, 640, 480,
        NULL, // parent window, NULL for a top level window
        NULL, // define a menu for the window, NULL for none
        hInstance, // the handle to this executable
        // a pointer to arbitrary data of type void*, here it's not used
        NULL
    );
    
    if (hwnd == NULL) {
        ExitProcess(0);
        return 0;
    }

    // Initialize the music file in memory
    WAVFile* music = music_init();
    // Play the sound file directly from memory, asynchronously for the
    // music to play in background
    if(!sndPlaySound((const char*)music, SND_ASYNC | SND_MEMORY)) {
        #ifdef DEBUG
        MessageBox(hwnd, "Failed to play sound", "Error", MB_OK);
        #endif
        ExitProcess(0);
        return 0;
    }

    // Get the window's handle to a device context. A device context
    // is a data structure that represents a surface.
    HDC hdc = GetDC(hwnd);

    // Initialize the device context
    // the pixel format descriptor describes the pixel format of a device
    // context
    PIXELFORMATDESCRIPTOR pfd = {0};

    pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 32;
    pfd.cDepthBits = 24;
    pfd.cStencilBits = 8;

    // attempts to match an appropriate pixel format supported by a device
    // context to a given pixel format specification.
    int pixelFormat = ChoosePixelFormat(hdc, &pfd);
    // sets the pixel format of the specified device context
    SetPixelFormat(hdc, pixelFormat, &pfd);

    // Initialize an OpenGL context for this device context
    HGLRC hglrc = wglCreateContext(hdc);
    wglMakeCurrent(hdc, hglrc);

    // Run the intro initialization
    intro_init(hwnd);

    // Main window loop
    DWORD startTime = timeGetTime(), elapsedTime = 0;

    #ifdef DEBUG
    // Store a variable to check if the window needs to be closed
    BOOL done = FALSE;
    SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)&done);
    MSG msg;

    while(!done)
    #else
    while(!GetAsyncKeyState(VK_ESCAPE) && elapsedTime < INTRO_DURATION*1000)
    #endif
    {
        #ifdef DEBUG
        while(PeekMessage(&msg, hwnd, 0, 0, PM_REMOVE)){
            TranslateMessage(&msg); // translate keyboard messages if necessary
            DispatchMessage(&msg); // call the window procedure
        }
        #endif

        // Pass the elapsed time in seconds since startup to the shaders
        elapsedTime = timeGetTime() - startTime;
        GLfloat time = ((GLfloat)elapsedTime) / 1000.f;

        intro_do(time);
        SwapBuffers(hdc);
        
        Sleep(1); // let other processes some time (1ms)
    }
    
    // Stop any remaining sound from playing
    sndPlaySound(NULL, 0);

    ExitProcess(0);
    return 0;
}