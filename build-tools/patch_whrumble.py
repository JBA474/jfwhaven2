"""MiSTer: rumble in Witchaven and Witchaven II (after patch_rumble.py in the
engine): a light burst when the player's weapon goes off (shootgun) and a
heavy one when the player loses health (healthpic with a negative amount),
stronger for a bigger hit.
usage: patch_whrumble.py <jfwhaven|jfwhaven2>
"""
import os, re, sys

root = sys.argv[1]
p = os.path.join(root, 'src', 'whplr.c')
s = open(p, encoding='utf-8', errors='replace', newline='').read()
nl = '\r\n' if '\r\n' in s else '\n'

if 'misterrumble' not in s:
    # shootgun: the first statement of the function (the two ports lay the
    # signature out differently)
    s, n = re.subn(r'(shootgun\s*\(\s*struct player\s*\*\s*plr,\s*short daang,\s*char guntype\s*\)\s*\{' + nl + ')',
                   r'\1#ifdef MISTER_HYBRID' + nl + '    misterrumble(0, 150, 110); // MiSTer: the weapon' + nl + '#endif' + nl, s, count=1)
    assert n == 1, 'shootgun not found'
    # healthpic: after plr is set
    m = re.search(r'healthpic\s*\(\s*int hp\s*\)\s*\{.*?plr\s*=\s*&player\[pyrn\];' + nl, s, flags=re.S)
    assert m, 'healthpic not found'
    hook = ('#ifdef MISTER_HYBRID' + nl +
            '    if (hp < 0) { // MiSTer: hit; harder for a bigger one' + nl +
            '        int strong = 90 - hp * 5;' + nl +
            '        misterrumble(strong > 255 ? 255 : strong, 60, 220);' + nl +
            '    }' + nl + '#endif' + nl)
    s = s[:m.end()] + hook + s[m.end():]
    open(p, 'w', encoding='utf-8', newline='').write(s)
    print('patched src/whplr.c')
else:
    print('already patched')
