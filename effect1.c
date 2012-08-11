#include <nds.h>

#include "Utils.h"
#include "RainbowTable.h"
#include "Loader.h"

u16* dot_sprite[8];
u16* front_sprite[12];

void effect1_init() {
	DISPCNT_A = DISPCNT_MODE_5 | DISPCNT_BG2_ON | DISPCNT_BG3_ON | DISPCNT_OBJ_ON | DISPCNT_ON;
	VRAMCNT_D = VRAMCNT_D_BG_VRAM_A_OFFS_128K;
	VRAMCNT_B = VRAMCNT_B_BG_VRAM_A_OFFS_0K;
	
	BG3CNT_A = BGxCNT_EXTENDED_BITMAP_16 | BGxCNT_BITMAP_SIZE_256x256 | BGxCNT_OVERFLOW_WRAP | BGxCNT_BITMAP_BASE_0K;
	BG3CNT_A = (BG3CNT_A&~BGxCNT_PRIORITY_MASK)|BGxCNT_PRIORITY_2;
	BG3PA_A = (1 << 8);
	BG3PB_A = 0;
	BG3PC_A = 0;
	BG3PD_A = (1 << 8);
	BG3X_A = 0;
	BG3Y_A = 0;

	loadImage( "nitro:/gfx/ecgback.img.bin", VRAM_A_OFFS_0K,256*256*2);
	
	BG2CNT_A = BGxCNT_EXTENDED_BITMAP_16 | BGxCNT_BITMAP_SIZE_256x256 | BGxCNT_OVERFLOW_TRANSPARENT | BGxCNT_BITMAP_BASE_128K;
	BG2CNT_A = (BG2CNT_A&~BGxCNT_PRIORITY_MASK)|BGxCNT_PRIORITY_1;
	BG2PA_A = icos(900)>>4;
	BG2PB_A = isin(900)>>4;
	BG2PC_A = -isin(900)>>4;
	BG2PD_A = icos(900)>>4;
	BG2X_A = -5000;
	BG2Y_A = 40000;

	VRAMCNT_A = VRAMCNT_A_OBJ_VRAM_A;

	oamInit(&oamMain, SpriteMapping_Bmp_1D_128, false);
	
	loadVRAMIndirect("nitro:/gfx/ecgdot_0.pal.bin", SPRITE_PALETTE, 512);

	dot_sprite[0] = loadBmpSpriteA( "nitro:/gfx/ecgdot_0.img.bin" );
	dot_sprite[1] = loadBmpSpriteA( "nitro:/gfx/ecgdot_1.img.bin" );
	dot_sprite[2] = loadBmpSpriteA( "nitro:/gfx/ecgdot_2.img.bin" );
	dot_sprite[3] = loadBmpSpriteA( "nitro:/gfx/ecgdot_3.img.bin" );
	dot_sprite[4] = loadBmpSpriteA( "nitro:/gfx/ecgdot_4.img.bin" );
	dot_sprite[5] = loadBmpSpriteA( "nitro:/gfx/ecgdot_5.img.bin" );
	dot_sprite[6] = loadBmpSpriteA( "nitro:/gfx/ecgdot_6.img.bin" );
	dot_sprite[7] = loadBmpSpriteA( "nitro:/gfx/ecgdot_7.img.bin" );

	front_sprite[0] = loadBmpSpriteAGreen( "nitro:/gfx/ecgfront_tiles_00.img.bin" );
	front_sprite[1] = loadBmpSpriteAGreen( "nitro:/gfx/ecgfront_tiles_01.img.bin" );
	front_sprite[2] = loadBmpSpriteAGreen( "nitro:/gfx/ecgfront_tiles_02.img.bin" );
	front_sprite[3] = loadBmpSpriteAGreen( "nitro:/gfx/ecgfront_tiles_03.img.bin" );
	front_sprite[4] = loadBmpSpriteAGreen( "nitro:/gfx/ecgfront_tiles_04.img.bin" );
	front_sprite[5] = loadBmpSpriteAGreen( "nitro:/gfx/ecgfront_tiles_05.img.bin" );
	front_sprite[6] = loadBmpSpriteAGreen( "nitro:/gfx/ecgfront_tiles_06.img.bin" );
	front_sprite[7] = loadBmpSpriteAGreen( "nitro:/gfx/ecgfront_tiles_07.img.bin" );
	front_sprite[8] = loadBmpSpriteAGreen( "nitro:/gfx/ecgfront_tiles_08.img.bin" );
	front_sprite[9] = loadBmpSpriteAGreen( "nitro:/gfx/ecgfront_tiles_09.img.bin" );
	front_sprite[10] = loadBmpSpriteAGreen( "nitro:/gfx/ecgfront_tiles_10.img.bin" );
	front_sprite[11] = loadBmpSpriteAGreen( "nitro:/gfx/ecgfront_tiles_11.img.bin" );
	
	vu16* mem_BLDCNT_A = (vu16*)(0x04000050);
	*mem_BLDCNT_A = /*BIT(4) |*/ BIT(6) | BIT(0) | BIT(8) | BIT(9) | BIT(10) | BIT(11) | BIT(12) | BIT(13) | BIT(2);
	BLDALPHA_A = BLDALPHA_EVA(15)|BLDALPHA_EVB(15);
}

int heartbeat(int tmod, int arra_s, int* arra_r, int station) {
	if(tmod < 7) {
		arra_s -= 1;
		*arra_r = 6;
	}
	else if(tmod < 14) {
		arra_s += 1;
		*arra_r = 6;
	}
	else if(tmod < 20) {
		// Nada
		*arra_r = 5;
	}
	else if(tmod < 25) {
		arra_s += 1;
		*arra_r = 6;
	}
	else if(tmod < 32) {
		arra_s -= 5;
		*arra_r = 15;
	}
	else if(tmod < 40) {
		arra_s += 5;
		*arra_r = 15;
	} else if(tmod < 43) {
		arra_s -= 4;
		*arra_r = 12;
	} else if(tmod < 51) {
		// Nada
		*arra_r = 5;
	} else if(tmod < 59) {
		arra_s -= 1;
		*arra_r = 6;
	}
	else if(tmod < 68) {
		arra_s += 1;
		*arra_r = 6;
	} else {
		arra_s = station;
		*arra_r = 5;
	}
	return arra_s;
}

int line_start[5];
void drawbars(int t) {
	u16* bg = (u16*)(VRAM_A_OFFS_0K+0x10000);
	
	int arra_r = 0;
	int arrb_r = 0;
	int arrc_r = 0;

	int line_size[5];

	for(int i = 0; i < 5; i++) {
		line_start[i] = heartbeat((t+((i*348923)%17777))%(80+((i*2981)%60)), line_start[i], &line_size[i], 50+i*35);
	}

	for(int x = 0; x < 256; x++) {
		uint16_t brightness = 0;
		for(int i = 0; i < 5; i++) {
			if( x >= line_start[i] && x <= line_start[i] + line_size[i]*2 ) {
				int brightness_local = line_size[i] - abs(x - (line_start[i] + line_size[i]));
				brightness+=brightness_local;
			}
		}
		if(brightness != 0) {
			int rb = (brightness * 7)/9 > 31 ? 31 : (brightness*7)/9;
			int g = (brightness * 12)/9 > 31 ? 31 : (brightness*12)/9;
			bg[x] = MakeRGB15(g,rb,rb);
		}
		else {
			bg[x] = ~BIT(15);
		}

	}

	for( int y = 192; y > 0; y-- ) {
		dmaCopy( &bg[(y-1)*256], &bg[y*256], 512 );
	}
}

u8 effect1_update( u32 t ) {
	BG3X_A = t*800;
	BG3Y_A = -t*512;
	
	drawbars(t);
	drawbars(t);

	for(int i = 0; i < 12; i++ ) {
		oamSet(
			&oamMain, i,
			64*(i%4),64*(i/4),
			0, 15,
			SpriteSize_64x64,
			SpriteColorFormat_Bmp,
			front_sprite[i],
			32, false, false, false, false, false
		);
	}
	
	float dx = (200.0-175.0) / (175.0-50.0);
	float dy = (165.0-40.0) / (175.0-50.0);
	for(int i = 0; i < 8; i++) {
		for(int dot = 0; dot < 5; dot++) {
			oamSet(
				&oamMain, 12+i*8+dot,
				168+dx*(line_start[dot]-50)-16,40+dy*(line_start[dot]-50)-16,
				0, 15-2*i,
				SpriteSize_32x32,
				SpriteColorFormat_Bmp,
				dot_sprite[7-i],
				32, false, false, false, false, false
			);
			oamMain.oamMemory[i*8+dot].blendMode = OBJMODE_BITMAP;
		}
	}
	
	oamUpdate(&oamMain);
	
	return( 0 );
}


void effect1_destroy() {
	irqDisable( IRQ_HBLANK );
	for(int i = 0; i < 7; i++) {
		oamFreeGfx(&oamMain,dot_sprite[i]);
	}
	for(int i = 0; i < 11; i++) {
		oamFreeGfx(&oamMain,front_sprite[i]);
	}
	oamClear(&oamMain,0,128);
	oamUpdate(&oamMain);

	BG3PA_A=(1<<8);
	BG3PB_A=0;
	BG3PC_A=0;
	BG3PD_A=(1<<8);
	BG3X_A=0;
	BG3Y_A=0;

	BLDCNT_A = 0;
	BLDALPHA_A = 0;
}
