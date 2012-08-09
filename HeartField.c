#include "Tunnel.h"

#include "DS3D/DS3D.h"
#include "DS3D/Utils.h"

#include <math.h>

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

#define NumHearts 128

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

void InitHeartField()
{
	DISPCNT_A=DISPCNT_MODE_0|DISPCNT_ON|DISPCNT_3D|DISPCNT_BG0_ON;

	DSInit3D();
	DSViewport(0,0,255,191);

	DSSetControl(DS_ANTIALIAS|DS_FOG|DS_TOON_SHADING|DS_OUTLINE);
	DSClearParams(0,0,0,31,63);

	for(int i=0;i<8;i++) DSSetOutlineColor3b(i,31,31-i*4,31-i*4,0);

	DSSetToonTableRange3b(0,31,24,4,4,0);
	DSSetToonTableRange3b(6,31,31,4,4,0);
	DSSetToonTableRange3b(14,31,31,8,8,0);
	DSSetToonTableRange3b(20,31,31,16,16,0);

	DSMatrixMode(DS_PROJECTION);
	DSLoadIdentity();
	DSPerspectivef(65,256.0/192.0,0.1,16);

	DSSetFogLinearf(0,0,0,31,1,10,0.1,16);

	MakeParts();

	for(int i=0;i<NumHearts;i++)
	{
		hearts[i].x=Random()%F(2)-F(1);
		hearts[i].y=Random()%F(2)-F(1);
		hearts[i].z=-F(16)*i/NumHearts;
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

/*	ivec3_t forward=ivec3norm(ivec3(dx,dy,F(1)));
	ivec3_t side=ivec3norm(ivec3cross(ivec3(0,F(1),0),forward));
	ivec3_t up=ivec3cross(forward,side);
	DSMultMatrix3x3(imat3x3vec3(side,up,forward));*/

	DSMatrixMode(DS_POSITION_AND_VECTOR);
	DSLoadIdentity();

	for(int i=0;i<NumHearts;i++)
	{
		DSLoadIdentity();
		DSTranslatef32(hearts[i].x,hearts[i].y,hearts[i].z);
		DSScalef(0.3,0.3,0.3);
		DSRotateYi(F(1)*i/NumHearts);

		int id=((-hearts[i].z)>>10)&0x3f;

		DSPolygonAttributes(DS_POLY_MODE_TOON|DS_POLY_CULL_FRONT|DS_POLY_LIGHT0|DS_POLY_FOG|
		DS_POLY_ALPHA(31)|DS_POLY_ID(id));

		DSCallList(heartlist);

		hearts[i].z+=F(0.1);
		if(hearts[i].z>0) hearts[i].z-=F(16);
	}

	DSMatrixMode(DS_PROJECTION);
	DSPopMatrix(0);

	DSSwapBuffers(0);
}


