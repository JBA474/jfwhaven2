"""MiSTer (TekWar, Witchaven): the OSD's Resolution line works while the game
runs, as in jfSW. The engine (jfbuild/src/sdlayer2.c, MiSTer branch) reads
the option in handleevents() and sets the new mode when it changes, once the
game has set its first mode.
usage: patch_osdres.py <jfbuild>
"""
import os, sys

p = os.path.join(sys.argv[1], 'src/sdlayer2.c')
s = open(p, encoding='utf-8', errors='replace').read()

def rep(old, new, what):
    global s
    if new in s:
        return
    assert s.count(old) == 1, f'{what}: {s.count(old)}'
    s = s.replace(old, new)

rep('''int handleevents(void)
{
''', '''// The OSD's Resolution line (status bits 24..25) followed while the game
// runs: the mode is set again when the option changes, from the first mode
// the game set on. A mode the core does not have is the one of 200 lines.
static void misterfollowresolution(void)
{
	static const int modes[4] = { MH_MODE_320x200, MH_MODE_640x200, MH_MODE_320x240, MH_MODE_640x240 };
	static int lastmode = -1;
	int mode;

	if (!misterfollowmodes || !MH_IsOpen()) return;
	mode = modes[MH_OSD_GAME_BITS(MH_OSDStatus(), 24, 2)];
	if (!MH_ModeAvailable(mode)) mode = MH_ModeWidth(mode) == 640 ? MH_MODE_640x200 : MH_MODE_320x200;
	if (mode == lastmode) return;
	lastmode = mode;
	if (MH_ModeWidth(mode) != xdim || MH_ModeHeight(mode) != ydim)
		setgamemode(fullscreen, MH_ModeWidth(mode), MH_ModeHeight(mode), 8);
}

int handleevents(void)
{
	misterfollowresolution();
''', 'handleevents')

rep('''	videomodereset = 0;
''', '''	videomodereset = 0;
	misterfollowmodes = 1;
''', 'setgamemode')

# the flag, declared before setgamemode() which sets it
rep('''static int misterjoybuttons = 0, misterjoyhat = 0, misterjoystickdpad = 1;
''', '''static int misterjoybuttons = 0, misterjoyhat = 0, misterjoystickdpad = 1;
static int misterfollowmodes = 0; // the OSD's Resolution line followed (misterfollowresolution)
''', 'flag')
# a tree patched by the first version of this script: the declaration moved
s = s.replace('''static int misterfollowmodes = 0;
static void misterfollowresolution(void)''', '''static void misterfollowresolution(void)''')

open(p, 'w', encoding='utf-8', newline='\n').write(s)
print('patched')
