// SPDX-License-Identifier: GPL-2.0-or-later
//
// Copyright (c) 2011-2015, 2019-2020, Antonio Niño Díaz
//
// GiiBiiAdvance - GBA/GB emulator

#ifndef GB_SGB__
#define GB_SGB__

#define SGB_MAX_PACKETS         (7)
#define SGB_BYTES_PER_PACKET    (16)

#define SGB_NUM_PALETTES        (8)

#define SGB_SCREEN_FREEZE       (1)
#define SGB_SCREEN_BLACK        (2)
#define SGB_SCREEN_BACKDROP     (3)

// A 60ms (4 frames) delay should be invoked between each packet transfer.
#define SGB_PACKET_DELAY        (280896)

#include "gameboy.h"

typedef struct
{
    u32 delay; // for delay between frames (not used for now)

    u32 sending;
    u32 continue_;
    u32 currbit;
    u32 currbyte;
    u32 currpacket;
    u32 numpackets;
    // 7 packets max, 16 * 8 bits each
    u32 data[SGB_MAX_PACKETS][SGB_BYTES_PER_PACKET];

    //--------------------------------------------------------------------------

    u32 multiplayer;
    u32 read_joypad;
    u32 current_joypad;

    //--------------------------------------------------------------------------

    u32 freeze_screen;

    //--------------------------------------------------------------------------

    u32 attracion_mode;

    u32 test_speed_mode;

    //--------------------------------------------------------------------------

    // 0-3 screen | 4-7 border. 0-3 can use only 4 colors
    u32 palette[SGB_NUM_PALETTES][16];
    u32 snes_palette[512][4];

    //--------------------------------------------------------------------------

    u32 curr_ATF;
    u32 ATF_list[0x2D][20 * 18];

    //--------------------------------------------------------------------------

    u32 tile_data[((8 * 8 * 4) / 8) * 256];
    u32 tile_map[32 * 32];

    //--------------------------------------------------------------------------

    u8 *sgb_bank0_ram;

    //--------------------------------------------------------------------------

    u32 disable_sgb;
} _SGB_INFO_;

extern _SGB_INFO_ SGBInfo;

void SGB_Init(void);
void SGB_End(void);

int SGB_MultiplayerIsEnabled(void); // Returns 0 if disabled, mode if enabled

void SGB_WriteP1(u32 value);
u32 SGB_ReadP1(void);

#endif // GB_SGB__
