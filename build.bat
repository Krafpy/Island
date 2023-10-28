@echo off
set "BUILD_DIR=obj"

if not exist "%BUILD_DIR%" (
    mkdir "%BUILD_DIR%"
)


if "%2"=="release" (
    cl /c /O1 /Os /Oi /GS- /arch:IA32 /fp:fast ^
       /Fo"%BUILD_DIR%\\" ^
       src\\main.c src\\intro.c src\\music.c src\\synth.c
) else (
    cl /c /O1 /Os /arch:IA32 /fp:fast ^
       /Fo"%BUILD_DIR%\\" /DDEBUG ^
       src\\main.c src\\intro.c src\\music.c src\\synth.c
)

if "%1"=="tiny" (
    echo Linking with crinkler
    crinkler /OUT:main.exe ^
             /SUBSYSTEM:WINDOWS ^
             /ENTRY:wWinMain ^
             /TINYHEADER ^
             %BUILD_DIR%\\main.obj %BUILD_DIR%\\intro.obj %BUILD_DIR%\\music.obj %BUILD_DIR%\\synth.obj ^
             kernel32.lib user32.lib gdi32.lib opengl32.lib bufferoverflowu.lib Winmm.lib
) else (
    echo Default linking
    link /OUT:main.exe ^
         %BUILD_DIR%\\main.obj %BUILD_DIR%\\intro.obj %BUILD_DIR%\\music.obj %BUILD_DIR%\\synth.obj ^
         user32.lib gdi32.lib opengl32.lib Winmm.lib
)