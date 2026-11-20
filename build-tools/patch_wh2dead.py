"""Witchaven II on the MiSTer (2026-11-19): after the player dies, playerdead() (whplr.c) waits
in two loops for Fire, Use, Esc or F9, then Space, without ever calling handleevents(), so
no key or pad press reaches keystatus[] and the game sits on the death view for good (Jess:
"when the character dies, there is no option to restart game"). Witchaven's playerdead()
calls handleevents() in its loops; this adds the same here.
usage: patch_wh2dead.py <jfwhaven2 tree>  (idempotent)"""
import os, sys
root = sys.argv[1]
p = os.path.join(root, 'src/whplr.c'); s = open(p, encoding='utf-8', errors='replace').read()
pairs = [
    ('''     while (keystatus[keys[KEYFIRE]] == 0 && keystatus[keys[KEYUSE]] == 0
            && keystatus[1] == 0 && keystatus[0x43] == 0) {
          getzrange(plr->x, plr->y, plr->z, plr->sector,
''', '''     while (keystatus[keys[KEYFIRE]] == 0 && keystatus[keys[KEYUSE]] == 0
            && keystatus[1] == 0 && keystatus[0x43] == 0) {
          handleevents();   // MiSTer: the keys and the pad arrive here (patch_wh2dead.py)
          getzrange(plr->x, plr->y, plr->z, plr->sector,
'''),
    ('''     while (!exit) {
          if (totalclock > goaltime)
               exit = 1;
          if (keystatus[0x39] > 0)
               exit = 1;
     }
''', '''     while (!exit) {
          handleevents();   // MiSTer (patch_wh2dead.py)
          if (totalclock > goaltime)
               exit = 1;
          if (keystatus[0x39] > 0)
               exit = 1;
     }
'''),
]
n = 0
for old, new in pairs:
    if new in s: continue
    assert s.count(old) == 1, (old[:60], s.count(old))
    s = s.replace(old, new); n += 1
if n: open(p, 'w', encoding='utf-8', newline='\n').write(s)
print('patched whplr.c (%d loops)' % n if n else 'already')
