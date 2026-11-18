// the game globals of the DOS source (extern.h, the WHAVEN branches)
#include "icorp.h"

int sparksx;
int sparksy;
int sparksz;
int attacktheme=0;
short mirrorwall[MAXMIRRORS], mirrorsector[MAXMIRRORS], mirrorcnt;
short floormirrorsector[64], floormirrorcnt;
int godmode=0;
signed char ceilingshadearray[MAXSECTORS];
signed char floorshadearray[MAXSECTORS];
signed char wallshadearray[MAXWALLS];
int  playertorch=0;
int  thefirstcone;
int dogchewtime;
point3d osprite[MAXSPRITESONSCREEN];
int   smoothratio;
int shieldpoints=0;
int poisoned=0;
int poisontime=0;
int  shockme=-1;
int  cyberenabled=0;
int  svgahealth=0;
int  victor=0;
int  autohoriz=0;
int  svga=0;
char boardname[80];
char tempboard[80];
char loadgamename[80];
int   delaycnt,
      engineinitflag,
      timerinitflag,
      videoinitflag;
