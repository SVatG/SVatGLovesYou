// AO Cubes

#include <nds.h>

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

	DISPCNT_A=DISPCNT_MODE_5|DISPCNT_3D|DISPCNT_BG0_ON|DISPCNT_ON;
	
	// Set up voxelcubes
	VRAMCNT_D=VRAMCNT_D_LCDC;
	VRAMCNT_F=VRAMCNT_F_LCDC;
	
	loadVRAMIndirect( "nitro:/textures.pal4", VRAM_LCDC_D,16384);

	for(int i=0;i<16;i++) VRAM_LCDC_F[i]=MakeRGB15(i+16,i+16,i+16);

	VRAMCNT_D=VRAMCNT_D_TEXTURE_OFFS_0K;
	VRAMCNT_F=VRAMCNT_F_TEXTURE_PALETTE_SLOT_0;

	DSInit3D();
	DSViewport(0,0,255,191);

	DSSetControl(DS_TEXTURING|DS_ANTIALIAS);
	DSClearParams(26,26,26,0,63);

	DSSetPaletteOffset(0,DS_TEX_FORMAT_PAL4);

	DSMatrixMode(DS_PROJECTION);
	DSLoadIdentity();
	DSPerspectivef(45,256.0/192.0,1,1024);

	InitVoxelBlock(&block,17,17,64,NULL);
}

void voxelSpiral(int t) {
	// Kludge
	static int lt = 0;
	lt++;
	t = lt;
	
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

	// Heart
	u16 col = MakeRGB15(31,31,31);
	if(t%32==0) {
		heart_at(posx, posy, 0, col);
	}
	else {
		heart_at(posx, posy, 0, 0);
	}
	
	RefreshVoxelBlock(&block);
	
	// Move things
	DSTranslatef(0,0,200);
	DSRotateZi(-t<<2);
	DSScalef(8,8,8);
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

