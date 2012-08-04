#include <nds.h>

#include "Loader.h"
#include "Utils.h"
#include <nds/registers_alt.h>

#include "RainbowTable.h"

void effect0_init() {

	// VRAM and DISP setup.
	VRAMCNT_C = VRAMCNT_C_BG_VRAM_B;
	DISPCNT_B = DISPCNT_MODE_5 | DISPCNT_BG2_ON | DISPCNT_BG3_ON | DISPCNT_ON;

	// Init BG2 top priority
	BG2CNT_B = BGxCNT_EXTENDED_BITMAP_8 | BGxCNT_BITMAP_SIZE_256x256 | BGxCNT_OVERFLOW_WRAP | BGxCNT_BITMAP_BASE_0K;
	BG2CNT_B = (BG2CNT_B&~BGxCNT_PRIORITY_MASK)|BGxCNT_PRIORITY_0;
	BG2PA_B = (1 << 8);
	BG2PB_B = 0;
	BG2PC_B = 0;
	BG2PD_B = (1 << 8);
	BG2X_B = 0;
	BG2Y_B = 0;

	// Init BG3 bottom priority
	BG3CNT_B = BGxCNT_EXTENDED_BITMAP_8 | BGxCNT_BITMAP_SIZE_256x256 | BGxCNT_OVERFLOW_WRAP | BGxCNT_BITMAP_BASE_64K;
	BG3CNT_B = (BG3CNT_B&~BGxCNT_PRIORITY_MASK)|BGxCNT_PRIORITY_1;
	BG3PA_B = (1 << 8);
	BG3PB_B = 0;
	BG3PC_B = 0;
	BG3PD_B = (1 << 8);
	BG3X_B = 0;
	BG3Y_B = 0;
	
	load8bVRAMIndirect("nitro:/gfx/svatgst_textonly.img.bin", VRAM_B,256*256);
	load8bVRAMIndirect("nitro:/gfx/svatgst_indexed.img.bin", VRAM_B+256*64*2,256*256);
	load8bVRAMIndirect("nitro:/gfx/svatgst_indexed.pal.bin", PALRAM_B,256);

}

void updatecol(int t) {
	dmaCopyHalfWords( 0, greyTable, PALRAM_B + 51 , 10 );
	for(int i = 0; i < 51; i++) {
		dmaCopyHalfWords( 0, cutePopTable + ((t/4+255-i)%11), PALRAM_B + i%255 , 2 );
	}
}

u8 effect0_update( u32 t ) {
	int dx = isin(t*4*6)>>2;
	int dy = isin(isin(t*4))>>2;
	BG2PA_B = (1 << 8);
	BG2PB_B = 0;
	BG2PC_B = 0;
	BG2PD_B = (1 << 8);
	BG2X_B = dx;
	BG2Y_B = 0;
// 
// 	BG3X_A = 100;
	
	updatecol(t);
	
	return( 0 );
}


void effect0_destroy() {
// 	BLEND_CR = BLEND_NONE;
// 	SUB_BLEND_CR = BLEND_NONE;
}
