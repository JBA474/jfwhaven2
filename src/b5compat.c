#include "build.h"
#include "pragmas.h"

void permanentwritesprite(int thex, int they, short tilenum, signed char shade,
        int cx1, int cy1, int cx2, int cy2, unsigned char dapalnum) {
    // the box is in the game's 320x200 coordinates
    cx1 = scale(cx1, xdim, 320); cx2 = scale(cx2 + 1, xdim, 320) - 1;
    cy1 = scale(cy1, ydim, 200); cy2 = scale(cy2 + 1, ydim, 200) - 1;
    if (cx1 < 0) cx1 = 0;
    if (cy1 < 0) cy1 = 0;
    if (cx2 > xdim - 1) cx2 = xdim - 1;
    if (cy2 > ydim - 1) cy2 = ydim - 1;
    rotatesprite(thex<<16,they<<16,65536L,0,tilenum,shade,
                 dapalnum,2+8+16+64+128,cx1,cy1,cx2,cy2);
}

void overwritesprite(int thex, int they, short tilenum, signed char shade,
        char stat, unsigned char dapalnum) {
    rotatesprite(thex<<16,they<<16,65536L,(stat&8)<<7,tilenum,shade,dapalnum,
                 (((stat&1)^1)<<4)+(stat&2)+((stat&4)>>2)+(((stat&16)>>2)^((stat&8)>>1)),
                 windowx1,windowy1,windowx2,windowy2);
}

void precache()
{
}
