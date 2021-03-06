# Binary name.
NAME = svatglovesyou

# Object files, for the ARM9 binary and the ARM7 binary
OBJS = Main.o ARM.o \
       Drawing.o \
       nitrofs.o \
       Loader.o \
       VoxelBlock.o \
       BoxBlock.o \
       RainbowTable.o \
       Field.o \
       HeartField.o \
       DS3D/DS3D.o \
       DS3D/Utils.o \
       DS3D/Vector/VectorFixed.o \
       DS3D/Vector/MatrixFixed.o DS3D/Vector/QuaternionFixed.o \
       effect0.o \
       metaballs.o \
       effect1.o \
       effect2.o \
       effect3.o \
       effect6.o \
       effect4.o \
       effect7.o
       
OBJS7 = Main.arm7.o

#DEVKITPRO=../devkit/devkitPro

# Libraries to link into the respective binaries
LIBS = -L$(DEVKITPRO)/libnds/lib -lfat -lnds9 -lm -lmm9
LIBS7 = -L$(DEVKITPRO)/libnds/lib -lnds7 -lmm7 -ldswifi7

# Bitmaps, to be converted to .bin files by grit and then stored in the
# nitrofs under /gfx.
# This needs work so that the images are not remade every time the binary is
# compiled.
BITMAPS = gfx/svatgst_indexed.bin \
          gfx/svatgst_textonly.bin \
	  gfx/ball_bg_stripes.bin \
	  gfx/ecgdot_0.bin \
	  gfx/ecgdot_1.bin \
	  gfx/ecgdot_2.bin \
	  gfx/ecgdot_3.bin \
	  gfx/ecgdot_4.bin \
	  gfx/ecgdot_5.bin \
	  gfx/ecgdot_6.bin \
	  gfx/ecgdot_7.bin \
	  gfx/ecgdot_8.bin \
	  gfx/ecgback.bin \
	  gfx/ecgfront_tiles_00.bin \
	  gfx/ecgfront_tiles_01.bin \
	  gfx/ecgfront_tiles_02.bin \
	  gfx/ecgfront_tiles_03.bin \
	  gfx/ecgfront_tiles_04.bin \
	  gfx/ecgfront_tiles_05.bin \
	  gfx/ecgfront_tiles_06.bin \
	  gfx/ecgfront_tiles_07.bin \
	  gfx/ecgfront_tiles_08.bin \
	  gfx/ecgfront_tiles_09.bin \
	  gfx/ecgfront_tiles_10.bin \
	  gfx/ecgfront_tiles_11.bin \
	  gfx/greethaato_blank.bin \
	  gfx/lovesyou.bin \
	  gfx/stripe_bg.bin \
	  gfx/greethaato_mercury.bin \
	  gfx/greethaato_k2.bin \
	  gfx/greethaato_nuance.bin \
	  gfx/greethaato_rno.bin \
	  gfx/greethaato_ms.bin \
	  gfx/greethaato_sysk.bin \
	  gfx/greethaato_gt.bin \
	  gfx/greets_border_00.bin \
	  gfx/greets_border_01.bin \
	  gfx/greets_border_02.bin \
	  gfx/greets_border_03.bin \
	  gfx/greets_border_04.bin \
	  gfx/greets_border_05.bin \
	  gfx/greets_border_06.bin \
	  gfx/greets_border_07.bin \
	  gfx/greets_border_08.bin \
	  gfx/greets_border_09.bin \
	  gfx/greets_border_10.bin \
	  gfx/greets_border_11.bin \
	gfx/ribbon_left_up_1.bin \
	gfx/ribbon_up_right_1.bin \
	gfx/ribbon_right_down_1.bin \
	gfx/ribbon_down_left_1.bin \
	gfx/ribbon_left_up_2.bin \
	gfx/ribbon_up_right_2.bin \
	gfx/ribbon_right_down_2.bin \
	gfx/ribbon_down_left_2.bin \
	gfx/ribbon_left_up_3.bin \
	gfx/ribbon_up_right_3.bin \
	gfx/ribbon_right_down_3.bin \
	gfx/ribbon_down_left_3.bin \
	gfx/ribbon_left_up_4.bin \
	gfx/ribbon_up_right_4.bin \
	gfx/ribbon_right_down_4.bin \
	gfx/ribbon_down_left_4.bin \
	gfx/ribbon_left_right_1.bin \
	gfx/ribbon_up_down_1.bin \
	gfx/ribbon_right_left_1.bin \
	gfx/ribbon_down_up_1.bin \
	gfx/ribbon_left_right_2.bin \
	gfx/ribbon_up_down_2.bin \
	gfx/ribbon_right_left_2.bin \
	gfx/ribbon_down_up_2.bin \
	gfx/ribbon_left_right_3.bin \
	gfx/ribbon_up_down_3.bin \
	gfx/ribbon_right_left_3.bin \
	gfx/ribbon_down_up_3.bin \
	gfx/ribbon_left_right_4.bin \
	gfx/ribbon_up_down_4.bin \
	gfx/ribbon_right_left_4.bin \
	gfx/ribbon_down_up_4.bin \
	gfx/ribbon_head_right_1.bin \
	gfx/ribbon_head_down_1.bin \
	gfx/ribbon_head_left_1.bin \
	gfx/ribbon_head_up_1.bin \
	gfx/ribbon_head_right_2.bin \
	gfx/ribbon_head_down_2.bin \
	gfx/ribbon_head_left_2.bin \
	gfx/ribbon_head_up_2.bin \
	gfx/ribbon_head_right_3.bin \
	gfx/ribbon_head_down_3.bin \
	gfx/ribbon_head_left_3.bin \
	gfx/ribbon_head_up_3.bin \
	gfx/ribbon_head_right_4.bin \
	gfx/ribbon_head_down_4.bin \
	gfx/ribbon_head_left_4.bin \
	gfx/ribbon_head_up_4.bin \
	gfx/ribbon_up_left_1.bin \
	gfx/ribbon_right_up_1.bin \
	gfx/ribbon_down_right_1.bin \
	gfx/ribbon_left_down_1.bin \
	gfx/ribbon_up_left_2.bin \
	gfx/ribbon_right_up_2.bin \
	gfx/ribbon_down_right_2.bin \
	gfx/ribbon_left_down_2.bin \
	gfx/ribbon_up_left_3.bin \
	gfx/ribbon_right_up_3.bin \
	gfx/ribbon_down_right_3.bin \
	gfx/ribbon_left_down_3.bin \
	gfx/ribbon_up_left_4.bin \
	gfx/ribbon_right_up_4.bin \
	gfx/ribbon_down_right_4.bin \
	gfx/ribbon_left_down_4.bin \
	gfx/ribbon_zero_right_1.bin \
	gfx/ribbon_zero_down_1.bin \
	gfx/ribbon_zero_left_1.bin \
	gfx/ribbon_zero_up_1.bin \
	gfx/ribbon_heart.bin \
	gfx/ribbon_frame.bin \
	gfx/tunnel_frame.bin \
	gfx/stripe_bg_hori.bin \
	gfx/self_heart.bin \
	gfx/vote_heart.bin \
	gfx/you_heart.bin \
	gfx/stripe_tile.bin \
	gfx/haato_inter.bin \
	gfx/haato_ribbons.bin \
	gfx/haato_metaballs.bin \
	gfx/haato_metaballs.bin \
	gfx/haato_tunnels.bin \
	gfx/haato_greets.bin \
	gfx/haato_cubes.bin \
	gfx/haato_thatone.bin \
	gfx/haato_raymarching.bin \
	gfx/haato_self.bin \
	gfx/haato_evoke.bin

	  
# Compiler setup
CC = $(DEVKITARM)/bin/arm-eabi-gcc
AS = $(DEVKITARM)/bin/arm-eabi-as
LD = $(DEVKITARM)/bin/arm-eabi-gcc

CFLAGS = -std=gnu99 -O3 -mcpu=arm9e -mtune=arm9e -ffast-math \
-mthumb -mthumb-interwork -I$(DEVKITPRO)/libnds/include -DARM9 $(DEFINES) \
-D__NITRO__ -DBINARY_NAME=\"$(NAME).nds\" -DDEBUG
CFLAGSARM = -std=gnu99 -Os -mcpu=arm9e -mtune=arm9e -ffast-math \
-marm -mthumb-interwork -I$(DEVKITPRO)/libnds/include -DARM9 $(DEFINES) \
-D__NITRO__ -DBINARY_NAME=\"$(NAME).nds\" -DDEBUG
CFLAGS7 = -std=gnu99 -Os -ffast-math -fomit-frame-pointer -mcpu=arm7tdmi \
-mtune=arm7tdmi -mthumb -mthumb-interwork -I$(DEVKITPRO)/libnds/include -DARM7 $(DEFINES)
LDFLAGS = -specs=ds_arm9.specs -mthumb -mthumb-interwork -mno-fpu
LDFLAGS7 = -specs=./ds_arm7_.specs -mthumb -mthumb-interwork -mno-fpu

# Special rules for making png files into .bin files.
# Grit currently appends .img.bin, breaking this a little.
.SUFFIXES: .bin .png
.png.bin :
	$(DEVKITARM)/bin/grit $< -ftb -o$<

# Rules for the final binary
# Builds an NDS binary with a nitrofs file system with the files from /Datafiles
# in it appended.
$(NAME).nds: $(NAME).arm9 $(NAME).arm7
	$(DEVKITARM)/bin/grit gfx_source/pal_images/pal_reduced.png -gB8 -gb -ftb -ogfx/palette.png
	rm gfx/palette.img.bin
	cp gfx/*.bin Datafiles/gfx
	$(DEVKITARM)/bin/ndstool -c $@ -9 $(NAME).arm9 -7 $(NAME).arm7 -r7 0x3800000 -e7 0x3800000 -d Datafiles

# Arm9 / Arm7 binary rules
$(NAME).arm9: $(NAME).arm9.elf
	$(DEVKITARM)/bin/arm-eabi-objcopy -O binary $< $@

$(NAME).arm7: $(NAME).arm7.elf
	$(DEVKITARM)/bin/arm-eabi-objcopy -O binary $< $@

$(NAME).arm9.elf: $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $(OBJS) $(LIBS)

$(NAME).arm7.elf: $(OBJS7)
	$(LD) $(LDFLAGS7) -o $@ $(OBJS7) $(LIBS7)
Main.arm7.o: Main.arm7.c
	$(CC) $(CFLAGS7) -c -o $@ $<

Main.o: Main.c $(BITMAPS)

ARM.o: ARM.c
	$(CC) $(CFLAGSARM) -c -o $@ $<

# To clean, delete all binary objects and converted images
clean:
	rm -f $(NAME).nds $(NAME).arm9 $(NAME).arm7 $(NAME).arm9.elf $(NAME).arm7.elf $(OBJS) $(OBJS7) $(BITMAPS) gfx/*.c gfx/*.h gfx/*.bin *~ Datafiles/gfx/*

# Clean images only
cleanimages:
	rm -f $(BITMAPS) gfx/*.c gfx/*.h gfx/*.bin Datafiles/gfx/*

# Compile if neccesary and run in emulator
test: $(NAME).nds
	/usr/bin/wine $(DEVKITPRO)/nocash/NOCASH.EXE $(NAME).nds

run: $(NAME).nds
	open $(NAME).nds

install: $(NAME).nds
	cp $(NAME).nds /Volumes/NDS/
	hdiutil eject /Volumes/NDS/

# Send binary to flash card for testing on the system
todisk: $(NAME).nds
	/bin/rm -rf disk
	/bin/mkdir disk
	/usr/bin/sudo mount /dev/sdb1 disk
	/usr/bin/sudo cp $(NAME).nds disk/
	/bin/sleep 1
	/usr/bin/sudo umount disk
	/bin/rm -rf disk
