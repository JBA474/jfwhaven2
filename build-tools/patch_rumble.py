"""MiSTer: a rumble service in the engine layer (jfbuild) for the jfSW-core
ports. A game asks for a burst, misterrumble(strong, weak, ms); the engine
keeps the hybrid library's MH_Rumble() fed every frame it presents while the
burst lasts (the library stops the motors by itself two seconds after the
last call) and stops it when it is over.
usage: patch_rumble.py <game root with jfbuild/>
"""
import os, sys

root = sys.argv[1]

def edit(rel, pairs):
    p = os.path.join(root, rel)
    s = open(p, encoding='utf-8', errors='replace', newline='').read()
    crlf = chr(13) + chr(10) in s
    for old, new in pairs:
        if crlf:
            old, new = old.replace(chr(10), chr(13) + chr(10)), new.replace(chr(10), chr(13) + chr(10))
        if new in s:
            continue
        assert s.count(old) == 1, f'{rel}: {old[:50]!r} x{s.count(old)}'
        s = s.replace(old, new)
    open(p, 'w', encoding='utf-8', newline='').write(s)
    print('patched', rel)

edit('jfbuild/src/sdlayer2.c', [
    ('''static int misterahead;
''', '''static int misterahead;

// Rumble: a game asks for a burst (misterrumble); the engine keeps the
// library's MH_Rumble() fed every frame it presents while the burst lasts and
// stops the motors when it is over
static int misterrumblestrong, misterrumbleweak;
static Uint32 misterrumbleuntil;
int MH_Rumble(int strong, int weak); // mister_hybrid.h, included further down
void misterrumble(int strong, int weak, int ms)
{
	Uint32 now = SDL_GetTicks();
	if (strong > misterrumblestrong) misterrumblestrong = strong;
	if (weak > misterrumbleweak) misterrumbleweak = weak;
	if (!misterrumbleuntil || (Sint32)(now + ms - misterrumbleuntil) > 0) misterrumbleuntil = now + ms;
	if (!misterrumbleuntil) misterrumbleuntil = 1;
	{
		static int said;
		int any = MH_Rumble(misterrumblestrong, misterrumbleweak);
		if (!said) {
			said = 1;
			buildprintf("mister: rumble %d/%d for %d ms: %s\\n", misterrumblestrong, misterrumbleweak, ms,
				any ? "a pad with rumble" : "no pad with rumble");
		}
	}
}
static void misterrumbletick(void)
{
	if (!misterrumbleuntil) return;
	if ((Sint32)(SDL_GetTicks() - misterrumbleuntil) >= 0) {
		misterrumblestrong = misterrumbleweak = 0;
		misterrumbleuntil = 0;
		MH_Rumble(0, 0);
	} else {
		MH_Rumble(misterrumblestrong, misterrumbleweak);
	}
}
'''),
    ('''	if (misterahead && !MH_Interlaced()) {
''', '''	misterrumbletick();
	if (misterahead && !MH_Interlaced()) {
'''),
])

edit('jfbuild/include/baselayer.h', [
    ('''int handleevents(void);
''', '''int handleevents(void);
// MiSTer: the gamepad's motors for ms (0..255 each); the engine keeps them going
void misterrumble(int strong, int weak, int ms);
'''),
])
