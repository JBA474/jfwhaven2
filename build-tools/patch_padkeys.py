"""MiSTer (TekWar, Witchaven): the pad works the games' own menus.

The games' menus and mission screens read the keyboard (keystatus[]) and
nothing else, and the SS1 has no keyboard. In the engine's joystick update
(jfbuild/src/sdlayer2.c, the MiSTer branch) the pad's d-pad presses the
arrow keys, Menu OK and Start press Enter, Menu Back presses Escape: Build
scan codes, as the keyboard driver sets them.
usage: patch_padkeys.py <jfbuild>
"""
import os, sys

p = os.path.join(sys.argv[1], 'src/sdlayer2.c')
s = open(p, encoding='utf-8', errors='replace').read()
old = '''	if (hat & SDL_HAT_RIGHT) b |= 1 << MISTERJOY_DPADRIGHT;
	joyb = b;
}
'''
new = '''	if (hat & SDL_HAT_RIGHT) b |= 1 << MISTERJOY_DPADRIGHT;
	joyb = b;

	// The pad as keys for the game's own menus (they read keystatus[] alone):
	// the d-pad is the arrow keys, Menu OK and Start are Enter, Menu Back is
	// Escape. Build scan codes, pressed and released with the buttons.
	{
		static const struct { int bit, key; } padkeys[] = {
			{ MISTERJOY_DPADUP, 0xc8 }, { MISTERJOY_DPADDOWN, 0xd0 },
			{ MISTERJOY_DPADLEFT, 0xcb }, { MISTERJOY_DPADRIGHT, 0xcd },
			{ MISTERJOY_MENUOK, 0x1c }, { MISTERJOY_ACTION1 + 7, 0x1c },
			{ MISTERJOY_MENUBACK, 0x01 },
		};
		static int lastb = 0;
		int changed = b ^ lastb, k;

		for (k = 0; k < (int)(sizeof padkeys / sizeof padkeys[0]); k++) {
			if (changed & (1 << padkeys[k].bit)) {
				int down = (b >> padkeys[k].bit) & 1;
				// two buttons on one key: released only when both are
				if (!down && padkeys[k].key == 0x1c &&
				    (b & ((1 << MISTERJOY_MENUOK) | (1 << (MISTERJOY_ACTION1 + 7)))))
					continue;
				keystatus[padkeys[k].key] = down;
			}
		}
		lastb = b;
	}
}
'''
if new in s:
    print('already patched'); sys.exit(0)
assert s.count(old) == 1, s.count(old)
open(p, 'w', encoding='utf-8', newline='\n').write(s.replace(old, new))
print('patched')
