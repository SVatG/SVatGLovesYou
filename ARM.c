#include "ARM.h"

#include <math.h>

void ClaimWRAM()
{
	WRAMCNT=WRAMCNT_ARM9_ALL;
	asm volatile (
	"	mcr p15,0,%0,C6,C2,0\n" // Configure region 2 as 16 MB from 0x3000000
	"	mrc p15,0,r0,C5,C0,0\n"
	"	orr	r0,r0,#0x30\n"
	"	mcr p15,0,r0,C5,C0,0\n" // Enable data read/write for region 2
	::"r" (0x300002f):"r2");
}

#define Width 128
#define FullWidth 128
#define Height 96

static inline int32_t approxabs(int32_t x) { return x^(x>>31); }

// Uncomment to force inlining.
//static inline int ATTR_ITCM __attribute__((always_inline)) RenderPixel(int32_t x,int32_t y,int32_t z,int32_t dx,int32_t dy,int32_t dz);
static inline int ATTR_ITCM RenderPixel(int32_t x,int32_t y,int32_t z,int32_t dx,int32_t dy,int32_t dz);
static inline int ATTR_ITCM RenderPixel(int32_t x,int32_t y,int32_t z,int32_t dx,int32_t dy,int32_t dz)
{
	int i=23;

	while(i)
	{
/*		int32_t tx=approxabs(x)>>15;
		int32_t ty=approxabs(y)>>15;
		int32_t tz=approxabs(z)>>15;

		int32_t dist=tx;
		if(ty>dist) dist=ty;
		if(tz>dist) dist=tz;

		dist-=0x8000;

		if(dist<0x800) break;*/

		int32_t tx=approxabs(x)>>16;
		int32_t ty=approxabs(y)>>16;
		int32_t tz=approxabs(z)>>16;

// 		int32_t dist=tx+ty+tz-0x8000;
		int32_t dist=(tx+ty+(tz&0xf0f0))-0x8000;
// 		int32_t dist=(tx+ty+(tz&0x005555))-0x8000;
// 		
		if(dist<=0x400) break;

/*		int32_t tx=approxabs(x)>>15;
		int32_t ty=approxabs(y)>>15;
		int32_t tz=approxabs(z)>>15;

		int32_t dist1=tx;
		if(tz>dist1) dist1=tz;

		int32_t dist2=tx;
		if(ty>dist2) dist2=ty;

		int32_t dist=dist1;
		if(dist2<dist1) dist=dist2;

		dist-=0x4000;

		if(dist<0x400) break;*/

/*		int32_t tx=approxabs(x)>>15;
		int32_t ty=approxabs(y)>>15;
		int32_t tz=approxabs(z)>>15;

		int32_t dist1=tx;
		if(tz>dist1) dist1=tz;

		int32_t dist2=tx;
		if(ty>dist2) dist2=ty;

		int32_t dist3=ty;
		if(tz>dist3) dist3=tz;

		int32_t dist=dist1;
		if(dist2<dist) dist=dist2;
		if(dist3<dist) dist=dist3;

		dist-=0x4000;

		if(dist<0x400) break;*/

/*		int32_t tx=approxabs(x)>>16;
		int32_t ty=approxabs(y)>>16;
		int32_t tz=approxabs(z)>>16;

		int32_t dist=approxabs(ty-tz)+tx-0x3000;

		if(dist<0x200) break;*/

/*		int32_t tx=approxabs(x)>>16;
		int32_t ty=approxabs(y)>>16;
		int32_t tz=approxabs(z)>>16;

//		int32_t dist=(tx|ty^tz)-0x5000;
//		int32_t dist=(tx|ty&tz)-0x5000;
//		int32_t dist=(tx|ty+tz)-0x5000;
//		int32_t dist=(tx+ty)&tz-0x8000;
		int32_t dist=(tx+ty+(tz&0xf0f0))-0x8000;
//		int32_t dist=(tx+ty+(tz&0x005555))-0x8000;

		if(dist<0x200) break;
*/

/*		int32_t tx=approxabs(x)>>16;
		int32_t ty=approxabs(y)>>15;
		int32_t tz=approxabs(z)>>16;

		int32_t dist=(tx+(ty&0xc3c3c000)+(tz&0xf0f0f0f0))-0x8000;

		if(dist<0x200) break;*/

		x+=dx*dist;
		y+=dy*dist;
		z+=dz*dist;

		i--;
	}
	return i;
}

void ATTR_ITCM RenderFieldARM(int t,uint16_t *vram,int16_t *rays)
{
	int32_t x0=(0+Fix(0.5))<<20;
	int32_t y0=(isin(t*7)+Fix(0.5))<<20;
	int32_t z0=(t*50+Fix(0.5))<<20;

	int32_t sin_a=isin(t*5);
	int32_t cos_a=icos(t*5);

	for(int y=0;y<Height;y++)
	{
		for(int x=0;x<Width/2;x++)
		{
			int32_t dx=*rays++;
			int32_t dy=*rays++;
			int32_t dz=*rays++;

			int32_t t=(dx*cos_a+dz*sin_a)>>12;
			dz=(-dx*sin_a+dz*cos_a)>>12;
			dx=t;

			int i1=RenderPixel(x0,y0,z0,dx,dy,dz);

			dx=*rays++;
			dy=*rays++;
			dz=*rays++;

			t=(dx*cos_a+dz*sin_a)>>12;
			dz=(-dx*sin_a+dz*cos_a)>>12;
			dx=t;

			int i2=RenderPixel(x0,y0,z0,dx,dy,dz);

			*vram++=(i2<<8)|i1;
		}
		vram+=(FullWidth-Width)/2;
	}
}

