# Island - 4k

4k intro released at [Revision 2024](https://2024.revision-party.net/).

## Links

- [Pouet](https://www.pouet.net/prod.php?which=96609)
- [Demozoo](https://demozoo.org/productions/342195/)
- [YouTube](https://youtu.be/1_wF9xH6PeI)

## Build

### Building the intro

Building requires the following tools to be installed and accessible via a
PowerShell command line (e.g. in PATH):

- Microsoft Visual C++ compiler `cl.exe` and linker `link.exe` for x86
- [`crinkler`](https://github.com/runestubbe/Crinkler)
- [`shader_minifier`](https://github.com/laurentlb/Shader_Minifier)

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
.\build.ps1 .\release.json -CrinklerTries 8000
```

and run:

```powershell
.\main.exe
```
