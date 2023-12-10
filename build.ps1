param (
    [switch]$help = $false,
    [switch]$tiny = $false,
    [switch]$release = $false,
    [int]$ordertries = 100,
    [switch]$nosound = $false,
    [switch]$nofullscreen = $false,
    [switch]$disasm = $false,
    [switch]$noexe = $false,
    [string]$out = "main.exe"
)


if($help) {
    Write-Output "
    usage: .\build.ps1 [-help] [{-tiny | -release}] [-nosound] [-disasm] [{-noexe | out}]

    options:

    -help: display this text (ignores other options)
    -tiny: compile tiny (uses crinkler)
    -release: compile tiny in release mode
    -ordertries: crinkler's /ORDERTRIES option (release mode only) (default: 100)
    -nosound: don't generate nor play sound
    -nofullscreen: open a framed window instead of fullscreen in release
    -disasm: disassemble compiled object files
    -noexe: don't produce (link) an executable
    out: output file name (default: main.exe)
    "
    return
}

$sourceDir = 'src' # Source files directory
$buildDir = 'obj' # Output directory of object files
$disasmDir = 'dis' # Output directory of disasembled files

# Check if MSVC build tools are accessible
try {
    Get-Command "cl" -ErrorAction Stop | Out-Null
    Get-Command "link" -ErrorAction Stop | Out-Null
} catch {
    Write-Error "MSVC Build Tools cl.exe or link.exe not found."
    return
}

$compileOptions = @(
    '/c', # Compile without linking (generate object files only)
    '/O1', '/Os', '/Oi', # Basic optimization
    '/arch:IA32', # Force to use x87 float instructions
    '/fp:fast', # Allow reordering of float operations
    ('/Fo"{0}\\"' -f $buildDir) # Output directory
)

if($tiny -or $release) {
    $compileOptions += '/GS-' # No buffer security check
} else {
    $compileOptions += '/DDEBUG' # Keep debug code when not in tiny mode
    $nofullscreen = $true # No fullscreen in debug mode
}
if($nosound) {
    $compileOptions += '/DNO_SOUND' # Optionally remove sound
}
if($nofullscreen) {
    $compileOptions += '/DNO_FULLSCREEN'
}

$srcFiles = Get-ChildItem -Path $sourceDir -Filter "*.c" -Recurse `
           | ForEach-Object {$_.FullName}

if (-Not (Test-Path -Path $buildDir)) {
    mkdir $buildDir
}
# Compile
cl $compileOptions $srcFiles

$objFiles = Get-ChildItem -Path $buildDir -Filter "*.obj" -Recurse `
            | ForEach-Object {$_.FullName}

if(-not $noexe) {
    # Link
    if ($tiny -or $release) {
        Write-Output "Linking with crinkler"
        if($release) {
            $extraOptions = @(
                "/COMPMODE:SLOW",
                "/ORDERTRIES:$ordertries"
            )
        }
        crinkler /OUT:$out `
                /SUBSYSTEM:WINDOWS `
                /ENTRY:wWinMain `
                /TINYHEADER `
                $extraOptions `
                $objFiles `
                kernel32.lib user32.lib gdi32.lib opengl32.lib bufferoverflowu.lib Winmm.lib

    } else {
        Write-Output "Default linking"
        link /OUT:$out `
            $objFiles `
            user32.lib gdi32.lib opengl32.lib Winmm.lib

    }
}

# Optional disassembly for debugging
if($disasm) {
    if(-Not (Test-Path -Path $disasmDir)) {
        mkdir $disasmDir
    }
    Write-Output "Disassembling generated object files"
    foreach($objFile in $objFiles) {
        $basename = (Split-Path $objFile -Leaf).Split('.')[0]
        $outOption = '/OUT:".\\{0}\\{1}.asm"' -f $disasmDir, $basename
        dumpbin /DISASM $objFile $outOption | Out-Null
    }
}