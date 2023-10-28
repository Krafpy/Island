@echo off
if %1.==. (
    echo No obj file name specified
) else (
    dumpbin /DISASM obj/%1.obj /OUT:%1.asm
)