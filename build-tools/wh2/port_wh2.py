"""Witchaven II on JonoF's Witchaven port: the DOS parts of the sequel's game
modules replaced by the port's, the way JonoF did it for Witchaven I.

Applied to fresh copies of the DOS sources (q_wh2full.sh): idempotent.
usage: port_wh2.py <jfwhaven2 tree>
"""
import os, re, sys

tree = sys.argv[1]
SRC = os.path.join(tree, 'src')

def load(name):
    return open(os.path.join(SRC, name), encoding='latin1', newline='').read().replace('\r\n', '\n')

def save(name, s):
    open(os.path.join(SRC, name), 'w', encoding='latin1', newline='\n').write(s)

def delete_function(s, name):
    """Remove the definition of function `name` (its type on the line before
    the name, or on the same line) from the signature to its closing brace."""
    m = re.search(r'(?m)^(?:[A-Za-z_][\w \*]*\n)?\**' + re.escape(name) + r'\s*\([^;{}]*?\)\s*\n?\{', s)
    if not m:
        return s
    start = m.start()
    # back up over a return type on the previous line(s)
    i = start
    while i > 0:
        prev = s.rfind('\n', 0, i - 1)
        line = s[prev + 1:i].strip()
        if line and re.match(r'^(void|int|short|char|long|unsigned|static|struct|__interrupt|__far|\w+)[\w \*]*$', line) and '(' not in line and ';' not in line:
            i = prev + 1
        else:
            break
    start = i
    depth = 0
    j = s.index('{', m.end() - 1)
    k = j
    while k < len(s):
        if s[k] == '{':
            depth += 1
        elif s[k] == '}':
            depth -= 1
            if depth == 0:
                break
        k += 1
    end = k + 1
    while end < len(s) and s[end] == '\n':
        end += 1
    return s[:start] + s[end:]

def delete_block(s, head):
    """Remove an `if (...) {` ... `}` statement whose first line starts with `head`."""
    while True:
        m = re.search(r'(?m)^[ \t]*' + re.escape(head) + r'[^\n]*\{[ \t]*\n', s)
        if not m:
            return s
        depth = 0
        k = s.index('{', m.start())
        j = k
        while j < len(s):
            if s[j] == '{':
                depth += 1
            elif s[j] == '}':
                depth -= 1
                if depth == 0:
                    break
            j += 1
        end = j + 1
        while end < len(s) and s[end] in ' \t':
            end += 1
        if s[end:end + 4] == 'else':  # keep an else branch as a plain block? drop it too
            # find the end of the else statement
            k2 = s.index('{', end)
            depth = 0; j2 = k2
            while j2 < len(s):
                if s[j2] == '{': depth += 1
                elif s[j2] == '}':
                    depth -= 1
                    if depth == 0: break
                j2 += 1
            end = j2 + 1
        while end < len(s) and s[end] == '\n':
            end += 1
        s = s[:m.start()] + s[end:]

# ---------------------------------------------------------------- whaven.c
s = load('whaven.c')
s = re.sub(r'(?m)^.*__interrupt.*\n', '', s)
for fn in ('faketimerhandler', 'getvmode', 'cls80x25', 'tprintf', 'showadditionalinfo', 'shutdown', 'crash',
           'inittimer', 'uninittimer', 'timerhandler', 'initkeys', 'uninitkeys', 'keyhandler',
           'main', 'setup3dscreen', 'permanentwritesprite', 'overwritesprite'):
    s = delete_function(s, fn)
# the Wingman and Spaceball parts of the control setup
s = delete_block(s, 'if (configWingman[0] == 1)')
s = delete_block(s, 'if (configAvenger[0] == 1)')
# the sound devices are the port's: music and sound on
s = delete_block(s, 'if (wMIDIDevice != -1)')
s = delete_block(s, 'if (wDIGIDevice != -1)')
s = s.replace('shutdown();', 'shutdowngame();')

PORT_CODE = r'''
// ---- the port's part (after JonoF's jfwhaven: whaven.c) ----
#include "osd.h"
#include "version.h"
#ifdef MISTER_HYBRID
#include "mister_port.h"
#endif

int xdimgame = 320, ydimgame = 200, bppgame = 8;
int forcesetup = 1;
int synctics;
int globhiz, globloz, globhihit, globlohit;
int chainxres[4] = {256,320,360,400};
int chainyres[11] = {200,240,256,270,300,350,360,400,480,512,540};
int vesares[7][2] = {{320,200},{640,400},{640,480},{800,600},{1024,768},{1280,1024},{1600,1200}};
char option[NUMOPTIONS] = {
  2,  // 0 - Video type (2 = 320x200 layout)
  0,  // 1 -
  0,  // 2 -
  1,  // 3 - 1 = Mouse, +2 = Joystick
  0,  // 4 -
  0,  // 5 -
  1,  // 6 - Video mode
  64+4+2, // 7 - Sound, 22khz 16bit stereo
  0,  // 8 -
  1,  // 9 - Music on
  0,  // 10 -
  1,  // 11 - Sound on
};
int digihz[8] = {6000,8000,11025,16000,22050,32000,44100,48000};
int  crashflag;
int  oldvmode;

void faketimerhandler(void) { }

// referenced by code the optimizer drops in a release build (an -O0 build links them)
__attribute__((weak)) int iglassenabled, vfx1enabled;
__attribute__((weak)) const unsigned char appicon_bmp[1];
__attribute__((weak)) const int appicon_bmp_size = 0;

// every frame the game shows: the engine's events first (the DOS game had a
// keyboard interrupt; icorp.h makes nextpage() this)
void wh2_nextpage(void) {
    handleevents();
    OSD_DispatchQueued();
    (nextpage)();
}

void rp_delay(int goal) {
    int dagoal = totalclock + goal;
    while (totalclock < dagoal) {
        handleevents();
    }
}

static void writepref(void) {
    int fil = open("pref.dat", O_BINARY | O_TRUNC | O_CREAT | O_WRONLY, S_IREAD | S_IWRITE);
    if (fil >= 0) {
        write(fil, &goreon, 2);
        write(fil, &brightness, 2);
        write(fil, &gbrightness, 2);
        write(fil, &digilevel, 2);
        write(fil, &musiclevel, 2);
        write(fil, &difficulty, 2);
        close(fil);
    }
}

void shutdowngame(void) {
    writepref();
    SND_Shutdown();
    netshutdown();
    if (engineinitflag) {
        uninitengine();
    }
    uninitgroupfile();
    if (crashflag) {
        return;
    }
    exit(0);
}

void crash(char *fmt, ...) {
    va_list argptr;
    crashflag = 1;
    shutdowngame();
    va_start(argptr, fmt);
    vprintf(fmt, argptr);
    va_end(argptr);
    printf("\n");
    exit(0);
}

void crashgame(char *fmt, ...) {
    va_list argptr;
    crashflag = 1;
    shutdowngame();
    va_start(argptr, fmt);
    vprintf(fmt, argptr);
    va_end(argptr);
    printf("\n");
    exit(0);
}

// the view of the 3D screen for a status bar screen size (320x200 layout
// scaled to the mode), after the DOS game's setup3dscreen()
void setviewport(int screensize) {
    int dax, day, dax2, day2;
    if (screensize > 320) {
        dax = day = 0;
        dax2 = xdim; day2 = ydim;
    } else {
        int ss = screensize;
        dax = scale(160 - (ss >> 1), xdim, 320);
        dax2 = scale(160 - (ss >> 1) + ss, xdim, 320);
        day = scale(84 - (((ss * 168) / 320) >> 1), ydim, 200);
        day2 = scale(STATUSSCREEN + 1, ydim, 200);
    }
    setview(dax, day, dax2 - 1, day2 - 1);
}

void setup3dscreen(void) {
    struct player *plr = &player[0];
    if (setgamemode(fullscreen, xdimgame, ydimgame, bppgame))
        crashgame("Could not set video mode");
    videoinitflag = 1;
    svga = 0;
    setviewport(plr->screensize);
    updatestatusbar();
}

int app_main(int argc, const char * const argv[]) {
    struct player *plr;
    int fil, j;

    plr = &player[0];
    netcheckargs(argc, (char **)argv);
#ifdef MISTER_HYBRID
    // the game data, the settings and the saved games are all in the folder
    // the launcher starts the game in
    {
        char cwd[BMAX_PATH+1];
        if (getcwd(cwd, sizeof(cwd))) {
            addsearchpath(cwd);
        }
    }
    MiSTer_Init(); // the launcher keeps what the game prints
#else
    buildsetlogfile("whaven2.log");
#endif
    wm_setapptitle("JFWitchaven II");
    buildprintf("\nJFWitchaven II\nBased on WITCHAVEN II Copyright (C) 1996 Intracorp Entertainment, Inc. v2.0g\n"
        "Version %s.\nBuilt %s %s.\n", game_version, game_date, game_time);

    if (preinitengine()) {
        wm_msgbox("Build Engine Initialisation Error",
            "There was a problem initialising the Build engine: %s", engineerrstr);
        exit(1);
    }
    initgroupfile("stuff.dat");
    buildprintf(" map name: level%d\n", mapon);

    strcpy(configFile, "HMISET.CFG");
    readControlConfigs(1);
    setOptions();
    option[0] = 2;

    if (access("pref.dat", F_OK) != 0) {
        goreon = 1;
        brightness = gbrightness = 0;
        digilevel = 11;
        musiclevel = 11;
        writepref();
    } else {
        fil = open("pref.dat", O_RDONLY | O_BINARY);
        if (fil >= 0) {
            read(fil, &goreon, 2);
            read(fil, &brightness, 2);
            read(fil, &gbrightness, 2);
            read(fil, &digilevel, 2);
            read(fil, &musiclevel, 2);
            read(fil, &difficulty, 2);
            close(fil);
        }
    }
#ifdef MISTER_HYBRID
    fullscreen = 0;
    xdimgame = MiSTer_ScreenWidth();
    ydimgame = MiSTer_ScreenHeight();
    bppgame = 8;
    MiSTer_Loading();
    MiSTer_PickGame();
#endif
    MusicMode = (option[8] == 1 || option[9] == 1);
    SoundMode = (option[10] == 1 || option[11] == 1);
    if (getenv("WH2NOMUSIC")) MusicMode = 0; // diagnostics
    if (getenv("WH2NOSOUND")) SoundMode = 0;

    initinput();
    if (configMouse[0] == 1) {
        initmouse();
    }
    initjstick();

    if (initengine()) {
        wm_msgbox("Build Engine Initialisation Error",
            "There was a problem initialising the Build engine: %s", engineerrstr);
        exit(1);
    }
    engineinitflag = 1;
    svga = 0;
    pskyoff[0] = 0; pskyoff[1] = 0; pskybits = 1;
    // the DOS game ran keytimerstuff() from its 120 Hz sound timer interrupt
    inittimer(120, keytimerstuff);
    buildputs(" loadpics()\n");
    loadpics("tiles000.art", 8*1048576);
    totalclock = 0;
    playerdie = 0;
    plr->oldsector = plr->sector;
    plr->horiz = 100;
    plr->zoom = 256;
    plr->screensize = 320;
    plr->dimension = 3;
    plr->height = PLAYERHEIGHT;
    plr->z = sector[plr->sector].floorz - (plr->height << 8);
    visibility = 1024;
    parallaxtype = 1;
    setup3dscreen();
    setbrightness(brightness, palette, 0);
    SND_Startup();
    initpaletteshifts();
    readpalettetable();
    for (j = 0; j < 256; j++)
        tempbuf[j] = ((j + 32) & 255);
    makepalookup(16, tempbuf, 0, 0, 0, 1);
    for (j = 0; j < 256; j++)
        tempbuf[j] = j;
    makepalookup(17, tempbuf, 24, 24, 24, 1);
    for (j = 0; j < 256; j++)
        tempbuf[j] = j;
    makepalookup(18, tempbuf, 8, 8, 48, 1);
    gameactivated = 0;
    escapetomenu = 0;
    if (netgame == 0) {
        smkplayseq("intro");
    }
    setbrightness(brightness, palette, 0);
    SND_MenuMusic(0);
    playloop();
    return 0;
}
'''
# the port's code goes before playloop()
m = re.search(r'(?m)^(extern\s*\n)?int\s+spikeme;\s*\n\s*void\s*\nplayloop\(void\)', s)
if m:
    s = s[:m.start()] + PORT_CODE + '\n' + s[m.start():]
elif 'int app_main(' not in s:
    s = s.replace('\nvoid\nplayloop(void)', PORT_CODE + '\nvoid\nplayloop(void)', 1)
# the loop pumps the engine's events
s = s.replace('     while (!exit) {\n          if (gameactivated == 0 || escapetomenu == 1) {',
              '     while (!exit) {\n          handleevents();\n          OSD_DispatchQueued();\n          if (gameactivated == 0 || escapetomenu == 1) {')
s = s.replace('exit = menuscreen(plr);\n               updatestatusbar();', 'exit = menuscreen(plr);\n               setviewport(plr->screensize);\n               updatestatusbar();')
# the engine's map loader
s = s.replace('loadboard(fname, &plr->x, &plr->y, &plr->z, &daang, &plr->sector)', 'loadboard(fname, 0, &plr->x, &plr->y, &plr->z, &daang, &plr->sector)')
save('whaven.c', s)

# ---------------------------------------------------------------- whmenu.c
s = load('whmenu.c')
s = re.sub(r'(?m)^extern SWIFT_3DStatus wingmaninfo;\n', '', s)
# the DOS palette port I/O and the Watcom inline assembly
s = re.sub(r'(?s)extern void near asmwaitvrt\(int parm1\);.*?modify\[eax ecx edx\];\n', '', s)
for fn in ('clearpal', 'getpalette', 'fillpalette', 'cehndlr', 'installcrerrhndlr'):
    s = delete_function(s, fn)
s = s.replace('char palette1[256][3],\n     palette2[256][3];', '''char palette1[256][3],
     palette2[256][3];

// the port's palette helpers (the DOS game wrote the VGA ports)
void clearpal(void) {
    fillpalette(0, 0, 0);
}
void getpalette(char *pal) {
    memcpy(pal, palette, 768);
}
void fillpalette(int red, int green, int blue) {
    unsigned char pal[768];
    int i;
    for (i = 0; i < 256; i++) { pal[i*3] = red; pal[i*3+1] = green; pal[i*3+2] = blue; }
    setbrightness(brightness, pal, 0);
}
''')
s = re.sub(r'asmsetpalette\(([^;]*)\);', r'setbrightness(brightness, (unsigned char *)\1, 0);', s)
s = re.sub(r'(?m)^[ \t]*asmwaitvrt\([^;]*\);[ \t]*\n', '          nextpage();\n', s)
s = re.sub(r'saveboard\(tempbuf, &plr->x, &plr->y, &plr->z, &plr->ang, &plr->sector\)',
           r'({ short wh2ang = plr->ang; saveboard(tempbuf, &plr->x, &plr->y, &plr->z, &wh2ang, &plr->sector); })', s)
save('whmenu.c', s)

# ---------------------------------------------------------------- whinp.c
s = load('whinp.c')
s = re.sub(r'(?m)^SPW_InputEvent sbpacket;\n', '', s)
s = re.sub(r'(?m)^SWIFT_3DStatus wingmaninfo,\n\s*oldwingmaninfo;\n', '', s)
s = delete_block(s, 'if (spaceballinitflag)')
s = delete_block(s, 'if (wingmanflag)')
s = re.sub(r'itoa\(([^,]+), ([^,]+), 10\)', r'sprintf(\2, "%d", \1)', s)
save('whinp.c', s)

# ---------------------------------------------------------- engine API forms
for name in ('whaven.c', 'whani.c', 'whinp.c', 'whmenu.c', 'whobj.c', 'whplr.c', 'whfx.c', 'whtag.c'):
    s = load(name)
    s = re.sub(r'setbrightness\((\w+), \(char \*\) &palette\[0\]\)', r'setbrightness(\1, palette, 0)', s)
    # hitscan: the engine takes a clip mask as well
    s = re.sub(r'(hitscan\((?:[^;()]|\([^;()]*\))*?&hit\w+, &hit\w+, &hit\w+, &hit\w+, &hit\w+, &hit\w+)\)', r'\1, CLIPMASK1)', s)
    s = s.replace('SND_MenuMusic()', 'SND_MenuMusic(0)')
    s = re.sub(r'\bstartsong\(', 'SND_StartMusic(', s)
    s = re.sub(r'\bdolevelmusic\(', 'SND_StartMusic(', s)
    s = re.sub(r'(?m)^[ \t]*(cls80x25|setvmode|tprintf)\([^;]*\);[ \t]*\n', '', s)
    s = re.sub(r'\bshutdown\s*\(', 'shutdowngame(', s)
    s = re.sub(r'itoa\(([^,]+), ([^,]+), 10\)', r'sprintf(\2, "%d", \1)', s)
    save(name, s)
print('ported')
