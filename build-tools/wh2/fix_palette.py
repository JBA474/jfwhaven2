"""port_wh2.py: the engine's setpalette() is a no-op in jfbuild (the palette
is curpalettefaded, set through setbrightness()), so the DOS game's palette
writes go through setbrightness, and the port sets the palette at startup
as JonoF's Witchaven does."""
import os
p = os.path.join(os.path.dirname(os.path.abspath(__file__)), 'port_wh2.py')
s = open(p, encoding='utf-8').read()
BS = chr(92)
pairs = [
    ("void clearpal(void) {\n    static unsigned char black[768];\n    setpalette(0, 256, black);\n}",
     "void clearpal(void) {\n    fillpalette(0, 0, 0);\n}"),
    ("    for (i = 0; i < 256; i++) { pal[i*3] = red; pal[i*3+1] = green; pal[i*3+2] = blue; }\n    setpalette(0, 256, pal);",
     "    for (i = 0; i < 256; i++) { pal[i*3] = red; pal[i*3+1] = green; pal[i*3+2] = blue; }\n    setbrightness(brightness, pal, 0);"),
    ("r'setpalette(0, 256, (unsigned char *)" + BS + "1);'",
     "r'setbrightness(brightness, (unsigned char *)" + BS + "1, 0);'"),
    ("        smkplayseq(\"intro\");\n    }\n    SND_MenuMusic(0);",
     "        smkplayseq(\"intro\");\n    }\n    setbrightness(brightness, palette, 0);\n    SND_MenuMusic(0);"),
    ("    setup3dscreen();\n    SND_Startup();",
     "    setup3dscreen();\n    setbrightness(brightness, palette, 0);\n    SND_Startup();"),
]
for old, new in pairs:
    if new in s:
        continue
    assert old in s, old[:60]
    s = s.replace(old, new)
open(p, 'w', encoding='utf-8', newline='\n').write(s)
print('palette fix in place')
