// Pretty meta-balls.

#include <nds.h>

#include "DS3D/DS3D.h"
#include "Hardware.h"

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <math.h>

#include "marching_cubes.h"

#define SWITCH(x,y) x=((x)^(y));y=((x)^(y));x=((x)^(y));
// #define SWITCH(x,y) t=(x);(x)=(y);(y)=t;
#define abs(x) ((x)<0)?(-(x)):(x)

// A single metaball call.
// function:
// f(x,y,z) = 1 / ((x − x0)^2 + (y − y0)^2 + (z − z0)^2)
// It's slow, so we're going to need clever tricks.
// IDEA: cache field strengths for metaballs of one radius, then just copy
// memory for position changes and somehow interpolate.
static inline s32 metaball( s32 x, s32 y, s32 z, s32 cx, s32 cy, s32 cz ) {
	s32 dx = abs(x - cx);
	s32 dy = abs(y - dy);
	s32 dz = abs(z - dz);
	s32 xs = mulf32( dx, dx );
	s32 ys = mulf32( dy, dy );
	s32 zs = mulf32( dz, dz );
	s32 rsq = (xs + ys + zs + (1<<9));
	rsq = rsq < 1 ? 1 : rsq;
	return( divf32( 1<<12, rsq ) );
}


#define PREGRID(x,y,z) pre_grid[30*30*z+30*y+x]

// Nice precalculated grid for the balls.
// BROKEN PRECELL *pre_grid = (s8*)(0x027C0000 + 256 * sizeof( s16 ) + 256 * 16 * sizeof(u8));
PRECELL* pre_grid;

// Dead polygon storage
TRIANGLE* triangles;

// Get a single grid cell by looking up shit in the precalc'd table
static inline void ATTR_ITCM cell_at( GRIDCELL* b, s32 x, s32 y, s32 z, s32 cx, s32 cy, s32 cz, u8 inf ) {
	s16 accx;
	s16 accy;
	s16 accz;
	
	accx = abs( x - cx );
	accy = abs( y - cy );
	accz = abs( z - cz );

	memcpy( &b->p, &PREGRID(x,y,z).p[ 0 ], 8 * sizeof( XYZ ) );
	memcpy( &b->val, &PREGRID( accx, accy, accz ).val[ 0 ], 8 * sizeof( u16 ) );
	if( (x - cx) < 0 ) {
		SWITCH( b->val[ 0 ],  b->val[ 1 ] );
		SWITCH( b->val[ 4 ],  b->val[ 5 ] );
		SWITCH( b->val[ 7 ],  b->val[ 6 ] );
		SWITCH( b->val[ 3 ],  b->val[ 2 ] );
	}
	if( (y - cy) < 0 ) {
		SWITCH( b->val[ 0 ],  b->val[ 4 ] );
		SWITCH( b->val[ 1 ],  b->val[ 5 ] );
		SWITCH( b->val[ 3 ],  b->val[ 7 ] );
		SWITCH( b->val[ 2 ],  b->val[ 6 ] );
	}
	if( (z - cz) < 0 ) {
		SWITCH( b->val[ 1 ],  b->val[ 2 ] );
		SWITCH( b->val[ 0 ],  b->val[ 3 ] );
		SWITCH( b->val[ 5 ],  b->val[ 6 ] );
		SWITCH( b->val[ 4 ],  b->val[ 7 ] );
	}

	for( u8 i = 0; i < 8; i++ ) {
		b->p[ i ].inf[inf] = b->val[ i ];
	}
}

// Same, but add to a previously created cell.
static inline void cell_add( GRIDCELL* b, s32 x, s32 y, s32 z, s32 cx, s32 cy, s32 cz, u8 inf ) {
	GRIDCELL c;
	
	s16 accx = abs( x - cx );
	s16 accy = abs( y - cy );
	s16 accz = abs( z - cz );

	memcpy( &c.val, &PREGRID( accx, accy, accz ).val[ 0 ], 8 * sizeof( u16 ) );
	if( (x - cx) < 0 ) {
		SWITCH( c.val[ 0 ],  c.val[ 1 ] );
		SWITCH( c.val[ 4 ],  c.val[ 5 ] );
		SWITCH( c.val[ 7 ],  c.val[ 6 ] );
		SWITCH( c.val[ 3 ],  c.val[ 2 ] );
	}
	if( (y - cy) < 0 ) {
		SWITCH( c.val[ 0 ],  c.val[ 4 ] );
		SWITCH( c.val[ 1 ],  c.val[ 5 ] );
		SWITCH( c.val[ 3 ],  c.val[ 7 ] );
		SWITCH( c.val[ 2 ],  c.val[ 6 ] );
	}
	if( (z - cz) < 0 ) {
		SWITCH( c.val[ 1 ],  c.val[ 2 ] );
		SWITCH( c.val[ 0 ],  c.val[ 3 ] );
		SWITCH( c.val[ 5 ],  c.val[ 6 ] );
		SWITCH( c.val[ 4 ],  c.val[ 7 ] );
	}

	for( int i = 0; i < 8; i++ ) {
		b->val[ i ] += c.val[ i ];
		b->p[ i ].inf[inf] = c.val[ i ];
	}
}

void metaballs_precompute() {
	pre_grid = malloc(30*30*30*sizeof(PRECELL));
	
	// Set up vertex position change variables.
	u32 cell_width = divf32( (2 << 10)*3, 20 << 12 );
	u32 cell_size = mulf32( cell_width, 2 << 12 );

	// Precalculate field strengths.
	s32 tx = 0;
	s32 ty = 0;
	s32 tz = 0;
	for( int x = 0; x < 30; x++ ) {
		ty = 0;
		for( int y = 0; y < 30; y++ ) {
			tz = 0;
			for( int z = 0; z < 30; z++ ) {
				PREGRID(x,y,z).p[ 0 ].x = tx - cell_width;
				PREGRID(x,y,z).p[ 0 ].y = ty - cell_width;
				PREGRID(x,y,z).p[ 0 ].z = tz + cell_width;

				PREGRID(x,y,z).p[ 1 ].x = tx + cell_width;
				PREGRID(x,y,z).p[ 1 ].y = ty - cell_width;
				PREGRID(x,y,z).p[ 1 ].z = tz + cell_width;

				PREGRID(x,y,z).p[ 2 ].x = tx + cell_width;
				PREGRID(x,y,z).p[ 2 ].y = ty - cell_width;
				PREGRID(x,y,z).p[ 2 ].z = tz - cell_width;

				PREGRID(x,y,z).p[ 3 ].x = tx - cell_width;
				PREGRID(x,y,z).p[ 3 ].y = ty - cell_width;
				PREGRID(x,y,z).p[ 3 ].z = tz - cell_width;

				PREGRID(x,y,z).p[ 4 ].x = tx - cell_width;
				PREGRID(x,y,z).p[ 4 ].y = ty + cell_width;
				PREGRID(x,y,z).p[ 4 ].z = tz + cell_width;

				PREGRID(x,y,z).p[ 5 ].x = tx + cell_width;
				PREGRID(x,y,z).p[ 5 ].y = ty + cell_width;
				PREGRID(x,y,z).p[ 5 ].z = tz + cell_width;

				PREGRID(x,y,z).p[ 6 ].x = tx + cell_width;
				PREGRID(x,y,z).p[ 6 ].y = ty + cell_width;
				PREGRID(x,y,z).p[ 6 ].z = tz - cell_width;

				PREGRID(x,y,z).p[ 7 ].x = tx - cell_width;
				PREGRID(x,y,z).p[ 7 ].y = ty + cell_width;
				PREGRID(x,y,z).p[ 7 ].z = tz - cell_width;

				for( int i = 0; i < 8; i++ ) {
					PREGRID(x,y,z).val[ i ] = metaball(
						PREGRID(x,y,z).p[ i ].x,
						PREGRID(x,y,z).p[ i ].y,
						PREGRID(x,y,z).p[ i ].z,
					0, 0, 0);
				}

				tz += cell_size;
			}
			ty += cell_size;
		}
		tx += cell_size;
	}
	
	copyTables();	
}

// Let's recurse but not!
// s16 pc[400][3];
s16 *pc = (s16*)(0x027C0000 + 256 * sizeof( s16 ) + 256*16*sizeof( s8 ));
// s16* pc;
#define PC(x,y) pc[(x)*3+(y)]

// mah balls.
void metaballs_init() {

// 	pc = malloc(400*3*sizeof(s16));
	
	DISPCNT_A=DISPCNT_MODE_5|DISPCNT_3D|DISPCNT_BG0_ON|DISPCNT_BG3_ON|DISPCNT_ON;

	BLDCNT_A = BLDCNT_SRC_A_BG3|BLDCNT_SRC_B_BG0|BLDCNT_EFFECT_ALPHA;
	BLDALPHA_A = BLDALPHA_EVA(8)|BLDALPHA_EVB(8);
	
	// VRAM and DISP setup.
	VRAMCNT_A = VRAMCNT_A_BG_VRAM_A;

	// Init BG0 priority as above BG3
	BG0CNT_A = (BG0CNT_A&~BGxCNT_PRIORITY_MASK)|BGxCNT_PRIORITY_1;
	
	// Init BG3 bottom priority
	BG3CNT_A = BGxCNT_EXTENDED_BITMAP_8 | BGxCNT_BITMAP_SIZE_256x256 | BGxCNT_OVERFLOW_WRAP | BGxCNT_BITMAP_BASE_0K;
	BG3CNT_A = (BG3CNT_A&~BGxCNT_PRIORITY_MASK)|BGxCNT_PRIORITY_0;
	BG3PA_A = (1 << 8);
	BG3PB_A = 0;
	BG3PC_A = 0;
	BG3PD_A = (1 << 8);
	BG3X_A = 0;
	BG3Y_A = 0;

	load8bVRAMIndirect("nitro:/gfx/ball_bg_stripes.img.bin", VRAM_A,256*256);
	load8bVRAMIndirect("nitro:/gfx/ball_bg_stripes.pal.bin", PALRAM_A,256);
	
	// Set up voxelcubes
	DSInit3D();
	DSViewport(0,0,255,191);

	DSSetControl(DS_TEXTURING|DS_ANTIALIAS);
	DSClearParams(21,21,21,0,63);

	DSMatrixMode(DS_PROJECTION);
	DSLoadIdentity();
	DSPerspectivef(45,256.0/192.0,0.01f,1000.0f);

	// Materials.
	glMaterialf( GL_AMBIENT, RGB15( 8, 8, 8 ) );
	glMaterialf( GL_DIFFUSE, RGB15( 24, 24, 24 ) );
	glMaterialf( GL_SPECULAR, RGB15( 0, 0, 0 ) );
	glMaterialf( GL_EMISSION, RGB15( 0, 0, 0 ) );

	DSPolygonAttributes(DS_POLY_CULL_NONE|DS_POLY_LIGHT0|DS_POLY_ALPHA(0)|DS_POLY_MODE_MODULATION);

	triangles = malloc(sizeof(TRIANGLE)*1200);
}

s16 g_tris = 0;
s8 mark_grid[30][30][30];

s16 cp = 0;

static inline void ATTR_ITCM step_to_pc( s32 x, s32 y, s32 z );
static inline void ATTR_ITCM step_to_pc( s32 x, s32 y, s32 z ) {
	PC(cp + 1 , 0 ) = x + 1;
	PC(cp + 1 , 1 ) = y;
	PC(cp + 1 , 2 ) = z;
	
	PC(cp + 2 , 0 ) = x - 1;
	PC(cp + 2 , 1 ) = y;
	PC(cp + 2 , 2 ) = z;
	
	PC(cp + 3 , 0 ) = x;
	PC(cp + 3 , 1 ) = y + 1;
	PC(cp + 3 , 2 ) = z;
	
	PC(cp + 4 , 0 ) = x;
	PC(cp + 4 , 1 ) = y - 1;
	PC(cp + 4 , 2 ) = z;

	PC(cp + 5 , 0 ) = x;
	PC(cp + 5 , 1 ) = y;
	PC(cp + 5 , 2 ) = z + 1;
	
	PC(cp + 6 , 0 ) = x;
	PC(cp + 6 , 1 ) = y;
	PC(cp + 6 , 2 ) = z - 1;

	cp += 7;
}

static inline u32 ATTR_ITCM balls( s32* x, s32* y, s32* z, s8 ball_count, TRIANGLE* tris );
static inline u32 ATTR_ITCM balls( s32* x, s32* y, s32* z, s8 ball_count, TRIANGLE* tris ) {
	g_tris = 0;

	// Reset status
	memset( mark_grid, 0, 30 * 30 * 30 * sizeof( u8 ) );

	for( s8 c = 0; c < ball_count; c++ ) {
		s32 cx = x[ c ];
		s32 cy = y[ c ];
		s32 cz = z[ c ];
		
		// Find first cell
		GRIDCELL b;
		s8 tri_add = 0;
		
		while( tri_add == 0 ) {
			cell_at( &b, cx, cy, cz, x[ 0 ], y[ 0 ], z[ 0 ], c );
			for( u8 i = 1; i < ball_count; i++ ) {
				cell_add( &b, cx, cy, cz, x[ i ], y[ i ], z[ i ], i );
			}
			tri_add = polygonise( &b, (1 << 14), &tris[ g_tris ] );
			cx++;
		}
		
		// Had that one already.
		if( mark_grid[ cx ][ cy ][ cz ] == 1 ) {
			continue;
		}

		// Commit triangles.
		g_tris += tri_add;
		
		// Found triangle, now recurse.
		mark_grid[ cx ][ cy ][ cz ] = 1;

		// Push first step.
		cp = 0;
		PC(cp,0) = cx;
		PC(cp,1) = cy;
		PC(cp,2) = cz;
		step_to_pc( PC(cp,0), PC(cp,1), PC(cp,2) );

		do {
			// Pop one step.
			cp--;
			
			// Visitation check.
			if( mark_grid[PC(cp, 0)][PC(cp, 1)][PC(cp, 2)] == 1 ) {
				continue;
			}
			mark_grid[PC(cp, 0)][PC(cp, 1)][PC(cp, 2)] = 1;

			// Tri check.
			cell_at( &b, PC(cp,0), PC(cp,1), PC(cp,2), x[ 0 ], y[ 0 ], z[ 0 ], 0 );
			for( u8 i = 1; i < ball_count; i++ ) {
				cell_add( &b, PC(cp,0), PC(cp,1), PC(cp,2), x[ i ], y[ i ], z[ i ], i );
			}
			tri_add = polygonise( &b, 1 << 14, &tris[ g_tris ] );

			if( tri_add != 0 ) {
				g_tris += tri_add;
				step_to_pc( PC(cp, 0), PC(cp, 1), PC(cp, 2) );
			}
			
		} while( cp != 0 );
	}
	
	return( g_tris );
}

// Position variables
void metaballs_update(s32 t) {
	DSMatrixMode( DS_MODELVIEW );
	DSLoadIdentity();
	DSTranslatef( 0.0f, -0.05f, -5.5f);
	DSRotatef32f((t*2)>>1,0,0,1<<12);
	DSRotatef32f((t*3)>>1,0,1<<12,0);
	DSRotatef32f((t*5)>>1,1<<12,0,0);
	DSTranslatef( -2.0f, -2.0f, -2.0f);

	GRIDCELL b;
	u16 tri_count = 0;

	// Calculate grid isolevel strengths and polygonise.
	s32 xa[ 3 ] = {
		12+((isin(t*25+12)*5)>>13),
		12+((isin(t*12+11)*5)>>12),
		12+((isin(t*44+19)*5)>>12),
	};
	s32 ya[ 3 ] = {
		12+((isin(t*13+2)*5)>>12),
		12+((isin(t*3+0)*5)>>13),
		12+((isin(t*8+33)*5)>>12),
	};
	s32 za[ 3 ] = {
		12+((isin(t*13+16)*5)>>12),
		12+((isin(t*33+15)*5)>>13),
		12+((isin(t*7+17)*5)>>12),
	};

	tri_count = balls( xa, ya, za, 3, &triangles[ 0 ] );
	
	// Draw the created mesh.
	DSBegin( DS_TRIANGLES );
	for( u16 i = 0; i < tri_count; i++ ) {
		for( s8 j = 0; j < 3; j++ ) {
			DSColor3b(
				(triangles[ i ].p[ j ].inf[0]*29 + triangles[ i ].p[ j ].inf[1]*30 + triangles[ i ].p[ j ].inf[2]*31)>>14,
				(triangles[ i ].p[ j ].inf[0]*29 + triangles[ i ].p[ j ].inf[1]*18 + triangles[ i ].p[ j ].inf[2]*8)>>14,
				(triangles[ i ].p[ j ].inf[0]*29 + triangles[ i ].p[ j ].inf[1]*18 + triangles[ i ].p[ j ].inf[2]*8)>>14

			);
			DSVertex3v16( triangles[ i ].p[ j ].x, triangles[ i ].p[ j ].y, triangles[ i ].p[ j ].z );
		}
	}
	DSEnd();
		
	DSSwapBuffers(0);
}

void metaballs_destroy() {
	BLDCNT_A = 0;
	BLDALPHA_A = 0;
	free(triangles);
	free(pre_grid);
// 	free(pc);
}