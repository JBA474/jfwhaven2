"""Witchaven II on JonoF's Witchaven port: the game's main header.

The DOS source has its prototypes in generated .def files that the archive
does not have, and its engine externs in extern.h with DOS types. This
writes src/icorp.h for the port: JonoF's includes, Witchaven II's own
structures and externs, prototypes generated from the sequel's sources, and
the port's own declarations.
usage: make_icorp.py <jfwhaven2 tree> <WHAVEN2/SRC of the archive>
"""
import os, re, sys

tree, dos = sys.argv[1], sys.argv[2]
NL = chr(10)

def read(p):
    return open(p, 'rb').read().decode('latin1').replace('\r\n', NL).replace('\r', NL)

def find(d, name):
    for f in os.listdir(d):
        if f.lower() == name.lower():
            return os.path.join(d, f)
    raise FileNotFoundError(name)

# the DOS-only functions of the main module, replaced by the port's (port_wh2.py)
DOS_ONLY = {'faketimerhandler', 'getvmode', 'cls80x25', 'tprintf', 'showadditionalinfo', 'shutdown', 'crash',
            'inittimer', 'uninittimer', 'timerhandler', 'initkeys', 'uninitkeys', 'keyhandler',
            'cehndlr', 'installcrerrhndlr', 'clearpal', 'getpalette', 'fillpalette', 'asmsetpalette', 'asmwaitvrt',
            'setup3dscreen', 'main', 'permanentwritesprite', 'overwritesprite'}
GAME = ['WHAVEN.C', 'WHANI.C', 'WHINP.C', 'WHMENU.C', 'WHOBJ.C', 'WHPLR.C', 'WHFX.C', 'WHTAG.C']

TYPEWORDS = r'(?:void|int|short|char|long|unsigned|static|struct \w+)'
protos = []
seen = set()
for f in GAME:
    s = read(find(dos, f))
    # a return type on the line before the name: joined onto the name's line
    s = re.sub(r'(?m)^(' + TYPEWORDS + r'[\w \*]*?)[ \t]*' + NL + r'(?=[A-Za-z_]\w*[ \t]*\()', r'\1 ', s)
    for m in re.finditer(r'(?m)^((?:[A-Za-z_]\w*[ \t\*]+)*)\**([A-Za-z_]\w*)[ \t]*\(([^;{}()]*)\)[ \t' + NL + r']*\{', s):
        rettype, name, args = m.group(1).strip(), m.group(2), ' '.join(m.group(3).split())
        if name in seen or name in ('if', 'for', 'while', 'switch') or name in DOS_ONLY:
            continue
        if 'far' in args or 'far' in rettype or 'interrupt' in rettype or rettype.startswith('static'):
            continue
        seen.add(name)
        if not rettype:
            rettype = 'int'
        args = args or 'void'
        protos.append(f'{rettype} {name}({args});')

icorp = read(find(dos, 'ICORP.H'))
body = icorp[icorp.index('#define   GONZOGSHXR'):]
body = re.sub(r'\blong\b', 'int', body)
protos = [re.sub(r'\blong\b', 'int', p) for p in protos]

# the game's own globals, declared in extern.h: the extern lines (joined when
# they run over several lines), without the engine's and the DOS keyboard's
ext = read(find(dos, 'EXTERN.H'))
externs = []
lines = ext.split(NL)
i = 0
while i < len(lines):
    t = lines[i].strip()
    if t.startswith('extern '):
        while not t.endswith(';') and i + 1 < len(lines):
            i += 1
            t += ' ' + lines[i].strip()
        if not any(k in t for k in ('keystatus', '__far', 'keyfifo', 'readch', 'palette', 'keys[]')):
            externs.append(re.sub(r'\blong\b', 'int', ' '.join(t.split())))
    i += 1

out = '''/***************************************************************************
 *   ICORP.H   - include file for Whaven II game (the port's version)      *
 *                                                     12/15/94 Les Bird   *
 ***************************************************************************/

#include <stdarg.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "build.h"
#include "pragmas.h"
#include "cache1d.h"
#include "baselayer.h"
#include "mmulti.h"
#include "names.h"
#include "sndmod.h"
#include "les.h"
#include "jeff.h"
#include "raf.h"
#include "whdefs.h"
#include "keydefs.h"
#include "wh2cntrl.h"

#define NUMOPTIONS 12
#define XDIM    320
#define YDIM    200
#define MAXMIRRORS 64
#ifndef O_BINARY
#define O_BINARY 0
#endif

''' + body + '''

// the game's globals (extern.h of the DOS source)
''' + NL.join(externs) + '''
extern int keys[];

// prototypes of the game's functions (the .def files of the DOS source)
''' + NL.join(protos) + '''

// the port's part (JonoF's modules and the main module's replacements)
extern int xdimgame, ydimgame, bppgame, forcesetup, crashflag;
extern char option[];
extern int digihz[];
void rp_delay(int goal);
void wh2_nextpage(void);
#define nextpage() wh2_nextpage()
void shutdowngame(void);
void crash(char *fmt, ...);
void crashgame(char *fmt, ...);
void setviewport(int screensize);
void setup3dscreen(void);
void clearpal(void);
void getpalette(char *pal);
void fillpalette(int red, int green, int blue);
int smkplayseq(char *name);
// b5compat.c (JonoF's)
void permanentwritesprite(int thex, int they, short tilenum, signed char shade, int cx1, int cy1, int cx2, int cy2, char dapalnum);
void overwritesprite(int thex, int they, short tilenum, signed char shade, char stat, char dapalnum);
void SND_DoBuffers(void);
void SND_UnDoBuffers(void);
void SND_Startup(void);
void SND_Shutdown(void);
void SND_Mixer(unsigned short wSource, unsigned short wVolume);
void SND_MenuMusic(int choose);
void SND_StartMusic(unsigned short level);
void SND_SongFlush(void);
void SND_FadeMusic(void);
void SND_Sting(unsigned short sound);
int SND_PlaySound(unsigned short sound, int x, int y, unsigned short Pan, int looptics);
int SND_Sound(unsigned short sound);
void SND_CheckLoops(void);
void SND_StopLoop(int which);
void SND_DIGIFlush(void);
void playsound_loc(int soundnum, int xplc, int yplc);
// the control settings of CONTROLS.CFG (readcfg.c)
extern char configFile[];
extern short configKeyboard[], configMouse[], configJoystick[], configGamepad[], configWingman[], configAvenger[];
void readControlConfigs(int which);
void setOptions(void);
#ifdef MISTER_HYBRID
#include "mister_port.h"
#endif
'''
open(os.path.join(tree, 'src', 'icorp.h'), 'w', encoding='latin1', newline=NL).write(out)

# the game's globals: the definitions of extern.h's "#ifdef WHAVEN" branches
# (the DOS main module defined WHAVEN; the engine has keystatus, the port
# whaven.c has the SVGA branch)
defs = []
depth = 0
for line in lines:
    t = line.strip()
    if t.startswith('#ifdef WHAVEN'):
        depth = 1
        continue
    if depth and (t.startswith('#else') or t.startswith('#endif')):
        depth = 0
        continue
    if depth and t and not any(k in t for k in ('keystatus', 'keyfifo', 'readch', '__far', 'batsnd', 'cartsnd', 'lavasnd')):
        defs.append(re.sub(r'\blong\b', 'int', line.rstrip()))
glob = '// the game globals of the DOS source (extern.h, the WHAVEN branches)' + NL + '#include "icorp.h"' + NL + NL + NL.join(defs) + NL
open(os.path.join(tree, 'src', 'wh2globals.c'), 'w', encoding='latin1', newline=NL).write(glob)
print('wh2globals.c:', len(defs), 'lines')
print('icorp.h:', len(protos), 'prototypes,', len(externs), 'externs')
