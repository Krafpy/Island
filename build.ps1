param ( 
    [string]$Config = ".\debug.json",
    [Parameter(DontShow)]
    $defaults = (Get-Content $Config | ConvertFrom-Json),
    [switch]$DebugMode = $defaults.DebugMode,
    [switch]$Tiny = $defaults.Tiny,
    [switch]$Sound = $defaults.Sound,
    [switch]$Fullscreen = $defaults.Fullscreen,
    [int]$XRes = $defaults.XRes,
    [int]$YRes = $defaults.YRes,
    [string]$OutName = $defaults.OutName,

    [switch]$NoExe,
    [switch]$Recompile,
    [switch]$Disasm,
    [switch]$SuffixWithRes,
    [int]$CrinklerTries = 0
)

if ($MyInvocation.BoundParameters['defaults']) {
    throw "The -defaults parameter is not allowed. Use -ConfigFile to specify the configuration file."
}

Write-Host "DebugMode:  $DebugMode"
Write-Host "Tiny:       $Tiny"
Write-Host "Sound:      $Sound"
Write-Host "Fullscreen: $Fullscreen"
Write-Host "XRes:       $XRes"
Write-Host "YRes:       $YRes"
Write-Host ""


$sourceDir = 'src' # Source files directory
$buildDir = 'obj' # Output directory of object files
$disasmDir = 'dis' # Output directory of disasembled files

$infoColor = "Cyan"

# Check if MSVC build tools are accessible
try {
    Get-Command "cl" -ErrorAction Stop | Out-Null
    Get-Command "link" -ErrorAction Stop | Out-Null
} catch {
    Write-Error "MSVC Build Tools cl.exe or link.exe not found."
    return
}
# Check if Shader Minifirer is available
try {
    Get-Command "shader_minifier" -ErrorAction Stop | Out-Null
} catch {
    Write-Error "shader_minifier.exe not found."
    return
}

# Utility functions to test if a given file needs to be updated based
# on its dependencies last update
function ItemNeedsUpdate($itemPath, $dependsPaths) {
    if(-not (Test-Path -Path $itemPath)) {
        return $true
    }
    $item = Get-Item $itemPath
    foreach($dependsPath in $dependsPaths) {
        $depend = Get-Item $dependsPath
        if($depend.LastWriteTime -gt $item.LastWriteTime) {
            return $true
        }
    }
}

# Generate the minified shader source, since this operation can
# take some time we only generate the file if it has been changed
$shadersDir = "$sourceDir/shaders"
$shadersIncludeFile = "$sourceDir/shaders.inl"
$shaderFiles = Get-ChildItem -Path $shadersDir -Recurse `
                | Where-Object{$_.Extension -match '^.(frag|vert|glsl)$'} `
                | ForEach-Object {$_.FullName}
if(ItemNeedsUpdate $shadersIncludeFile $shaderFiles) {
    Write-Host "Minifying shaders..." -ForegroundColor $infoColor
    shader_minifier $shaderFiles -o $shadersIncludeFile
}

# Available options:
# https://learn.microsoft.com/en-us/cpp/build/reference/compiler-options-listed-by-category?view=msvc-170
$compileOptions = @(
    '/c', # Compile without linking (generate object files only)
    '/O1', '/Os', '/Oi', # Basic optimization
    '/arch:IA32', # Force to use x87 float instructions
    '/fp:fast', # Allow reordering of float operations
    "/Fo$buildDir/" # Output directory
)

if($DebugMode) {
    $compileOptions += '/DDEBUG'
} else {
    $compileOptions += '/GS-' # No buffer security check
}
if(-not $Sound) { 
    $compileOptions += '/DNO_SOUND'
}
if(-not $Fullscreen) {
    $compileOptions += '/DNO_FULLSCREEN'
}
$compileOptions += "/DXRES=$XRes"
$compileOptions += "/DYRES=$YRes"

$sourceFiles = Get-ChildItem -Path $sourceDir -Filter "*.c" -Recurse `
                | ForEach-Object {$_.FullName}

if (-not (Test-Path -Path $buildDir)) {
    mkdir $buildDir | Out-Null
}


# Compile
# Basic incremental build implementation

function FindDependencies($fullFilePath) {
    $paths = @{}
    $ignore = @("glext.h", "khrplatform.h")

    function RecurseDependencies($fullFilePath) {
        $parentPath = Split-Path $fullFilePath -Parent
        $fileContent = Get-Content $fullFilePath
        $includePattern = '^\s*#include\s+"([^"]+)"'

        foreach ($line in $fileContent) {
            if ($line -match $includePattern) {
                $includedFilePath = $Matches[1]
                $fullIncludePath = Resolve-Path "$parentPath/$includedFilePath"
                $fullIncludePath = $fullIncludePath.ToString()

                $includedFile = Split-Path $fullIncludePath -Leaf
                if($includedFile -in $ignore) {
                    continue
                }
                
                if(-not $paths.ContainsKey($fullIncludePath)) {
                    $paths[$fullIncludePath] = $true
                    RecurseDependencies $fullIncludePath
                }
            }
        }
    }

    RecurseDependencies $fullFilePath
    $paths | Select-Object -ExpandProperty Keys
}

# Compile only the sources that have been modified except if
# explicitly told to compile all or if compile options changed

$prevOptsPath = "./build.tmp"
if(-not $Recompile) {
    if(-not (Test-Path -Path $prevOptsPath)) {
        $Recompile = $true
    } else {
        $prevOpts = @(Get-Content -Path $prevOptsPath)
        $cmp = Compare-Object $compileOptions $prevOpts
        $Recompile = -not $null -eq $cmp
    }
}
if($Recompile) {
    $compileOptions | Set-Content -Path $prevOptsPath
}

$compileSources = @()
if(-not $Recompile) {
    foreach($source in $sourceFiles) {
        $objPath = "$buildDir/$((Get-Item $source).BaseName).obj"
        $depsPaths = @($source)
        $depsPaths += FindDependencies (Resolve-Path $source)
        if(ItemNeedsUpdate $objPath $depsPaths) {
            $compileSources += $source
        }
    }
} else {
    $compileSources = $sourceFiles
}

if($compileSources.count -eq 0) {
    Write-Host "Up to date. Nothing to compile." -ForegroundColor $infoColor
} else {
    Write-Host "Compile options: $compileOptions"
    Write-Host "Compiling" -ForegroundColor $infoColor
    cl $compileOptions $compileSources
}

$objectFiles = Get-ChildItem -Path $buildDir -Filter "*.obj" -Recurse `
                | ForEach-Object {$_.FullName}

if(-not $NoExe) {
    if($SuffixWithRes) {
        $outFile = "$OutName-$YRes.exe"
    } else {
        $outFile = "$OutName.exe"
    }

    # Link
    if ($Tiny) {
        Write-Host "Linking with crinkler" -ForegroundColor $infoColor
        # Doc: https://github.com/runestubbe/Crinkler/blob/master/doc/manual.txt

        $extraOptions = @()
        if($CrinklerTries -gt 0) {
            $extraOptions += "/ORDERTRIES:$CrinklerTries"
        }

        crinkler /OUT:$outFile `
                /SUBSYSTEM:WINDOWS `
                /ENTRY:wWinMain `
                /TINYHEADER `
                $extraOptions `
                $objectFiles `
                kernel32.lib user32.lib gdi32.lib opengl32.lib bufferoverflowu.lib Winmm.lib

    } else {
        Write-Host "Default linking" -ForegroundColor $infoColor
        link /OUT:$outFile `
            $objectFiles `
            user32.lib gdi32.lib opengl32.lib Winmm.lib

    }

    Write-Host "Output file: $outFile" -ForegroundColor $infoColor
}

# Optional disassembly for debugging
if($Disasm) {
    if(-not (Test-Path -Path $disasmDir)) {
        mkdir $disasmDir | Out-Null
    }
    Write-Host "Disassembling generated object files" -ForegroundColor $infoColor
    foreach($objectFile in $objectFiles) {
        $baseName = (Split-Path $objectFile -Leaf).Split('.')[0]
        $outOption = "/OUT:./$disasmDir/$baseName.asm"
        dumpbin /DISASM $objectFile $outOption | Out-Null
    }
}