// AO Cubes

#include <nds.h>
#include <nds/registers_alt.h>

#include "DS3D/Utils.h"
#include "DS3D/DS3D.h"
#include "VoxelBlock.h"
#include "Hardware.h"
#include "Loader.h"
#include "RainbowTable.h"

VoxelBlock block;

void heart_at(int posx, int posy, int z, u16 col) {
	SetVoxelAt(&block,posx-0,posy-2,z,col);
	SetVoxelAt(&block,posx-1,posy-3,z,col);
	SetVoxelAt(&block,posx-2,posy-4,z,col);
	SetVoxelAt(&block,posx-3,posy-3,z,col);
	SetVoxelAt(&block,posx-4,posy-2,z,col);
	SetVoxelAt(&block,posx-4,posy-1,z,col);
	SetVoxelAt(&block,posx-3,posy-0,z,col);
	SetVoxelAt(&block,posx-3,posy-0,z,col);
	SetVoxelAt(&block,posx-2,posy+1,z,col);
	SetVoxelAt(&block,posx-1,posy+2,z,col);
	SetVoxelAt(&block,posx-0,posy+3,z,col);
	SetVoxelAt(&block,posx+1,posy+2,z,col);
	SetVoxelAt(&block,posx+2,posy+1,z,col);
	SetVoxelAt(&block,posx+3,posy+0,z,col);
	SetVoxelAt(&block,posx+4,posy-1,z,col);
	SetVoxelAt(&block,posx+4,posy-2,z,col);
	SetVoxelAt(&block,posx+3,posy-3,z,col);
	SetVoxelAt(&block,posx+2,posy-4,z,col);
	SetVoxelAt(&block,posx+1,posy-3,z,col);
}

void effect6_init() {

	DISPCNT_A=DISPCNT_MODE_5|DISPCNT_3D|DISPCNT_BG0_ON|DISPCNT_BG2_ON|DISPCNT_BG3_ON|DISPCNT_ON;

	BLDCNT_A = BLDCNT_SRC_A_BG3|BLDCNT_SRC_B_BG0|BLDCNT_EFFECT_ALPHA;
	BLDALPHA_A = BLDALPHA_EVA(8)|BLDALPHA_EVB(15);

	// BG and frame
	VRAMCNT_B = VRAMCNT_B_BG_VRAM_A_OFFS_0K;
	VRAMCNT_A = VRAMCNT_A_BG_VRAM_A_OFFS_128K;

	BG2CNT_A = BGxCNT_EXTENDED_BITMAP_16 | BGxCNT_OVERFLOW_WRAP | BGxCNT_BITMAP_SIZE_256x256 | BGxCNT_BITMAP_BASE_0K;
	BG2CNT_A = (BG2CNT_A&~BGxCNT_PRIORITY_MASK)|BGxCNT_PRIORITY_0;
	BG2_XDX = (1 << 8);
	BG2_XDY = 0;
	BG2_YDX = 0;
	BG2_YDY = (1 << 8);
	BG2_CX = 0;
	BG2_CY = 0;

	// Init BG0 priority as above BG3
	BG0CNT_A = (BG0CNT_A&~BGxCNT_PRIORITY_MASK)|BGxCNT_PRIORITY_2;
	
	BG3CNT_A = BGxCNT_EXTENDED_BITMAP_16 | BGxCNT_OVERFLOW_WRAP | BGxCNT_BITMAP_SIZE_256x256 | BGxCNT_BITMAP_BASE_128K;
	BG3CNT_A = (BG3CNT_A&~BGxCNT_PRIORITY_MASK)|BGxCNT_PRIORITY_1;
	BG3_XDX = (1 << 8);
	BG3_XDY = 0;
	BG3_YDX = 0;
	BG3_YDY = (1 << 8);
	BG3_CX = 0;
	BG3_CY = 0;

	loadImageVRAMIndirectGreen( "nitro:/gfx/tunnel_frame.img.bin", VRAM_A_OFFS_0K,256*256*2);
	loadImage( "nitro:/gfx/stripe_bg_hori.img.bin", VRAM_A_OFFS_128K,256*256*2);
	
	// Set up voxelcubes
	VRAMCNT_D=VRAMCNT_D_LCDC;
	VRAMCNT_F=VRAMCNT_F_LCDC;
	
	loadVRAMIndirect( "nitro:/textures.pal4", VRAM_LCDC_D,16384);

	for(int i=0;i<16;i++) VRAM_LCDC_F[i]=MakeRGB15(i+16,i+16,i+16);

	VRAMCNT_D=VRAMCNT_D_TEXTURE_OFFS_0K;
	VRAMCNT_F=VRAMCNT_F_TEXTURE_PALETTE_SLOT_0;

	DSInit3D();
	DSViewport(0,0,255,191);

	DSSetControl(DS_TEXTURING|DS_ANTIALIAS|DS_FOG);
	
	DSClearParams(26,26,26,0,63);

	DSSetPaletteOffset(0,DS_TEX_FORMAT_PAL4);

	DSMatrixMode(DS_PROJECTION);
	DSLoadIdentity();
	
	DSPerspectivef(65,256.0/192.0,0.1,16);
	DSSetFogLinearf(0,0,0,31,8,16,0.1,16);

	InitVoxelBlock(&block,17,17,64,NULL);
}

int offx = 0;
int offy = 0;
void voxelSpiral(int t) {

	BG3_CY = t*512*2;
	
	// Kludge
	static int lt = 0;
	lt++;
	t = lt*2;

	static uint8_t ri = 0;
	DSMatrixMode(DS_POSITION);
	DSLoadIdentity();
	
	// Move blocks
	if(t%4==0) {
		ScrollVoxelBlockByZ(&block);
	}

	// Spiral rainbows
	int posx = 8;
	int posy = 8;

	float offx_f = ((float)isin((t-4*64)<<4))/250.0;;
	float offy_f = ((float)icos((t-4*64)<<4))/250.0;
	int new_offx = isin(t<<4)/1500;
	int new_offy = icos(t<<4)/1500;
	
	// Clear old heart
	if(new_offx != offx || new_offy != offy) {
// 		SetVoxelAt(&block,posx-offx,posy-offy,0,0);		
		heart_at(posx-offx, posy-offy, 0, 0);
	}

	offx = new_offx;
	offy = new_offy;
	
	// Heart
	if((t/4)%16<=2) {
		int bright = abs(((t/4)%16));
		u16 col = 0;
		if((t/8)%16<=2) {
			col = MakeRGB15(27 + 2 * bright, 13 + 2*bright, 13 + 2*bright);
		}
		else {
			col = MakeRGB15(25 + 3 * bright, 25 + 3*bright, 25 + 3*bright);
		}
// 		SetVoxelAt(&block,posx-offx,posy-offy,0,col);
		heart_at(posx-offx, posy-offy, 0, col);
	}
	else {
// 		SetVoxelAt(&block,posx-offx,posy-offy,0,0);
		heart_at(posx-offx, posy-offy, 0, 0);
	}
	
	RefreshVoxelBlock(&block);
	
	// Move things
	DSRotateZi(-t<<2);
	DSTranslatef(offx_f*0.025,offy_f*0.025,0);	
	DSScalef(0.25,0.25,0.25);
	DSTranslatef32(DSf32(0),DSf32(0),((t&3)<<10)-DSf32(33));
	DrawVoxelBlock(&block);
	DSSwapBuffers(0);
}

void objectShow(int t) {
	static uint8_t ri = 0;
	DSMatrixMode(DS_POSITION);
	DSLoadIdentity();

	DSSwapBuffers(0);
}

uint8_t effect6_update( uint32_t t ) {
	voxelSpiral(t);
	
	return 0;
}


void effect6_destroy() {
	CleanupVoxelBlock(&block);
}

