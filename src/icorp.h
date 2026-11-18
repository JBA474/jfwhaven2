/***************************************************************************
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

#define   GONZOGSHXR     35
#define   GONZOGSHYR     35
#define   GONZOGHMXR     35
#define   GONZOGHMYR     35
#define   GONZOGSWXR     35
#define   GONZOGSWYR     35
#define   NEWGUYXR       35
#define   NEWGUYYR       35
#define   GONZOCSWXR     35
#define   GONZOCSWYR     35
#define   GONZOCSWATXR   35
#define   GONZOCSWATYR   35
#define   KOBOLDXR       60
#define   KOBOLDYR       60
#define   DEVILXR        50
#define   DEVILYR        50
#define   IMPXR          25
#define   IMPYR          25
#define   MINOTAURXR     64
#define   MINOTAURYR     64
#define   SKELETONXR     35
#define   SKELETONYR     35
#define   GRONHALXR      35
#define   GRONHALYR      35
#define   GRONSWXR       35
#define   GRONSWYR       35
#define   GRONMUXR       35
#define   GRONMUYR       35
#define   WILLOWXR       35
#define   WILLOWYR       35
#define   DEMONXR        38
#define   DEMONYR        41
#define   GUARDIANXR     35
#define   GUARDIANYR     35
#define   KURTXR         35
#define   KURTYR         35
#define   KATIEXR        35
#define   KATIEYR        35

extern int  swingcnt;

struct swingdoor {
     int  wall[8];
     int  sector;
     int  angopen;
     int  angclosed;
     int  angopendir;
     int  ang;
     int  anginc;
     int x[8];
     int y[8];
};

extern
struct swingdoor swingdoor[MAXSWINGDOORS];

//
// plr->flags defines
//

//
// plr->equipment flag values
//


struct player {
     int x,y,z;
     int ang;
     int horiz;
     int zoom;
     int height;
     int hvel;               
     short sector,oldsector;
     short screensize;
     short spritenum;
     char dimension;
     unsigned int flags;
     int  weapon[MAXWEAPONS],preenchantedweapon[MAXWEAPONS];
     int  ammo[MAXWEAPONS],preenchantedammo[MAXWEAPONS];
     int  orbammo[MAXNUMORBS];
     int  treasure[MAXTREASURES];
     int  orbactive[MAXNUMORBS];
     int  orb[MAXNUMORBS];
     int  potion[MAXPOTIONS];
     int  lvl;
     int score;
     int  health;
     int  maxhealth;
     int  armor;
     int  armortype;
     char onsomething;
     int fallz;
};

extern struct player player[MAXPLAYERS];

struct delayitem {
     void (*func)(int);
     int  item;
     int  timer;
};

extern
struct delayitem delayitem[MAXSECTORS];

struct sectoreffect {
     unsigned int sectorflags;
     int animate;
     int  hi,lo;
     int  delay,delayreset;
};

extern
int  secnt,
     sexref[];

extern
struct sectoreffect sectoreffect[MAXSECTORS];

extern
int  pyrn;

extern
int angvel,
     svel,
     vel;

extern
int *animateptr[],
     animategoal[],
     animatevel[],
     animatecnt,
     neartagdist,
     neartaghitdist;

extern
unsigned int flags32[];

extern
short neartagsector,
     neartagwall,
     neartagsprite;

extern
int lockclock;

void lb_printf(int,int,char *,...);

extern
char displaybuf[50],
     tempbuf[576],
     scorebuf[50],
     healthbuf[50],
     armorbuf[50],
     potionbuf[50],
     boardname[80],
     tempboard[80],
     loadgamename[80],
     scantoasc[128],
     scantoascwithshift[128],
     typemessage[162],
     fancy[41];

#define   ARROWCOUNTLIMIT     100
#define   THROWPIKELIMIT      100

extern
short arrowsprite[ARROWCOUNTLIMIT],
     throwpikesprite[THROWPIKELIMIT];



// the game's globals (extern.h of the DOS source)
extern int sparksx;
extern int sparksy;
extern int sparksz;
extern int attacktheme;
extern int batsnd;
extern int cartsnd;
extern int lavasnd;
extern short mirrorwall[], mirrorsector[], mirrorcnt;
extern short floormirrorsector[], floormirrorcnt;
extern int godmode;
extern signed char ceilingshadearray[];
extern signed char floorshadearray[];
extern signed char wallshadearray[];
extern int playertorch;
extern int thefirstcone;
extern int dogchewtime;
extern point3d osprite[];
extern int smoothratio;
extern int shieldpoints;
extern int poisoned;
extern int poisontime;
extern int shockme;
extern int cyberenabled;
extern int svgahealth;
extern int victor;
extern int autohoriz;
extern int svga;
extern char boardname[];
extern char tempboard[];
extern char loadgamename[];
extern int delaycnt, engineinitflag, timerinitflag, videoinitflag;
extern int synctics;
extern int globhiz, globloz, globhihit, globlohit;
extern int chainxres[];
extern int chainyres[];
extern int vesares[7][2];
extern char option[];
extern char keys[];
extern unsigned short wDIGIVol, wMIDIVol;
#define TICSPERFRAME 3
#ifndef _MAX_PATH
#define _MAX_PATH BMAX_PATH
#define _MAX_FNAME 256
#define _MAX_EXT 256
#endif

// prototypes of the game's functions (the .def files of the DOS source)
void doanimations(int numtics);
int getanimationgoal(int *animptr);
int setanimation(int *animptr, int thegoal, int thevel);
void dodelayitems(int tics);
void setupboard(char *fname);
void drawscreen(struct player * plr);
void setOptions(void);
int smkplayseqnum(int s);
void dolevelmusic(int level);
void setdelayfunc(void (*func)(), int item, int delay);
// whnet.c (JonoF's)
void netcheckargs(int argc, const char * const argv[]);
void netmarkflag(short i);
void netsendmove(void);
void netshootgun(short s, char guntype);
void netdamageactor(short s, short o);
void dropflagstart(short teamno);
void netdropflag(void);
void netgetmove(void);
void netpickmonster(void);
void initmulti(int numplayers);
void netshutdown(void);
void whnetmon(void);
void netrestartplayer(struct player *plr);
void playloop(void);
void drawoverheadmap(struct player * plr);
void readpalettetable(void);
int adjusthp(int hp);
void searchmap(short startsector);
void animateobjs(struct player * plr);
int findapatrolpoint(short i);
void checkhit(short i, short j);
void initjstick(void);
void keytimerstuff(void);
void dophysics(struct player * plr, int goalz, short flyupdn, int v);
void processinput(struct player * plr);
void autothehoriz(struct player * plr);
void nettypeletter(void);
void typeletter(void);
void checkcheat(void);
void typecheat(char ch);
void dosoundthing(void);
void updatestatusbar(void);
void debugchain(void);
void loadsavetoscreen(void);
void menutoscreen(void);
void menutoscreenblank(void);
void itemtoscreen(int x, int y, short dapic, signed char dashade, char dapal);
void fancyfont(int x, int y, short tilenum, char *string, char pal);
void fancyfontscreen(int x, int y, short tilenum, char *string);
int menuscreen(struct player * plr);
void help(void);
void loadsave(struct player * plr);
void quit(void);
void thedifficulty(void);
void startnewgame(struct player * plr);
void loadgame(struct player * plr);
void savegame(struct player * plr);
void savegametext(int select);
int savedgamename(int gn);
int savedgamedat(int gn);
void loadplayerstuff(void);
void optionspage(void);
void thesound(void);
void thecontrols(void);
void screenfx(struct player * plr);
void fadeout(int start, int end, int red, int green, int blue, int steps);
void fadein(int start, int end, int steps);
void fog1(void);
void fog2(void);
void makefxlookups(void);
void initpaletteshifts(void);
void startgreenflash(int greentime);
void startblueflash(int bluetime);
void startredflash(int damage);
void startwhiteflash(int bonus);
void updatepaletteshifts(void);
void finishpaletteshifts(void);
void clearkeys(void);
void TEMPSND(void);
void cleanup(void);
void monitor(void);
void processobjs(struct player * plr);
int potionspace(int vial);
void updatepotion(int vial);
void transformactors(struct player * plr);
void newstatus(short sn, int seq);
void firebreath(short i, int a, int b, int c);
void castspell(short i);
void skullycastspell(short i);
int checkheat(short i);
int checkfacing(short i, int x, int y);
int checkmedusadist(short i, int x, int y, int z, int lvl);
int checkdist(short i, int x, int y, int z);
int checksight(short i, short *daang);
void checkmove(short i, short *movestat);
void shieldhit(int hp);
void attack(short i);
void fireballblast(short i);
void makeafire(short i, int firetype);
void explosion(short i, int x, int y, int z, short owner);
void explosion2(short i, int x, int y, int z, short owner);
void trailingsmoke(short i, short ball);
void icecubes(short i, int x, int y, int z, short owner);
int damageactor(short hitobject, short i);
void nukespell(short j);
void medusa(short j);
int movesprite(short spritenum, int dx, int dy, int dz, int ceildist, int flordist, char cliptype);
int actormovesprite(short spritenum);
void guardianfire(short i, int k, struct player * plr);
void gonzopike(short s);
void newguyarrow(short s);
void trowajavlin(int s);
void throwhalberd(int s);
void spawnhornskull(short i);
void monsterweapon(short i);
void madenoise(int val, int x, int y, int z);
void monsternoise(short i);
void randompotion(short i);
void spawnabaddy(short i, short monster);
void spawnapentagram(short sn);
int isvalidactor(short i);
int actoruse(short i);
void deaddude(short sn);
int isActor(int spritenum);
void loadinglevelmsg(void);
void playerdead(struct player * plr);
void spikeheart(struct player * plr);
void updateloadedplayer(int i);
void initplayersprite(void);
void autoweaponchange(int dagun);
void weaponchange(void);
void potiontext(void);
void swingdacrunch(int daweapon);
void swingdasound(int daweapon);
void lockon(struct player *plr,short numshots,short shootguntype);
void swingdaweapon(struct player * plr);
void plrfireweapon(struct player * plr);
void activatedaorb(struct player * plr);
void plruse(struct player * plr);
void loadnewlevel(int mapon);
void victory(int demoflag);
void drawweapons(struct player * plr);
void castaorb(struct player * plr);
void chunksofmeat(struct player * plr, short hitsprite, int hitx, int hity, int hitz, short hitsect, short daang);
void swingdapunch(int daweapon);
void shootgun(struct player * plr, short daang, char guntype);
void singleshot(short bstatus);
void potionpic(int currentpotion);
void usapotion(struct player * plr);
void orbpic(int currentorb);
void healthpic(int hp);
void armorpic(int arm);
void levelpic(void);
void score(int score);
void goesupalevel(struct player * plr);
int checkweapondist(short i, int x, int y, int z, char guntype);
void updatepics(void);
void captureflagpic(void);
void fragspic(void);
void keyspic(void);
int adjustscore(int score);
int lvlspellcheck(struct player * plr);
void gronmissile(int s);
void displayspelltext(void);
void painsound(int xplc, int yplc);
void turntostone(short i);
int inView(struct player *plr,int i);
void panningfx(void);
void revolvefx(void);
void bobbingsector(void);
void teleporter(void);
void warp(int *x, int *y, int *z, short *daang, short *dasector);
void warpsprite(short spritenum);
void ironbars(void);
void sectorsounds(void);
void scary(void);
void dofx(void);
void weaponpowerup(void);
void thunder(void);
void thesplash(void);
void makeasplash(int picnum, struct player * plr);
void makemonstersplash(int picnum, int i);
void bats(short k);
void warpfxsprite(int s);
void makesparks(short i, int type);
void shards(short i, short type);
void operatesprite(int s);
void operatesector(int s);
void animatetags(struct player * plr);

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
extern char configKeyboard[], configMouse[], configJoystick[], configGamepad[], configWingman[], configAvenger[];
void readControlConfigs(int which);
void setOptions(void);
int smkplayseqnum(int s);
void dolevelmusic(int level);
void setdelayfunc(void (*func)(), int item, int delay);
// whnet.c (JonoF's)
void netcheckargs(int argc, const char * const argv[]);
void netmarkflag(short i);
void netsendmove(void);
void netshootgun(short s, char guntype);
void netdamageactor(short s, short o);
void dropflagstart(short teamno);
void netdropflag(void);
void netgetmove(void);
void netpickmonster(void);
void initmulti(int numplayers);
void netshutdown(void);
void whnetmon(void);
void netrestartplayer(struct player *plr);
#ifdef MISTER_HYBRID
#include "mister_port.h"
#endif
