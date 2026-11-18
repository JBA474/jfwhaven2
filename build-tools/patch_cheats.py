"""MiSTer: the core's OSD Cheats page (jfSW.sv: God Mode bit 33, No Clipping
bit 34, the triggers Give All T56 and Next Level T57) wired into the jfSW-core
ports. The engine layer reads the page (mistercheatbits, in sdlayer2.c, logged
on every change); each game applies it once a frame while a level plays, as
the Shadow Warrior port does: a mode on the page holds while it is on, turning
it off turns it off, and the game's own cheat codes still work while it is off;
a trigger does its thing once each time it is picked.
usage: patch_cheats.py <game root with jfbuild/> <jftekwar|jfwhaven|jfwhaven2|jfcorr8>
"""
import os, sys

root, game = sys.argv[1], sys.argv[2]

def edit(rel, pairs):
    p = os.path.join(root, rel)
    s = open(p, encoding='utf-8', errors='replace', newline='').read()
    crlf = chr(13) + chr(10) in s
    for old, new in pairs:
        if crlf:
            old, new = old.replace(chr(10), chr(13) + chr(10)), new.replace(chr(10), chr(13) + chr(10))
        if new in s:
            continue
        assert s.count(old) == 1, f'{rel}: {old[:60]!r} x{s.count(old)}'
        s = s.replace(old, new)
    open(p, 'w', encoding='utf-8', newline='').write(s)
    print('patched', rel)

def append(rel, text):
    p = os.path.join(root, rel)
    s = open(p, encoding='utf-8', errors='replace', newline='').read()
    if text.strip()[:40] in s:
        return
    if not s.endswith(chr(10)):
        s += chr(10)
    open(p, 'w', encoding='utf-8', newline='').write(s + text)
    print('appended', rel)

# --- the engine: the page read once a frame by a game ---------------------
edit('jfbuild/src/sdlayer2.c', [
    ('''static void misterfollowresolution(void)
{
''', '''// The OSD's Cheats page (jfSW.sv): God Mode (status bit 33) and No Clipping
// (bit 34) as they stand, the triggers Give All (T56) and Next Level (T57) as
// bits 0 and 1 of the result, once each time one is picked. A game reads it
// once a frame; a change is logged.
int mistercheatbits(int *god, int *clip)
{
	static int lastgod = -1, lastclip = -1, lastopen = -1;
	static uint64_t laststatus, lasthigh;
	uint64_t status = MH_IsOpen() ? MH_OSDStatus() : 0;
	uint64_t high = MH_IsOpen() ? MH_OSDStatusHigh() : 0;
	int triggers = MH_IsOpen() ? MH_OSDTriggers() : 0;
	int open = MH_IsOpen() ? MH_OSDOpen() : 0;

	*god = MH_OSD_GAME_BITS(status, 33, 1);
	*clip = MH_OSD_GAME_BITS(status, 34, 1);
	if (open != lastopen) { // the OSD's menu shown or hidden (the game's input is held back meanwhile)
		buildprintf("mister: osd %s\\n", open ? "open" : "closed");
		lastopen = open;
	}
	if (status != laststatus || high != lasthigh || *god != lastgod || *clip != lastclip || triggers) {
		buildprintf("mister: cheats god %d noclip %d triggers %x (status %08x%08x high %08x%08x)\\n", *god, *clip, triggers,
			(unsigned)(status >> 32), (unsigned)status, (unsigned)(high >> 32), (unsigned)high);
		laststatus = status;
		lasthigh = high;
		lastgod = *god;
		lastclip = *clip;
	}
	return triggers;
}

static void misterfollowresolution(void)
{
'''),
])
edit('jfbuild/include/baselayer.h', [
    ('''void misterrumble(int strong, int weak, int ms);
''', '''void misterrumble(int strong, int weak, int ms);
// MiSTer: the OSD's Cheats page: God Mode and No Clipping as they stand, the
// triggers (bit 0 Give All, bit 1 Next Level) once each time one is picked
int mistercheatbits(int *god, int *clip);
'''),
])

# A mode of the page: on, it holds; turned off, it turns the game's off; off
# and unchanged, the game's own cheat stays as it is.
def mode(var, god):
    return f'''	if ({god}) {var} = 1;
	else if (last{god} > 0) {var} = 0;
	last{god} = {god};
'''

if game == 'jftekwar':
    append('src/tekmsc.c', '''
#ifdef MISTER_HYBRID
// The core's OSD Cheats page, once a frame while a level plays (tekgame.c)
int misternoclip;
void mistercheats(int playing)
{
	static int lastgod = -1, lastclip = -1;
	int god, clip, i, p = myconnectindex;
	int triggers = mistercheatbits(&god, &clip);

	if (!playing) {
		lastgod = lastclip = -1;
		return;
	}
	if (lastgod >= 0 && god != lastgod) showmessage("GOD MODE %s", god ? "ON" : "OFF");
''' + mode('toggles[TOGGLE_GODMODE]', 'god') + '''	if (lastclip >= 0 && clip != lastclip) showmessage("NO CLIPPING %s", clip ? "ON" : "OFF");
	lastclip = misternoclip = clip;
	if (triggers & 1) { // Give All: every gun, full ammo, full health
		weapons[p] = flags32[GUN1FLAG] | flags32[GUN2FLAG] | flags32[GUN3FLAG] | flags32[GUN4FLAG]
			| flags32[GUN5FLAG] | flags32[GUN6FLAG] | flags32[GUN7FLAG] | flags32[GUN8FLAG];
		ammo1[p] = ammo2[p] = ammo3[p] = ammo4[p] = ammo5[p] = ammo6[p] = ammo7[p] = ammo8[p] = MAXAMMO;
		health[p] = MAXHEALTH;
		showmessage("GIVE ALL");
	}
	if (triggers & 2) { // Next Level: the mission's next map after this one
		int cur = -1, next = -1;
		for (i = 0; i < MAXMAPSPERMISSION; i++)
			if (missionset[mission][i] == currentmapno) { cur = i; break; }
		for (i = cur + 1; i < MAXMAPSPERMISSION; i++) {
			int mn = missionset[mission][i];
			if (mn >= 4 && accessiblemap(mn)) { next = mn; break; }
		}
		if (next >= 0) newmap(next);
		else showmessage("NO NEXT MAP IN THIS MISSION");
	}
}
#endif
''')
    edit('src/tekgame.c', [
        ('''char toggles[MAXTOGGLES] = { 1,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0 };
''', '''char toggles[MAXTOGGLES] = { 1,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0 };
#ifdef MISTER_HYBRID
void mistercheats(int playing); // tekmsc.c: the core's OSD Cheats page
extern int misternoclip;
#endif
'''),
        ('''        handleevents();
        OSD_DispatchQueued();
          while( movefifoplc != movefifoend ) {
''', '''        handleevents();
        OSD_DispatchQueued();
#ifdef MISTER_HYBRID
        mistercheats(activemenu == 0 && option[4] == 0);
#endif
          while( movefifoplc != movefifoend ) {
'''),
        ('''          clipmove(&posx[snum],&posy[snum],&posz[snum],&cursectnum[snum],xvect,yvect,128L,4<<8,4<<8,CLIPMASK0);
          frameinterpolate = 1;
''', '''#ifdef MISTER_HYBRID
          if (misternoclip) { // the OSD's No Clipping: through the walls
               __typeof__(cursectnum[snum]) ns = cursectnum[snum];
               posx[snum] += xvect >> 14;
               posy[snum] += yvect >> 14;
               updatesector(posx[snum], posy[snum], &ns);
               if (ns >= 0) cursectnum[snum] = ns;
          }
          else
#endif
          clipmove(&posx[snum],&posy[snum],&posz[snum],&cursectnum[snum],xvect,yvect,128L,4<<8,4<<8,CLIPMASK0);
          frameinterpolate = 1;
'''),
        ('''     if( pushmove(&posx[snum],&posy[snum],&posz[snum],&cursectnum[snum],128L,4<<8,4<<8,CLIPMASK0) < 0 ) {
''', '''     if( !misternoclip && pushmove(&posx[snum],&posy[snum],&posz[snum],&cursectnum[snum],128L,4<<8,4<<8,CLIPMASK0) < 0 ) {
'''),
    ])

elif game == 'jfwhaven':
    append('src/whinp.c', '''
#ifdef MISTER_HYBRID
// The core's OSD Cheats page, once a frame while a level plays (whaven.c).
// God Mode: healthpic() (whplr.c) ignores a hit while mistergod is on.
int mistergod, misternoclip;
extern int horizvel, justteleported;
void mistercheats(int playing)
{
	static int lastgod = -1, lastclip = -1;
	int god, clip;
	int triggers = mistercheatbits(&god, &clip);
	struct player *plr = &player[pyrn];

	if (!playing) {
		lastgod = lastclip = -1;
		return;
	}
	if (lastgod >= 0 && god != lastgod) { strcpy(displaybuf, god ? "GOD MODE ON" : "GOD MODE OFF"); displaytime = 360; }
	lastgod = mistergod = god;
	if (god && plr->health < plr->maxhealth) healthpic(plr->maxhealth - plr->health);
	if (lastclip >= 0 && clip != lastclip) { strcpy(displaybuf, clip ? "NO CLIPPING ON" : "NO CLIPPING OFF"); displaytime = 360; }
	lastclip = misternoclip = clip;
	if (triggers & 1) { // Give All: the game's weapon, potion and spell cheats
		strcpy(displaybuf, "SCOOTER"); checkcheat();
		strcpy(displaybuf, "MOMMY"); checkcheat();
		strcpy(displaybuf, "WANGO"); checkcheat();
		strcpy(displaybuf, "GIVE ALL"); displaytime = 360;
	}
	if (triggers & 2) { // Next Level: as the level's exit
		if (mapon < numlevels) {
			justteleported = 1;
			mapon++;
			vel = 0; angvel = 0; svel = 0; horizvel = 0;
			playsound_loc(S_CHAINDOOR1, plr->x, plr->y);
			loadnewlevel(mapon);
			warpfxsprite(plr->spritenum);
			plr->treasure[14] = plr->treasure[15] = plr->treasure[16] = plr->treasure[17] = 0;
			plr->treasure[4] = plr->treasure[5] = 0;
			SND_CheckLoops();
		}
		else { strcpy(displaybuf, "LAST LEVEL"); displaytime = 360; }
	}
}
#endif
''')
    edit('src/whinp.c', [
        ('''extern int  displaytime;
''', '''extern int  displaytime;
#ifdef MISTER_HYBRID
extern int misternoclip; // below: the OSD's No Clipping
#endif
'''),
        ('''		clipmove(&plr->x,&plr->y,&plr->z,&plr->sector,xvect,yvect,128L,4<<8,4<<8,CLIPMASK0);

		if (plr->sector!=plr->lastsndsector) {
''', '''#ifdef MISTER_HYBRID
		if (misternoclip) { // the OSD's No Clipping: through the walls
			__typeof__(plr->sector) ns = plr->sector;
			plr->x += xvect >> 14;
			plr->y += yvect >> 14;
			updatesector(plr->x, plr->y, &ns);
			if (ns >= 0) plr->sector = ns;
		}
		else
#endif
		clipmove(&plr->x,&plr->y,&plr->z,&plr->sector,xvect,yvect,128L,4<<8,4<<8,CLIPMASK0);

		if (plr->sector!=plr->lastsndsector) {
'''),
    ])
    edit('src/whplr.c', [
        ('''extern char displaybuf[];
''', '''extern char displaybuf[];
#ifdef MISTER_HYBRID
extern int mistergod; // whinp.c: the OSD's God Mode
#endif
'''),
        ('''        misterrumble(strong > 255 ? 255 : strong, 60, 220);
    }
#endif
	plr->health+=hp;
''', '''        misterrumble(strong > 255 ? 255 : strong, 60, 220);
    }
    if (hp < 0 && mistergod) hp = 0; // the OSD's God Mode: no harm done
#endif
	plr->health+=hp;
'''),
    ])
    edit('src/whaven.c', [
        ('''int gameactivated=0;
''', '''int gameactivated=0;
#ifdef MISTER_HYBRID
void mistercheats(int playing); // whinp.c: the core's OSD Cheats page
#endif
'''),
        ('''        handleevents();
        OSD_DispatchQueued();

        if(gameactivated == 0 || escapetomenu == 1) {
''', '''        handleevents();
        OSD_DispatchQueued();
#ifdef MISTER_HYBRID
        mistercheats(gameactivated == 1 && escapetomenu == 0);
#endif

        if(gameactivated == 0 || escapetomenu == 1) {
'''),
    ])

elif game == 'jfwhaven2':
    append('src/whinp.c', '''
#ifdef MISTER_HYBRID
// The core's OSD Cheats page, once a frame while a level plays (whaven.c).
// God Mode is the game's own (godmode: healthpic() keeps 200 health).
int misternoclip;
extern int displaytime;
void mistercheats(int playing)
{
	static int lastgod = -1, lastclip = -1;
	int god, clip;
	int triggers = mistercheatbits(&god, &clip);
	struct player *plr = &player[pyrn];

	if (!playing) {
		lastgod = lastclip = -1;
		return;
	}
	if (lastgod >= 0 && god != lastgod) { strcpy(displaybuf, god ? "GOD MODE ON" : "GOD MODE OFF"); displaytime = 360; }
''' + mode('godmode', 'god') + '''	if (god && plr->health < 200) healthpic(0);
	if (lastclip >= 0 && clip != lastclip) { strcpy(displaybuf, clip ? "NO CLIPPING ON" : "NO CLIPPING OFF"); displaytime = 360; }
	lastclip = misternoclip = clip;
	if (triggers & 1) { // Give All: the game's weapon, potion and spell cheats
		strcpy(displaybuf, "WEAPONS"); checkcheat();
		strcpy(displaybuf, "POTIONS"); checkcheat();
		strcpy(displaybuf, "SPELLS"); checkcheat();
		strcpy(displaybuf, "GIVE ALL"); displaytime = 360;
	}
	if (triggers & 2) { // Next Level: as the game's level warp cheats
		buildprintf("mister: next level: map %d of 15\\n", mapon);
		if (mapon < 15) {
			mapon++;
			justteleported = 1;
			vel = 0; angvel = 0; svel = 0;
			playertorch = 0;
			playsound_loc(S_WARP, plr->x, plr->y);
			loadnewlevel(mapon);
			warpfxsprite(plr->spritenum);
			plr->treasure[14] = plr->treasure[15] = plr->treasure[16] = 0;
			plr->treasure[17] = plr->treasure[4] = plr->treasure[5] = 0;
			SND_CheckLoops();
		}
		else { strcpy(displaybuf, "LAST LEVEL"); displaytime = 360; }
	}
}
#endif
''')
    edit('src/whinp.c', [
        ('''extern int reccnt;
''', '''extern int reccnt;
#ifdef MISTER_HYBRID
extern int misternoclip; // below: the OSD's No Clipping
#endif
'''),
        ('''          clipmove(&plr->x, &plr->y, &plr->z, &plr->sector, xvect, yvect, 128L, 4 << 8, 4 << 8, 0);
''', '''#ifdef MISTER_HYBRID
          if (misternoclip) { // the OSD's No Clipping: through the walls
               __typeof__(plr->sector) ns = plr->sector;
               plr->x += xvect >> 14;
               plr->y += yvect >> 14;
               updatesector(plr->x, plr->y, &ns);
               if (ns >= 0) plr->sector = ns;
          }
          else
#endif
          clipmove(&plr->x, &plr->y, &plr->z, &plr->sector, xvect, yvect, 128L, 4 << 8, 4 << 8, 0);
'''),
    ])
    edit('src/whaven.c', [
        ('''int  gameactivated = 0;
''', '''int  gameactivated = 0;
#ifdef MISTER_HYBRID
void mistercheats(int playing); // whinp.c: the core's OSD Cheats page
#endif
'''),
        ('''          OSD_DispatchQueued();
          if (gameactivated == 0 || escapetomenu == 1) {
''', '''          OSD_DispatchQueued();
#ifdef MISTER_HYBRID
          mistercheats(gameactivated == 1 && escapetomenu == 0);
#endif
          if (gameactivated == 0 || escapetomenu == 1) {
'''),
    ])

elif game == 'jfcorr8':
    append('src/corr7gam.c', '''
#ifdef MISTER_HYBRID
// The core's OSD Cheats page, once a frame of the game (GAM_doFrame)
int misternoclip;
void mistercheats(void)
{
	static int lastgod = -1, lastclip = -1;
	int god, clip, t;
	int triggers = mistercheatbits(&god, &clip);
	short ps = PLR_getPlayerSprite(myconnectindex);

	if (lastgod >= 0 && god != lastgod) EFF_displayMessage("GOD MODE %s", god ? "ON" : "OFF");
''' + mode('cheatGodMode', 'god') + '''	if (lastclip >= 0 && clip != lastclip) EFF_displayMessage("NO CLIPPING %s", clip ? "ON" : "OFF");
	lastclip = misternoclip = clip;
	if (triggers & 1) { // Give All: every ammo type full, full health
		for (t = 0; t < MAXWEAPONTYPES; t++) WEP_setAmmo(ps, t, WEP_getMaxAmmo(ps, t));
		ACT_setHealth(ps, playerHealth);
		EFF_displayMessage("GIVE ALL");
	}
	if (triggers & 2) GAM_nextLevel();
}
#endif
''')
    edit('src/corr7gam.c', [
        ('''     GAM_setMadeNoise(-1);
     GAM_checkCheats();
''', '''     GAM_setMadeNoise(-1);
     GAM_checkCheats();
#ifdef MISTER_HYBRID
     mistercheats();
#endif
'''),
        ('''int gotLastPacketClock;
''', '''int gotLastPacketClock;
#ifdef MISTER_HYBRID
void mistercheats(void); // below: the core's OSD Cheats page
#endif
'''),
    ])
    edit('src/corr7plr.c', [
        ('''void
PLR_movePlayer(short p)
''', '''#ifdef MISTER_HYBRID
extern int misternoclip; // corr7gam.c: the OSD's No Clipping
#endif
void
PLR_movePlayer(short p)
'''),
        ('''          clipmove(&x,&y,&z,&sect,xvect,yvect,clipdist,clipheight,clipheight,0);
     }
''', '''#ifdef MISTER_HYBRID
          if (misternoclip && s == PLR_getPlayerSprite(myconnectindex)) { // through the walls
               __typeof__(sect) ns = sect;
               x += xvect >> 14;
               y += yvect >> 14;
               updatesector(x, y, &ns);
               if (ns >= 0) sect = ns;
          }
          else
#endif
          clipmove(&x,&y,&z,&sect,xvect,yvect,clipdist,clipheight,clipheight,0);
     }
'''),
        ('''     if (pushmove(&x,&y,&z,&sect,clipdist,clipheight,clipheight,0) < 0) {
''', '''     if (!(misternoclip && s == PLR_getPlayerSprite(myconnectindex)) && pushmove(&x,&y,&z,&sect,clipdist,clipheight,clipheight,0) < 0) {
'''),
    ])
else:
    sys.exit('unknown game ' + game)
print('cheats OK', game)
