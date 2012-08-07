#ifndef __ARM_H__
#define __ARM_H__

#include "Utils.h"
#include <stdint.h>

void ClaimWRAM();

void ATTR_ITCM RenderFieldARM(int t,uint16_t *vram,int16_t *rays);

#endif
