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
u16* zeroSprite[4];

typedef struct ribbon_tile {
	int x;
	int y;
	int from;
	int to;
} ribbon_tile;

typedef struct ribbon {
	ribbon_tile tiles[512];
	int tile_count;
	int head_status;
	int head_dir;
	int x;
	int y;
} ribbon;

ribbon ribbons[1];

typedef struct row_tile {
	int x;
	int y;
	u16* sprite;
	int beforeHead;
} row_tile;

row_tile row_tiles[12][124];

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

	// Zero-Heads
	zeroSprite[DIR_UP] = loadSprite16A( "nitro:/gfx/ribbon_headup_zero.img.bin" );
	zeroSprite[DIR_RIGHT] = loadSprite16A( "nitro:/gfx/ribbon_headright_zero.img.bin" );
	zeroSprite[DIR_LEFT] = loadSprite16A( "nitro:/gfx/ribbon_headleft_zero.img.bin" );
	zeroSprite[DIR_DOWN] = loadSprite16A( "nitro:/gfx/ribbon_headdown_zero.img.bin" );
	
	// Heads
	ribbonSprite[DIR_UP][DIR_UP] = loadSprite16A( "nitro:/gfx/ribbon_headup.img.bin" );
	ribbonSprite[DIR_RIGHT][DIR_RIGHT] = loadSprite16A( "nitro:/gfx/ribbon_headright.img.bin" );
	ribbonSprite[DIR_LEFT][DIR_LEFT] = loadSprite16A( "nitro:/gfx/ribbon_headleft.img.bin" );
	ribbonSprite[DIR_DOWN][DIR_DOWN] = loadSprite16A( "nitro:/gfx/ribbon_headdown.img.bin" );

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
	ribbonSprite[DIR_RIGHT][DIR_UP] = loadSprite16A( "nitro:/gfx/ribbon_rightup.img.bin" );
	ribbonSprite[DIR_RIGHT][DIR_DOWN] = loadSprite16A( "nitro:/gfx/ribbon_rightdown.img.bin" );

	// Palette
	load8bVRAMIndirect("nitro:/gfx/ribbon_rightleft.pal.bin", PALRAM_OBJ_A,256*2);

	// Base ribbon for testing
	ribbons[0].tiles[0].x = 8;
	ribbons[0].tiles[0].y = 0;
	ribbons[0].tiles[0].from = DIR_UP;
	ribbons[0].tiles[0].to = DIR_DOWN;

	ribbons[0].tile_count = 1;
	ribbons[0].x = 8;
	ribbons[0].y = 0;
	ribbons[0].head_dir = DIR_DOWN;
	ribbons[0].head_status = 3;
}

void update_ribbons() {
	int ribbon_cnt = 0;
	ribbons[ribbon_cnt].head_status = (ribbons[ribbon_cnt].head_status + 1) % 8;
	if(ribbons[ribbon_cnt].head_status == 0) {
		int forbidden_dir = -1;
		int forbidden_dir_2 = -1;
		if(ribbons[ribbon_cnt].head_dir == DIR_RIGHT) {
			ribbons[ribbon_cnt].x++;
			forbidden_dir = DIR_LEFT;
		}
		if(ribbons[ribbon_cnt].head_dir == DIR_LEFT) {
			ribbons[ribbon_cnt].x--;
			forbidden_dir = DIR_RIGHT;
		}
		if(ribbons[ribbon_cnt].head_dir == DIR_DOWN) {
			ribbons[ribbon_cnt].y++;
			forbidden_dir = DIR_UP;
		}
		if(ribbons[ribbon_cnt].head_dir == DIR_UP) {
			ribbons[ribbon_cnt].y--;
			forbidden_dir = DIR_DOWN;
		}
		if(ribbons[ribbon_cnt].x < 0) {
			forbidden_dir_2 = DIR_LEFT;
		}
		if(ribbons[ribbon_cnt].x > 15) {
			forbidden_dir_2 = DIR_RIGHT;
		}
		if(ribbons[ribbon_cnt].y < 0) {
			forbidden_dir_2 = DIR_UP;
		}
		if(ribbons[ribbon_cnt].y > 12) {
			forbidden_dir_2 = DIR_DOWN;
		}
		ribbons[ribbon_cnt].tiles[ribbons[ribbon_cnt].tile_count].x = ribbons[ribbon_cnt].x;
		ribbons[ribbon_cnt].tiles[ribbons[ribbon_cnt].tile_count].y = ribbons[ribbon_cnt].y;
		ribbons[ribbon_cnt].tiles[ribbons[ribbon_cnt].tile_count].from = forbidden_dir;
		do {
			do {
				ribbons[ribbon_cnt].head_dir = Random() % 4;
			} while(ribbons[ribbon_cnt].head_dir == forbidden_dir);
			
		} while(ribbons[ribbon_cnt].head_dir == forbidden_dir_2);
		ribbons[ribbon_cnt].tiles[ribbons[ribbon_cnt].tile_count].to = ribbons[ribbon_cnt].head_dir;
		ribbons[ribbon_cnt].tile_count++;		
	}
}

u8 effect3_update( u32 t ) {

	update_ribbons();
	
	int sprite_cnt = 0;
	int ribbon_cnt = 0;

	// Newest
	u16* newestSprite = ribbonSprite[
		ribbons[ribbon_cnt].tiles[ribbons[ribbon_cnt].tile_count-1].from
	][
		ribbons[ribbon_cnt].tiles[ribbons[ribbon_cnt].tile_count-1].to
	];
	if(ribbons[ribbon_cnt].head_status == 0) {
		int old_head;
		if(ribbons[ribbon_cnt].tiles[ribbons[ribbon_cnt].tile_count-1].from == DIR_RIGHT) {
			old_head = DIR_LEFT;
		}
		if(ribbons[ribbon_cnt].tiles[ribbons[ribbon_cnt].tile_count-1].from == DIR_LEFT) {
			old_head = DIR_RIGHT;
		}
		if(ribbons[ribbon_cnt].tiles[ribbons[ribbon_cnt].tile_count-1].from == DIR_DOWN) {
			old_head = DIR_UP;
		}
		if(ribbons[ribbon_cnt].tiles[ribbons[ribbon_cnt].tile_count-1].from == DIR_UP) {
			old_head = DIR_DOWN;
		}
		newestSprite = ribbonSprite[old_head][old_head];
	}

	oamSet(
		&oamMain, sprite_cnt,
		ribbons[ribbon_cnt].tiles[ribbons[ribbon_cnt].tile_count-1].x*16,
		ribbons[ribbon_cnt].tiles[ribbons[ribbon_cnt].tile_count-1].y*16,
		1, 0,
		SpriteSize_16x16,
		SpriteColorFormat_256Color,
		newestSprite,
		32, false, false, false, false, false
	);
	sprite_cnt++;

	// Head
	int head_xd = 0;
	int head_yd = 0;
	if(ribbons[ribbon_cnt].head_dir == DIR_RIGHT) {
		head_xd = 2*ribbons[ribbon_cnt].head_status;
	}
	if(ribbons[ribbon_cnt].head_dir == DIR_LEFT) {
		head_xd = -2*ribbons[ribbon_cnt].head_status;
	}
	if(ribbons[ribbon_cnt].head_dir == DIR_DOWN) {
		head_yd = 2*ribbons[ribbon_cnt].head_status;
	}
	if(ribbons[ribbon_cnt].head_dir == DIR_UP) {
		head_yd = -2*ribbons[ribbon_cnt].head_status;
	}
	oamSet(
		&oamMain, sprite_cnt,
		ribbons[ribbon_cnt].x*16+head_xd,ribbons[ribbon_cnt].y*16+head_yd,
		ribbons[ribbon_cnt].head_status <= 1 ? 0 : 1, 0,
		SpriteSize_16x16,
		SpriteColorFormat_256Color,
		ribbons[ribbon_cnt].head_status <= 1 ?
			zeroSprite[ribbons[ribbon_cnt].head_dir] :
			ribbonSprite[ribbons[ribbon_cnt].head_dir][ribbons[ribbon_cnt].head_dir],
		32, false, false, false, false, false
	);
	sprite_cnt++;
	
	// All-but-newest
	for(int tile_cnt = ribbons[ribbon_cnt].tile_count - 2; tile_cnt >= 0; tile_cnt-- ) {
		oamSet(
			&oamMain, sprite_cnt,
			ribbons[ribbon_cnt].tiles[tile_cnt].x*16,ribbons[ribbon_cnt].tiles[tile_cnt].y*16,
			1, 0,
			SpriteSize_16x16,
			SpriteColorFormat_256Color,
			ribbonSprite[ribbons[ribbon_cnt].tiles[tile_cnt].from][ribbons[ribbon_cnt].tiles[tile_cnt].to],
			32, false, false, false, false, false
		);
		sprite_cnt++;
	}

	oamUpdate(&oamMain);
	
	return( 0 );
}


void effect3_destroy() {

}
