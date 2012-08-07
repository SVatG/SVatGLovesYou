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
#include "ARM.h"

// Sound!
#include <maxmod9.h>
#include "music.h"

volatile uint32_t t;
static void vblank();

extern int tempImage;

uint8_t ATTR_DTCM dtcm_buffer[12288];

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
// 0: Subscreen whopper. PERMAUSES VRAM C don't use that.
// Metaballs: Metaballs.

int main()
{
	// Turn on everything.
	POWCNT1 = POWCNT1_ALL_SWAP;
	irqEnable( IRQ_VBLANK );
	irqSet(IRQ_VBLANK,vblank);

// 	ClaimWRAM();

	// Init NitroFS for data loading.
	nitroFSInitAdv( BINARY_NAME );
	tempImage = malloc(256*256*2);

	#ifdef DEBUG
	consoleDemoInit();
	iprintf( "Debug mode.\n" );
	#endif

	t = 0;

	// Main loop
	#define EFFECT_DEBUG
	#ifdef EFFECT_DEBUG
	effect0_init();
	effect2_init();
	metaballs_precompute();
	#else
	effect0_init();
	effect1_init();
	#endif
	
	uint8_t *wram=(uint8_t *)0x3000000;
//	memset(wram,0,128*96);

	mmInitDefault( "nitro:/zik/music.bin" );
	mmLoad( MOD_RAINBOWS_CLN );
	mmStart( MOD_RAINBOWS_CLN, MM_PLAY_ONCE );
	effect0_change(0);

	while( t<140*60 ) {

		#ifdef EFFECT_DEBUG
		effect0_update(t);
		effect2_update(t);
// 		metaballs_update(t);
		if( t == 16*20+30 ) {
			effect0_change(1);
		}
		#else
		
		if( t < 16*60+30 ) {
// 			RunRadial(t,dtcm_buffer);
// 			fadeout(t,16*60+30);
// 			if( t >= 16*60+30-2 ) {
// 				StopRadial();
// 				cubemode = 0;
// 				effect6_init();
// 			}
		}
		else if(t < 27*60) {
// 			effect6_update(t);
// 			fadein(t,16*60+30);
// 			fadeout(t,27*60);
// 			if( t >= 27*60-3 ) {
// 				effect6_destroy();
// 				effect2_init();
// 			}
		}
		else if( t < 37*60 ) {
// 			effect2_update(t);
// 			fadein(t,27*60);
// 			fadeout(t,37*60);
// 			if( t >= 37*60-3 ) {
// 				effect2_destroy();
// 				effect1_init();
// 			}
		}
		else if( t < 129*60 ) {
// 			Truchet(t);
// 			fadein(t,108*60);
// 			if( t >= 129*60-2 ) {
// 				effect7_init();
// 			}
		}
		else {
// 			effect7_update(t);
		}
		#endif
		
 		swiWaitForVBlank();
	}

	POWCNT1 = POWCNT1_ALL;

	for(;;);

	return 0;
}

static void vblank() { t++; }


