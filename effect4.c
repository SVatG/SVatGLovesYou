#include <nds.h>

#include "Utils.h"
#include "RainbowTable.h"
#include "Loader.h"

void effect4_init() {

	// DISPCNT_A = DISPCNT_MODE_5 | DISPCNT_BG3_ON | DISPCNT_ON;
	
	VRAMCNT_D = VRAMCNT_D_BG_VRAM_A_OFFS_128K;
	VRAMCNT_B = VRAMCNT_B_BG_VRAM_A_OFFS_0K;

	BG3CNT_A = BGxCNT_EXTENDED_BITMAP_16 | BGxCNT_BITMAP_SIZE_256x256 | BGxCNT_OVERFLOW_WRAP | BGxCNT_BITMAP_BASE_128K;
	BG3CNT_A = (BG3CNT_A&~BGxCNT_PRIORITY_MASK)|BGxCNT_PRIORITY_1;
	BG3PA_A = (1 << 8);
	BG3PB_A = 0;
	BG3PC_A = 0;
	BG3PD_A = (1 << 8);
	BG3X_A = 0;
	BG3Y_A = 0;
}

u8 effect4_update( u32 t ) {

	u16* screen = VRAM_A_OFFS_128K;
	int32_t x1 = 60 + (isin(t<<3)>>7);
	int32_t y1 = 30 + (icos((t+123)<<4)>>6);
	int32_t x2 = 220 + (icos((t+12)<<4)>>6);
	int32_t y2 = 180 + (isin(t<<4)>>5);
	
	int32_t d1 = 0;
	int32_t d2 = 0;
	int32_t dy1 = 0;
	int32_t dy2 = 0;
	int32_t dx1 = 0;
	int32_t dx2 = 0;

	
	
	for(int y = 0; y < 192; y++) {
		dy1 = (y-y1);
		dy1 = dy1*dy1;
		dy2 = (y-y2);
		dy2 = dy2*dy2;
		for(int x = 0; x < 256; x++) {
			dx1 = (x-x1);
			dx1 = dx1*dx1;
			dx2 = (x-x2);
			dx2 = dx2*dx2;
			d1 = ((dx1+dy1)>>12) & 1;
			d2 = ((dx2+dy2)>>12) & 1;
			if((d1^d2) == 1) {
				screen[x+y*256] = MakeRGB15(27,10,10);
			}
			else {
				screen[x+y*256] = MakeRGB15(28,28,28);
			}
		}
	}
	return( 0 );
}


void effect4_destroy() {
	
}
