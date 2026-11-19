"""Witchaven II (2026-11-17): the Gameplay page of the Options menu
(thedifficulty() in whmenu.c) hung the MiSTer hard. The DOS game had a
timer interrupt; in the port the clock (totalclock, sampletimer()) and the
keys advance in handleevents(), which this page only reached through
nextpage() inside its "if (totalclock >= goaltime)" block: the clock never
got there, so the page spun forever with no input. Each turn of the loop
now pumps the events first, as wh2_nextpage() does for the other pages.
usage: patch_wh2menu.py <jfwhaven2 tree>  (idempotent)"""
import os, sys
root = sys.argv[1]

def edit(rel, pairs):
    p = os.path.join(root, rel); s = open(p, encoding='utf-8', errors='replace', newline='').read(); crlf = '\r\n' in s
    for old, new in pairs:
        if crlf: old, new = old.replace('\n', '\r\n'), new.replace('\n', '\r\n')
        if new in s: continue
        assert s.count(old) == 1, f'{rel}: {old[:60]!r} x{s.count(old)}'
        s = s.replace(old, new)
    open(p, 'w', encoding='utf-8', newline='').write(s); print('patched', rel)

edit('src/whmenu.c', [
    ('''     goaltime = totalclock + SCOTTSPEED;

     while (!exit) {

          if (totalclock >= goaltime) {
               goaltime = totalclock + SCOTTSPEED;
               cnt++;
               if (cnt > 2)
                    cnt = 0;

               if (keystatus[keys[KEYFWD]] > 0 || keystatus[RUP] > 0) {
                    TEMPSND();
                    select2 = 0;
               }
''', '''     goaltime = totalclock + SCOTTSPEED;

     while (!exit) {
          // the clock and the keys (the DOS game's timer interrupt): without
          // this the page never reaches its timed block and hangs (MiSTer)
          handleevents();

          if (totalclock >= goaltime) {
               goaltime = totalclock + SCOTTSPEED;
               cnt++;
               if (cnt > 2)
                    cnt = 0;

               if (keystatus[keys[KEYFWD]] > 0 || keystatus[RUP] > 0) {
                    TEMPSND();
                    select2 = 0;
               }
'''),
])
print('wh2 menu OK')
