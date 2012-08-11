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

#define RIBBON_COUNT 4
#define MAX_SEGMENTS 512
#define MAX_PERROW 56

u16* ribbonSprite[4][4][4];
u16* zeroSprite[4];
u16* heartSprite;

extern volatile int unts;
extern volatile int unts_proc;

typedef struct ribbon_tile {
	int x;
	int y;
	int from;
	int to;
	int tile;
} ribbon_tile;

typedef struct ribbon {
	ribbon_tile* tiles;
	int tile_count;
	int head_status;
	int head_tile;
	int head_dir;
	int x;
	int y;
} ribbon;

ribbon ribbons[RIBBON_COUNT];

typedef struct row_tile {
	u16 x;
	u16 y;
	u16* sprite;
	u8 prio;
} row_tile;

row_tile row_tiles[12][MAX_PERROW];
int row_tile_counts[12];

void switchSprites() {
	int row = ((VCOUNT+16)>>4)&0xF;
	int line = (VCOUNT)&0xF;
	if(line <= 13) {
		for(int i = line*4; i < line*4+4; i++) {
			oamSet(
				&oamMain, RIBBON_COUNT*2 + i + 62*(row&0x1),
				row_tiles[row][i].x,row_tiles[row][i].y,
				row_tiles[row][i].prio, 0,
				SpriteSize_16x16,
				SpriteColorFormat_256Color,
				row_tiles[row][i].sprite,
				32, false, false, false, false, false
			);
		}
	}
	if(line == 13) {
		DC_FlushRange(oamMain.oamMemory + 64*(row&0x1), sizeof(SpriteEntry)*64);
		dmaCopy(oamMain.oamMemory + 64*(row&0x1), OAM, sizeof(SpriteEntry)*64);
	}
}

void reset_rows() {
	// Reset counts.
	for(int i = 0; i < 12; i++) {
		row_tile_counts[i] = 0;
	}

	// Reset positions
	for(int i = 0; i < 12; i++) {
		for(int j = 0; j < MAX_PERROW; j++) {
			row_tiles[i][j].x = -16;
			row_tiles[i][j].y = -16;
			row_tiles[i][j].sprite = ribbonSprite[DIR_RIGHT][DIR_UP][0];
			row_tiles[i][j].prio = 0;
		}
	}
}

void reset_ribbons() {
	// Base ribbons.
	ribbons[0].tiles[0].x = 8;
	ribbons[0].tiles[0].y = 6;
	ribbons[0].tiles[0].from = DIR_UP;
	ribbons[0].tiles[0].to = DIR_DOWN;
	ribbons[0].tiles[0].tile = 3;

	ribbons[0].tile_count = 1;
	ribbons[0].x = 8;
	ribbons[0].y = 6;
	ribbons[0].head_dir = DIR_DOWN;
	ribbons[0].head_status = 3;
	ribbons[0].head_tile = 0;
	
	ribbons[1].tiles[0].x = 8;
	ribbons[1].tiles[0].y = 6;
	ribbons[1].tiles[0].from = DIR_LEFT;
	ribbons[1].tiles[0].to = DIR_RIGHT;
	ribbons[1].tiles[0].tile = 3;
	
	ribbons[1].tile_count = 1;
	ribbons[1].x = 8;
	ribbons[1].y = 6;
	ribbons[1].head_dir = DIR_RIGHT;
	ribbons[1].head_status = 3;
	ribbons[1].head_tile = 0;
	
	ribbons[2].tiles[0].x = 8;
	ribbons[2].tiles[0].y = 6;
	ribbons[2].tiles[0].from = DIR_DOWN;
	ribbons[2].tiles[0].to = DIR_UP;
	ribbons[2].tiles[0].tile = 3;

	ribbons[2].tile_count = 1;
	ribbons[2].x = 8;
	ribbons[2].y = 6;
	ribbons[2].head_dir = DIR_UP;
	ribbons[2].head_status = 3;
	ribbons[2].head_tile = 0;

	ribbons[3].tiles[0].x = 8;
	ribbons[3].tiles[0].y = 6;
	ribbons[3].tiles[0].from = DIR_RIGHT;
	ribbons[3].tiles[0].to = DIR_LEFT;
	ribbons[3].tiles[0].tile = 3;

	ribbons[3].tile_count = 1;
	ribbons[3].x = 8;
	ribbons[3].y = 6;
	ribbons[3].head_dir = DIR_LEFT;
	ribbons[3].head_status = 3;
	ribbons[3].head_tile = 0;
}

void loadSprites() {
	// Heart
	heartSprite = loadBmpSpriteAGreen( "nitro:/gfx/ribbon_heart.img.bin" );
	
	// Zero-Heads
	zeroSprite[DIR_UP] = loadSprite16A( "nitro:/gfx/ribbon_zero_up_1.img.bin" );
	zeroSprite[DIR_RIGHT] = loadSprite16A( "nitro:/gfx/ribbon_zero_right_1.img.bin" );
	zeroSprite[DIR_LEFT] = loadSprite16A( "nitro:/gfx/ribbon_zero_left_1.img.bin" );
	zeroSprite[DIR_DOWN] = loadSprite16A( "nitro:/gfx/ribbon_zero_down_1.img.bin" );

	// 4 frames each.

	// Frame 1
	// Heads
	ribbonSprite[DIR_UP][DIR_UP][0] = loadSprite16A( "nitro:/gfx/ribbon_head_up_1.img.bin" );
	ribbonSprite[DIR_RIGHT][DIR_RIGHT][0] = loadSprite16A( "nitro:/gfx/ribbon_head_right_1.img.bin" );
	ribbonSprite[DIR_LEFT][DIR_LEFT][0] = loadSprite16A( "nitro:/gfx/ribbon_head_left_1.img.bin" );
	ribbonSprite[DIR_DOWN][DIR_DOWN][0] = loadSprite16A( "nitro:/gfx/ribbon_head_down_1.img.bin" );

	// Straights
	ribbonSprite[DIR_UP][DIR_DOWN][0] = loadSprite16A( "nitro:/gfx/ribbon_up_down_1.img.bin" );
	ribbonSprite[DIR_DOWN][DIR_UP][0] = loadSprite16A( "nitro:/gfx/ribbon_down_up_1.img.bin" );
	ribbonSprite[DIR_LEFT][DIR_RIGHT][0] = loadSprite16A( "nitro:/gfx/ribbon_left_right_1.img.bin" );
	ribbonSprite[DIR_RIGHT][DIR_LEFT][0] = loadSprite16A( "nitro:/gfx/ribbon_right_left_1.img.bin" );

	// Curves from up
	ribbonSprite[DIR_UP][DIR_RIGHT][0] = loadSprite16A( "nitro:/gfx/ribbon_up_right_1.img.bin" );
	ribbonSprite[DIR_UP][DIR_LEFT][0] = loadSprite16A( "nitro:/gfx/ribbon_up_left_1.img.bin" );

	// Curves from down
	ribbonSprite[DIR_DOWN][DIR_RIGHT][0] = loadSprite16A( "nitro:/gfx/ribbon_down_right_1.img.bin" );
	ribbonSprite[DIR_DOWN][DIR_LEFT][0] = loadSprite16A( "nitro:/gfx/ribbon_down_left_1.img.bin" );

	// Curves from left
	ribbonSprite[DIR_LEFT][DIR_UP][0] = loadSprite16A( "nitro:/gfx/ribbon_left_up_1.img.bin" );
	ribbonSprite[DIR_LEFT][DIR_DOWN][0] = loadSprite16A( "nitro:/gfx/ribbon_left_down_1.img.bin" );

	// Curves from right
	ribbonSprite[DIR_RIGHT][DIR_UP][0] = loadSprite16A( "nitro:/gfx/ribbon_right_up_1.img.bin" );
	ribbonSprite[DIR_RIGHT][DIR_DOWN][0] = loadSprite16A( "nitro:/gfx/ribbon_right_down_1.img.bin" );

	// Frame 2
	ribbonSprite[DIR_UP][DIR_UP][1] = loadSprite16A( "nitro:/gfx/ribbon_head_up_2.img.bin" );
	ribbonSprite[DIR_RIGHT][DIR_RIGHT][1] = loadSprite16A( "nitro:/gfx/ribbon_head_right_2.img.bin" );
	ribbonSprite[DIR_LEFT][DIR_LEFT][1] = loadSprite16A( "nitro:/gfx/ribbon_head_left_2.img.bin" );
	ribbonSprite[DIR_DOWN][DIR_DOWN][1] = loadSprite16A( "nitro:/gfx/ribbon_head_down_2.img.bin" );

	// Straights
	ribbonSprite[DIR_UP][DIR_DOWN][1] = loadSprite16A( "nitro:/gfx/ribbon_up_down_2.img.bin" );
	ribbonSprite[DIR_DOWN][DIR_UP][1] = loadSprite16A( "nitro:/gfx/ribbon_down_up_2.img.bin" );
	ribbonSprite[DIR_LEFT][DIR_RIGHT][1] = loadSprite16A( "nitro:/gfx/ribbon_left_right_2.img.bin" );
	ribbonSprite[DIR_RIGHT][DIR_LEFT][1] = loadSprite16A( "nitro:/gfx/ribbon_right_left_2.img.bin" );

	// Curves from up
	ribbonSprite[DIR_UP][DIR_RIGHT][1] = loadSprite16A( "nitro:/gfx/ribbon_up_right_2.img.bin" );
	ribbonSprite[DIR_UP][DIR_LEFT][1] = loadSprite16A( "nitro:/gfx/ribbon_up_left_2.img.bin" );

	// Curves from down
	ribbonSprite[DIR_DOWN][DIR_RIGHT][1] = loadSprite16A( "nitro:/gfx/ribbon_down_right_2.img.bin" );
	ribbonSprite[DIR_DOWN][DIR_LEFT][1] = loadSprite16A( "nitro:/gfx/ribbon_down_left_2.img.bin" );

	// Curves from left
	ribbonSprite[DIR_LEFT][DIR_UP][1] = loadSprite16A( "nitro:/gfx/ribbon_left_up_2.img.bin" );
	ribbonSprite[DIR_LEFT][DIR_DOWN][1] = loadSprite16A( "nitro:/gfx/ribbon_left_down_2.img.bin" );

	// Curves from right
	ribbonSprite[DIR_RIGHT][DIR_UP][1] = loadSprite16A( "nitro:/gfx/ribbon_right_up_2.img.bin" );
	ribbonSprite[DIR_RIGHT][DIR_DOWN][1] = loadSprite16A( "nitro:/gfx/ribbon_right_down_2.img.bin" );

	// Frame 3
	// Heads
	ribbonSprite[DIR_UP][DIR_UP][2] = loadSprite16A( "nitro:/gfx/ribbon_head_up_3.img.bin" );
	ribbonSprite[DIR_RIGHT][DIR_RIGHT][2] = loadSprite16A( "nitro:/gfx/ribbon_head_right_3.img.bin" );
	ribbonSprite[DIR_LEFT][DIR_LEFT][2] = loadSprite16A( "nitro:/gfx/ribbon_head_left_3.img.bin" );
	ribbonSprite[DIR_DOWN][DIR_DOWN][2] = loadSprite16A( "nitro:/gfx/ribbon_head_down_3.img.bin" );

	// Straights
	ribbonSprite[DIR_UP][DIR_DOWN][2] = loadSprite16A( "nitro:/gfx/ribbon_up_down_3.img.bin" );
	ribbonSprite[DIR_DOWN][DIR_UP][2] = loadSprite16A( "nitro:/gfx/ribbon_down_up_3.img.bin" );
	ribbonSprite[DIR_LEFT][DIR_RIGHT][2] = loadSprite16A( "nitro:/gfx/ribbon_left_right_3.img.bin" );
	ribbonSprite[DIR_RIGHT][DIR_LEFT][2] = loadSprite16A( "nitro:/gfx/ribbon_right_left_3.img.bin" );

	// Curves from up
	ribbonSprite[DIR_UP][DIR_RIGHT][2] = loadSprite16A( "nitro:/gfx/ribbon_up_right_3.img.bin" );
	ribbonSprite[DIR_UP][DIR_LEFT][2] = loadSprite16A( "nitro:/gfx/ribbon_up_left_3.img.bin" );

	// Curves from down
	ribbonSprite[DIR_DOWN][DIR_RIGHT][2] = loadSprite16A( "nitro:/gfx/ribbon_down_right_3.img.bin" );
	ribbonSprite[DIR_DOWN][DIR_LEFT][2] = loadSprite16A( "nitro:/gfx/ribbon_down_left_3.img.bin" );

	// Curves from left
	ribbonSprite[DIR_LEFT][DIR_UP][2] = loadSprite16A( "nitro:/gfx/ribbon_left_up_3.img.bin" );
	ribbonSprite[DIR_LEFT][DIR_DOWN][2] = loadSprite16A( "nitro:/gfx/ribbon_left_down_3.img.bin" );

	// Curves from right
	ribbonSprite[DIR_RIGHT][DIR_UP][2] = loadSprite16A( "nitro:/gfx/ribbon_right_up_3.img.bin" );
	ribbonSprite[DIR_RIGHT][DIR_DOWN][2] = loadSprite16A( "nitro:/gfx/ribbon_right_down_3.img.bin" );

	// Frame 4
	// Heads
	ribbonSprite[DIR_UP][DIR_UP][3] = loadSprite16A( "nitro:/gfx/ribbon_head_up_4.img.bin" );
	ribbonSprite[DIR_RIGHT][DIR_RIGHT][3] = loadSprite16A( "nitro:/gfx/ribbon_head_right_4.img.bin" );
	ribbonSprite[DIR_LEFT][DIR_LEFT][3] = loadSprite16A( "nitro:/gfx/ribbon_head_left_4.img.bin" );
	ribbonSprite[DIR_DOWN][DIR_DOWN][3] = loadSprite16A( "nitro:/gfx/ribbon_head_down_4.img.bin" );

	// Straights
	ribbonSprite[DIR_UP][DIR_DOWN][3] = loadSprite16A( "nitro:/gfx/ribbon_up_down_4.img.bin" );
	ribbonSprite[DIR_DOWN][DIR_UP][3] = loadSprite16A( "nitro:/gfx/ribbon_down_up_4.img.bin" );
	ribbonSprite[DIR_LEFT][DIR_RIGHT][3] = loadSprite16A( "nitro:/gfx/ribbon_left_right_4.img.bin" );
	ribbonSprite[DIR_RIGHT][DIR_LEFT][3] = loadSprite16A( "nitro:/gfx/ribbon_right_left_4.img.bin" );

	// Curves from up
	ribbonSprite[DIR_UP][DIR_RIGHT][3] = loadSprite16A( "nitro:/gfx/ribbon_up_right_4.img.bin" );
	ribbonSprite[DIR_UP][DIR_LEFT][3] = loadSprite16A( "nitro:/gfx/ribbon_up_left_4.img.bin" );

	// Curves from down
	ribbonSprite[DIR_DOWN][DIR_RIGHT][3] = loadSprite16A( "nitro:/gfx/ribbon_down_right_4.img.bin" );
	ribbonSprite[DIR_DOWN][DIR_LEFT][3] = loadSprite16A( "nitro:/gfx/ribbon_down_left_4.img.bin" );

	// Curves from left
	ribbonSprite[DIR_LEFT][DIR_UP][3] = loadSprite16A( "nitro:/gfx/ribbon_left_up_4.img.bin" );
	ribbonSprite[DIR_LEFT][DIR_DOWN][3] = loadSprite16A( "nitro:/gfx/ribbon_left_down_4.img.bin" );

	// Curves from right
	ribbonSprite[DIR_RIGHT][DIR_UP][3] = loadSprite16A( "nitro:/gfx/ribbon_right_up_4.img.bin" );
	ribbonSprite[DIR_RIGHT][DIR_DOWN][3] = loadSprite16A( "nitro:/gfx/ribbon_right_down_4.img.bin" );
}

// Only possible if vram bank a remains untouched.
void preloadSprites() {
	DISPCNT_A = DISPCNT_MODE_5 | DISPCNT_OBJ_ON | DISPCNT_BG2_ON | DISPCNT_BG3_ON | DISPCNT_ON;
	VRAMCNT_A = VRAMCNT_A_OBJ_VRAM_A;
	VRAMCNT_B = VRAMCNT_B_BG_VRAM_A_OFFS_0K;
	VRAMCNT_D = VRAMCNT_D_BG_VRAM_A_OFFS_128K;
	oamInit(&oamMain, SpriteMapping_Bmp_1D_128, false);
	loadSprites();
}

void effect3_init() {

// 	for(int i = 0; i < 12; i++) {
// 		row_tiles[i] = malloc(sizeof(row_tile)*MAX_PERROW);
// 	}

	for(int i = 0; i < RIBBON_COUNT; i++) {
		ribbons[i].tiles = malloc(MAX_SEGMENTS * sizeof(ribbon_tile));
	}
	
	// DISPCNT_A = DISPCNT_MODE_5 | DISPCNT_OBJ_ON | DISPCNT_BG2_ON | DISPCNT_BG3_ON | DISPCNT_ON;

	VRAMCNT_A = VRAMCNT_A_OBJ_VRAM_A;
	VRAMCNT_B = VRAMCNT_B_BG_VRAM_A_OFFS_0K;
	VRAMCNT_D = VRAMCNT_D_BG_VRAM_A_OFFS_128K;
	
	BG2CNT_A = BGxCNT_EXTENDED_BITMAP_16 | BGxCNT_OVERFLOW_WRAP | BGxCNT_BITMAP_SIZE_256x256 | BGxCNT_BITMAP_BASE_0K;
	BG2CNT_A = (BG2CNT_A&~BGxCNT_PRIORITY_MASK)|BGxCNT_PRIORITY_0;
	BG2_XDX = (1 << 8);
	BG2_XDY = 0;
	BG2_YDX = 0;
	BG2_YDY = (1 << 8);
	BG2_CX = 0;
	BG2_CY = 0;

	BG3CNT_A = BGxCNT_EXTENDED_BITMAP_16 | BGxCNT_OVERFLOW_WRAP | BGxCNT_BITMAP_SIZE_256x256 | BGxCNT_BITMAP_BASE_128K;
	BG3CNT_A = (BG3CNT_A&~BGxCNT_PRIORITY_MASK)|BGxCNT_PRIORITY_3;
	BG3_XDX = (1 << 8);
	BG3_XDY = 0;
	BG3_YDX = 0;
	BG3_YDY = (1 << 8);
	BG3_CX = 0;
	BG3_CY = 0;

	loadImageVRAMIndirectGreen( "nitro:/gfx/ribbon_frame.img.bin", VRAM_A_OFFS_0K,256*256*2);
	loadImageVRAMIndirectGreen( "nitro:/gfx/stripe_bg.img.bin", VRAM_A_OFFS_128K,256*256*2);

	vu16* mem_BLDCNT_A = (vu16*)(0x04000050);
	*mem_BLDCNT_A = BLDCNT_SRC_A_OBJ | BLDCNT_SRC_B_BG3 | BLDCNT_EFFECT_ALPHA;
	BLDALPHA_A = BLDALPHA_EVA(13)|BLDALPHA_EVB(2);
	
	// Palette
	load8bVRAMIndirect("nitro:/gfx/ribbon_right_left_1.pal.bin", PALRAM_OBJ_A,256);

	reset_ribbons();
	reset_rows();
	
	irqSet(IRQ_HBLANK,&switchSprites);
	irqEnable( IRQ_HBLANK );	
}

int rcounter = 0;
int rcounter2 = 0;
void update_ribbons() {
	if(unts != 0 && unts_proc == 0) {
		rcounter++;
		unts_proc = 1;
		if(rcounter == 4) {
			rcounter2++;
			rcounter = 1;
			if(rcounter2 <= 2) {
				reset_ribbons();
			}
		}
	}
	for(int ribbon_cnt = 0; ribbon_cnt < RIBBON_COUNT; ribbon_cnt++) {
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
			ribbons[ribbon_cnt].tiles[ribbons[ribbon_cnt].tile_count].tile = ribbons[ribbon_cnt].head_tile;
			ribbons[ribbon_cnt].head_tile = (ribbons[ribbon_cnt].head_tile + 1)%4;
			ribbons[ribbon_cnt].tile_count++;
		}
	}
}

void ribbons_to_rowribbons() {

	reset_rows();
	
	// Newest tiles
	for(int ribbon_cnt = 0; ribbon_cnt < RIBBON_COUNT; ribbon_cnt++) {
		u16* newestSprite = ribbonSprite[
			ribbons[ribbon_cnt].tiles[ribbons[ribbon_cnt].tile_count-1].from
		][
			ribbons[ribbon_cnt].tiles[ribbons[ribbon_cnt].tile_count-1].to
		][ribbons[ribbon_cnt].tiles[ribbons[ribbon_cnt].tile_count-1].tile];
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
			newestSprite = ribbonSprite[old_head][old_head][ribbons[ribbon_cnt].tiles[ribbons[ribbon_cnt].tile_count-1].tile];
		}

		int idx_y = ribbons[ribbon_cnt].tiles[ribbons[ribbon_cnt].tile_count-1].y;
		if(idx_y < 0 || idx_y > 11) {
			continue;
		}
		row_tiles[idx_y][row_tile_counts[idx_y]].x = ribbons[ribbon_cnt].tiles[ribbons[ribbon_cnt].tile_count-1].x*16;
		row_tiles[idx_y][row_tile_counts[idx_y]].y = ribbons[ribbon_cnt].tiles[ribbons[ribbon_cnt].tile_count-1].y*16;
		row_tiles[idx_y][row_tile_counts[idx_y]].sprite = newestSprite;
		row_tiles[idx_y][row_tile_counts[idx_y]].prio = 1;
		row_tile_counts[idx_y]++;
	}

	// Other tiles
	int set_one = 0;
	for(int tile_cnt = MAX_SEGMENTS; tile_cnt > 0; tile_cnt--) {
		set_one = 0;
		for(int ribbon_cnt = 0; ribbon_cnt < RIBBON_COUNT; ribbon_cnt++) {
			if(tile_cnt >= ribbons[ribbon_cnt].tile_count) {
				continue;
			}

			int idx_y = ribbons[ribbon_cnt].tiles[tile_cnt].y;
			if(idx_y < 0 || idx_y > 11 || row_tile_counts[idx_y] >= MAX_PERROW) {
				continue;
			}
			set_one = 1;
			row_tiles[idx_y][row_tile_counts[idx_y]].x = ribbons[ribbon_cnt].tiles[tile_cnt].x*16;
			row_tiles[idx_y][row_tile_counts[idx_y]].y = ribbons[ribbon_cnt].tiles[tile_cnt].y*16;
			row_tiles[idx_y][row_tile_counts[idx_y]].sprite = ribbonSprite[
				ribbons[ribbon_cnt].tiles[tile_cnt].from][ribbons[ribbon_cnt].tiles[tile_cnt].to
			][ribbons[ribbon_cnt].tiles[tile_cnt].tile];
			row_tiles[idx_y][row_tile_counts[idx_y]].prio = 2;
			row_tile_counts[idx_y]++;
		}
	}

	// Heart. Twice. For reasons.
	int sprite_cnt = 0;	
	oamSet(
		&oamMain, sprite_cnt,
		256/2-32,192/2-32,
		0, 15,
		SpriteSize_64x64,
		SpriteColorFormat_Bmp,
		heartSprite,
		32, false, false, false, false, false
	);
	oamSet(
		&oamMain, 64+sprite_cnt,
		256/2-32,192/2-32,
		0, 15,
		SpriteSize_64x64,
		SpriteColorFormat_Bmp,
		heartSprite,
		32, false, false, false, false, false
	);
	sprite_cnt++;
	
	// Draw heads directly, in TWO DIFFERENT PLACES.
	// This is for reasons. As above.
	for(int ribbon_cnt = 0; ribbon_cnt < RIBBON_COUNT; ribbon_cnt++) {
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
			ribbons[ribbon_cnt].head_status <= 1 ? 1 : 2, 0,
			SpriteSize_16x16,
			SpriteColorFormat_256Color,
			ribbons[ribbon_cnt].head_status <= 1 ?
				zeroSprite[ribbons[ribbon_cnt].head_dir] :
				ribbonSprite[ribbons[ribbon_cnt].head_dir][ribbons[ribbon_cnt].head_dir][ribbons[ribbon_cnt].head_tile],
			32, false, false, false, false, false
		);
		oamSet(
			&oamMain, 64+sprite_cnt,
			ribbons[ribbon_cnt].x*16+head_xd,ribbons[ribbon_cnt].y*16+head_yd,
			ribbons[ribbon_cnt].head_status <= 1 ? 1 : 2, 0,
			SpriteSize_16x16,
			SpriteColorFormat_256Color,
			ribbons[ribbon_cnt].head_status <= 1 ?
				zeroSprite[ribbons[ribbon_cnt].head_dir] :
				ribbonSprite[ribbons[ribbon_cnt].head_dir][ribbons[ribbon_cnt].head_dir][ribbons[ribbon_cnt].head_tile],
			32, false, false, false, false, false
		);
		sprite_cnt++;
	}
}

// void rowribbon_test() {
// 	int sprite_cnt = 0;
// 	for(int row = 0; row < 12; row++) {
// 		for(int i = 0; i < row_tile_counts[row]; i++) {
// 			oamSet(
// 				&oamMain, RIBBON_COUNT + sprite_cnt++,
// 				row_tiles[row][i].x,row_tiles[row][i].y,
// 				row_tiles[row][i].prio, 0,
// 				SpriteSize_16x16,
// 				SpriteColorFormat_256Color,
// 				row_tiles[row][i].sprite,
// 				32, false, false, false, false, false
// 			);
// 		}
// 	}
// 	oamUpdate(&oamMain);
// }
// 
// void draw_ribbons_direct() {
// 	int sprite_cnt = 0;
// 	for(int ribbon_cnt = 0; ribbon_cnt < RIBBON_COUNT; ribbon_cnt++) {
// 		// Newest
// 		u16* newestSprite = ribbonSprite[
// 			ribbons[ribbon_cnt].tiles[ribbons[ribbon_cnt].tile_count-1].from
// 		][
// 			ribbons[ribbon_cnt].tiles[ribbons[ribbon_cnt].tile_count-1].to
// 		][ribbons[ribbon_cnt].tiles[ribbons[ribbon_cnt].tile_count-1].tile];
// 		if(ribbons[ribbon_cnt].head_status == 0) {
// 			int old_head;
// 			if(ribbons[ribbon_cnt].tiles[ribbons[ribbon_cnt].tile_count-1].from == DIR_RIGHT) {
// 				old_head = DIR_LEFT;
// 			}
// 			if(ribbons[ribbon_cnt].tiles[ribbons[ribbon_cnt].tile_count-1].from == DIR_LEFT) {
// 				old_head = DIR_RIGHT;
// 			}
// 			if(ribbons[ribbon_cnt].tiles[ribbons[ribbon_cnt].tile_count-1].from == DIR_DOWN) {
// 				old_head = DIR_UP;
// 			}
// 			if(ribbons[ribbon_cnt].tiles[ribbons[ribbon_cnt].tile_count-1].from == DIR_UP) {
// 				old_head = DIR_DOWN;
// 			}
// 			newestSprite = ribbonSprite[old_head][old_head][ribbons[ribbon_cnt].tiles[ribbons[ribbon_cnt].tile_count-1].tile];
// 		}
// 
// 		oamSet(
// 			&oamMain, sprite_cnt,
// 			ribbons[ribbon_cnt].tiles[ribbons[ribbon_cnt].tile_count-1].x*16,
// 			ribbons[ribbon_cnt].tiles[ribbons[ribbon_cnt].tile_count-1].y*16,
// 			1, 0,
// 			SpriteSize_16x16,
// 			SpriteColorFormat_256Color,
// 			newestSprite,
// 			32, false, false, false, false, false
// 		);
// 		sprite_cnt++;
// 
// 		// Head
// 		int head_xd = 0;
// 		int head_yd = 0;
// 		if(ribbons[ribbon_cnt].head_dir == DIR_RIGHT) {
// 			head_xd = 2*ribbons[ribbon_cnt].head_status;
// 		}
// 		if(ribbons[ribbon_cnt].head_dir == DIR_LEFT) {
// 			head_xd = -2*ribbons[ribbon_cnt].head_status;
// 		}
// 		if(ribbons[ribbon_cnt].head_dir == DIR_DOWN) {
// 			head_yd = 2*ribbons[ribbon_cnt].head_status;
// 		}
// 		if(ribbons[ribbon_cnt].head_dir == DIR_UP) {
// 			head_yd = -2*ribbons[ribbon_cnt].head_status;
// 		}
// 		oamSet(
// 			&oamMain, sprite_cnt,
// 			ribbons[ribbon_cnt].x*16+head_xd,ribbons[ribbon_cnt].y*16+head_yd,
// 			ribbons[ribbon_cnt].head_status <= 1 ? 0 : 1, 0,
// 			SpriteSize_16x16,
// 			SpriteColorFormat_256Color,
// 			ribbons[ribbon_cnt].head_status <= 1 ?
// 				zeroSprite[ribbons[ribbon_cnt].head_dir] :
// 				ribbonSprite[ribbons[ribbon_cnt].head_dir][ribbons[ribbon_cnt].head_dir][ribbons[ribbon_cnt].head_tile],
// 			32, false, false, false, false, false
// 		);
// 		sprite_cnt++;
// 
// 		// All-but-newest
// 		for(int tile_cnt = ribbons[ribbon_cnt].tile_count - 2; tile_cnt >= 0; tile_cnt-- ) {
// 			oamSet(
// 				&oamMain, sprite_cnt,
// 				ribbons[ribbon_cnt].tiles[tile_cnt].x*16,ribbons[ribbon_cnt].tiles[tile_cnt].y*16,
// 				1, 0,
// 				SpriteSize_16x16,
// 				SpriteColorFormat_256Color,
// 				ribbonSprite[
// 					ribbons[ribbon_cnt].tiles[tile_cnt].from][ribbons[ribbon_cnt].tiles[tile_cnt].to
// 				][ribbons[ribbon_cnt].tiles[tile_cnt].tile],
// 				32, false, false, false, false, false
// 			);
// 			sprite_cnt++;
// 		}
// 	}
// 	
// 	oamUpdate(&oamMain);
// }

u8 effect3_update( u32 t ) {

	uint16_t* palram = PALRAM_OBJ_A;
	for(int p = 0; p < 32; p++) {
		int ps = (p-t)%32;
		palram[p+4] = MakeRGB15(abs(ps-16)+10,3,3);
	}
	
	update_ribbons();
	
	ribbons_to_rowribbons();
	
	return( 0 );
}


void effect3_destroy() {
	for(int i = 0; i < 4; i++) {
		oamFreeGfx(&oamMain,zeroSprite[i]);
		for(int j = 0; j < 4; j++) {
			for(int k = 0; k < 4; k++) {
				oamFreeGfx(&oamMain,ribbonSprite[i][j][k]);
			}
		}
	}
// 	for(int i = 0; i < 12; i++) {
// 		free(row_tiles[i]);
// 	}
	for(int i = 0; i < RIBBON_COUNT; i++) {
		free(ribbons[i].tiles);
	}
	irqDisable( IRQ_HBLANK );
}
