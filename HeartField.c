#include "Tunnel.h"

#include "DS3D/DS3D.h"
#include "DS3D/Utils.h"

#include <math.h>

#include <nds.h>
#include <nds/registers_alt.h>

#define PartSize 1024

uint32_t heartlist[PartSize];

static inline float CalcNormalU(float u1,float v1,float u2,float v2)
{
	float du=u2-u1;
	float dv=v2-v1;
	float r=sqrtf(du*du+dv*dv);
	return dv/r;
}

static inline float CalcNormalV(float u1,float v1,float u2,float v2)
{
	float du=u2-u1;
	float dv=v2-v1;
	float r=sqrtf(du*du+dv*dv);
	return -du/r;
}

#define YOFFS 0.3
#define Y0 (-1+YOFFS)
#define X1 0.5
#define Y1 (-0.5+YOFFS)
#define Z1 0.2
#define X2 0.7
#define Y2 (0+YOFFS)
#define Z2 0.3
#define X3 0.5
#define Y3 (0.3+YOFFS)
#define X4 0.2
#define Y4 (0.35+YOFFS)

#define N1_0_X CalcNormalU(0,Y0,X2,Y2)
#define N1_0_Y CalcNormalV(0,Y0,X2,Y2)
#define N1_1_Y CalcNormalU(Y2,Z2,Y0,0)
#define N1_1_Z CalcNormalV(Y2,Z2,Y0,0)

#define N2_0_X CalcNormalU(X1,Y1,X3,Y3)
#define N2_0_Y CalcNormalV(X1,Y1,X3,Y3)
#define N2_1_Y CalcNormalU(Y2,-Z2,Y1,Z1)
#define N2_1_Z CalcNormalV(Y2,-Z2,Y1,Z1)

#define N3_X CalcNormalU(X2,Y2,X4,Y4)
#define N3_Y CalcNormalV(X2,Y2,X4,Y4)

#define N4_X CalcNormalU(X3,Y3,0,Y2)
#define N4_Y CalcNormalV(X3,Y3,0,Y2)

#define NumHearts 100

struct Heart
{
	int32_t x,y,z;
} hearts[NumHearts];

static void MakeParts()
{
	DSStartList(heartlist,PartSize);

	DSListBegin(DS_TRIANGLE_STRIP);

	DSListNormal3f(0,-1,0);
	DSListVertex3f(0,Y0,0);

	DSListNormal3f(-N1_0_X,N1_0_Y,0);
	DSListVertex3f(-X1,Y1,0);

	DSListNormal3f(0,N1_1_Y,N1_1_Z);
	DSListVertex3f(0,Y1,Z1);

	DSListNormal3f(-N2_0_X,N2_0_Y,0);
	DSListVertex3f(-X2,Y2,0);

	DSListNormal3f(0,N2_1_Y,N2_1_Z);
	DSListVertex3f(0,Y2,Z2);

	DSListNormal3f(-N3_X,N3_Y,0);
	DSListVertex3f(-X3,Y3,0);

	DSListNormal3f(-N4_X,N4_Y,0);
	DSListVertex3f(-X4,Y4,0);

	DSListEnd();

	DSListBegin(DS_TRIANGLE_STRIP);

	DSListNormal3f(N4_X,N4_Y,0);
	DSListVertex3f(X4,Y4,0);

	DSListNormal3f(N3_X,N3_Y,0);
	DSListVertex3f(X3,Y3,0);

	DSListNormal3f(0,N2_1_Y,N2_1_Z);
	DSListVertex3f(0,Y2,Z2);

	DSListNormal3f(N2_0_X,N2_0_Y,0);
	DSListVertex3f(X2,Y2,0);

	DSListNormal3f(0,N1_1_Y,N1_1_Z);
	DSListVertex3f(0,Y1,Z1);

	DSListNormal3f(N1_0_X,N1_0_Y,0);
	DSListVertex3f(X1,Y1,0);

	DSListNormal3f(0,-1,0);
	DSListVertex3f(0,Y0,0);

	DSListEnd();

	DSListBegin(DS_TRIANGLE_STRIP);

	DSListNormal3f(0,-1,0);
	DSListVertex3f(0,Y0,0);

	DSListNormal3f(N1_0_X,N1_0_Y,0);
	DSListVertex3f(X1,Y1,0);

	DSListNormal3f(0,N1_1_Y,-N1_1_Z);
	DSListVertex3f(0,Y1,-Z1);

	DSListNormal3f(N2_0_X,N2_0_Y,0);
	DSListVertex3f(X2,Y2,0);

	DSListNormal3f(0,N2_1_Y,-N2_1_Z);
	DSListVertex3f(0,Y2,-Z2);

	DSListNormal3f(N3_X,N3_Y,0);
	DSListVertex3f(X3,Y3,0);

	DSListNormal3f(N4_X,N4_Y,0);
	DSListVertex3f(X4,Y4,0);

	DSListEnd();

	DSListBegin(DS_TRIANGLE_STRIP);

	DSListNormal3f(-N4_X,N4_Y,0);
	DSListVertex3f(-X4,Y4,0);

	DSListNormal3f(-N3_X,N3_Y,0);
	DSListVertex3f(-X3,Y3,0);

	DSListNormal3f(0,N2_1_Y,-N2_1_Z);
	DSListVertex3f(0,Y2,-Z2);

	DSListNormal3f(-N2_0_X,N2_0_Y,0);
	DSListVertex3f(-X2,Y2,0);

	DSListNormal3f(0,N1_1_Y,-N1_1_Z);
	DSListVertex3f(0,Y1,-Z1);

	DSListNormal3f(-N1_0_X,N1_0_Y,0);
	DSListVertex3f(-X1,Y1,0);

	DSListNormal3f(0,-1,0);
	DSListVertex3f(0,Y0,0);

	DSListEnd();

	DSListBegin(DS_TRIANGLE_STRIP);

	DSListNormal3f(-N4_X,N4_Y,0);
	DSListVertex3f(-X4,Y4,0);

	DSListNormal3f(0,N2_1_Y,-N2_1_Z);
	DSListVertex3f(0,Y2,-Z2);

	DSListNormal3f(0,N2_1_Y,N2_1_Z);
	DSListVertex3f(0,Y2,Z2);

	DSListNormal3f(N4_X,N4_Y,0);
	DSListVertex3f(X4,Y4,0);

	DSListEnd();

	if(DSFinishList()>PartSize) for(;;);
}

static int a=0;
static int b=0;

static void PlaceHeart(struct Heart *heart)
{
	heart->x=isin(a)+7*isin(b*3);
	heart->y=icos(a)+7*icos(b*3);
	a+=b;
	b+=0x2;
}

void InitHeartField()
{
	DISPCNT_A=DISPCNT_MODE_5|DISPCNT_3D|DISPCNT_BG0_ON|DISPCNT_BG2_ON|DISPCNT_BG3_ON|DISPCNT_ON;

	BLDCNT_A = BLDCNT_SRC_A_BG3|BLDCNT_SRC_B_BG0|BLDCNT_EFFECT_ALPHA;
	BLDALPHA_A = BLDALPHA_EVA(8)|BLDALPHA_EVB(15);

	// BG and frame
	VRAMCNT_D = VRAMCNT_D_LCDC;
	VRAMCNT_B = VRAMCNT_B_BG_VRAM_A_OFFS_0K;
	VRAMCNT_A = VRAMCNT_A_BG_VRAM_A_OFFS_128K;

	loadImageVRAMIndirectGreen( "nitro:/gfx/tunnel_frame.img.bin", VRAM_A_OFFS_0K,256*256*2);
	loadImageVRAMIndirectGreen( "nitro:/gfx/stripe_bg_hori.img.bin", VRAM_A_OFFS_128K,256*256*2);

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
	

	DSInit3D();
	DSViewport(0,0,255,191);

	DSSetControl(DS_FOG|DS_TOON_SHADING|DS_OUTLINE);
	DSClearParams(0,0,0,31,63);

	for(int i=0;i<8;i++) DSSetOutlineColor3b(i,31,31,31,0);

	DSSetToonTableRange3b(0,31,24,0,0,0);
	DSSetToonTableRange3b(12,31,31,0,0,0);
	//DSSetToonTableRange3b(24,31,31,4,4,0);
	//DSSetToonTableRange3b(28,31,31,8,8,0);

	DSMatrixMode(DS_PROJECTION);
	DSLoadIdentity();
	DSPerspectivef(65,256.0/192.0,0.1,16);

	DSSetFogLinearf(0,0,0,31,6,10,0.1,16);

	MakeParts();

	for(int i=0;i<NumHearts;i++)
	{
		PlaceHeart(&hearts[i]);
		hearts[i].z=-F(10)*i/NumHearts;
	}
}

void RunHeartField(int t)
{
	DSMatrixMode(DS_POSITION_AND_VECTOR);
	DSLoadIdentity();

	#define LX 0.5
	#define LY -0.5
	#define LZ -1
	DSLight3f(0,0x7fff,LX/sqrtf(LX*LX+LY*LY+LZ*LZ),LY/sqrtf(LX*LX+LY*LY+LZ*LZ),LZ/sqrtf(LX*LX+LY*LY+LZ*LZ));
//	DSMaterialDiffuseAndAmbient(0x3def,0x3def);
//	DSMaterialDiffuseAndAmbient6b(15,15,15,16,16,16);
	DSMaterialDiffuseAndAmbient6b(31,0,0,0,0,0);

	DSMatrixMode(DS_PROJECTION);
	DSPushMatrix();

	int dx=7*isin(b*3-NumHearts*2*3);
	int dy=7*icos(b*3-NumHearts*2*3);

	ivec3_t forward=ivec3norm(ivec3(dy,-dx,F(6)));
	ivec3_t side=ivec3norm(ivec3cross(ivec3(0,F(1),0),forward));
	ivec3_t up=ivec3cross(forward,side);
	DSMultMatrix3x3(imat3x3vec3(side,up,forward));

	DSTranslatef32(-dx,-dy,0);

	DSMatrixMode(DS_POSITION_AND_VECTOR);
	DSLoadIdentity();

	for(int i=0;i<NumHearts;i++)
	{
		DSLoadIdentity();
		DSTranslatef32(hearts[i].x,hearts[i].y,hearts[i].z);

		int32_t scale=isin(F(1)*i/NumHearts-t*24);
		scale=scale/5+F(0.4);

		DSScalef32(scale,scale,scale);
		DSRotateYi(F(1)*i/NumHearts+t*0x10);

		int id=((-hearts[i].z)>>10)&0x3f;

		DSPolygonAttributes(DS_POLY_MODE_TOON|DS_POLY_CULL_FRONT|DS_POLY_LIGHT0|DS_POLY_FOG|
		DS_POLY_ALPHA(31)|DS_POLY_ID(id));

		DSCallList(heartlist);

		hearts[i].z+=F(0.1);
		if(hearts[i].z>=0)
		{
			PlaceHeart(&hearts[i]);
			hearts[i].z-=F(10);
		}
	}

	DSMatrixMode(DS_PROJECTION);
	DSPopMatrix(0);

	DSSwapBuffers(0);
}


