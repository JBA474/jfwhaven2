/***************************************************************************
 *    WITCHAVEN.C  - main game code for Witchaven game                     *
 *                                                                         *
 ***************************************************************************/

#define WHAVEN

#define GAME
#define SVGA
#include "icorp.h"


unsigned vixen;                    // drive letter

#define updatecrc16(crc,dat) (crc = (crc<<8)^crctable[(crc>>8)^(dat&255)])

#define WORD int


int  treasurescnt = 0,
     treasuresfound = 0,
     killcnt = 0,
     kills = 0,
     expgained = 0,
     bonus = 0;

extern WORD hSOSDriverHandles[];

int  overtheshoulder;

static char detailmode = 0,
     ready2send = 0;

int screentilt = 0;
extern int frameplace,
     pageoffset,
     ydim16;
extern int cachecount,
     transarea;

extern char chainstat;
extern int displaytime;


int *animateptr[MAXANIMATES],
     animategoal[MAXANIMATES],
     animatevel[MAXANIMATES],
     animatecnt = 0;

unsigned int flags32[32] = {
     0x80000000, 0x40000000, 0x20000000, 0x10000000,
     0x08000000, 0x04000000, 0x02000000, 0x01000000,
     0x00800000, 0x00400000, 0x00200000, 0x00100000,
     0x00080000, 0x00040000, 0x00020000, 0x00010000,
     0x00008000, 0x00004000, 0x00002000, 0x00001000,
     0x00000800, 0x00000400, 0x00000200, 0x00000100,
     0x00000080, 0x00000040, 0x00000020, 0x00000010,
     0x00000008, 0x00000004, 0x00000002, 0x00000001
};

extern short cddrive;

int followx,
     followy;

int  ratcnt = 0;

int  gameactivated = 0;
#ifdef MISTER_HYBRID
void mistercheats(int playing); // whinp.c: the core's OSD Cheats page
#endif
int  escapetomenu = 0;

int selectedgun;
int  difficulty = 2;

int totsynctics,
     frames;

extern int followmode;
extern int loadedgame;
extern int invincibletime,
     spiked;

extern short brightness,
     gbrightness;
extern int goreon;
extern int musiclevel;
extern int digilevel;


struct delayitem delayitem[MAXSECTORS];

int  actorcnt,
     secnt,
     sexref[MAXSECTORS],
     swingcnt;

short spikecnt;
short spikesector[64];

short lavadrylandsector[32];
short lavadrylandcnt;

short xpanningsectorlist[64],
     xpanningsectorcnt;
short ypanningwalllist[64],
     ypanningwallcnt;
short floorpanninglist[64],
     floorpanningcnt;
short skypanlist[64],
     skypancnt;

short crushsectorlist[32],
     crushsectorcnt;
short crushsectoranim[32],
     crushsectordone[32];

short revolvesector[16],
     revolveang[16],
     revolveclip[16],
     revolvecnt;
int revolvex[16][16],
     revolvey[16][16];
int revolvepivotx[16],
     revolvepivoty[16];

short dragsectorlist[16],
     dragxdir[16],
     dragydir[16],
     dragsectorcnt;
int dragx1[16],
     dragy1[16],
     dragx2[16],
     dragy2[16],
     dragfloorz[16];

short warpsectorlist[16],
     warpsectorcnt;

short bobbingsectorlist[16],
     bobbingsectorcnt;

extern volatile unsigned joetime;  // temp

//JSA ends

int  justteleported = 0;
int  playerdie = 0;

int  wet = 0;
int  oldvmode;
int dasizeofit;

short ironbarsector[16],
     ironbarscnt;
int ironbarsgoal1[16],
     ironbarsgoal2[16];
short ironbarsdone[16],
     ironbarsanim[16];
int ironbarsgoal[16];

extern int mapon;
int  mapflag;

char keys[NUMKEYS]={
     0xC8,0xD0,0xCB,0xCD,0x2A,0x38,0x1D,0x39,
     0x1E,0x2C,0xC9,0xD1,0xC7,0x33,0x34,0x0F,
     0x0D,0x0C,0x1C,0x29,0x00,0x00,0xCF,0xD2,
     0xD3,0x00,0x00,0x1B,0x1A,0x1F
};

//** Les 01/16/96   - START
extern
int  spaceballon;
extern
short spaceballinitflag;
//** Les 01/16/96   - END

//
//
//
#if 0
#endif


//   basic text functions from tekwar
//
//   basic text functions
//

int  crashflag;

void
doanimations(int numtics)
{

     int i,
          animval;

     for (i = animatecnt - 1; i >= 0; i--) {
          animval = *animateptr[i];
          if (animval < animategoal[i]) {
               animval += (numtics * animatevel[i]);
               if (animval > animategoal[i]) {
                    animval = animategoal[i];
               }
          }
          if (animval > animategoal[i]) {
               animval -= (numtics * animatevel[i]);
               if (animval < animategoal[i]) {
                    animval = animategoal[i];
               }
          }
          *animateptr[i] = animval;
          if (animval == animategoal[i]) {
               animatecnt--;
               animateptr[i] = animateptr[animatecnt];
               animategoal[i] = animategoal[animatecnt];
               animatevel[i] = animatevel[animatecnt];
          }
     }
}

int
getanimationgoal(int *animptr)
{
     int i,
          j;

     j = -1;
     for (i = 0; i < animatecnt; i++)
          if (*animptr == animateptr[i]) {
               j = i;
               break;
          }
     return (j);
}

int
setanimation(int *animptr, int thegoal, int thevel)
{

     int i,
          j;

     if (animatecnt >= MAXANIMATES - 1) {
          return (-1);
     }
     j = animatecnt;
     for (i = 0; i < animatecnt; i++) {
          if (animptr == animateptr[i]) {
               j = i;
               break;
          }
     }
     animateptr[j] = animptr;
     animategoal[j] = thegoal;
     animatevel[j] = thevel;
     if (j == animatecnt) {
          animatecnt++;
     }
     return (animatecnt - 1);
}


void
setdelayfunc(void (*func) (), int item, int delay)
{

     int  i,
          j;

     for (i = 0; i < delaycnt; i++) {
          if (delayitem[i].func == func && delayitem[i].item == item) {
               if (delay == 0) {
                    delayitem[i].func = NULL;
               }
               delayitem[i].timer = delay;
               return;
          }
     }
     if (delay > 0) {
          delayitem[delaycnt].func = func;
          delayitem[delaycnt].item = item;
          delayitem[delaycnt].timer = delay;
          delaycnt++;
     }
}

void
dodelayitems(int tics)
{

     int  cnt,
          i,
          j;

     cnt = delaycnt;
     for (i = 0; i < cnt; i++) {
          if (delayitem[i].func == NULL) {
               j = delaycnt - 1;
               memmove(&delayitem[i], &delayitem[j], sizeof(struct delayitem));
               delaycnt = j;
          }
          if (delayitem[i].timer > 0) {
               if ((delayitem[i].timer -= tics) <= 0) {
                    delayitem[i].timer = 0;
                    (*delayitem[i].func) (delayitem[i].item);
                    delayitem[i].func = NULL;
               }
          }
     }
}


void
setupboard(char *fname)
{

     int  effect,
          endwall,
          i,
          j,
          k,
          s,
          startwall;
     struct player *plr;
     int dax,
          day;
     short treesize;
     int dasector;
     int dax2,
          day2;
     short daang;

     plr = &player[0];

     randomseed = 17L;

     if (loadboard(fname, 0, &plr->x, &plr->y, &plr->z, &daang, &plr->sector) == -1) {
          crash("Board not found");
     }

     plr->ang = daang;

     searchmap(plr->sector);

     ratcnt = 0;
     actorcnt = secnt = swingcnt = 0;
     xpanningsectorcnt = 0;
     ypanningwallcnt = 0;
     floorpanningcnt = 0;
     crushsectorcnt = 0;
     revolvecnt = 0;
     warpsectorcnt = 0;
     dragsectorcnt = 0;
     ironbarscnt = 0;
     spikecnt = 0;
     bobbingsectorcnt = 0;
     lavadrylandcnt = 0;
     treasurescnt = 0;
     treasuresfound = 0;
     killcnt = 0;
     kills = 0;
     expgained = 0;
     // numinterpolations=0;

     // the new mirror code
     floormirrorcnt = 0;
     tilesizx[FLOORMIRROR] = 0;
     tilesizy[FLOORMIRROR] = 0;

     for (i=0 ; i < ARROWCOUNTLIMIT ; i++) {
          arrowsprite[i]=-1;
     }
     for (i=0 ; i < THROWPIKELIMIT ; i++) {
          throwpikesprite[i]=-1;
     }

     for (i = 0; i < MAXSPRITES; i++) { // setup sector effect options
          if (sprite[i].statnum >= MAXSTATUS) {
               continue;
          }
          if (sprite[i].picnum == CONE) {
               sparksx = sprite[i].x;
               sparksy = sprite[i].y;
               sparksz = sprite[i].z;
               for (j = 0; j < 10; j++) {
                    makesparks(i, 1);
               }
               for (j = 10; j < 20; j++) {
                    makesparks(i, 2);
               }
               for (j = 20; j < 30; j++) {
                    makesparks(i, 3);
               }
               sprite[i].cstat &= ~3;
               sprite[i].cstat |= 0x8000;
               sprite[i].clipdist = 4;
               changespritestat((short) i, 0);
               sector[sprite[i].sectnum].lotag = 50;
               sector[sprite[i].sectnum].hitag = sprite[i].hitag;
               if (sector[sprite[i].sectnum].hitag == 0)
                    sector[sprite[i].sectnum].hitag = 1;
          }

          if (sprite[i].picnum == GRONSW && sprite[i].pal == 10) {
               deletesprite((short) i);
          }

          if (sprite[i].picnum == RAT) {
               ratcnt++;
               if (ratcnt > 10) {
                    deletesprite((short) i);
               }
          }

          if (sprite[i].picnum == SPAWN) {
               deletesprite((short) i);
          }

          if (sprite[i].picnum == TORCH) {
               sprite[i].cstat &= ~3;
               changespritestat(i, TORCHLIGHT);
          }

          if (sprite[i].picnum == STANDINTORCH ||
              sprite[i].picnum == BOWLOFFIRE) {
               changespritestat(i, TORCHLIGHT);
          }




          if (sprite[i].picnum == SNDEFFECT) {
               sector[sprite[i].sectnum].extra = sprite[i].lotag;
               deletesprite((short) i);
          }

          if (sprite[i].picnum == SNDLOOP) { // loop on
               sector[sprite[i].sectnum].extra = (32768 | (sprite[i].lotag << 1) | 1);
               deletesprite((short) i);
          }

          if (sprite[i].picnum == SNDLOOPOFF) {   // loop off
               sector[sprite[i].sectnum].extra = (32768 | (sprite[i].lotag << 1));
               deletesprite((short) i);
          }

          if (sprite[i].lotag == 80) {
               ironbarsector[ironbarscnt] = sprite[i].sectnum;
               ironbarsdone[ironbarscnt] = 0;
               ironbarsanim[ironbarscnt] = i;
               ironbarsgoal[ironbarscnt] = 0;
               ironbarscnt++;
          }
          if (sprite[i].statnum < MAXSTATUS) {
               switch (sprite[i].picnum) {
               case GRONHAL:
                    sprite[i].xrepeat = GRONHALXR;
                    sprite[i].yrepeat = GRONHALYR;
                    sprite[i].clipdist = 64;
                    changespritestat(i, FACE);
                    sprite[i].hitag = adjusthp(65);
                    sprite[i].lotag = 100;
                    sprite[i].cstat |= 0x101;
                    sprite[i].extra = 4;
                    killcnt++;
                    break;
               case GRONMU:
                    sprite[i].xrepeat = GRONMUXR;
                    sprite[i].yrepeat = GRONMUYR;
                    sprite[i].clipdist = 64;
                    changespritestat(i, FACE);
                    sprite[i].hitag = adjusthp(70);
                    sprite[i].lotag = 100;
                    sprite[i].cstat |= 0x101;
                    sprite[i].extra = 2;
                    killcnt++;
                    break;
               case GRONSW:
                    sprite[i].xrepeat = GRONSWXR;
                    sprite[i].yrepeat = GRONSWYR;
                    sprite[i].clipdist = 64;
                    changespritestat(i, FACE);
                    sprite[i].hitag = adjusthp(60);
                    sprite[i].lotag = 100;
                    sprite[i].cstat |= 0x101;
                    sprite[i].extra = 0;
                    killcnt++;
                    break;
               case RAT:
                    sprite[i].xrepeat = 32;
                    sprite[i].yrepeat = 32;
                    sprite[i].shade = 12;
                    sprite[i].pal = 5;
                    changespritestat(i, FACE);
                    sprite[i].hitag = 10;
                    sprite[i].lotag = 100;
                    sprite[i].cstat = 0x101;
                    break;
               case FISH:
                    sprite[i].clipdist = 32;
                    changespritestat(i, FACE);
                    sprite[i].hitag = 10;
                    sprite[i].lotag = 100;
                    sprite[i].cstat |= 0x101;
                    break;
               case WILLOW:
                    sprite[i].xrepeat = WILLOWXR;
                    sprite[i].yrepeat = WILLOWYR;
                    sprite[i].clipdist = 64;
                    changespritestat(i, FACE);
                    sprite[i].hitag = adjusthp(5);
                    sprite[i].lotag = 100;
                    sprite[i].cstat |= 0x101;
                    killcnt++;
                    break;
               case DEVIL:
               case DEVILSTAND:
                    sprite[i].xrepeat = DEVILXR;
                    sprite[i].yrepeat = DEVILYR;
                    sprite[i].clipdist = 64;
                    changespritestat(i, FACE);
                    if (sprite[i].pal == 2) {
                         sprite[i].hitag = adjusthp(60);
                    }
                    else {
                         sprite[i].hitag = adjusthp(50);
                    }
                    sprite[i].lotag = 100;
                    sprite[i].cstat |= 0x101;
                    killcnt++;
                    break;
               case HANGMAN + 1:
                    sprite[i].xrepeat = 28;
                    sprite[i].yrepeat = 28;
                    break;
               case HANGMAN:
                    sprite[i].xrepeat = 28;
                    sprite[i].yrepeat = 28;
                    sprite[i].clipdist = 64;
                    changespritestat(i, STAND);
                    sprite[i].hitag = adjusthp(25);
                    sprite[i].lotag = 100;
                    sprite[i].cstat |= 0x101;
                    killcnt++;
                    break;
               case SKELETON:
                    sprite[i].xrepeat = SKELETONXR;
                    sprite[i].yrepeat = SKELETONYR;
                    sprite[i].clipdist = 64;
                    changespritestat(i, FACE);
                    sprite[i].hitag = adjusthp(25);
                    sprite[i].lotag = 100;
                    sprite[i].cstat |= 0x101;
                    killcnt++;
                    break;
               case MINOTAUR:
                    sprite[i].clipdist = 64;
                    changespritestat(i, FACE);
                    sprite[i].hitag = adjusthp(80);
                    sprite[i].lotag = 100;
                    sprite[i].cstat |= 0x101;
                    if (rand() % 100 > 50) {
                         sprite[i].extra = 1;
                    }
                    killcnt++;
                    break;
               case KOBOLD:
                    sprite[i].clipdist = 64;
                    // newstatus((short)i,PATROL);
                    // changespritestat(i,PATROL);
                    changespritestat(i, FACE);
                    if (sprite[i].pal == 8) {
                         sprite[i].hitag = adjusthp(40);
                    }
                    else if (sprite[i].pal == 7) {
                         sprite[i].hitag = adjusthp(60);
                    }
                    else {
                         sprite[i].hitag = adjusthp(20);
                    }
                    sprite[i].lotag = 100;
                    sprite[i].xrepeat = KOBOLDXR;
                    sprite[i].yrepeat = KOBOLDYR;
                    sprite[i].cstat |= 0x101;
                    killcnt++;
                    break;
               case DEMON:
                    sprite[i].clipdist = 64;
                    changespritestat(i, FACE);
                    sprite[i].hitag = adjusthp(300);
                    sprite[i].lotag = 100;
                    sprite[i].xrepeat = DEMONXR;
                    sprite[i].yrepeat = DEMONYR;
                    sprite[i].cstat |= 0x101;
                    killcnt++;
                    break;
               case GUARDIAN:
                    sprite[i].clipdist = 64;
                    changespritestat(i, FACE);
                    sprite[i].hitag = adjusthp(100);
                    sprite[i].lotag = 100;
                    sprite[i].xrepeat = GUARDIANXR;
                    sprite[i].yrepeat = GUARDIANYR;
                    sprite[i].cstat |= 0x101;
                    killcnt++;
                    break;

               case IMP:
                    sprite[i].clipdist = 64;
                    changespritestat(i, FACE);
                    sprite[i].hitag = adjusthp(20);
                    sprite[i].lotag = 100;
                    sprite[i].xrepeat = IMPXR;
                    sprite[i].yrepeat = IMPYR;
                    sprite[i].shade = -4;
                    sprite[i].cstat |= 0x101;
                    killcnt++;
                    break;

               case STONEGONZOBSH:
               case STONEGONZOBSW2:
               case STONEGONZOCHM:
               case STONEGONZOGSH:
               case STONEGRONDOVAL:
               case STONEGONZOBSW:
                    sprite[i].xrepeat = 24;
                    sprite[i].yrepeat = 24;
                    break;

               case GONZOHMJUMP:
               case GONZOSHJUMP:
                    sprite[i].xrepeat = 24;
                    sprite[i].yrepeat = 24;
                    sprite[i].clipdist = 32;
                    sprite[i].extra = sprite[i].lotag;
                    sprite[i].lotag = 20;
                    if (sprite[i].extra == 3) {
                         sprite[i].lotag = 80;
                    }
                    sprite[i].cstat |= 0x101;
                    killcnt++;
                    break;

               case NEWGUYCAST:
               case NEWGUYBOW:
               case NEWGUYMACE:
               case NEWGUYPUNCH:
               case NEWGUY:
                    switch (sprite[i].picnum) {
                    case NEWGUYCAST:
                         sprite[i].extra = 30;
                         sprite[i].hitag = adjusthp(85);
                         break;
                    case NEWGUYBOW:
                         sprite[i].extra = 20;
                         sprite[i].hitag = adjusthp(85);
                         break;
                    case NEWGUYMACE:
                         sprite[i].extra = 10;
                         sprite[i].hitag = adjusthp(45);
                         break;
                    case NEWGUYPUNCH:
                         sprite[i].extra = 0;
                         sprite[i].hitag = adjusthp(15);
                         break;
                    default:
                         sprite[i].hitag = adjusthp(90);
                         break;
                    }
                    sprite[i].xrepeat = NEWGUYXR;
                    sprite[i].yrepeat = NEWGUYYR;
                    sprite[i].clipdist = 48;
                    changespritestat(i, FACE);
                    sprite[i].lotag = 100;
                    sprite[i].cstat |= 0x101;
                    sprite[i].picnum = NEWGUY;
                    killcnt++;
                    break;
               case NEWGUYSTAND:
                    sprite[i].xrepeat = NEWGUYXR;
                    sprite[i].yrepeat = NEWGUYYR;
                    sprite[i].clipdist = 48;
                    changespritestat(i, STAND);
                    sprite[i].hitag = adjusthp(50);
                    sprite[i].lotag = 100;
                    sprite[i].cstat |= 0x101;
                    sprite[i].extra = 20;
                    sprite[i].picnum = NEWGUYSTAND;
                    killcnt++;
                    break;
               case NEWGUYKNEE:
                    sprite[i].xrepeat = NEWGUYXR;
                    sprite[i].yrepeat = NEWGUYYR;
                    sprite[i].clipdist = 48;
                    changespritestat(i, STAND);
                    sprite[i].hitag = adjusthp(50);
                    sprite[i].lotag = 100;
                    sprite[i].cstat |= 0x101;
                    sprite[i].extra = 30;
                    sprite[i].picnum = NEWGUYKNEE;
                    killcnt++;
                    break;

               case KURTSTAND:
                    sprite[i].xrepeat = KURTXR;
                    sprite[i].yrepeat = KURTYR;
                    sprite[i].clipdist = 48;
                    changespritestat(i, STAND);
                    sprite[i].hitag = adjusthp(50);
                    sprite[i].lotag = 100;
                    sprite[i].cstat |= 0x101;
                    sprite[i].extra = 20;
                    sprite[i].picnum = KURTSTAND;
                    killcnt++;
                    break;
               case KURTKNEE:
                    sprite[i].xrepeat = KURTXR;
                    sprite[i].yrepeat = KURTYR;
                    sprite[i].clipdist = 48;
                    changespritestat(i, STAND);
                    sprite[i].hitag = adjusthp(50);
                    sprite[i].lotag = 100;
                    sprite[i].cstat |= 0x101;
                    sprite[i].extra = 10;
                    sprite[i].picnum = KURTKNEE;
                    killcnt++;
                    break;
               case KURTAT:
                    sprite[i].xrepeat = KURTXR;
                    sprite[i].yrepeat = KURTYR;
                    sprite[i].clipdist = 48;
                    changespritestat(i, FACE);
                    sprite[i].hitag = adjusthp(50);
                    sprite[i].lotag = 100;
                    sprite[i].cstat |= 0x101;
                    sprite[i].extra = 10;
                    sprite[i].picnum = GONZOCSW;
                    killcnt++;
                    break;
               case KURTPUNCH:
                    sprite[i].xrepeat = KURTXR;
                    sprite[i].yrepeat = KURTYR;
                    sprite[i].clipdist = 48;
                    changespritestat(i, FACE);
                    sprite[i].hitag = adjusthp(15);
                    sprite[i].lotag = 100;
                    sprite[i].cstat |= 0x101;
                    sprite[i].extra = 0;
                    sprite[i].picnum = GONZOCSW;
                    killcnt++;
                    break;
               case GONZOCSW:
                    sprite[i].xrepeat = GONZOCSWXR;
                    sprite[i].yrepeat = GONZOCSWYR;
                    sprite[i].clipdist = 48;
                    changespritestat(i, FACE);
                    sprite[i].hitag = adjusthp(50);
                    sprite[i].lotag = 100;
                    sprite[i].cstat |= 0x101;
                    sprite[i].extra = 20;
                    killcnt++;
                    break;

               case GONZOGSW:
               case GONZOGHM:
               case GONZOGSH:
                    switch (sprite[i].picnum) {
                    case GONZOGSW:
                         sprite[i].hitag = adjusthp(100);
                         break;
                    case GONZOGHM:
                         sprite[i].hitag = adjusthp(40);
                         break;
                    case GONZOGSH:
                         sprite[i].hitag = adjusthp(50);
                         break;
                    }
                    sprite[i].xrepeat = GONZOGSWXR;
                    sprite[i].yrepeat = GONZOGSWYR;
                    sprite[i].clipdist = 32;
                    changespritestat(i, FACE);
                    sprite[i].lotag = 100;
                    sprite[i].cstat |= 0x101;
                    sprite[i].extra = 0;
                    killcnt++;
                    break;
               case KATIE:
                    sprite[i].clipdist = 64;
                    changespritestat(i, FACE);
                    sprite[i].hitag = adjusthp(200);
                    sprite[i].lotag = 100;
                    sprite[i].xrepeat = KATIEXR;
                    sprite[i].yrepeat = KATIEYR;
                    sprite[i].extra = 5;
                    sprite[i].cstat |= 0x101;
                    killcnt++;
                    break;
               case GOBWEAPON:
               case WEAPON1:
                    sprite[i].xrepeat = 34;
                    sprite[i].yrepeat = 21;
                    break;
               case WEAPON2:
                    sprite[i].xrepeat = 26;
                    sprite[i].yrepeat = 26;
                    break;
               case WEAPON3:
                    sprite[i].xrepeat = 44;
                    sprite[i].yrepeat = 39;
                    break;
               case WEAPON4:
                    sprite[i].xrepeat = 25;
                    sprite[i].yrepeat = 20;
                    break;
               case WEAPON6:
                    sprite[i].xrepeat = 20;
                    sprite[i].yrepeat = 15;
                    sprite[i].cstat &= ~3;
                    break;
               case WEAPON7:
                    sprite[i].xrepeat = 41;
                    sprite[i].yrepeat = 36;
                    sprite[i].cstat &= ~3;
                    break;
               case QUIVER:
                    sprite[i].xrepeat = 27;
                    sprite[i].yrepeat = 27;
                    break;
               case LEATHERARMOR:
                    sprite[i].xrepeat = 47;
                    sprite[i].yrepeat = 50;
                    break;
               case CHAINMAIL:
               case PLATEARMOR:
                    sprite[i].xrepeat = sprite[i].yrepeat = 26;
                    treasurescnt++;
                    break;
               case GONZOBSHIELD:
               case GONZOCSHIELD:
               case GONZOGSHIELD:
                    sprite[i].xrepeat = sprite[i].yrepeat = 12;
                    break;
               case SHIELD:
                    sprite[i].xrepeat = sprite[i].yrepeat = 26;
                    break;
               case HELMET:
                    sprite[i].xrepeat = 27;
                    sprite[i].yrepeat = 28;
                    treasurescnt++;
                    break;
               case PENTAGRAM:
                    sprite[i].cstat &= ~3;
                    treasurescnt++;
                    break;
               case SCROLLSCARE:
               case SCROLLNUKE:
               case SCROLLFLY:
               case SCROLLFIREBALL:
               case SCROLLFREEZE:
               case SCROLLNIGHT:
               case SCROLLMAGIC:
               case SCROLLOPEN:
                    sprite[i].xrepeat = 35;
                    sprite[i].yrepeat = 36;
                    sprite[i].cstat &= ~3;
                    treasurescnt++;
                    break;
               case DIAMONDRING:
                    sprite[i].xrepeat = 14;
                    sprite[i].yrepeat = 14;
                    treasurescnt++;
                    break;
               case SHADOWAMULET:
                    sprite[i].xrepeat = 30;
                    sprite[i].yrepeat = 23;
                    treasurescnt++;
                    break;
               case GLASSSKULL:
                    sprite[i].xrepeat = 22;
                    sprite[i].yrepeat = 22;
                    treasurescnt++;
                    break;
               case AHNK:
                    sprite[i].xrepeat = 51;
                    sprite[i].yrepeat = 54;
                    treasurescnt++;
                    break;
               case BLUESCEPTER:
                    sprite[i].xrepeat = 32;
                    sprite[i].yrepeat = 32;
                    treasurescnt++;
                    break;
               case YELLOWSCEPTER:
                    sprite[i].xrepeat = 32;
                    sprite[i].yrepeat = 32;
                    treasurescnt++;
                    break;
               case ADAMANTINERING:
                    sprite[i].xrepeat = 14;
                    sprite[i].yrepeat = 14;
                    treasurescnt++;
                    break;
               case ONYXRING:
                    sprite[i].xrepeat = 42;
                    sprite[i].yrepeat = 28;
                    treasurescnt++;
                    break;
               case HORNEDSKULL:
                    sprite[i].xrepeat = 64;
                    sprite[i].yrepeat = 64;
                    treasurescnt++;
                    break;
               case CRYSTALSTAFF:
                    sprite[i].xrepeat = 64;
                    sprite[i].yrepeat = 64;
                    treasurescnt++;
                    break;
               case AMULETOFTHEMIST:
                    sprite[i].xrepeat = 26;
                    sprite[i].yrepeat = 28;
                    treasurescnt++;
                    break;
               case SAPHIRERING:
                    sprite[i].xrepeat = 30;
                    sprite[i].yrepeat = 20;
                    treasurescnt++;
                    break;
               case PINE:
                    sprite[i].xrepeat = treesize = ((krand() % 5) + 3) << 4;
                    sprite[i].yrepeat = treesize;
                    break;
               case GIFTBOX:
                    sprite[i].xrepeat = 56;
                    sprite[i].yrepeat = 49;
                    treasurescnt++;
                    break;
               case PATROLPOINT:
                    sprite[i].xrepeat = 24;
                    sprite[i].yrepeat = 32;
                    sprite[i].cstat &= ~3;
                    sprite[i].cstat |= 0x8000;
                    sprite[i].clipdist = 4;
                    changespritestat((short) i, APATROLPOINT);
                    break;
               case BARREL:
               case VASEA:
               case VASEB:
               case VASEC:
               case STAINGLASS1:
               case STAINGLASS2:
               case STAINGLASS3:
               case STAINGLASS4:
               case STAINGLASS5:
               case STAINGLASS6:
               case STAINGLASS7:
               case STAINGLASS8:
               case STAINGLASS9:
                    sprite[i].cstat |= 0x101;
                    sprite[i].clipdist = 64;
                    break;
               case BRASSKEY:
                    sprite[i].xrepeat = sprite[i].yrepeat = 24;
                    treasurescnt++;
                    break;
               case BLACKKEY:
                    sprite[i].xrepeat = sprite[i].yrepeat = 24;
                    treasurescnt++;
                    break;
               case GLASSKEY:
                    sprite[i].xrepeat = sprite[i].yrepeat = 24;
                    treasurescnt++;
                    break;
               case IVORYKEY:
                    sprite[i].xrepeat = sprite[i].yrepeat = 24;
                    treasurescnt++;
                    break;
               case THEHORN:
                    sprite[i].xrepeat = sprite[i].yrepeat = 32;
                    treasurescnt++;
                    break;
               case FLASKBLUE:
               case FLASKRED:
               case FLASKGREEN:
               case FLASKOCHRE:
               case FLASKTAN:
                    treasurescnt++;
                    break;
               case 2232:          // team flags
                    netmarkflag(i);
                    break;
               }
          }
     }

     for (i = 0; i < numsectors; i++) {
#if 0
          // missing a wall?
          startwall = sector[i].wallptr;
          endwall = startwall + sector[i].wallnum - 1;
          for (j = startwall; j <= endwall; j++) {
               if (j == 1497) {    // mark the wall# from map2zon1
                    for (k = startwall; k <= endwall; k++) {
                         wall[k].picnum = 1;
                         sector[i].floorpicnum = 1;
                         sector[i].ceilingpicnum = 1;
                    }
               }
          }
#endif

          ceilingshadearray[i] = sector[i].ceilingshade;
          floorshadearray[i] = sector[i].floorshade;
          if (sector[i].lotag == 100) {
               spikesector[spikecnt++] = i;
          }
          if (sector[i].lotag == 70) {
               skypanlist[skypancnt++] = i;
          }
          if (sector[i].lotag >= 80 && sector[i].lotag <= 89) {
               floorpanninglist[floorpanningcnt++] = i;
          }
          if (sector[i].lotag >= 900 && sector[i].lotag <= 999) {
               lavadrylandsector[lavadrylandcnt] = i;
               lavadrylandcnt++;
          }
          /* if (sector[i].lotag >= 1900 && sector[i].lotag <= 1999) {
           * crushsectorlist[crushsectorcnt]=i; crushsectoranim[i]=0;
           * crushsectordone[i]=0; crushsectorcnt++; } */
          if (sector[i].lotag >= 2100 && sector[i].lotag <= 2199) {
               startwall = sector[i].wallptr;
               endwall = startwall + sector[i].wallnum - 1;
               dax = 0L;
               day = 0L;
               for (j = startwall; j <= endwall; j++) {
                    dax += wall[j].x;
                    day += wall[j].y;
               }
               revolvepivotx[revolvecnt] = dax / (endwall - startwall + 1);
               revolvepivoty[revolvecnt] = day / (endwall - startwall + 1);

               k = 0;
               for (j = startwall; j <= endwall; j++) {
                    revolvex[revolvecnt][k] = wall[j].x;
                    revolvey[revolvecnt][k] = wall[j].y;
                    k++;
               }
               revolvesector[revolvecnt] = i;
               revolveang[revolvecnt] = 0;
               revolveclip[revolvecnt] = 1;
               if (sector[i].ceilingz == sector[wall[startwall].nextsector].ceilingz) {
                    revolveclip[revolvecnt] = 0;
               }
               revolvecnt++;
          }
          switch (sector[i].lotag) {
          case 131:
          case 132:
          case 133:
          case 134:
          case DOORSWINGTAG:

               startwall = sector[i].wallptr;
               endwall = startwall + sector[i].wallnum - 1;
               for (j = startwall; j <= endwall; j++) {
                    if (wall[j].lotag == 4) {
                         k = wall[wall[wall[wall[j].point2].point2].point2].point2;
                         if ((wall[j].x == wall[k].x) && (wall[j].y == wall[k].y)) {
                              swingdoor[swingcnt].wall[0] = j;
                              swingdoor[swingcnt].wall[1] = wall[j].point2;
                              swingdoor[swingcnt].wall[2] = wall[wall[j].point2].point2;
                              swingdoor[swingcnt].wall[3] = wall[wall[wall[j].point2].point2].point2;
                              swingdoor[swingcnt].angopen = 1536;
                              swingdoor[swingcnt].angclosed = 0;
                              swingdoor[swingcnt].angopendir = -1;
                         }
                         else {
                              swingdoor[swingcnt].wall[0] = wall[j].point2;
                              swingdoor[swingcnt].wall[1] = j;
                              swingdoor[swingcnt].wall[2] = lastwall(j);
                              swingdoor[swingcnt].wall[3] = lastwall(swingdoor[swingcnt].wall[2]);
                              swingdoor[swingcnt].angopen = 512;
                              swingdoor[swingcnt].angclosed = 0;
                              swingdoor[swingcnt].angopendir = 1;
                         }
                         for (k = 0; k < 4; k++) {
                              swingdoor[swingcnt].x[k] = wall[swingdoor[swingcnt].wall[k]].x;
                              swingdoor[swingcnt].y[k] = wall[swingdoor[swingcnt].wall[k]].y;
                         }
                         swingdoor[swingcnt].sector = i;
                         swingdoor[swingcnt].ang = swingdoor[swingcnt].angclosed;
                         swingdoor[swingcnt].anginc = 0;
                         swingcnt++;
                    }
               }
               break;
          case 11:
               xpanningsectorlist[xpanningsectorcnt++] = i;
               break;
          case 12:
               dasector = i;
               dax = 0x7fffffff;
               day = 0x7fffffff;
               dax2 = 0x80000000;
               day2 = 0x80000000;
               startwall = sector[i].wallptr;
               endwall = startwall + sector[i].wallnum - 1;
               for (j = startwall; j <= endwall; j++) {
                    if (wall[j].x < dax)
                         dax = wall[j].x;
                    if (wall[j].y < day)
                         day = wall[j].y;
                    if (wall[j].x > dax2)
                         dax2 = wall[j].x;
                    if (wall[j].y > day2)
                         day2 = wall[j].y;
                    if (wall[j].lotag == 3)
                         k = j;
               }
               if (wall[k].x == dax)
                    dragxdir[dragsectorcnt] = -16;
               if (wall[k].y == day)
                    dragydir[dragsectorcnt] = -16;
               if (wall[k].x == dax2)
                    dragxdir[dragsectorcnt] = 16;
               if (wall[k].y == day2)
                    dragydir[dragsectorcnt] = 16;

               dasector = wall[startwall].nextsector;
               dragx1[dragsectorcnt] = 0x7fffffff;
               dragy1[dragsectorcnt] = 0x7fffffff;
               dragx2[dragsectorcnt] = 0x80000000;
               dragy2[dragsectorcnt] = 0x80000000;
               startwall = sector[dasector].wallptr;
               endwall = startwall + sector[dasector].wallnum - 1;
               for (j = startwall; j <= endwall; j++) {
                    if (wall[j].x < dragx1[dragsectorcnt])
                         dragx1[dragsectorcnt] = wall[j].x;
                    if (wall[j].y < dragy1[dragsectorcnt])
                         dragy1[dragsectorcnt] = wall[j].y;
                    if (wall[j].x > dragx2[dragsectorcnt])
                         dragx2[dragsectorcnt] = wall[j].x;
                    if (wall[j].y > dragy2[dragsectorcnt])
                         dragy2[dragsectorcnt] = wall[j].y;
               }
               dragx1[dragsectorcnt] += (wall[sector[i].wallptr].x - dax);
               dragy1[dragsectorcnt] += (wall[sector[i].wallptr].y - day);
               dragx2[dragsectorcnt] -= (dax2 - wall[sector[i].wallptr].x);
               dragy2[dragsectorcnt] -= (day2 - wall[sector[i].wallptr].y);

               dragfloorz[dragsectorcnt] = sector[i].floorz;
               dragsectorlist[dragsectorcnt++] = i;
               break;
          case 10:
          case 14:
          case 4002:
               warpsectorlist[warpsectorcnt++] = i;
               break;
          case 10000:
               bobbingsectorlist[bobbingsectorcnt++] = i;
               break;
          }
          if (sector[i].floorpicnum == TELEPAD && sector[i].lotag == 0) {
               warpsectorlist[warpsectorcnt++] = i;
          }
          if (sector[i].floorpicnum == FLOORMIRROR)
               floormirrorsector[mirrorcnt++] = i;

     }

     ypanningwallcnt = 0;
     for (i = 0; i < numwalls; i++) {
          wallshadearray[i] = wall[i].shade;
          if (wall[i].lotag == 1) {
               ypanningwalllist[ypanningwallcnt++] = i;
          }
     }


#if 0     // doing this in initplayersprite() called after each setupboard
     if (justteleported == 1 || loadedgame == 1) {
          plr->hvel = 0;
          angvel = 0;
          svel = 0;
          vel = 0;
          plr->spritenum = insertsprite(plr->sector, 0);
          plr->oldsector = plr->sector;

          sprite[plr->spritenum].x = plr->x;
          sprite[plr->spritenum].y = plr->y;
          sprite[plr->spritenum].z = sector[plr->sector].floorz;
          sprite[plr->spritenum].cstat = 1 + 256 + 32768;
          sprite[plr->spritenum].picnum = GRONSW;
          sprite[plr->spritenum].shade = 0;
          sprite[plr->spritenum].xrepeat = 36;
          sprite[plr->spritenum].yrepeat = 36;
          sprite[plr->spritenum].ang = plr->ang;
          sprite[plr->spritenum].xvel = 0;
          sprite[plr->spritenum].yvel = 0;
          sprite[plr->spritenum].zvel = 0;
          sprite[plr->spritenum].owner = 4096;
          sprite[plr->spritenum].lotag = 0;
          sprite[plr->spritenum].hitag = 0;
          // sprite[plr->spritenum].pal=1;
          sprite[plr->spritenum].pal = 10;
          sprite[plr->spritenum].clipdist = 48;
          setsprite(plr->spritenum, plr->x, plr->y, plr->z + (PLAYERHEIGHT << 8));
          justteleported = 0;
          loadedgame = 0;
     }
#endif

     // Map starts out completed
     // for(i=0;i<(MAXSECTORS>>3);i++) show2dsector[i] = 0xff;
     for(i=0;i<(MAXSECTORS>>3);i++) show2dsector[i] = 0x00;
     // for(i=0;i<(MAXWALLS>>3);i++) show2dwall[i] = 0xff;
     for(i=0;i<(MAXWALLS>>3);i++) show2dwall[i] = 0x00;
     // for(i=0;i<(MAXSPRITES>>3);i++) show2dsprite[i] = 0xff;
     for(i=0;i<(MAXSPRITES>>3);i++) show2dsprite[i] = 0x00;

}

#define   CSIZE  32

int  outsideview = 0;

extern
int  jumphoriz;

void
drawscreen(struct player * plr)
{

     int dax,
          day,
          dax2,
          day2;
     int olddist;
     short daang;
     char ch,
         *ptr,
         *ptr2,
         *ptr3,
         *ptr4;
     int i,
          j,
          k,
          l;
     int x1,
          y1,
          x2,
          y2,
          ox1,
          oy1,
          ox2,
          oy2,
          dist,
          maxdist;
     int tiltlock,
         *longptr,
          ovisibility,
          oparallaxvisibility;
//    int cposx, cposy;

     if (plr->dimension == 3 || plr->dimension == 2) {
          // Init for screen rotation
          tiltlock = screentilt;
          if ((tiltlock) || (detailmode)) {
               walock[4094] = 255;
               if (waloff[4094] == 0)
                    allocache((void **)&waloff[4094], 320 * 320, &walock[4094]);

               if ((tiltlock & 1023) == 0)
                    setviewtotile(4094, 200L >> detailmode, 320L >> detailmode);
               else
                    setviewtotile(4094, 320L >> detailmode, 320L >> detailmode);
               if ((tiltlock & 1023) == 512) {    // Block off unscreen section
                                                  // of 90� tilted screen
                    j = ((320 - 60) >> detailmode);
                    for (i = (60 >> detailmode) - 1; i >= 0; i--) {
                         startumost[i] = 1;
                         startumost[i + j] = 1;
                         startdmost[i] = 0;
                         startdmost[i + j] = 0;
                    }
               }
          }

          // wango
          if ((gotpic[FLOORMIRROR >> 3] & (1 << (FLOORMIRROR & 7))) > 0) {
               if (!chainstat) {
                    dist = 0x7fffffff;
                    i = 0;
                    for (k = floormirrorcnt - 1; k >= 0; k--) {
                         j = klabs(wall[sector[floormirrorsector[k]].wallptr].x - plr->x);
                         j += klabs(wall[sector[floormirrorsector[k]].wallptr].y - plr->y);
                         if (j < dist)
                              dist = j, i = k;
                    }

                    j = floormirrorsector[i];

                    drawrooms(plr->x, plr->y, (sector[j].floorz << 1) - plr->z, plr->ang, 201 - plr->horiz, j);
                    transformactors(plr);
                    drawmasks();

                    // Temp horizon
                    l = scale(plr->horiz - 100, windowx2 - windowx1, 320) + ((windowy1 + windowy2) >> 1);
                    for (y1 = windowy1, y2 = windowy2; y1 < y2; y1++, y2--) {
                         ptr = (char *) (frameplace + ylookup[y1]);
                         ptr2 = (char *) (frameplace + ylookup[y2]);
                         // ptr3=palookup[7];
                         ptr3 = palookup[0];
                         ptr3 += (min(klabs(y1 - l) >> 2, 31) << 8);
                         // ptr4=palookup[7];
                         ptr4 = palookup[0];
                         ptr4 += (min(klabs(y2 - l) >> 2, 31) << 8);

                         j = sintable[((y2 + numframes) << 7) & 2047];
                         j += sintable[((y2 - numframes) << 8) & 2047];

                         j >>= 14;
                         ptr2 += j;

                         for (x1 = windowx1; x1 <= windowx2; x1++) {
                              ch = ptr[x1];
                              ptr[x1] = ptr3[ptr2[x1]];
                              ptr2[x1] = ptr4[ch];
                         }
                    }
                    gotpic[FLOORMIRROR >> 3] &= ~(1 << (FLOORMIRROR & 7));
               }
          }

          if ((gotpic[FLOORMIRROR >> 3] & (1 << (FLOORMIRROR & 7))) > 0)
               transarea += (windowx2 - windowx1) * (windowy2 - windowy1);

          if ((tiltlock) || (detailmode)) {
               setviewback();
               rotatesprite((int) 320 << 15, (int) 200 << 15, 65536 + (detailmode << 16), tiltlock + 512, 4094, 0, 0, 2 + 4 + 64, windowx1, windowy1, windowx2, windowy2);
               walock[4094] = 1;
          }

          // cposx=plr->x;
          // cposy=plr->y;

          // cposx -= (sintable[(plr->ang+512)&2047]>>4);
          // cposy -= (sintable[plr->ang&2047]>>4);

          i = plr->horiz;
          plr->horiz += jumphoriz;
          drawrooms(plr->x, plr->y, plr->z, plr->ang, plr->horiz, plr->sector);
          plr->horiz = i;

          // drawrooms(cposx,cposy,plr->z,plr->ang,plr->horiz,plr->sector);

          transformactors(plr);
          debugchain();
          drawmasks();
          if (playerdie == 0)
               drawweapons(plr);
          if (spiked == 1) {
               spikeheart(plr);
          }
          dofx();
     }
     if (plr->dimension == 2) {
          drawoverheadmap(plr);
     }
     monitor();
     screenfx(plr);
     plr->oldsector = plr->sector;

     if (netgame) {
          whnetmon();
     }
     if (iglassenabled || cyberenabled) {
     }

     nextpage();

}

extern
short mousekeys[];

extern
short spacekeys[];

extern
short wingmankeys[];

extern
int  wingmanflag;

extern
WORD wDIGIDevice,
     wMIDIDevice;

extern
signed char videoModeOption;

void
setOptions(void)
{
     if (videoModeOption == 0) {
          option[0] = 2;
     }
     else if (videoModeOption == 1) {
          option[0] = 1;
     }
     keys[KEYFWD]=configKeyboard[ACT_FORWARD];
     keys[KEYBACK]=configKeyboard[ACT_BACKWARD];
     keys[KEYLEFT]=configKeyboard[ACT_LEFT];
     keys[KEYRIGHT]=configKeyboard[ACT_RIGHT];
     keys[KEYRUN]=configKeyboard[ACT_RUN];
     keys[KEYSTRAFE]=configKeyboard[ACT_STRAFE];
     keys[KEYFIRE]=configKeyboard[ACT_USEWEAP];
     keys[KEYUSE]=configKeyboard[ACT_USEITEM];
     keys[KEYJUMP]=configKeyboard[ACT_JUMP];
     keys[KEYCROUCH]=configKeyboard[ACT_CROUCH];
     keys[KEYLKUP]=configKeyboard[ACT_LOOKUP];
     keys[KEYLKDN]=configKeyboard[ACT_LOOKDN];
     keys[KEYCNTR]=configKeyboard[ACT_CENTER];
     keys[KEYSTFL]=configKeyboard[ACT_STRAFEL];
     keys[KEYSTFR]=configKeyboard[ACT_STRAFER];
     keys[KEYUSEP]=configKeyboard[ACT_USEPOTION];
     keys[KEYCAST]=configKeyboard[ACT_CASTSPELL];
     keys[KEYFLYUP]=configKeyboard[ACT_FLYUP];
     keys[KEYFLYDN]=configKeyboard[ACT_FLYDN];
     keys[KEYSHIELD]=configKeyboard[ACT_SHIELD];
     option[3]=0;
     if (configMouse[0] == 1) {
          option[3]=configMouse[0];
          mousekeys[0]=configMouse[1];
          mousekeys[1]=configMouse[2];
          mousekeys[2]=configMouse[3];
     }
}

char configFile[BMAX_PATH];


// ---- the port's part (after JonoF's jfwhaven: whaven.c) ----
#include "osd.h"
#include "version.h"
#ifdef MISTER_HYBRID
#include "mister_port.h"
#endif

int xdimgame = 320, ydimgame = 200, bppgame = 8;
int forcesetup = 1;
int synctics;
int globhiz, globloz, globhihit, globlohit;
int chainxres[4] = {256,320,360,400};
int chainyres[11] = {200,240,256,270,300,350,360,400,480,512,540};
int vesares[7][2] = {{320,200},{640,400},{640,480},{800,600},{1024,768},{1280,1024},{1600,1200}};
char option[NUMOPTIONS] = {
  2,  // 0 - Video type (2 = 320x200 layout)
  0,  // 1 -
  0,  // 2 -
  1,  // 3 - 1 = Mouse, +2 = Joystick
  0,  // 4 -
  0,  // 5 -
  1,  // 6 - Video mode
  64+4+2, // 7 - Sound, 22khz 16bit stereo
  0,  // 8 -
  1,  // 9 - Music on
  0,  // 10 -
  1,  // 11 - Sound on
};
int digihz[8] = {6000,8000,11025,16000,22050,32000,44100,48000};
int  crashflag;
int  oldvmode;

void faketimerhandler(void) { }

// referenced by code the optimizer drops in a release build (an -O0 build links them)
__attribute__((weak)) int iglassenabled, vfx1enabled;
__attribute__((weak)) const unsigned char appicon_bmp[1];
__attribute__((weak)) const int appicon_bmp_size = 0;

// every frame the game shows: the engine's events first (the DOS game had a
// keyboard interrupt; icorp.h makes nextpage() this)
void wh2_nextpage(void) {
    handleevents();
    OSD_DispatchQueued();
    (nextpage)();
}

void rp_delay(int goal) {
    int dagoal = totalclock + goal;
    while (totalclock < dagoal) {
        handleevents();
    }
}

static void writepref(void) {
    int fil = open("pref.dat", O_BINARY | O_TRUNC | O_CREAT | O_WRONLY, S_IREAD | S_IWRITE);
    if (fil >= 0) {
        write(fil, &goreon, 2);
        write(fil, &brightness, 2);
        write(fil, &gbrightness, 2);
        write(fil, &digilevel, 2);
        write(fil, &musiclevel, 2);
        write(fil, &difficulty, 2);
        close(fil);
    }
}

void shutdowngame(void) {
    writepref();
    SND_Shutdown();
    netshutdown();
    if (engineinitflag) {
        uninitengine();
    }
    uninitgroupfile();
    if (crashflag) {
        return;
    }
    exit(0);
}

void crash(char *fmt, ...) {
    va_list argptr;
    crashflag = 1;
    shutdowngame();
    va_start(argptr, fmt);
    vprintf(fmt, argptr);
    va_end(argptr);
    printf("\n");
    exit(0);
}

void crashgame(char *fmt, ...) {
    va_list argptr;
    crashflag = 1;
    shutdowngame();
    va_start(argptr, fmt);
    vprintf(fmt, argptr);
    va_end(argptr);
    printf("\n");
    exit(0);
}

// the view of the 3D screen for a status bar screen size (320x200 layout
// scaled to the mode), after the DOS game's setup3dscreen()
void setviewport(int screensize) {
    int dax, day, dax2, day2;
    if (screensize > 320) {
        dax = day = 0;
        dax2 = xdim; day2 = ydim;
    } else {
        int ss = screensize;
        dax = scale(160 - (ss >> 1), xdim, 320);
        dax2 = scale(160 - (ss >> 1) + ss, xdim, 320);
        day = scale(84 - (((ss * 168) / 320) >> 1), ydim, 200);
        day2 = scale(STATUSSCREEN + 1, ydim, 200);
    }
    setview(dax, day, dax2 - 1, day2 - 1);
}

void setup3dscreen(void) {
    struct player *plr = &player[0];
    if (setgamemode(fullscreen, xdimgame, ydimgame, bppgame))
        crashgame("Could not set video mode");
    videoinitflag = 1;
    svga = 0;
    setviewport(plr->screensize);
    updatestatusbar();
}

// the sequel's videos, by the DOS game's numbering
static const char *smknames[] = { "intro", "ending1", "stairs", "ending2", "ending3" };
int smkplayseqnum(int s) {
    if (s < 0 || s >= 5) return 0;
    return smkplayseq((char *)smknames[s]);
}
unsigned short wDIGIVol = 0x7fff, wMIDIVol = 0x7f; // the DOS sound setup's volumes (unused here)
char tempbuf[576]; // the DOS text-mode module's (whmsc.c) scratch buffer
char chainstat;    // old Build engine globals the game still names
int transarea;

int app_main(int argc, const char * const argv[]) {
    struct player *plr;
    int fil, j;

    plr = &player[0];
    netcheckargs(argc, argv);
#ifdef MISTER_HYBRID
    // the game data, the settings and the saved games are all in the folder
    // the launcher starts the game in
    {
        char cwd[BMAX_PATH+1];
        if (getcwd(cwd, sizeof(cwd))) {
            addsearchpath(cwd);
        }
    }
    MiSTer_Init(); // the launcher keeps what the game prints
#else
    buildsetlogfile("whaven2.log");
#endif
    wm_setapptitle("JFWitchaven II");
    buildprintf("\nJFWitchaven II\nBased on WITCHAVEN II Copyright (C) 1996 Intracorp Entertainment, Inc. v2.0g\n"
        "Version %s.\nBuilt %s %s.\n", game_version, game_date, game_time);

    if (preinitengine()) {
        wm_msgbox("Build Engine Initialisation Error",
            "There was a problem initialising the Build engine: %s", engineerrstr);
        exit(1);
    }
    initgroupfile("stuff.dat");
    buildprintf(" map name: level%d\n", mapon);

    strcpy(configFile, "HMISET.CFG");
    readControlConfigs(1);
    setOptions();
    option[0] = 2;

    if (access("pref.dat", F_OK) != 0) {
        goreon = 1;
        brightness = gbrightness = 0;
        digilevel = 11;
        musiclevel = 11;
        writepref();
    } else {
        fil = open("pref.dat", O_RDONLY | O_BINARY);
        if (fil >= 0) {
            read(fil, &goreon, 2);
            read(fil, &brightness, 2);
            read(fil, &gbrightness, 2);
            read(fil, &digilevel, 2);
            read(fil, &musiclevel, 2);
            read(fil, &difficulty, 2);
            close(fil);
        }
    }
#ifdef MISTER_HYBRID
    fullscreen = 0;
    xdimgame = MiSTer_ScreenWidth();
    ydimgame = MiSTer_ScreenHeight();
    bppgame = 8;
    MiSTer_Loading();
    MiSTer_PickGame();
#endif
    MusicMode = (option[8] == 1 || option[9] == 1);
    SoundMode = (option[10] == 1 || option[11] == 1);
    if (getenv("WH2NOMUSIC")) MusicMode = 0; // diagnostics
    if (getenv("WH2NOSOUND")) SoundMode = 0;

    initinput();
    if (configMouse[0] == 1) {
        initmouse();
    }
    initjstick();

    if (initengine()) {
        wm_msgbox("Build Engine Initialisation Error",
            "There was a problem initialising the Build engine: %s", engineerrstr);
        exit(1);
    }
    engineinitflag = 1;
    svga = 0;
    pskyoff[0] = 0; pskyoff[1] = 0; pskybits = 1;
    // the DOS game ran keytimerstuff() from its 120 Hz sound timer interrupt
    inittimer(120, keytimerstuff);
    buildputs(" loadpics()\n");
    loadpics("tiles000.art", 8*1048576);
    totalclock = 0;
    playerdie = 0;
    plr->oldsector = plr->sector;
    plr->horiz = 100;
    plr->zoom = 256;
    plr->screensize = 320;
    plr->dimension = 3;
    plr->height = PLAYERHEIGHT;
    plr->z = sector[plr->sector].floorz - (plr->height << 8);
    visibility = 1024;
    parallaxtype = 1;
    setup3dscreen();
    setbrightness(brightness, palette, 0);
    SND_Startup();
    initpaletteshifts();
    readpalettetable();
    for (j = 0; j < 256; j++)
        tempbuf[j] = ((j + 32) & 255);
    makepalookup(16, tempbuf, 0, 0, 0, 1);
    for (j = 0; j < 256; j++)
        tempbuf[j] = j;
    makepalookup(17, tempbuf, 24, 24, 24, 1);
    for (j = 0; j < 256; j++)
        tempbuf[j] = j;
    makepalookup(18, tempbuf, 8, 8, 48, 1);
    gameactivated = 0;
    escapetomenu = 0;
    if (netgame == 0) {
        smkplayseq("intro");
    }
    setbrightness(brightness, palette, 0);
    SND_MenuMusic(0);
    playloop();
    return 0;
}

extern
int  spikeme;

void
playloop(void)
{

     struct player *plr;
     int  exit = 0;

     plr = &player[pyrn];

     if (netgame) {
          initmulti(MAXPLAYERS);
     }

     while (!exit) {
          handleevents();
          OSD_DispatchQueued();
#ifdef MISTER_HYBRID
          mistercheats(gameactivated == 1 && escapetomenu == 0);
#endif
          if (gameactivated == 0 || escapetomenu == 1) {
               exit = menuscreen(plr);
               setviewport(plr->screensize);
               updatestatusbar();
#if 0
               if (svga == 0) {
                    permanentwritesprite(0L, 0L, BACKGROUND, 0, 0L, 0L, 319L, 199L, 0);
                    permanentwritesprite(0L, 154L, NEWSTATUSBAR, 0, 0L, 0L, 319L, 199L, 0);
                    updatepics();
               }
               if (svga == 1) {
                    if (plr->screensize == 320) {
                         rotatesprite((xdim - 640) << 15, (ydim - tilesizy[SSTATUSBAR]) << 16, 65536L, 0, SSTATUSBAR, 0, 0, 8 + 16 + 64 + 128, 0L, 0L, xdim - 1L, ydim - 1L);
                         updatepics();
                    }
               }
#endif
               plr->z = sector[plr->sector].floorz - (PLAYERHEIGHT << 8);
          }
          if (spikeme) {
               godmode = 0;
               invincibletime = 0;
               plr->health = 0;
               healthpic(-1);
               spiked = 1;
               plr->horiz = 200;
               playerdead(plr);
               spikeme=0;
          }
          drawscreen(plr);
          processinput(plr);

          if (netgame) {
               netgetmove();
               netsendmove();
          }
          processobjs(plr);
          animateobjs(plr);
          animatetags(plr);
          doanimations((int) synctics);
          dodelayitems((int) synctics);
     }
}

extern
int  show2dobjectsflag,
     show2dmapflag;

void
drawoverheadmap(struct player * plr)
{

     int i,
          j,
          k,
          l,
          x1,
          y1,
          x2,
          y2,
          x3,
          y3,
          x4,
          y4,
          ox,
          oy,
          xoff,
          yoff;
     int dax,
          day,
          cosang,
          sinang,
          xspan,
          yspan,
          sprx,
          spry;
     int xrepeat,
          yrepeat,
          z1,
          z2,
          startwall,
          endwall,
          tilenum,
          daang;
     int xvect,
          yvect,
          xvect2,
          yvect2;
     char col;
     walltype *wal,
         *wal2;
     spritetype *spr;
     short cang;
     int czoom;

     cang = (short) plr->ang;
     czoom = plr->zoom;

     xvect = sintable[(2048 - cang) & 2047] * czoom;
     yvect = sintable[(1536 - cang) & 2047] * czoom;
     xvect2 = mulscale(xvect, yxaspect, 16);
     yvect2 = mulscale(yvect, yxaspect, 16);

     // Draw red lines
     for (i = 0; i < numsectors; i++) {
          startwall = sector[i].wallptr;
          endwall = sector[i].wallptr + sector[i].wallnum - 1;

          z1 = sector[i].ceilingz;
          z2 = sector[i].floorz;

          for (j = startwall, wal = &wall[startwall]; j <= endwall; j++, wal++) {
               k = wal->nextwall;
               if (k < 0)
                    continue;

               if ((show2dwall[j >> 3] & (1 << (j & 7))) == 0 && !show2dmapflag) {
                    continue;
               }
               if ((k > j) && ((show2dwall[k >> 3] & (1 << (k & 7))) > 0))
                    continue;

               if (sector[wal->nextsector].ceilingz == z1)
                    if (sector[wal->nextsector].floorz == z2)
                         if (((wal->cstat | wall[wal->nextwall].cstat) & (16 + 32)) == 0)
                              continue;

               col = 152;

               if (plr->dimension == 2) {
                    if (sector[i].floorz != sector[i].ceilingz)
                         if (sector[wal->nextsector].floorz != sector[wal->nextsector].ceilingz)
                              if (((wal->cstat | wall[wal->nextwall].cstat) & (16 + 32)) == 0)
                                   if (sector[i].floorz == sector[wal->nextsector].floorz)
                                        continue;
                    if (sector[i].floorpicnum != sector[wal->nextsector].floorpicnum)
                         continue;
                    if (sector[i].floorshade != sector[wal->nextsector].floorshade)
                         continue;
                    col = 12;
               }

               ox = wal->x - plr->x - followx;
               oy = wal->y - plr->y - followy;
               x1 = mulscale(ox, xvect, 16) - mulscale(oy, yvect, 16);
               y1 = mulscale(oy, xvect2, 16) + mulscale(ox, yvect2, 16);

               wal2 = &wall[wal->point2];
               ox = wal2->x - plr->x - followx;
               oy = wal2->y - plr->y - followy;
               x2 = mulscale(ox, xvect, 16) - mulscale(oy, yvect, 16);
               y2 = mulscale(oy, xvect2, 16) + mulscale(ox, yvect2, 16);

               drawline256(x1 + (xdim << 11), y1 + (ydim << 11), x2 + (xdim << 11), y2 + (ydim << 11), col);
          }
     }

     // Draw sprites
     k = plr->spritenum;
     show2dsprite[plr->spritenum >> 3] |= (1 << (plr->spritenum & 7));
     for (i = 0; i < numsectors; i++)
          for (j = headspritesect[i]; j >= 0; j = nextspritesect[j])
               if ((show2dsprite[j >> 3] & (1 << (j & 7))) > 0 || show2dmapflag) {
                    if (!show2dobjectsflag && !show2dmapflag) {
                         if (plr->spritenum != j) {
                              continue;
                         }
                    }
                    spr = &sprite[j];
                    if (spr->cstat == -1) {
                         // wango
                         strcpy(displaybuf, "bug");
                         displaytime = 120;
                         continue;
                    }
                    col = 56;
                    if ((spr->cstat & 1) > 0)
                         col = 248;
                    if (j == k)
                         col = 31;

                    sprx = spr->x;
                    spry = spr->y;

                    k = spr->statnum;
                    if ((k >= 1) && (k <= 8) && (k != 2)) { // Interpolate moving
                                                            // sprite
                         sprx = osprite[j].x + mulscale(sprx - osprite[j].x, smoothratio, 16);
                         spry = osprite[j].y + mulscale(spry - osprite[j].y, smoothratio, 16);
                    }

                    switch (spr->cstat & 48) {
                    case 0:
                         ox = sprx - plr->x - followx;
                         oy = spry - plr->y - followy;
                         x1 = mulscale(ox, xvect, 16) - mulscale(oy, yvect, 16);
                         y1 = mulscale(oy, xvect2, 16) + mulscale(ox, yvect2, 16);

                         // if (plr->dimension == 1)
                         if (plr->dimension == 2) {
                              ox = (sintable[(spr->ang + 512) & 2047] >> 7);
                              oy = (sintable[(spr->ang) & 2047] >> 7);
                              x2 = mulscale(ox, xvect, 16) - mulscale(oy, yvect, 16);
                              y2 = mulscale(oy, xvect, 16) + mulscale(ox, yvect, 16);

                              if (j == plr->spritenum) {
                                   x2 = 0L;
                                   y2 = -(czoom << 5);
                              }

                              x3 = mulscale(x2, yxaspect, 16);
                              y3 = mulscale(y2, yxaspect, 16);

                              drawline256(x1 - x2 + (xdim << 11), y1 - y3 + (ydim << 11),
                                          x1 + x2 + (xdim << 11), y1 + y3 + (ydim << 11), col);
                              drawline256(x1 - y2 + (xdim << 11), y1 + x3 + (ydim << 11),
                                          x1 + x2 + (xdim << 11), y1 + y3 + (ydim << 11), col);
                              drawline256(x1 + y2 + (xdim << 11), y1 - x3 + (ydim << 11),
                                          x1 + x2 + (xdim << 11), y1 + y3 + (ydim << 11), col);
                         }
                         else {
                              if (((gotsector[i >> 3] & (1 << (i & 7))) > 0) && (czoom > 192)) {
                                   daang = (spr->ang - cang) & 2047;
                                   // if (j == plr->spritenum )
                                   // { x1 = 0; y1 = (yxaspect<<2); daang = 0;
                                   // }
                                   // rotatesprite((x1<<4)+(xdim<<15),(y1<<4)+(y
                                   // dim<<15),mulscale(czoom*spr->yrepeat,yxasp
                                   // ect,16),daang,spr->picnum,spr->shade,spr->
                                   // pal,(spr->cstat&2)>>1);
                                   rotatesprite((x1 << 4) + (xdim << 15),
                                                (y1 << 4) + (ydim << 15),
                                                mulscale16(czoom * spr->yrepeat,
                                                yxaspect), daang, spr->picnum,
                                                spr->shade, spr->pal,
                                                (spr->cstat & 2) >> 1,
                                                windowx1, windowy1,
                                                windowx2, windowy2);
                              }
                         }
                         break;
                    case 16:
                         x1 = sprx;
                         y1 = spry;
                         tilenum = spr->picnum;
                         xoff = (int) ((signed char) ((picanm[tilenum] >> 8) & 255)) + ((int) spr->xoffset);
                         if ((spr->cstat & 4) > 0)
                              xoff = -xoff;
                         k = spr->ang;
                         l = spr->xrepeat;
                         dax = sintable[k & 2047] * l;
                         day = sintable[(k + 1536) & 2047] * l;
                         l = tilesizx[tilenum];
                         k = (l >> 1) + xoff;
                         x1 -= mulscale(dax, k, 16);
                         x2 = x1 + mulscale(dax, l, 16);
                         y1 -= mulscale(day, k, 16);
                         y2 = y1 + mulscale(day, l, 16);

                         ox = x1 - plr->x - followx;
                         oy = y1 - plr->y - followy;
                         x1 = mulscale(ox, xvect, 16) - mulscale(oy, yvect, 16);
                         y1 = mulscale(oy, xvect2, 16) + mulscale(ox, yvect2, 16);

                         ox = x2 - plr->x - followx;
                         oy = y2 - plr->y - followy;
                         x2 = mulscale(ox, xvect, 16) - mulscale(oy, yvect, 16);
                         y2 = mulscale(oy, xvect2, 16) + mulscale(ox, yvect2, 16);

                         drawline256(x1 + (xdim << 11), y1 + (ydim << 11),
                                   x2 + (xdim << 11), y2 + (ydim << 11), col);

                         break;
                    case 32:
                         if (plr->dimension == 1) {
                              tilenum = spr->picnum;
                              xoff = (int) ((signed char) ((picanm[tilenum] >> 8) & 255)) + ((int) spr->xoffset);
                              yoff = (int) ((signed char) ((picanm[tilenum] >> 16) & 255)) + ((int) spr->yoffset);
                              if ((spr->cstat & 4) > 0)
                                   xoff = -xoff;
                              if ((spr->cstat & 8) > 0)
                                   yoff = -yoff;

                              k = spr->ang;
                              cosang = sintable[(k + 512) & 2047];
                              sinang = sintable[k];
                              xspan = tilesizx[tilenum];
                              xrepeat = spr->xrepeat;
                              yspan = tilesizy[tilenum];
                              yrepeat = spr->yrepeat;

                              dax = ((xspan >> 1) + xoff) * xrepeat;
                              day = ((yspan >> 1) + yoff) * yrepeat;
                              x1 = sprx + mulscale(sinang, dax, 16) + mulscale(cosang, day, 16);
                              y1 = spry + mulscale(sinang, day, 16) - mulscale(cosang, dax, 16);
                              l = xspan * xrepeat;
                              x2 = x1 - mulscale(sinang, l, 16);
                              y2 = y1 + mulscale(cosang, l, 16);
                              l = yspan * yrepeat;
                              k = -mulscale(cosang, l, 16);
                              x3 = x2 + k;
                              x4 = x1 + k;
                              k = -mulscale(sinang, l, 16);
                              y3 = y2 + k;
                              y4 = y1 + k;

                              ox = x1 - plr->x - followx;
                              oy = y1 - plr->y - followy;
                              x1 = mulscale(ox, xvect, 16) - mulscale(oy, yvect, 16);
                              y1 = mulscale(oy, xvect2, 16) + mulscale(ox, yvect2, 16);

                              ox = x2 - plr->x - followx;
                              oy = y2 - plr->y - followy;
                              x2 = mulscale(ox, xvect, 16) - mulscale(oy, yvect, 16);
                              y2 = mulscale(oy, xvect2, 16) + mulscale(ox, yvect2, 16);

                              ox = x3 - plr->x - followx;
                              oy = y3 - plr->y - followy;
                              x3 = mulscale(ox, xvect, 16) - mulscale(oy, yvect, 16);
                              y3 = mulscale(oy, xvect2, 16) + mulscale(ox, yvect2, 16);

                              ox = x4 - plr->x - followx;
                              oy = y4 - plr->y - followy;
                              x4 = mulscale(ox, xvect, 16) - mulscale(oy, yvect, 16);
                              y4 = mulscale(oy, xvect2, 16) + mulscale(ox, yvect2, 16);

                              drawline256(x1 + (xdim << 11), y1 + (ydim << 11),
                                   x2 + (xdim << 11), y2 + (ydim << 11), col);

                              drawline256(x2 + (xdim << 11), y2 + (ydim << 11),
                                   x3 + (xdim << 11), y3 + (ydim << 11), col);

                              drawline256(x3 + (xdim << 11), y3 + (ydim << 11),
                                   x4 + (xdim << 11), y4 + (ydim << 11), col);

                              drawline256(x4 + (xdim << 11), y4 + (ydim << 11),
                                   x1 + (xdim << 11), y1 + (ydim << 11), col);

                         }
                         break;
                    }
               }

     // Draw white lines
     for (i = 0; i < numsectors; i++) {
          startwall = sector[i].wallptr;
          endwall = sector[i].wallptr + sector[i].wallnum - 1;

          for (j = startwall, wal = &wall[startwall]; j <= endwall; j++, wal++) {
               if (wal->nextwall >= 0)
                    continue;

               if ((show2dwall[j >> 3] & (1 << (j & 7))) == 0 && !show2dmapflag) {
                    continue;
               }

               if (tilesizx[wal->picnum] == 0)
                    continue;
               if (tilesizy[wal->picnum] == 0)
                    continue;

               ox = wal->x - plr->x - followx;
               oy = wal->y - plr->y - followy;
               x1 = mulscale(ox, xvect, 16) - mulscale(oy, yvect, 16);
               y1 = mulscale(oy, xvect2, 16) + mulscale(ox, yvect2, 16);

               wal2 = &wall[wal->point2];
               ox = wal2->x - plr->x - followx;
               oy = wal2->y - plr->y - followy;
               x2 = mulscale(ox, xvect, 16) - mulscale(oy, yvect, 16);
               y2 = mulscale(oy, xvect2, 16) + mulscale(ox, yvect2, 16);

               drawline256(x1 + (xdim << 11), y1 + (ydim << 11), x2 + (xdim << 11), y2 + (ydim << 11), 24);
          }
     }
}



void
readpalettetable(void)
{
     FILE *fp;
     int  i,
          j;
     char num_tables,
          lookup_num;

     if ((fp = fopen("lookup.dat", "rb")) == NULL) {
          return;
     }
     num_tables = getc(fp);
     for (j = 0; j < num_tables; j++) {
          lookup_num = getc(fp);
          for (i = 0; i < 256; i++) {
               tempbuf[i] = getc(fp);
          }
          makepalookup(lookup_num, tempbuf, 0, 0, 0, 1);
     }
     fclose(fp);
}

int 
adjusthp(int hp)
{

     float factor;
     int  howhard;

     factor = (krand() % 20) / 100;
     howhard = difficulty;

     if (krand() % 100 > 50)
          return ((hp * (factor + 1)) * howhard);
     else
          return ((hp - (hp * (factor))) * howhard);

}


#if 0     //venom
void inittimer(void)
{
     outp(0x43, 54);
     outp(0x40, 9942 & 255);
     outp(0x40, 9942 >> 8);        // 120 times/sec
     oldtimerhandler = _dos_getvect(0x8);
     _disable();
     _dos_setvect(0x8, timerhandler);
     _enable();
}

#endif


/*
 rotatesprite((xdim-320)<<15,
               (ydim-32)<<16,
               65536L,
               0,
               STATUSBAR,
               0,
               0,
               8+16+64+128,
               0L,
               0L,
               xdim-1L,
               ydim-1L);
*/

//permanentwritesprite(0,240,SVGAMENU2,0,0,240,639,479,0);
static short tempshort[MAXSECTORS];

void
searchmap(short startsector)
{
     int i,
          j,
          dasect,
          splc,
          send,
          startwall,
          endwall;
     short dapic;
     walltype *wal;

     if ((startsector < 0) || (startsector >= numsectors))
          return;
     for (i = 0; i < (MAXSECTORS >> 3); i++)
          show2dsector[i] = 0;
     for (i = 0; i < (MAXWALLS >> 3); i++)
          show2dwall[i] = 0;
     for (i = 0; i < (MAXSPRITES >> 3); i++)
          show2dsprite[i] = 0;

     automapping = 1;

     // Search your area recursively & set all show2dsector/show2dwalls
     tempshort[0] = startsector;
     if (automapping == 0) {
          show2dsector[startsector >> 3] |= (1 << (startsector & 7));
     }
     dapic = sector[startsector].ceilingpicnum;
     if (waloff[dapic] == 0)
          loadtile(dapic);
     dapic = sector[startsector].floorpicnum;
     if (waloff[dapic] == 0)
          loadtile(dapic);
     for (splc = 0, send = 1; splc < send; splc++) {
          dasect = tempshort[splc];
          startwall = sector[dasect].wallptr;
          endwall = startwall + sector[dasect].wallnum;
          for (i = startwall, wal = &wall[startwall]; i < endwall; i++, wal++) {
               if (automapping == 0) {
                    show2dwall[i >> 3] |= (1 << (i & 7));
               }
               dapic = wall[i].picnum;
               if (waloff[dapic] == 0)
                    loadtile(dapic);
               dapic = wall[i].overpicnum;
               if (((dapic & 0xfffff000) == 0) && (waloff[dapic] == 0))
                    loadtile(dapic);

               j = wal->nextsector;
               if ((j >= 0) && ((show2dsector[j >> 3] & (1 << (j & 7))) == 0)) {
                    show2dsector[j >> 3] |= (1 << (j & 7));
                    dapic = sector[j].ceilingpicnum;
                    if (waloff[dapic] == 0)
                         loadtile(dapic);
                    dapic = sector[j].floorpicnum;
                    if (waloff[dapic] == 0)
                         loadtile(dapic);

                    tempshort[send++] = (short) j;
               }
          }

          for (i = headspritesect[dasect]; i >= 0; i = nextspritesect[i]) {
               if (automapping == 0) {
                    show2dsprite[i >> 3] |= (1 << (i & 7));
               }
               dapic = sprite[i].picnum;
               if (waloff[dapic] == 0)
                    loadtile(dapic);
          }
     }
}
