/**
 * Functions that load things from the file system.
 */

#include <nds.h>
#include "Loader.h"

// Load a single 16bit image into RAM, ensuring it is visible
void loadImage(char* path, uint16_t* buffer, uint32_t size) {
	int fd = open( path, O_RDONLY );
	read( fd, buffer, size );
	close( fd );

	// Ensure alpha bit is set
	for( int i = 0; i < size/2; i++ ) {
		buffer[i] |= BIT(15);
	}
}

// Load data into VRAM at a specified position by first loading
// it into main memory and them DMA'ing it to VRAM.
void loadVRAMIndirect(char* path, uint16_t* vramPos, uint32_t size) {
	int fd = open( path, O_RDONLY );
	read( fd, tempImage, size );
	close( fd );

 	for( int i = 0; i < size/2; i++ ) {
		vramPos[i] = tempImage[i];
	}
}

// Load data into VRAM at a specified position by first loading
// it into main memory and them DMA'ing it to VRAM and also visibe &c &c
void loadImageVRAMIndirect(char* path, uint16_t* vramPos, uint32_t size) {
	int fd = open( path, O_RDONLY );
	read( fd, tempImage, size*2 );
	close( fd );

 	for( int i = 0; i < size; i++ ) {
		vramPos[i] = tempImage[i] | (tempImage[i] == 0?0:BIT(15));
	}
}

// Fuck everything these functions are now anything but general
void loadImageVRAMIndirectGreen(char* path, uint16_t* vramPos, uint32_t size) {
	int fd = open( path, O_RDONLY );
	read( fd, tempImage, size*2 );
	close( fd );

 	for( int i = 0; i < size; i++ ) {
		vramPos[i] = tempImage[i] | ((tempImage[i]|BIT(15)) == ((31<<5)|BIT(15)) ?0:BIT(15));
	}
}



// Load a single 8bit image into VRAM at a specified position by first loading
// it into main memory and them DMA'ing it to VRAM.
void load8bVRAMIndirect(char* path, uint16_t* vramPos, uint32_t size) {
	int fd = open( path, O_RDONLY );
	read( fd, tempImage, size );
	close( fd );

	for( int i = 0; i < size; i++ ) {
		vramPos[i] = tempImage[i];
	}
}


// Load any data into main ram.
void loadData(char* path, uint8_t* target, uint32_t size) {
	int fd = open( path, O_RDONLY | O_BINARY );
	read( fd, target, size );
	close( fd );
}

// Load an 64x64 image into A/B OBJ RAM, return pointer.
uint16_t* loadSpriteA( char* path ) {
	uint16_t* newSprite = oamAllocateGfx(&oamMain, SpriteSize_64x64, SpriteColorFormat_256Color);
	load8bVRAMIndirect( path, newSprite, 64*64*2 );
	return( newSprite );
}
uint16_t* loadBmpSpriteA( char* path ) {
	uint16_t* newSprite = oamAllocateGfx(&oamMain, SpriteSize_32x32, SpriteColorFormat_Bmp);
	loadImageVRAMIndirect( path, newSprite, 64*64 );
	return( newSprite );
}
uint16_t* loadBmpSpriteB( char* path ) {
	uint16_t* newSprite = oamAllocateGfx(&oamSub, SpriteSize_32x32, SpriteColorFormat_Bmp);
	loadImageVRAMIndirect( path, newSprite, 64*64 );
	return( newSprite );
}
uint16_t* loadBmpSpriteAGreen( char* path ) {
	uint16_t* newSprite = oamAllocateGfx(&oamMain, SpriteSize_64x64, SpriteColorFormat_Bmp);
	loadImageVRAMIndirectGreen( path, newSprite, 64*64*2 );
	return( newSprite );
}
uint16_t* loadSpriteB( char* path ) {
	uint16_t* newSprite = oamAllocateGfx(&oamSub, SpriteSize_64x64, SpriteColorFormat_256Color);
	load8bVRAMIndirect( path, newSprite, 64*64*2 );
	return( newSprite );
}

// Same, 32x32.
uint16_t* loadSprite32A( char* path ) {
	uint16_t* newSprite = oamAllocateGfx(&oamMain, SpriteSize_32x32, SpriteColorFormat_256Color);
	load8bVRAMIndirect( path, newSprite, 32*32*2 );
	return( newSprite );
}

uint16_t* loadSprite32B( char* path ) {
	uint16_t* newSprite = oamAllocateGfx(&oamSub, SpriteSize_32x32, SpriteColorFormat_256Color);
	load8bVRAMIndirect( path, newSprite, 32*32*2 );
	return( newSprite );
}

// 16x16
uint16_t* loadSprite16A( char* path ) {
	uint16_t* newSprite = oamAllocateGfx(&oamMain, SpriteSize_16x16, SpriteColorFormat_256Color);
	load8bVRAMIndirect( path, newSprite, 16*16*2 );
	return( newSprite );
}