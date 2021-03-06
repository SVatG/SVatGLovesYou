/**
 * JBDS main entry point
 */

#include <nds.h>
#include <stdio.h>

#include <nds/ndstypes.h>
#include <nds/interrupts.h>
#include <string.h>

// Utilities to make development easier
#include "Utils.h"

// NitroFS access
#include "nitrofs.h"

// Effects!
#include "effects.h"
#include "Field.h"
#include "HeartField.h"
#include "ARM.h"

// Sound!
#include <maxmod9.h>
#include "music.h"

volatile uint32_t t;
volatile uint32_t toff;
volatile uint32_t scur;
volatile uint32_t ecur;
volatile uint32_t blockload;
volatile uint32_t fadesub;

static void vblank();

extern int tempImage;

// void fadeout(int t, int b) {
// 	uint16_t* love_coloured_master_bright = (uint16_t*)(0x400006C);
// 	if( t > b-16 ) {
// 		uint16_t val = 18-(b-t);
// 		memset( love_coloured_master_bright, (1<<7) | val, 2 );
// 	}
// 	else if(t > b) {
// 		memset( love_coloured_master_bright, (1<<7) | 16, 2 );
// 	}
// }
// 
// void fadein(int t, int b) {
// 	uint16_t* master_bright = (uint16_t*)(0x400006C);
// 	if( t < b+16 ) {
// 		uint16_t val = (b+17-t);
// 		memset( master_bright, (1<<7) | val, 2 );
// 	}
// 	else {
// 		memset( master_bright, (1<<7) | 0, 2 );
// 	}
// }
uint16_t* master_bright = (uint16_t*)(0x400006C);
uint16_t* master_bright_sub = (uint16_t*)(0x400106C);

void fadeinout(int t, int s, int e) {
// 	if( t < s+32 ) {
// 		int16_t val = (s+32)-t;
// 		if(val > 15) {
// 			val = 15;
// 		}
// 		memset( master_bright, (1<<7) | val, 2 );
// 	}
// 	else if(t < e - 16) {
// 		memset( master_bright, (1<<7) | 0, 2 );
// 	}
// 	else if( t < e ) {
// 		int16_t val = 16-(e-t);
// 		memset( master_bright, (1<<7) | val, 2 );
// 	}
// 	else {
// 		memset( master_bright, (1<<7) | 16, 2 );
// 	}
	scur = s;
	ecur = e;
}

void fadeinoutsub(int t2, int scur, int ecur) {
	if( t2 <= scur ) {
		memset( master_bright_sub, (1<<7) | 16, 2 );
	} else if( t2 < scur+20 ) {
		int16_t val = (scur+20)-t2;
		if(val > 15) {
			val = 15;
		}
		memset( master_bright_sub, (1<<7) | val, 2 );
	}
	else if(t2 < ecur - 16) {
		memset( master_bright_sub, (1<<7) | 0, 2 );
	}
	else if( t2 < ecur ) {
		int16_t val = 16-(ecur-t2);
		memset( master_bright_sub, (1<<7) | val, 2 );
	}
	else {
		memset( master_bright_sub, (1<<7) | 16, 2 );
	}

}

// EFFECTS:

// Sub:
// 0: Subscreen whopper. PERMAUSES VRAM C don't use that.

// Main:
// Metaballs: Metaballs
// 1: ECG
// 2: Greets
// 3: Ribbons
// Field: Raymarching

// TODO:
// Starfield?
// Tunnel?
// Vectorballs?

// Intro effect with title (and names?)
// Outro effect with "loves you / vote for us damnit"

// 80ms / row
// 0.08s / row
// 12.5 rows / s
// 60 frames / second
// 4.8 frames / row
// 0.208333333333333 rows / frame

volatile int unts = 0;
volatile int unts_proc = 0;
mm_word myEventHandler( mm_word msg, mm_word param )
{
    switch( msg )
    {
	case MMCB_SONGMESSAGE:
		unts = param;
		unts_proc = 0;
	break;
    }
}

int main()
{
	// Turn on everything.
	POWCNT1 = POWCNT1_ALL_SWAP;
	irqEnable( IRQ_VBLANK );
	irqSet(IRQ_VBLANK,vblank);

	ClaimWRAM();

	t = 0;
	blockload = 1;
	memset( master_bright, (1<<7) | 16, 2 );
	memset( master_bright_sub, (1<<7) | 16, 2 );

	// Init NitroFS for data loading.
	nitroFSInitAdv( BINARY_NAME );
	tempImage = malloc(256*256*2);

	#ifdef DEBUG
// 	consoleDemoInit();
// 	iprintf( "Debug mode.\n" );
	#endif

	// Main loop
// 	#define EFFECT_DEBUG
	#ifdef EFFECT_DEBUG
	effect0_init();
	effect2_init();
// 	InitField();
// 	InitHeartField();
	metaballs_precompute();
	#else
// 	effect3_init();
// 	effect3_destroy();
	effect0_init();
	metaballs_precompute();
	preloadSprites();
	
	// LATER TODO: Init intro
	effect7_init();
	
	#endif
	
	uint8_t *wram=(uint8_t *)0x3000000;
//	memset(wram,0,128*96);

	mmInitDefault( "nitro:/zik/music.bin" );
	mmLoad( MOD_WIENERLONG );
	mmStart( MOD_WIENERLONG, MM_PLAY_ONCE );
	mmSetEventHandler( myEventHandler );
	
	int next_effect_init = 0;
	toff = t;
	
	while( 1 ) { // TODO duration

		#define DUR (64.0 * 2.0 * (4.8))

		//#define DUR (60*4)
		
		#ifdef EFFECT_DEBUG
		effect0_update(t-toff);
// //  		RunHeartField(t);
		effect2_update(t);
		blockload = 0;
// 		RunField(t);
// // 		metaballs_update(t);
// 		if( t == 16*20+30 ) {
// 			effect0_change(1);
// 		}
		#else

		if( t - toff < 2*DUR ) {
			effect7_update(t);
			fadeinout(t-toff,0,2*DUR);
			fadeinoutsub(t-toff,1*DUR,200*DUR);
			blockload = 0;
		}
		else if(t - toff < 3*DUR) {
			if(next_effect_init < 1) {
				effect0_change(0);
				fadeinout(t-toff,2*DUR,3*DUR);
				blockload = 1;
				memset( master_bright, (1<<7) | 16, 2 );
				next_effect_init++;
				effect7_destroy();
				effect4_init();
				effect4_update(t);
				blockload = 0;
			}
			effect4_update(t);
		}
		else if(t - toff  < 4*DUR) {
			if(next_effect_init < 2) {
				effect0_change(1);
				fadeinout(t-toff,3*DUR,4*DUR);				
				next_effect_init++;
				blockload = 1;
				memset( master_bright, (1<<7) | 16, 2 );
				effect4_destroy();
				effect3_init();
				effect3_update(t);
				blockload = 0;
			}
			effect3_update(t);
		}
		else if(t - toff < 5*DUR) {
			if(next_effect_init < 3) {
				effect0_change(2);
				fadeinout(t-toff,4*DUR,5*DUR);				
				next_effect_init++;
				blockload = 1;
				memset( master_bright, (1<<7) | 16, 2 );
				effect3_destroy();
				metaballs_init();
				blockload = 0;
			}
			metaballs_update(t);
		}
		else if(t - toff < 6*DUR) {
			if(next_effect_init < 4) {
				effect0_change(3);
				next_effect_init++;
				blockload = 1;
				memset( master_bright, (1<<7) | 16, 2 );
				metaballs_destroy();
				InitHeartField();
				RunHeartField(t);
				blockload = 0;
			}
			RunHeartField(t);
			fadeinout(t-toff,5*DUR,6*DUR);
		}
		else if(t - toff < 8*DUR) {
			if(next_effect_init < 5) {
				effect0_change(4);
				next_effect_init++;
				blockload = 1;
				memset( master_bright, (1<<7) | 16, 2 );
				// No stop heart field.
				effect2_init();
				effect2_update(t);
				blockload = 0;
			}
			effect2_update(t);
			fadeinout(t-toff,6*DUR,8*DUR);
		}
		else if(t - toff < 9*DUR) {
			if(next_effect_init < 6) {
				effect0_change(5);
				next_effect_init++;
				blockload = 1;
				memset( master_bright, (1<<7) | 16, 2 );
				effect2_destroy();
				effect6_init();
				effect6_update(t);
				blockload = 0;
			}
			effect6_update(t);
			fadeinout(t-toff,8*DUR,9*DUR);
		}
		else if(t - toff < 10*DUR) {
			if(next_effect_init < 7) {
				effect0_change(6);
				next_effect_init++;
				blockload = 1;
				memset( master_bright, (1<<7) | 16, 2 );
				effect6_destroy();
				effect1_init();
				effect1_update(t);				
				blockload = 0;
			}
			effect1_update(t);
			fadeinout(t-toff,9*DUR,10*DUR);
		}
		else if(t - toff < 12*DUR) {
			if(next_effect_init < 8) {
				effect0_change(7);
				next_effect_init++;
				blockload = 1;
				memset( master_bright, (1<<7) | 16, 2 );
				effect1_destroy();
				InitField();
				RunField(t);
				RunField(t);
				blockload = 0;
			}
			if(t - toff > 11*DUR && unts == 1) {
				RunField(t+1200);
			}
			else {
				RunField(t);
			}
			fadeinout(t-toff,10*DUR,12*DUR);
		}
		else if(t - toff < 13*DUR) {
			if(next_effect_init < 9) {
				effect0_change(8);
				next_effect_init++;
				blockload = 1;
				memset( master_bright, (1<<7) | 16, 2 );
				StopField();
				effect7_init_2();
				effect7_update(t);
				blockload = 0;
			}
			effect7_update(t);
			fadeinout(t-toff,12*DUR,13*DUR);
		}
		else {
			if(next_effect_init < 10) {
				effect0_change(9);
				next_effect_init++;
				blockload = 1;
				memset( master_bright, (1<<7) | 16, 2 );
				effect7_init_3();
				effect7_update(t);
				blockload = 0;
			}
			effect7_update(t);
			fadeinout(t-toff,13*DUR,14*DUR);
			fadeinoutsub(t-toff,0,14*DUR);
		}
		#endif
		
 		swiWaitForVBlank();
	}

	POWCNT1 = POWCNT1_ALL;

	for(;;);

	return 0;
}

static void vblank() {
	t++;

	uint32_t t2 = t - toff;
	
	if( t2 <= scur || blockload ) {
		memset( master_bright, (1<<7) | 16, 2 );
	} else if( t2 < scur+20 ) {
		int16_t val = (scur+20)-t2;
		if(val > 15) {
			val = 15;
		}
		memset( master_bright, (1<<7) | val, 2 );
	}
	else if(t2 < ecur - 16) {
		memset( master_bright, (1<<7) | 0, 2 );
	}
	else if( t2 < ecur ) {
		int16_t val = 16-(ecur-t2);
		memset( master_bright, (1<<7) | val, 2 );
	}
	else {
		memset( master_bright, (1<<7) | 16, 2 );
	}

	effect0_update(t-toff);
}


