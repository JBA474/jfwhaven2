/* A stand-in for the HMI Sound Operating System header the DOS source
   included: the types its config reader (readcfg.c, profile.c) uses. */
#ifndef WH2_SOS_SHIM_H
#define WH2_SOS_SHIM_H
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#ifndef O_BINARY
#define O_BINARY 0
#endif
typedef unsigned short WORD;
typedef unsigned char BYTE;
typedef unsigned int DWORD;
typedef char *PSTR;
typedef void VOID;
typedef int BOOL;
#define _TRUE 1
#define _FALSE 0
#define _NULL NULL
#define cdecl
#define far
#define near
#ifndef _MAX_PATH
#define _MAX_PATH 260
#endif
typedef struct { WORD wPort, wDMA, wIRQ, wParam, wID, wTimerID; } _SOS_HARDWARE;
typedef struct { WORD wPort, wDMA, wIRQ, wID, wTimerID; } _SOS_MIDI_HARDWARE;
#include <sys/stat.h>
#ifndef S_IREAD
#define S_IREAD S_IRUSR
#define S_IWRITE S_IWUSR
#endif
#define stricmp strcasecmp
#define strcmpi strcasecmp
#endif
