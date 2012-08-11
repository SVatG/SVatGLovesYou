#include "Field.h"
#include "ARM.h"
#include "Hardware.h"
#include "Utils.h"

#include <math.h>
#include <string.h>

#include <nds.h>

#define Width 128
#define Height 96

static int frame;
static int16_t* rayarray;

u16* field_border_sprite[12];

void InitField()
{
	VRAMCNT_B=VRAMCNT_B_LCDC;
// 	VRAMCNT_C=VRAMCNT_C_BG_VRAM_A_OFFS_0K;
	VRAMCNT_D=VRAMCNT_D_LCDC;
	VRAMCNT_A=VRAMCNT_A_LCDC;
	
	VRAMCNT_D=VRAMCNT_D_BG_VRAM_A_OFFS_0K;
	VRAMCNT_A=VRAMCNT_A_BG_VRAM_A_OFFS_128K;
	VRAMCNT_B=VRAMCNT_B_OBJ_VRAM_A;
	
	DISPCNT_A=DISPCNT_MODE_5|DISPCNT_BG3_ON|DISPCNT_BG2_ON|DISPCNT_ON;

	oamInit(&oamMain, SpriteMapping_Bmp_1D_128, false);

	field_border_sprite[0] = loadBmpSpriteAGreen( "nitro:/gfx/greets_border_00.img.bin" );
	field_border_sprite[1] = loadBmpSpriteAGreen( "nitro:/gfx/greets_border_01.img.bin" );
	field_border_sprite[2] = loadBmpSpriteAGreen( "nitro:/gfx/greets_border_02.img.bin" );
	field_border_sprite[3] = loadBmpSpriteAGreen( "nitro:/gfx/greets_border_03.img.bin" );
	field_border_sprite[4] = loadBmpSpriteAGreen( "nitro:/gfx/greets_border_04.img.bin" );
	field_border_sprite[5] = loadBmpSpriteAGreen( "nitro:/gfx/greets_border_05.img.bin" );
	field_border_sprite[6] = loadBmpSpriteAGreen( "nitro:/gfx/greets_border_06.img.bin" );
	field_border_sprite[7] = loadBmpSpriteAGreen( "nitro:/gfx/greets_border_07.img.bin" );
	field_border_sprite[8] = loadBmpSpriteAGreen( "nitro:/gfx/greets_border_08.img.bin" );
	field_border_sprite[9] = loadBmpSpriteAGreen( "nitro:/gfx/greets_border_09.img.bin" );
	field_border_sprite[10] = loadBmpSpriteAGreen( "nitro:/gfx/greets_border_10.img.bin" );
	field_border_sprite[11] = loadBmpSpriteAGreen( "nitro:/gfx/greets_border_11.img.bin" );

	for(int i = 0; i < 12; i++ ) {
		oamSet(
			&oamMain, i,
			64*(i%4),64*(i/4),
			0, 15,
			SpriteSize_64x64,
			SpriteColorFormat_Bmp,
			field_border_sprite[i],
			32, false, false, false, false, false
		);
	}

	oamUpdate(&oamMain);
	
	BG3CNT_A=BGxCNT_BITMAP_BASE_0K|BGxCNT_EXTENDED_BITMAP_8
			|BGxCNT_BITMAP_SIZE_128x128;
	BG3PA_A=0x80;
	BG3PB_A=0;
	BG3PC_A=0;
	BG3PD_A=0x80;
	BG3HOFS_A=0;
	BG3VOFS_A=0;

	BG3CNT_A = (BG3CNT_A&~BGxCNT_PRIORITY_MASK)|BGxCNT_PRIORITY_0;

	BLDCNT_A = BLDCNT_SRC_A_BG3|BLDCNT_SRC_B_BG2|BLDCNT_EFFECT_ALPHA;
	BLDALPHA_A = BLDALPHA_EVA(15)|BLDALPHA_EVB(15);

	loadImageVRAMIndirectGreen( "nitro:/gfx/stripe_bg.img.bin", VRAM_A_OFFS_128K,256*256*2);

	BG2CNT_A = BGxCNT_EXTENDED_BITMAP_16 | BGxCNT_OVERFLOW_WRAP | BGxCNT_BITMAP_SIZE_256x256 | BGxCNT_BITMAP_BASE_128K;
	BG2CNT_A = (BG2CNT_A&~BGxCNT_PRIORITY_MASK)|BGxCNT_PRIORITY_1;
	BG2PA_A = (1 << 8);
	BG2PB_A = 0;
	BG2PC_A = 0;
	BG2PD_A = (1 << 8);
	BG2X_A = 0;
	BG2Y_A = 0;

	
	frame=0;

	rayarray = malloc(sizeof(int16_t)*Width*Height*3);
	int16_t *rays=rayarray;

	for(int y=0;y<Height;y++)
	for(int x=0;x<Width;x++)
	{
		int32_t fx=Fix(2*x+1-Width)/(Width);
		int32_t fy=Fix(2*y+1-Height)/(Width);
		int32_t fz=-Fix(1)+isq(fx)/2+isq(fy)/2;

		int32_t r2=isq(fx)+isq(fy)+isq(fz);
		int32_t r=sqrti(r2<<12);

		rays[3*(x+y*Width)+0]=idiv(fx,r)<<3;
		rays[3*(x+y*Width)+1]=idiv(fy,r)<<3;
		rays[3*(x+y*Width)+2]=idiv(fz,r)<<3;
	}
}

void StopField()
{
	free(rayarray);

	for(int i = 0; i < 11; i++) {
		oamFreeGfx(&oamMain,field_border_sprite[i]);
	}
	oamClear(&oamMain,0,128);
	oamUpdate(&oamMain);
}

void RunField(int t)
{
	uint16_t *vram;
	if(frame)
	{
		BG3CNT_A=BGxCNT_BITMAP_BASE_0K|BGxCNT_EXTENDED_BITMAP_8
				|BGxCNT_BITMAP_SIZE_128x128;
		BG3CNT_A = (BG3CNT_A&~BGxCNT_PRIORITY_MASK)|BGxCNT_PRIORITY_0;
		vram=VRAM_A_OFFS_64K;
	}
	else
	{
		BG3CNT_A=BGxCNT_BITMAP_BASE_64K|BGxCNT_EXTENDED_BITMAP_8
				|BGxCNT_BITMAP_SIZE_128x128;
		BG3CNT_A = (BG3CNT_A&~BGxCNT_PRIORITY_MASK)|BGxCNT_PRIORITY_0;
		vram=VRAM_A_OFFS_0K;
	}
	frame^=1;

	for(int i=0;i<12;i++)
	PALRAM_A[i]=0x8000|MakeHSV(0,Fix(0.8),Fix(i)/12);

	for(int i=0;i<12;i++)
	PALRAM_A[i+12]=0x8000|MakeHSV(0,Fix(11-i)/16,Fix(1));

	RenderFieldARM(t,vram,rayarray);
}
