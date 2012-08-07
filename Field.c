#include "Field.h"
#include "ARM.h"
#include "Hardware.h"
#include "Utils.h"

#include <math.h>
#include <string.h>

#define Width 128
#define Height 96

static int frame;
static int16_t rayarray[Width*Height*3];

void InitField()
{
	VRAMCNT_A=VRAMCNT_A_LCDC;
	VRAMCNT_B=VRAMCNT_B_LCDC;
	VRAMCNT_C=VRAMCNT_C_BG_VRAM_A_OFFS_0K;
	VRAMCNT_D=VRAMCNT_D_LCDC;

	DISPCNT_A=DISPCNT_MODE_4|DISPCNT_BG3_ON|DISPCNT_ON;
	BG3CNT_A=BGxCNT_BITMAP_BASE_0K|BGxCNT_EXTENDED_BITMAP_8
			|BGxCNT_BITMAP_SIZE_128x128;
	BG3PA_A=0x80;
	BG3PB_A=0;
	BG3PC_A=0;
	BG3PD_A=0x80;
	BG3HOFS_A=0;
	BG3VOFS_A=0;

	frame=0;

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
}

void RunField(int t)
{
	uint16_t *vram;
	if(frame)
	{
		BG3CNT_A=BGxCNT_BITMAP_BASE_0K|BGxCNT_EXTENDED_BITMAP_8
				|BGxCNT_BITMAP_SIZE_128x128;
		vram=VRAM_A_OFFS_64K;
	}
	else
	{
		BG3CNT_A=BGxCNT_BITMAP_BASE_64K|BGxCNT_EXTENDED_BITMAP_8
				|BGxCNT_BITMAP_SIZE_128x128;
		vram=VRAM_A_OFFS_0K;
	}
	frame^=1;

	for(int i=0;i<12;i++)
	PALRAM_A[i]=0x8000|MakeHSV(210,Fix(1),Fix(i)/12);

	for(int i=0;i<12;i++)
	PALRAM_A[i+12]=0x8000|MakeHSV(210,Fix(11-i)/12,Fix(1));

	RenderFieldARM(t,vram,rayarray);
}
