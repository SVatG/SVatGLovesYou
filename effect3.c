#include <nds.h>
#include <nds/fifocommon.h>
#include <stdlib.h>
#include <nds/registers_alt.h>
#include "Hardware.h"

#include "DS3D/DS3D.h"

#define DIR_UP 0
#define DIR_RIGHT 1
#define DIR_LEFT 2
#define DIR_DOWN 3

u16* ribbonSprite[4][4];

typedef struct ribbon {
	int tiles[50];
	int tile_count;
	int head_status;
	int x;
	int y;
} ribbon;

void effect3_init() {
	
	DISPCNT_A = DISPCNT_MODE_5 | DISPCNT_OBJ_ON | DISPCNT_ON;

	VRAMCNT_A = VRAMCNT_A_BG_VRAM_A_OFFS_0K;
	BG2CNT_A = BGxCNT_EXTENDED_BITMAP_16 | BGxCNT_OVERFLOW_WRAP | BGxCNT_BITMAP_SIZE_256x256 | BGxCNT_BITMAP_BASE_0K;
	BG2CNT_A = (BG2CNT_A&~BGxCNT_PRIORITY_MASK)|BGxCNT_PRIORITY_1;
	BG2_XDX = (1 << 10);
	BG2_XDY = 0;
	BG2_YDX = 0;
	BG2_YDY = (1 << 10);
	BG2_CX = 0;
	BG2_CY = 0;

	VRAMCNT_A = VRAMCNT_A_OBJ_VRAM_A;

	oamInit(&oamMain, SpriteMapping_Bmp_1D_128, false);

	// Heads
	ribbonSprite[DIR_UP][DIR_UP] = loadSprite16A( "nitro:/gfx/ribbon_headup.img.bin" );
	ribbonSprite[DIR_RIGHT][DIR_RIGHT] = loadSprite16A( "nitro:/gfx/ribbon_headup.img.bin" );
	ribbonSprite[DIR_LEFT][DIR_LEFT] = loadSprite16A( "nitro:/gfx/ribbon_headup.img.bin" );
	ribbonSprite[DIR_DOWN][DIR_DOWN] = loadSprite16A( "nitro:/gfx/ribbon_headup.img.bin" );

	// Straights
	ribbonSprite[DIR_UP][DIR_DOWN] = loadSprite16A( "nitro:/gfx/ribbon_updown.img.bin" );
	ribbonSprite[DIR_DOWN][DIR_UP] = loadSprite16A( "nitro:/gfx/ribbon_downup.img.bin" );
	ribbonSprite[DIR_LEFT][DIR_RIGHT] = loadSprite16A( "nitro:/gfx/ribbon_leftright.img.bin" );
	ribbonSprite[DIR_RIGHT][DIR_LEFT] = loadSprite16A( "nitro:/gfx/ribbon_rightleft.img.bin" );

	// Curves from up
	ribbonSprite[DIR_UP][DIR_RIGHT] = loadSprite16A( "nitro:/gfx/ribbon_upright.img.bin" );
	ribbonSprite[DIR_UP][DIR_LEFT] = loadSprite16A( "nitro:/gfx/ribbon_upleft.img.bin" );

	// Curves from down
	ribbonSprite[DIR_DOWN][DIR_RIGHT] = loadSprite16A( "nitro:/gfx/ribbon_downright.img.bin" );
	ribbonSprite[DIR_DOWN][DIR_LEFT] = loadSprite16A( "nitro:/gfx/ribbon_downleft.img.bin" );

	// Curves from left
	ribbonSprite[DIR_LEFT][DIR_UP] = loadSprite16A( "nitro:/gfx/ribbon_leftup.img.bin" );
	ribbonSprite[DIR_LEFT][DIR_DOWN] = loadSprite16A( "nitro:/gfx/ribbon_leftdown.img.bin" );

	// Curves from right
	ribbonSprite[DIR_LEFT][DIR_UP] = loadSprite16A( "nitro:/gfx/ribbon_rightup.img.bin" );
	ribbonSprite[DIR_LEFT][DIR_DOWN] = loadSprite16A( "nitro:/gfx/ribbon_rightdown.img.bin" );

	// Palette
	load8bVRAMIndirect("nitro:/gfx/ribbon_rightdown.pal.bin", PALRAM_A,256*2);
}

u8 effect3_update( u32 t ) {
	for(int x = 0; x < 4; x++ ) {
		for(int y = 0; y < 4; y++ ) {
			oamSet(
				&oamMain, 4*x+y,
				x*16,y*16,
				0, 0,
				SpriteSize_16x16,
				SpriteColorFormat_256Color,
				ribbonSprite[x][y],
				32, false, false, false, false, false
			);
		}
	}

	oamUpdate(&oamMain);
	
	return( 0 );
}


void effect3_destroy() {

}
