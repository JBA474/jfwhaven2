"""Witchaven II on the MiSTer (2026-11-19): the menu's Help (whmenu.c help()) locked the game:
its loop never calls handleevents(), so no key reaches it, and goaltime starts uninitialized.
The cheat-name display (whinp.c) spins on Esc the same way. Both poll now.
usage: patch_wh2help.py <jfwhaven2 tree>  (idempotent)"""
import os, sys
root = sys.argv[1]
def edit(rel, pairs):
    p = os.path.join(root, rel); s = open(p, encoding='utf-8', errors='replace').read(); n = 0
    for old, new in pairs:
        if new in s: continue
        assert s.count(old) == 1, (rel, old[:50], s.count(old))
        s = s.replace(old, new); n += 1
    if n: open(p, 'w', encoding='utf-8', newline='\n').write(s)
    print(rel, 'patched %d' % n if n else 'already')
edit('src/whmenu.c', [
    ('''     int  select = 0;
     int goaltime;
     int  exit = 0;

     while (!exit) {
          if (totalclock >= goaltime) {
''', '''     int  select = 0;
     int goaltime = 0;   // MiSTer: was uninitialized (patch_wh2help.py)
     int  exit = 0;

     while (!exit) {
          handleevents();   // MiSTer: the keys and the pad arrive here (patch_wh2help.py)
          if (totalclock >= goaltime) {
'''),
])
edit('src/whinp.c', [
    ('''          keystatus[0x01]=0;
          while (keystatus[0x01] == 0);
          keystatus[0x01]=0;
''', '''          keystatus[0x01]=0;
          while (keystatus[0x01] == 0) handleevents();   // MiSTer (patch_wh2help.py)
          keystatus[0x01]=0;
'''),
])
