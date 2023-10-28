# 4k intro template

This is an example template for building 4kB intros on Windows (but can apply to
larger ones as well). It is highly inspired by
[Inigo Quilez's (iq) templates](https://madethisthing.com/iq/Demo-Framework-4k)
for 4k shader-based intros. I wanted to reimplement it myself with a bit of
a different structure and lots of comments so that the code appears less obscure
for beginners. The rendering and music generation is mostly based on iq's example,
but the synthetizer is custom made.

For complete beginners new demoscene, check out this article on how to
[*Teach Yourself Demoscene in 14 Days*](https://github.com/psenough/teach_yourself_demoscene_in_14_days).

This template itself contains a very simple example intro that
can be built and run directly. Of course when designing a real demo, you will want adapt,
inline some of the code here in addition to writing your own (possibly in assembly).

## File structure

- `main.c`: entrypoint, creates the window and starts the music and rendering loop;
- `config.h`: global settings;
- `glext.h` and `khrplatform.h`: self-contained interfaces for OpenGL functions;
- `fp.h`: useful set of approximate floats (by iq);
- `intro.h`/`intro.c`: rendering initialisation and update;
- `shader.inl`: the shaders' sources;
- `synth.h`/`synth.c`: functions for the audio synthetizer;
- `music.h`/`music.c`: music file generation.

## Build

The compilation tricks to generate a small executable follows the recommandations described
by iq [in this article](https://iquilezles.org/articles/compilingsmall/).
Building the project requires the following tools to be installed and accessible via the command line:

- Microsoft Visual Studio's build tools `cl.exe`, `link.exe`
(e.g. using the Developer Command Prompt)
- [`crinkler`](https://github.com/runestubbe/Crinkler) (e.g. in PATH)

If you are using an antivirus software it is likely to detect the compiled executable as
a trojan. You'll need to whitelist the file or the entire folder in your antivirus before running it
(or even before compiling it).

Build an uncompressed debug version (uses Microsoft's linker):

```bash
build.bat
```

Or build a debug compressed executable (uses crinkler)

```bash
build.bat tiny
```

Or the compressed release (non debug):

```bash
build.bat tiny release
```

And run:

```bash
main.exe
```
