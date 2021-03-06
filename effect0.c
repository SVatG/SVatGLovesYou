#include <nds.h>

#include "Loader.h"
#include "Utils.h"
#include <nds/registers_alt.h>

#include "RainbowTable.h"

const char* spriteNames[10] = {
	"nitro:/gfx/haato_inter.img.bin",
	"nitro:/gfx/haato_ribbons.img.bin",
	"nitro:/gfx/haato_metaballs.img.bin",
	"nitro:/gfx/haato_tunnels.img.bin",
	"nitro:/gfx/haato_greets.img.bin",
	"nitro:/gfx/haato_cubes.img.bin",
	"nitro:/gfx/haato_thatone.img.bin",
	"nitro:/gfx/haato_raymarching.img.bin",
	"nitro:/gfx/haato_self.img.bin",
	"nitro:/gfx/haato_evoke.img.bin",
};

uint8_t* effect0_loadimage;
int32_t switchin_status = 128;

void effect0_change(int toIndex) {
	int fd = open( spriteNames[toIndex], O_RDONLY );
	read( fd, effect0_loadimage, 128*128 );
	close( fd );
	switchin_status = 0;
}

u16* bgtile;
void effect0_init() {

	VRAMCNT_I = VRAMCNT_I_OBJ_VRAM_B;

	// Temp image for loadin
	effect0_loadimage = (uint8_t*)malloc(128*128);
	
	// VRAM and DISP setup.
	VRAMCNT_C = VRAMCNT_C_BG_VRAM_B;
	DISPCNT_B = DISPCNT_MODE_5 | DISPCNT_BG2_ON | DISPCNT_BG3_ON | DISPCNT_OBJ_ON | DISPCNT_ON;

	oamInit(&oamSub, SpriteMapping_Bmp_1D_128, false);
	bgtile = loadBmpSpriteB( "nitro:/gfx/stripe_tile.img.bin" );

	for(int i = 0; i < 48; i++ ) {
		oamSet(
			&oamSub, i,
			32*(i%8),32*(i/8),
			0, 4,
			SpriteSize_32x32,
			SpriteColorFormat_Bmp,
			bgtile,
			32, false, false, false, false, false
		);
	}

	oamUpdate(&oamSub);
	
	BLDCNT_B = BLDCNT_SRC_A_OBJ|BLDCNT_SRC_B_BG2|BLDCNT_SRC_B_BG3|BLDCNT_EFFECT_ALPHA;
	BLDALPHA_B = BLDALPHA_EVA(15)|BLDALPHA_EVB(15);
	
	// Init BG2 top priority
	BG2CNT_B = BGxCNT_EXTENDED_BITMAP_8 | BGxCNT_BITMAP_SIZE_256x256 | BGxCNT_OVERFLOW_WRAP | BGxCNT_BITMAP_BASE_0K;
	BG2CNT_B = (BG2CNT_B&~BGxCNT_PRIORITY_MASK)|BGxCNT_PRIORITY_1;
	BG2PA_B = (1 << 8);
	BG2PB_B = 0;
	BG2PC_B = 0;
	BG2PD_B = (1 << 8);
	BG2X_B = 0;
	BG2Y_B = 0;

	// Init BG3 bottom priority
	BG3CNT_B = BGxCNT_EXTENDED_BITMAP_8 | BGxCNT_BITMAP_SIZE_256x256 | BGxCNT_OVERFLOW_WRAP | BGxCNT_BITMAP_BASE_64K;
	BG3CNT_B = (BG3CNT_B&~BGxCNT_PRIORITY_MASK)|BGxCNT_PRIORITY_2;
	BG3PA_B = (1 << 8);
	BG3PB_B = 0;
	BG3PC_B = 0;
	BG3PD_B = (1 << 8);
	BG3X_B = 0;
	BG3Y_B = 0;

	load8bVRAMIndirect("nitro:/gfx/svatgst_indexed.img.bin", VRAM_B+64*512,256*128);
	load8bVRAMIndirect("nitro:/gfx/svatgst_textonly.img.bin", VRAM_B,256*150);
	load8bVRAMIndirect("nitro:/gfx/svatgst_indexed.pal.bin", PALRAM_B,256);

}

void updatecol(int t) {
	dmaCopyHalfWords( 0, textTable, PALRAM_B + 51 , 12 );
	for(int i = 0; i < 51; i++) {
		dmaCopyHalfWords( 0, flowTable + (((t+8*4)/4+255-i)%12), PALRAM_B + i%255 , 2 );
	}
}

u8 effect0_update( u32 t ) {
	t += 10;
	t *= ((12.5004*16.0)/60.0);
	int dx = (abs(isin((t*8192)/512)))>>8;
	BG2PA_B = (1 << 8) - dx;
	BG2PB_B = 0;
	BG2PC_B = 0;
	BG2PD_B = (1 << 8) - dx;
	BG2X_B = (1<<10);
	BG2Y_B = (1<<10);
	
	updatecol((t*12.0)/32.0);

	if(switchin_status < 128) {
		for(int y = 0; y < 128; y++) {
			if(effect0_loadimage[128*y+switchin_status] != 51) {
				uint16_t* vrampointer = (uint16_t*)(((uint8_t*)(VRAM_B+256*64*2))+256*(60+y)+124+switchin_status);
				uint16_t old = vrampointer[0];
				uint16_t new = effect0_loadimage[128*y+switchin_status];
				old =
					(old & (switchin_status%2==1?0x00FF:0xFF00)) |
					(new<<(switchin_status%2==1?8:0));
				vrampointer[0] = old;
			}
		}
		switchin_status++;
		for(int y = 0; y < 128; y++) {
			if(effect0_loadimage[128*y+switchin_status] != 51) {
				uint16_t* vrampointer = (uint16_t*)(((uint8_t*)(VRAM_B+256*64*2))+256*(60+y)+124+switchin_status);
				uint16_t old = vrampointer[0];
				uint16_t new = effect0_loadimage[128*y+switchin_status];
				old =
					(old & (switchin_status%2==1?0x00FF:0xFF00)) |
					(new<<(switchin_status%2==1?8:0));
				vrampointer[0] = old;
			}
		}
		switchin_status++;
	}
	
	return( 0 );
}


void effect0_destroy() {
// 	BLEND_CR = BLEND_NONE;
// 	SUB_BLEND_CR = BLEND_NONE;
}
