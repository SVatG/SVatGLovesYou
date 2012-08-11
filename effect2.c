#include <nds.h>

#include "Utils.h"
#include "DS3D/DS3D.h"
#include "Loader.h"
#include "RainbowTable.h"

static int flip;
static uint32_t whitetexture;

int greet_id;

char* greet_images[5] = {
	"nitro:/gfx/greethaato_blank.img.bin",
	"nitro:/gfx/greethaato_nuance.img.bin",
	"nitro:/gfx/greethaato_rno.img.bin",
	"nitro:/gfx/greethaato_mercury.img.bin",
	"nitro:/gfx/greethaato_k2.img.bin",
};

u16* greet_border_sprite[12];
void effect2_init() {
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

	greet_id = 0;
	loadImage( greet_images[greet_id], VRAM_A_OFFS_0K,256*256*2);

	BG2CNT_A = BGxCNT_EXTENDED_BITMAP_16 | BGxCNT_BITMAP_SIZE_256x256 | BGxCNT_OVERFLOW_WRAP | BGxCNT_BITMAP_BASE_128K;
	BG2CNT_A = (BG2CNT_A&~BGxCNT_PRIORITY_MASK)|BGxCNT_PRIORITY_1;
	BG2PA_A = (1 << 8);
	BG2PB_A = 0;
	BG2PC_A = 0;
	BG2PD_A = (1 << 8);
	BG2X_A = 0;
	BG2Y_A = 0;

	loadImage( "nitro:/gfx/stripe_bg.img.bin", VRAM_A_OFFS_128K,256*256*2);

	vu16* mem_BLDCNT_A = (vu16*)(0x04000050);
	*mem_BLDCNT_A = BIT(6) | BIT(0) | BIT(8) | BIT(9) | BIT(10) | BIT(11) | BIT(12) | BIT(13) | BIT(2);
	BLDALPHA_A = BLDALPHA_EVA(15)|BLDALPHA_EVB(15);

	VRAMCNT_A = VRAMCNT_A_OBJ_VRAM_A;

	oamInit(&oamMain, SpriteMapping_Bmp_1D_128, false);

	greet_border_sprite[0] = loadBmpSpriteAGreen( "nitro:/gfx/greets_border_00.img.bin" );
	greet_border_sprite[1] = loadBmpSpriteAGreen( "nitro:/gfx/greets_border_01.img.bin" );
	greet_border_sprite[2] = loadBmpSpriteAGreen( "nitro:/gfx/greets_border_02.img.bin" );
	greet_border_sprite[3] = loadBmpSpriteAGreen( "nitro:/gfx/greets_border_03.img.bin" );
	greet_border_sprite[4] = loadBmpSpriteAGreen( "nitro:/gfx/greets_border_04.img.bin" );
	greet_border_sprite[5] = loadBmpSpriteAGreen( "nitro:/gfx/greets_border_05.img.bin" );
	greet_border_sprite[6] = loadBmpSpriteAGreen( "nitro:/gfx/greets_border_06.img.bin" );
	greet_border_sprite[7] = loadBmpSpriteAGreen( "nitro:/gfx/greets_border_07.img.bin" );
	greet_border_sprite[8] = loadBmpSpriteAGreen( "nitro:/gfx/greets_border_08.img.bin" );
	greet_border_sprite[9] = loadBmpSpriteAGreen( "nitro:/gfx/greets_border_09.img.bin" );
	greet_border_sprite[10] = loadBmpSpriteAGreen( "nitro:/gfx/greets_border_10.img.bin" );
	greet_border_sprite[11] = loadBmpSpriteAGreen( "nitro:/gfx/greets_border_11.img.bin" );
}

u8 effect2_update( u32 t ) {
	float scale = (((float)isin(t*16)+4096) / 256.0)+0.5f;
	int dx=icos(t*64)>>4;
	int dy=isin(t*64)>>4;

	for(int i = 0; i < 12; i++ ) {
		oamSet(
			&oamMain, i,
			64*(i%4),64*(i/4),
			0, 15,
			SpriteSize_64x64,
			SpriteColorFormat_Bmp,
			greet_border_sprite[i],
			32, false, false, false, false, false
		);
	}
	
	if(t%256 == 0) {
		greet_id = (greet_id + 1)%5;
		loadImage(greet_images[greet_id],VRAM_A_OFFS_0K,256*256*2);
	}
	
	dx *= scale;
	dy *= scale;
	
	BG3PA_A=dx;
	BG3PB_A=dy;
	BG3PC_A=-dy;
	BG3PD_A=dx;
	BG3X_A=(-128*dx-92*dy+(128<<8));
	BG3Y_A=(+128*dy-92*dx+(128<<8));

	oamUpdate(&oamMain);
}


void effect2_destroy() {
	for(int i = 0; i < 11; i++) {
		oamFreeGfx(&oamMain,greet_border_sprite[i]);
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
