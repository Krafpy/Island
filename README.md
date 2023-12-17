# 4k intro template

This is a (yet another) example minimal template for building 4kB
[intros](https://en.wikipedia.org/wiki/Demoscene) (but can apply
to other sizes as well) on Windows, with an emphasis on being
*educational and beginner-friendly*. It was initially inspired by
[Inigo Quilez's (iq) templates](https://madethisthing.com/iq/Demo-Framework-4k)
for 4k shader-based intros, but with a more explicit
code and lots of comments. It is a command-line only alternative to existing
frameworks and only requires Visual Studio Build Tools to be installed. This was
made in order to clearly detail the compilation process without
MSBuild/Visual Studio's automation. For Visual Studio compatible frameworks,
you probably want to look into iq's one or
[Leviathan](https://github.com/armak/Leviathan-2.0).

For complete beginners to demoscene, check out this article on how to
[*Teach Yourself Demoscene in 14 Days*](https://github.com/psenough/teach_yourself_demoscene_in_14_days).

This template itself contains a very simple example intro that
can be built and run directly.
The rendering and music generation is mostly based on iq's example,
but the synthetizer is custom made (and probably a bad example).
Of course, when designing a production demo, you will want adapt,
inline, rewrite or remove some of the code here in addition to writing your own.
For the shader files, you probably would want to use
[Shader Minifier](https://github.com/laurentlb/Shader_Minifier).

## File structure

- `main.c`: entrypoint, creates the window and starts the music and rendering loop;
- `config.h`: global settings;
- `glext.h`, `khrplatform.h`: self-contained interfaces of OpenGL functions;
- `fp.h`: useful set of approximate floats ([by iq](https://iquilezles.org/articles/float4k/));
- `intro.h`/`intro.c`: rendering initialisation and update;
- `shaders.inl`: the shaders' sources;
- `synth.h`/`synth.c`: functions for the audio synthetizer;
- `music.h`/`music.c`: music file generation.

## Build

The compilation tricks to generate a small executable follow the recommandations described
in [this article by iq](https://iquilezles.org/articles/compilingsmall/), with some updates.
All the build process is contained in the `build.ps1` PowerShell script and uses
custom minimal JSON configuration files. By default, the `debug.json` configuration is used.
You can add your own for specific configurations.

Building the project requires the following tools to be installed and accessible via a
PowerShell command line:

- Microsoft Visual C++ compiler `cl.exe` and linker `link.exe` for x86
(e.g. using the Powershell Developer Command Prompt). These are bundled in the the
[Build Tools for Visual Studio](https://visualstudio.microsoft.com/downloads/?q=build+tools+for+visual+studio)
(note that you don't need to install Visual Studio itself).
- [`crinkler`](https://github.com/runestubbe/Crinkler) (e.g. in PATH)

If you are using an antivirus software (Windows Defender included),
it is likely to detect the compiled executable as
a [trojan](https://en.wikipedia.org/wiki/Trojan_horse_(computing)).
You'll need to whitelist the file or the entire folder in your antivirus before compiling or running it.

Build an uncompressed debug version (uses MSVC's linker):

```powershell
.\build.ps1
```

or the compressed release (uses crinkler):

```powershell
.\build.ps1 .\release.json
```

and run:

```powershell
.\main.exe
```

To see all the build options enter:

```powershell
Get-Help .\build.ps1
```
