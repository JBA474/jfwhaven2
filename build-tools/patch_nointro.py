"""MISTER_HYBRID_NOINTRO=1 (in the set's eduke32_env.sh or fury_env.sh, which
the launcher sources): the startup videos are skipped, the game opens on its
title or menu. Jess asked for a way to bypass all startup videos (2026-11-17).
- Witchaven: intro.smk (whaven.c)
- Witchaven II: smk/intro.smk (whaven.c; the endings still play)
- TekWar: tvopen / tekndie / tekintro / intro (tekgame.c; the Smacker main
  menu is not a video and stays)
- EDuke32 (Ion Fury, NAM, WWII GI, Zombie Crisis, DukePlus, A.W.O.L.): the
  logo animations and the title intro, as the -nologo option (game.cpp)
Corridor 8's Smacker player is a stub and Corridor 7's CD animations are
not shipped, so nothing to skip there.
usage: patch_nointro.py <tree> <jfwhaven|jfwhaven2|jftekwar|eduke32>  (idempotent)"""
import os, sys
root, game = sys.argv[1], sys.argv[2]

def edit(rel, pairs):
    p = os.path.join(root, rel); s = open(p, encoding='utf-8', errors='replace', newline='').read(); crlf = '\r\n' in s
    for old, new in pairs:
        if crlf: old, new = old.replace('\n', '\r\n'), new.replace('\n', '\r\n')
        if new in s: continue
        assert s.count(old) == 1, f'{rel}: {old[:60]!r} x{s.count(old)}'
        s = s.replace(old, new)
    open(p, 'w', encoding='utf-8', newline='').write(s); print('patched', rel)

NOINTRO = '(getenv("MISTER_HYBRID_NOINTRO") && atoi(getenv("MISTER_HYBRID_NOINTRO")))'

if game == 'jfwhaven':
    edit('src/whaven.c', [
        ('''    smkplayseq("intro");
    setbrightness(brightness, palette, 0);
''', '''    // MISTER_HYBRID_NOINTRO=1 (the set's eduke32_env.sh): no intro video
    if (!%s)
        smkplayseq("intro");
    setbrightness(brightness, palette, 0);
''' % NOINTRO),
    ])
elif game == 'jfwhaven2':
    edit('src/whaven.c', [
        ('''    if (netgame == 0) {
        smkplayseq("intro");
    }
''', '''    // MISTER_HYBRID_NOINTRO=1 (the set's eduke32_env.sh): no intro video
    if (netgame == 0 && !%s) {
        smkplayseq("intro");
    }
''' % NOINTRO),
    ])
elif game == 'jftekwar':
    edit('src/tekgame.c', [
        ('''     if( option[4] == 0 ) {
          i = smkplayseq("tvopen");
''', '''     // MISTER_HYBRID_NOINTRO=1 (the set's eduke32_env.sh): no intro videos
     if( option[4] == 0 && !%s ) {
          i = smkplayseq("tvopen");
''' % NOINTRO),
    ])
elif game == 'eduke32':
    edit('source/duke3d/src/game.cpp', [
        ('''    G_CheckCommandLine(argc,argv);

    // This needs to happen afterwards, as G_CheckCommandLine() is where we set
''', '''    G_CheckCommandLine(argc,argv);

#ifdef MISTER_HYBRID
    // MISTER_HYBRID_NOINTRO=1 (the set's eduke32_env.sh / fury_env.sh): the
    // logo animations and the title intro are skipped, as with -nologo
    {
        char const *e = getenv("MISTER_HYBRID_NOINTRO");
        if (e && atoi(e))
            g_noLogoAnim = g_noLogo = 1;
    }
#endif

    // This needs to happen afterwards, as G_CheckCommandLine() is where we set
'''),
    ])
else:
    sys.exit('unknown game ' + game)
print('nointro OK', game)
