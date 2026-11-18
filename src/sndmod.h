//
//    SNDMOD HEADER AND DEF'S
//



#define  MAX_SAMPLES       200
#define  MAX_ACTIVE_SAMPLES   10
#define  MAX_ACTIVE_SONGS     4
//num of array elements in ambsoundarray[]
#define MAX_AMB_SOUNDS       8
#define  MAX_ACTIVE_FXLOOPS   4

#define     S_THUNDER1             0                      
#define     S_THUNDER2             1 
#define     S_THUNDER3             2 
#define     S_WINDLOOP1            3 
#define     S_WAVELOOP1            4 
#define     S_LAVALOOP1            5 
#define     S_FIRELOOP1            6 
#define     S_STONELOOP1           7 
#define     S_BATSLOOP             8 
#define     S_PLRWEAPON0           9 
#define     S_PLRWEAPON1           10 
#define     S_PLRWEAPON2           11 
#define     S_PLRWEAPON3           12 
#define     S_PLRWEAPON4           13 
#define     S_KSNARL1              14 
#define     S_KSNARL2              15 
#define     S_KSNARL3              16 
#define     S_KSNARL4              17 
#define     S_KPAIN1               18 
#define     S_KPAIN2               19 
#define     S_KDIE1                20 
#define     S_KDIE2                21 
#define     S_DEMON1               22 
#define     S_DEMON2               23 
#define     S_DEMON3               24 
#define     S_DEMON4               25 
#define     S_DEMON5               26 
#define     S_DEMONDIE1            27 
#define     S_DEMONDIE2            28 
#define     S_MSNARL1              29 
#define     S_MSNARL2              30 
#define     S_MSNARL3              31 
#define     S_MSNARL4              32 
#define     S_MPAIN1               33 
#define     S_MDEATH1              34 
#define     S_RIP1                 35 
#define     S_RIP2                 36 
#define     S_RIP3                 37 
#define     S_SKELHIT1             38 
#define     S_SKELHIT2             39 
#define     S_SKELETONDIE          40 
#define     S_SKELSEE              41   
#define     S_GUARDIAN1            42  
#define     S_GUARDIAN2            43 
#define     S_GUARDIANPAIN1        44 
#define     S_GUARDIANPAIN2        45 
#define     S_GUARDIANDIE          46 
#define     S_WISP                 47 
#define     S_WISP2                48 
#define     S_SPLASH1              49 
#define     S_SPLASH2              50 
#define     S_SPLASH3              51 
#define     S_SPLASH4              52 
#define     S_SPLASH5              53 
#define     S_WILLOWDIE            54 
#define     S_JUDY1                55 
#define     S_JUDYDIE              56 
#define     S_CHAIN1               57               
#define     S_FLAME1               58 
#define     S_CLUNK                59
#define     S_DEMONTHROW           60
#define     S_DOOR1                61
#define     S_DOOR2                62
#define     S_DOOR3                63 
#define     S_CREAKDOOR1           64 
#define     S_CREAKDOOR2           65 
#define     S_CREAKDOOR3           66 
#define     S_STING1               67 
#define     S_STING2               68 
#define     S_POTION1              69 
#define     S_GENTHROW             70 
#define     S_GENSWING             71 
#define     S_ARROWHIT             72 
#define     S_WALLHIT1             73 
#define     S_GONG                 74 
#define     S_SPELL1               75 
#define     S_FREEZE               76 
#define     S_FREEZEDIE            77 
#define     S_TRAP1                78 
#define     S_TRAP2                79 
#define     S_RATS1                80 
#define     S_RATS2                81 
#define     S_WINDLOOP2            82 
#define     S_BREATH1              83 
#define     S_BREATH2              84 
#define     S_PUSH1                85   
#define     S_PUSH2                86      
#define     S_PLRPAIN1             87             
#define     S_PLRPAIN2             88 
#define     S_GORE1                89 
#define     S_GORE2                90 
#define     S_GORE1A               91 
#define     S_GORE1B               92 
#define     S_DEADSTEP             93 
#define     S_HEARTBEAT            94 
#define     S_SOFTCHAINWALK        95 
#define     S_SOFTCREAKWALK        96  
#define     S_LOUDCHAINWALK        97  
#define     S_SCARYDUDE            98  
#define     S_WATERY               99  
#define     S_GLASSBREAK1          100 
#define     S_GLASSBREAK2          101 
#define     S_GLASSBREAK3          102
#define     S_TREASURE1            103 
#define     S_SWORD1               104 
#define     S_SWORD2               105 
#define     S_SWORDCLINK1          106 
#define     S_SWORDCLINK2          107 
#define     S_SWORDCLINK3          108 
#define     S_SWORDCLINK4          109 
#define     S_SOCK1                110 
#define     S_SOCK2                111 
#define     S_SOCK3                112 
#define     S_SOCK4                113 
#define     S_KOBOLDHIT            114 
#define     S_FIREBALL             115 
#define     S_PULLCHAIN1           116 
#define     S_PICKUPAXE            117 
#define     S_EXPLODE              118 
#define     S_BARRELBREAK          119 
#define     S_WARP                 120 
#define     S_PICKUPFLAG           121 
#define     S_DROPFLAG             122 
#define     S_LAUGH                123 
#define     S_DRINK                124 
#define     S_PAGE                 125 
#define     S_BOTTLES              126 
#define     S_CRACKING             127 
#define     S_PLRDIE1              128
#define     S_AGM_ATTACK           129     
#define     S_AGM_PAIN1            130
#define     S_AGM_PAIN2            131
#define     S_AGM_PAIN3            132
#define     S_AGM_DIE1             133
#define     S_AGM_DIE2             134
#define     S_AGM_DIE3             135
#define     S_GRONDEATHA           136
#define     S_GRONDEATHB           137
#define     S_GRONDEATHC           138  
#define     S_HGM_DIE1             139  
#define     S_HGM_DIE2             140  
#define     S_HGM_DIE3             141  
#define     S_GRONPAINA            142
#define     S_HGM_PAIN1            143  
#define     S_HGM_PAIN2            144  
#define     S_FIRESWING            145
#define     S_FIREWEAPONLOOP       146
#define     S_ENERGYWEAPONLOOP     147
#define     S_ENERGYSWING          148
#define     S_BIGGLASSBREAK1       149
#define     S_BIGGLASSBREAK2       150
#define     S_BIGGLASSBREAK3       151
#define     S_IMPGROWL1            152
#define     S_IMPGROWL2            153
#define     S_IMPGROWL3            154
#define     S_IMPDIE1              155
#define     S_IMPDIE2              156
#define     S_SWINGDOOR            157
#define     S_NUKESPELL            158
#define     S_DOORSPELL            159
#define     S_FIRESPELL            160
#define     S_GENERALMAGIC1        161
#define     S_GENERALMAGIC2        162
#define     S_GENERALMAGIC3        163
#define     S_GENERALMAGIC4        164
#define     S_HAWKSOUND            165
#define     S_OWL SOUND            166
#define     S_STEPSSOUND           167

#define  MAX_SND_LOOPS        8

#define  DIG_MIDI 2
#define  MIDI     1
#define  DIGI     0

#define  MENUSONG   0
#define  DEATHSONG   1

#define BASE_SONG   0
#define EMB_SONG        1
#define SPICE_SONG  2

#define  WINDSNDBIT        1
#define  WAVESNDBIT        2
#define  WATERSNDBIT       4
#define  LAVASNDBIT        8
#define  GRATESNDBIT       16
#define  CARTSNDBIT        32
#define  FIRE1SNDBIT       64
#define  FIRE2SNDBIT       128

struct ambsounds {
	int soundnum;       //the actual sound number S_x
	int hsound;     //the handle returned by hmi
};

#ifdef SND_CALLER

	//
	//    MIDI SPECIFIC
	//

//midimusic vars

	unsigned char     ActiveSongBits = 0x00;


	struct ambsounds ambsoundarray[] = {
		{ 0,0 },
		{ S_WINDLOOP1,-1 },
		{ S_WINDLOOP2,-1 },
		{ S_WAVELOOP1,-1 },
		{ S_LAVALOOP1,-1 },
		{ S_WATERY,-1 },
		{ S_STONELOOP1,-1 },
		{ S_BATSLOOP,-1 }
	};




	//
	//          GLOBAL VARIABLES
	//
	int     SoundMode;
	int     MusicMode;


#else




extern int  SoundMode,MusicMode;

extern struct ambsounds ambsoundarray[];
extern int lavasnd,batsnd,cartsnd;


#endif



