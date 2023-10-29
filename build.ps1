param (
    [switch]$tiny = $false,
    [switch]$release = $false,
    [string]$out = "main.exe",
    [switch]$disasm = $false
)

$sourceDir = 'src' # Source files directory
$buildDir = 'obj' # Output directory of object files
$disasmDir = 'dis' # Output directory of disasembled files

$compileOptions = @(
    '/c', '/O1', '/Os', '/Oi', # basic optimization
    '/arch:IA32', # force to use x87 float instructions
    '/fp:fast', # allow reordering of float operations
    ('/Fo"{0}\\"' -f $buildDir) # Output directory
)
if($release) {
    $compileOptions += '/GS-'
} else {
    $compileOptions += '/DDEBUG'
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

# Link
if ($tiny -or $release) {
    Write-Output "Linking with crinkler"
    crinkler /OUT:$out `
             /SUBSYSTEM:WINDOWS `
             /ENTRY:wWinMain `
             /TINYHEADER `
             $objFiles `
             kernel32.lib user32.lib gdi32.lib opengl32.lib bufferoverflowu.lib Winmm.lib
} else {
    Write-Output "Default linking"
    link /OUT:$out `
         $objFiles `
         user32.lib gdi32.lib opengl32.lib Winmm.lib
}


# Optional disassembly for debugging
if($disasm) {
    if(-Not (Test-Path -Path $disasmDir)) {
        mkdir $disasmDir
    }
    Write-Output "Disasembling generated object files"
    foreach($objFile in $objFiles) {
        $basename = (Split-Path $objFile -Leaf).Split('.')[0]
        $outOption = '/OUT:".\\{0}\\{1}.asm"' -f $disasmDir, $basename
        dumpbin /DISASM $objFile $outOption
    }
}