#include <windows.h>
#include <GL/gl.h>
#include <memory.h>
#include "intro.h"
#include "music.h"
#include "config.h"
#include "capture.h"


static const PIXELFORMATDESCRIPTOR pfd = {
    .nSize = sizeof(PIXELFORMATDESCRIPTOR),
    .nVersion = 1,
    .dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
    .iPixelType = PFD_TYPE_RGBA,
    .cColorBits = 32,
    .cDepthBits = 24,
    .cStencilBits = 8
};

static DEVMODE displaySettings;

static HWAVEOUT waveHandle;

static short waveBuffer[MUSIC_DATA_BYTES];

// https://learn.microsoft.com/en-us/windows/win32/api/mmeapi/ns-mmeapi-waveformatex
static WAVEFORMATEX waveFormat = {
    .wFormatTag = WAVE_FORMAT_PCM,
    .nChannels = NUM_CHANNELS,
    .nSamplesPerSec = SAMPLE_RATE,
    .nAvgBytesPerSec = BYTE_RATE,
    .nBlockAlign = SAMPLE_ALIGNMENT,
    .wBitsPerSample = BIT_DEPTH,
    .cbSize = 0
};

// https://learn.microsoft.com/en-us/previous-versions/dd743837(v=vs.85)
static WAVEHDR waveHeader = {
    .lpData = (LPSTR)waveBuffer,
    .dwBufferLength = MUSIC_DATA_BYTES,
    .dwBytesRecorded = 0,
    .dwUser = 0,
    .dwFlags = WHDR_PREPARED,
    .dwLoops = 0,
    .lpNext = 0,
    .reserved = 0
};

// https://learn.microsoft.com/en-us/previous-versions/dd757347(v=vs.85)
MMTIME musicTime = {
    .wType = TIME_SAMPLES,
    .u = {0}
};

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

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

    #ifdef FULLSCREEN
    // Change display settings to fullscreen
    EnumDisplaySettings(NULL, 0, &displaySettings);
    displaySettings.dmPelsWidth  = XRES;
    displaySettings.dmPelsHeight = YRES;
    displaySettings.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT;
    if(ChangeDisplaySettings(&displaySettings, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL) {
        ExitProcess(0);
        return 0;
    }
    ShowCursor(FALSE);
    
    HWND hwnd = CreateWindow(
        CLASS_NAME,
        0,
        WS_POPUP | WS_VISIBLE, // unframed window (for fullscreen)
        0, 0, XRES, YRES,
        NULL, NULL, hInstance,
        NULL
    );
    #else
    HWND hwnd = CreateWindow(
        CLASS_NAME, // the name of the window class to use for this window
        0, // the title of the window
        // set of flags to describe the look and feel of the window,
        // this is the default combination of flags that set a title bar, border,
        // menu, minimize and maximize buttons
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        // xy-position, height and width of the window, set to default
        0, 0, XRES, YRES,
        NULL, // parent window, NULL for a top level window
        NULL, // define a menu for the window, NULL for none
        hInstance, // the handle to this executable
        // a pointer to arbitrary data of type void*, here it's not used
        NULL
    );
    #endif
    
    if (hwnd == NULL) {
        ExitProcess(0);
        return 0;
    }

    // Get the window's handle to a device context. A device context
    // is a data structure that represents a surface.
    HDC hdc = GetDC(hwnd);
    // attempts to match an appropriate pixel format supported by a device
    // context to a given pixel format specification.
    int pixelFormat = ChoosePixelFormat(hdc, &pfd);
    // sets the pixel format of the specified device context
    SetPixelFormat(hdc, pixelFormat, &pfd);
    // Initialize an OpenGL context for this device context
    HGLRC hglrc = wglCreateContext(hdc);
    wglMakeCurrent(hdc, hglrc);

    #ifndef CAPTURE
        // Initialize the intro's rendering pipeline
        intro_init(hwnd);

        #ifdef SOUND
        // Initialize the music file in memory
        music_init(waveBuffer);
        // Play the sound file directly from memory, asynchronously for the
        // music to play in background
        if (waveOutOpen(&waveHandle, WAVE_MAPPER, &waveFormat, 0, 0, CALLBACK_NULL)) {
            #ifdef DEBUG
            MessageBox(hwnd, "Failed to play sound (waveOutOpen)", "Error", MB_OK);
            #endif
            ExitProcess(0);
            return 0;
        }
        waveOutWrite(waveHandle, &waveHeader, sizeof(waveHeader));

        #define INTRO_NOT_DONE musicTime.u.sample < NUM_SAMPLES
        #else
        DWORD startTime = timeGetTime(), elapsedTime = 0;

        #define INTRO_NOT_DONE elapsedTime < INTRO_DURATION*1000
        #endif

        #ifdef DEBUG
        // Store a variable to check if the window needs to be closed
        BOOL done = FALSE;
        SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)&done);
        MSG msg;
        while(!done)
        #else
        while(!GetAsyncKeyState(VK_ESCAPE) && INTRO_NOT_DONE)
        #endif
        {
            #ifdef DEBUG
            while(PeekMessage(&msg, hwnd, 0, 0, PM_REMOVE)){
                TranslateMessage(&msg); // translate keyboard messages if necessary
                DispatchMessage(&msg); // call the window procedure
            }
            #endif

            // Pass the elapsed time in seconds since startup to the shaders
            #ifdef SOUND
            GLfloat time = ((GLfloat)musicTime.u.sample / SAMPLE_RATE);
            #else
            elapsedTime = timeGetTime() - startTime;
            GLfloat time = (GLfloat)elapsedTime / 1000.f;
            #endif

            intro_do(time);
            SwapBuffers(hdc);
            
            Sleep(1); // let other processes some time (1ms)
            // Get the new music time
            #ifdef SOUND
            waveOutGetPosition(waveHandle, &musicTime, sizeof(MMTIME));
            #endif
        }
    #else
        init_capture(hwnd);

        intro_init(hwnd);
        
        #ifdef SOUND
        music_init(waveBuffer);
        save_audio(waveBuffer, MUSIC_DATA_BYTES, hwnd);
        #endif

        #ifdef VIDEO
        BOOL done = FALSE;
        SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)&done);
        MSG msg;
        for(int i = 0; i < INTRO_DURATION*CAPTURE_FRAMERATE && !done; i++) {
            while(PeekMessage(&msg, hwnd, 0, 0, PM_REMOVE)){
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }

            GLfloat time = ((GLfloat)i / (GLfloat)CAPTURE_FRAMERATE);

            intro_do(time);
            SwapBuffers(hdc);

            save_frame(i);
        }
        #endif
    #endif

    #ifdef FULLSCREEN
    // Back to default display settings
    ChangeDisplaySettings(NULL, 0);
    ShowCursor(TRUE);
    #endif

    ExitProcess(0);
    return 0;
}

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
