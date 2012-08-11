#include "Utils.h"

#include <nds.h>

void effect7_init() {
	BG3CNT_A = BGxCNT_EXTENDED_BITMAP_16 | BGxCNT_BITMAP_SIZE_256x256 | BGxCNT_OVERFLOW_TRANSPARENT | BGxCNT_BITMAP_BASE_0K;
	BG3CNT_A = (BG3CNT_A&~BGxCNT_PRIORITY_MASK)|BGxCNT_PRIORITY_2;
	BG3PA_A = (1 << 8);
	BG3PB_A = 0;
	BG3PC_A = 0;
	BG3PD_A = (1 << 8);
	BG3X_A = 0;
	BG3Y_A = 0;

	BG2CNT_A = BGxCNT_EXTENDED_BITMAP_16 | BGxCNT_BITMAP_SIZE_256x256 | BGxCNT_OVERFLOW_WRAP | BGxCNT_BITMAP_BASE_128K;
	BG2CNT_A = (BG2CNT_A&~BGxCNT_PRIORITY_MASK)|BGxCNT_PRIORITY_1;
	BG2PA_A = (1 << 8);
	BG2PB_A = 0;
	BG2PC_A = 0;
	BG2PD_A = (1 << 8);
	BG2X_A = 0;
	BG2Y_A = 0;

	loadImage( "nitro:/gfx/stripe_bg.img.bin", VRAM_A_OFFS_128K,256*256*2);
	loadImage( "nitro:/gfx/lovesyou.img.bin", VRAM_A_OFFS_0K,256*256*2);

	vu16* mem_BLDCNT_A = (vu16*)(0x04000050);
	*mem_BLDCNT_A = BIT(6) | BIT(0) | BIT(8) | BIT(9) | BIT(10) | BIT(11) | BIT(12) | BIT(13) | BIT(2);
	BLDALPHA_A = BLDALPHA_EVA(15)|BLDALPHA_EVB(15);
}

int bouncem = 0;
void effect7_init_2() {
	effect7_init();
	bouncem = 1;
	loadImage( "nitro:/gfx/self_heart.img.bin", VRAM_A_OFFS_0K,256*256*2);
}

void effect7_init_3() {
	loadImage( "nitro:/gfx/you_heart.img.bin", VRAM_A_OFFS_0K,256*256*2);
}

int ts = -1;
int effect7_update( int t ) {
	if(ts == -1) {
		ts = t;
		t = 0;
	}
	else {
		t = (t - ts);
	}

	t *= ((12.5*8.0)/60.0);
	
	float scale = (1024.0 - t);
	if(t >= 1024) {
		scale = ((float)isin((t-1024)*16)) / 512.0;
		if(bouncem == 1) {
			scale = fabs(scale/8.0)+0.5;
		}
		else {
			scale *= 0.5;
		}
	}
	int dx=icos(0)>>4;
	int dy=isin(0)>>4;

	dx *= scale;
	dy *= scale;

	BG3PA_A=dx;
	BG3PB_A=dy;
	BG3PC_A=-dy;
	BG3PD_A=dx;
	BG3X_A=(-128*dx-92*dy+(128<<8));
	BG3Y_A=(+128*dy-92*dx+(128<<8));
	
	return( 0 ); 
}


void effect7_destroy() {

	BG3PA_A=(1<<8);
	BG3PB_A=0;
	BG3PC_A=0;
	BG3PD_A=(1<<8);
	BG3X_A=0;
	BG3Y_A=0;

	BLDCNT_A = 0;
	BLDALPHA_A = 0;
}
