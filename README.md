# eqclientmod for EverQuest Trilogy client
solar@takp.info

This is a game mod implemented as a planted DLL file.  It works by wrapping an existing DLL the game already uses and because it's in the game directory it's loaded before the one in the system directory.

I use ancient Visual Studio 2005 to develop and build this mod which allows it to work even on old versions of Windows like 95 and 98.  If you want to compile it yourself, you can use newer tools and it will probably work,
but if you want to do it how I do, you should use Visual Studio 2005 and don't use newer APIs or language features.  The EverQuest client was written in the late 90s with tools of that era and the Trilogy version is from 2001ish.

# Installation
To install just copy `winmm.dll` to the game directory (the directory that contains `eqgame.exe`).
To uninstall remove or rename `winmm.dll`.

Once the game runs the mod will create a config file named `eqclientmod.ini` which contains toggles for the included mods along with a brief description of what they do.

You can test that it's working by typing `/eqclientmod` in game which should give you a version string if the mod loaded properly.

# Mods

### CPU High Speed Fix
CPU high clock speed overflow fix.  If you have a CPU that's more than 4.2 Ghz you probably need this to make the game run at the right speed.
This issue happens with all AMD Ryzen 7xxx and 9xxxx CPUs at least.  The game normally samples the TSC by using the `rdtsc` instruction.  This mod makes it use a modern API called `QueryPerformanceCounter`.

### Disable Gamma Change
The game has a gamma slider in it but it does it in a really annoying way that changes the desktop gamma.  It tries to restore it when the game exits but this doesn't always happen with multiple clients and crashes.
This mod disables the gamma functionality by detouring the function that would do this and instead doing nothing.  It's disabled by default in `eqclientmod.ini`.

### CommandHandler
This mod adds the extra command handling that some of the other hacks use but it's not strictly necessary to enable this to use the other hacks.  Basic commands included: /eqclientmod /crash
