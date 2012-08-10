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
static void vblank();

extern int tempImage;

void fadeout(int t, int b) {
	uint16_t* love_coloured_master_bright = (uint16_t*)(0x400006C);
	if( t > b-16 ) {
		uint16_t val = 18-(b-t);
		memset( love_coloured_master_bright, (1<<7) | val, 2 );
	}
// 	else {
// 		memset( love_coloured_master_bright, (1<<7) | 15, 2 );
// 	}
}

void fadein(int t, int b) {
	uint16_t* master_bright = (uint16_t*)(0x400006C);
	if( t < b+16 ) {
		uint16_t val = (b+17-t);
		memset( master_bright, (1<<7) | val, 2 );
	}
	else {
		memset( master_bright, (1<<7) | 0, 2 );
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

int main()
{
	// Turn on everything.
	POWCNT1 = POWCNT1_ALL_SWAP;
	irqEnable( IRQ_VBLANK );
	irqSet(IRQ_VBLANK,vblank);

	ClaimWRAM();

	// Init NitroFS for data loading.
	nitroFSInitAdv( BINARY_NAME );
	tempImage = malloc(256*256*2);

	#ifdef DEBUG
	consoleDemoInit();
	iprintf( "Debug mode.\n" );
	#endif

	t = 0;

	// Main loop
// 	#define EFFECT_DEBUG
	#ifdef EFFECT_DEBUG
	effect0_init();
// 	effect4_init();
	InitField();
// 	InitHeartField();
	metaballs_precompute();
	#else
	metaballs_precompute();	
	effect0_init();
	InitField();
	#endif
	
	uint8_t *wram=(uint8_t *)0x3000000;
//	memset(wram,0,128*96);

	mmInitDefault( "nitro:/zik/music.bin" );
	mmLoad( MOD_WIENERLONG );
	mmStart( MOD_WIENERLONG, MM_PLAY_ONCE );
	effect0_change(0);

	int next_effect_init = 0;
	while( 1 ) {

		#define DUR 60
		#ifdef EFFECT_DEBUG
		effect0_update(t);
//  		RunHeartField(t);
		effect4_update(t);
// 		RunField(t);
// 		metaballs_update(t);
		if( t == 16*20+30 ) {
			effect0_change(1);
		}
		#else

		effect0_update(t);
		if( t < 10*DUR ) {
			RunField(t);
		}
		else if(t < 20*DUR) {
			if(next_effect_init < 1) {
				next_effect_init++;
				StopField();
				metaballs_init();
			}
			metaballs_update(t);
		}
		else if(t < 30*DUR) {
			if(next_effect_init < 2) {
				next_effect_init++;
				metaballs_destroy();
				effect2_init();
			}
			effect2_update(t);
		}
		else if(t < 40*DUR) {
			if(next_effect_init < 3) {
				next_effect_init++;
				effect2_destroy();
				effect3_init();
			}
			effect3_update(t);
		}
		else if(t < 50*DUR) {
			if(next_effect_init < 4) {
				next_effect_init++;
				effect3_destroy();
				effect1_init();
			}
			effect1_update(t);
		}
		else if(t < 60*DUR) {
			if(next_effect_init < 5) {
				next_effect_init++;
				effect1_destroy();
				effect4_init();
			}
			effect4_update(t);
		}
		else if(t < 70*DUR) {
			if(next_effect_init < 6) {
				next_effect_init++;
				effect4_destroy();
				effect6_init();
			}
			effect6_update(t);
		}
		else if(t < 80*DUR) {
			if(next_effect_init < 7) {
				next_effect_init++;
				effect6_destroy();
				InitHeartField();
			}
			RunHeartField(t);
		}
		else {

		}
		#endif
		
 		swiWaitForVBlank();
	}

	POWCNT1 = POWCNT1_ALL;

	for(;;);

	return 0;
}

static void vblank() { t++; }


