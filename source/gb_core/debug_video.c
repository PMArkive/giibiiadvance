// SPDX-License-Identifier: GPL-2.0-or-later
//
// Copyright (c) 2011-2015, 2019-2020, Antonio Niño Díaz
//
// GiiBiiAdvance - GBA/GB emulator

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../build_options.h"
#include "../font_utils.h"
#include "../general_utils.h"

#include "camera.h"
#include "cpu.h"
#include "debug_video.h"
#include "gameboy.h"
#include "general.h"
#include "memory.h"
#include "video.h"

//------------------------------------------------------------------------------

extern _GB_CONTEXT_ GameBoy;

//------------------------------------------------------------------------------

static u32 gb_pal_colors[4][3] = {
    { 255, 255, 255 }, { 168, 168, 168 }, { 80, 80, 80 }, { 0, 0, 0 }
};

//------------------------------------------------------------------------------

static u32 rgb16to32(u16 color)
{
    int r = (color & 31) << 3;
    int g = ((color >> 5) & 31) << 3;
    int b = ((color >> 10) & 31) << 3;
    return (b << 16) | (g << 8) | r;
}

//------------------------------------------------------------------------------

void GB_Debug_GetPalette(int is_sprite, int num, int color,
                         u32 *red, u32 *green, u32 *blue)
{
    _GB_MEMORY_ *mem = &GameBoy.Memory;

    if (GameBoy.Emulator.CGBEnabled)
    {
        u32 color_;

        if (is_sprite)
        {
            color_ = GameBoy.Emulator.spr_pal[(num * 8) + (color * 2)]
                     | GameBoy.Emulator.spr_pal[(num * 8) + (color * 2) + 1] << 8;
        }
        else
        {
            color_ = GameBoy.Emulator.bg_pal[(num * 8) + (color * 2)]
                     | GameBoy.Emulator.bg_pal[(num * 8) + (color * 2) + 1] << 8;
        }

        *red = (color_ & 0x1F) << 3;
        *green = ((color_ >> 5) & 0x1F) << 3;
        *blue = ((color_ >> 10) & 0x1F) << 3;
    }
    else if (GameBoy.Emulator.gbc_in_gb_mode)
    {
        if (is_sprite)
        {
            if (num == 0)
            {
                u32 obp0_reg = mem->IO_Ports[OBP0_REG - 0xFF00];
                u32 out_color = (obp0_reg >> (color * 2)) & 0x3;
                out_color = gbc_getsprpalcolor(0, out_color);
                *red = (out_color & 0x1F) << 3;
                *green = ((out_color >> 5) & 0x1F) << 3;
                *blue = ((out_color >> 10) & 0x1F) << 3;
            }
            else if (num == 1)
            {
                u32 obp1_reg = mem->IO_Ports[OBP1_REG - 0xFF00];
                u32 out_color = (obp1_reg >> (color * 2)) & 0x3;
                out_color = gbc_getsprpalcolor(1, out_color);
                *red = (out_color & 0x1F) << 3;
                *green = ((out_color >> 5) & 0x1F) << 3;
                *blue = ((out_color >> 10) & 0x1F) << 3;
            }
            else
            {
                *red = 0;
                *green = 0;
                *blue = 0;
            }
        }
        else
        {
            if (num == 0)
            {
                u32 bgp_reg = mem->IO_Ports[BGP_REG - 0xFF00];
                u32 out_color = (bgp_reg >> (color * 2)) & 0x3;
                out_color = gbc_getbgpalcolor(0, out_color);
                *red = (out_color & 0x1F) << 3;
                *green = ((out_color >> 5) & 0x1F) << 3;
                *blue = ((out_color >> 10) & 0x1F) << 3;
            }
            else
            {
                *red = 0;
                *green = 0;
                *blue = 0;
            }
        }
    }
    else
    {
        if (is_sprite)
        {
            if (num == 0)
            {
                u32 obp0_reg = mem->IO_Ports[OBP0_REG - 0xFF00];
                u32 out_color = (obp0_reg >> (color * 2)) & 0x3;
                *red = gb_pal_colors[out_color][0];
                *green = gb_pal_colors[out_color][1];
                *blue = gb_pal_colors[out_color][2];
            }
            else if (num == 1)
            {
                u32 obp1_reg = mem->IO_Ports[OBP1_REG - 0xFF00];
                u32 pal = (obp1_reg >> (color * 2)) & 0x3;
                *red = gb_pal_colors[pal][0];
                *green = gb_pal_colors[pal][1];
                *blue = gb_pal_colors[pal][2];
            }
            else
            {
                *red = 0;
                *green = 0;
                *blue = 0;
            }
        }
        else
        {
            if (num == 0)
            {
                u32 bgp_reg = mem->IO_Ports[BGP_REG - 0xFF00];
                u32 out_color = (bgp_reg >> (color * 2)) & 0x3;
                *red = gb_pal_colors[out_color][0];
                *green = gb_pal_colors[out_color][1];
                *blue = gb_pal_colors[out_color][2];
            }
            else
            {
                *red = 0;
                *green = 0;
                *blue = 0;
            }
        }
    }
}

//------------------------------------------------------------------------------

void GB_Debug_GetSpriteInfo(int sprnum, u8 *x, u8 *y, u8 *tile, u8 *info)
{
    _GB_OAM_ENTRY_ *GB_Sprite =
            &(((_GB_OAM_ *)(GameBoy.Memory.ObjAttrMem))->Sprite[sprnum]);

    *x = GB_Sprite->X;
    *y = GB_Sprite->Y;
    *tile = GB_Sprite->Tile;
    *info = GB_Sprite->Info;
}

void GB_Debug_PrintSprite(unsigned char *buf, int bufw, unused__ int bufh,
                          int buff_x, int buff_y, int sprite)
{
    _GB_MEMORY_ *mem = &GameBoy.Memory;
    s32 spriteheight = 8 << ((mem->IO_Ports[LCDC_REG - 0xFF00] & (1 << 2)) != 0);
    _GB_OAM_ENTRY_ *GB_Sprite = &(((_GB_OAM_ *)(mem->ObjAttrMem))->Sprite[sprite]);
    u32 tile = GB_Sprite->Tile & ((spriteheight == 16) ? 0xFE : 0xFF);
    u8 *spr_data = &mem->VideoRAM[tile << 4]; // Bank 0

    bool isvisible = false;

    if ((GB_Sprite->X > 0) && (GB_Sprite->X < 168))
    {
        if (spriteheight == 16)
        {
            if ((GB_Sprite->Y > 0) && (GB_Sprite->Y < 160))
                isvisible = true;
        }
        else
        {
            if ((GB_Sprite->Y > 8) && (GB_Sprite->Y < 160))
                isvisible = true;
        }
    }

    if (GameBoy.Emulator.CGBEnabled)
    {
        if (GB_Sprite->Info & (1 << 3)) // If bank 1...
            spr_data += 0x2000;

        u32 pal_index = (GB_Sprite->Info & 7) * 8;

        u32 palette[4];
        palette[0] = GameBoy.Emulator.spr_pal[pal_index]
                     | (GameBoy.Emulator.spr_pal[pal_index + 1] << 8);
        palette[1] = GameBoy.Emulator.spr_pal[pal_index + 2]
                     | (GameBoy.Emulator.spr_pal[pal_index + 3] << 8);
        palette[2] = GameBoy.Emulator.spr_pal[pal_index + 4]
                     | (GameBoy.Emulator.spr_pal[pal_index + 5] << 8);
        palette[3] = GameBoy.Emulator.spr_pal[pal_index + 6]
                     | (GameBoy.Emulator.spr_pal[pal_index + 7] << 8);

        for (int y = 0; y < spriteheight; y++)
        {
            for (int x = 0; x < 8; x++)
            {
                u8 *data = spr_data;

                if (GB_Sprite->Info & (1 << 6))
                    data += (spriteheight - y - 1) * 2; // Flip Y
                else
                    data += y * 2;

                int x_;

                if (GB_Sprite->Info & (1 << 5)) // Flip X
                    x_ = 7 - x;
                else
                    x_ = x;

                x_ = 7 - x_;

                u32 color = ((*data >> x_) & 1)
                            | ((((*(data + 1)) >> x_) << 1) & 2);

                for (int i = 0; i < 2; i++)
                {
                    for (int j = 0; j < 2; j++)
                    {
                        if (!isvisible
                            && (((((x * 2) + i) ^ ((y * 2) + j)) & 3) == 0))
                        {
                            int xx_ = buff_x + (x * 2) + i;
                            int yy_ = buff_y + (y * 2) + j;
                            buf[(yy_ * bufw + xx_) * 3 + 0] = 255;
                            buf[(yy_ * bufw + xx_) * 3 + 1] = 0;
                            buf[(yy_ * bufw + xx_) * 3 + 2] = 0;
                        }
                        else
                        {
                            if (color != 0) // Don't display transparent color
                            {
                                int xx_ = buff_x + (x * 2) + i;
                                int yy_ = buff_y + (y * 2) + j;
                                buf[(yy_ * bufw + xx_) * 3 + 0] =
                                        (palette[color] & 0x1F) << 3;
                                buf[(yy_ * bufw + xx_) * 3 + 1] =
                                        ((palette[color] >> 5) & 0x1F) << 3;
                                buf[(yy_ * bufw + xx_) * 3 + 2] =
                                        ((palette[color] >> 10) & 0x1F) << 3;
                            }
                        }
                    }
                }
            }
        }
    }
    else
    {
        u32 palette[4];
        u32 objX_reg;
        if (GB_Sprite->Info & (1 << 4))
            objX_reg = mem->IO_Ports[OBP1_REG - 0xFF00];
        else
            objX_reg = mem->IO_Ports[OBP0_REG - 0xFF00];

        palette[0] = objX_reg & 0x3;
        palette[1] = (objX_reg >> 2) & 0x3;
        palette[2] = (objX_reg >> 4) & 0x3;
        palette[3] = (objX_reg >> 6) & 0x3;

        for (int y = 0; y < spriteheight; y++)
        {
            for (int x = 0; x < 8; x++)
            {
                u8 *data = spr_data;

                if (GB_Sprite->Info & (1 << 6))
                    data += (spriteheight - y - 1) * 2; // Flip Y
                else
                    data += y * 2;

                int x_;

                if (GB_Sprite->Info & (1 << 5))
                    x_ = 7 - x; // Flip X
                else
                    x_ = x;

                x_ = 7 - x_;

                u32 color =
                        ((*data >> x_) & 1) | ((((*(data + 1)) >> x_) << 1) & 2);

                for (int i = 0; i < 2; i++)
                {
                    for (int j = 0; j < 2; j++)
                    {
                        if (!isvisible
                            && (((((x * 2) + i) ^ ((y * 2) + j)) & 3) == 0))
                        {
                            int xx_ = buff_x + (x * 2) + i;
                            int yy_ = buff_y + (y * 2) + j;
                            buf[(yy_ * bufw + xx_) * 3 + 0] = 255;
                            buf[(yy_ * bufw + xx_) * 3 + 1] = 0;
                            buf[(yy_ * bufw + xx_) * 3 + 2] = 0;
                        }
                        else
                        {
                            if (color != 0) // Don't display transparent color
                            {
                                int xx_ = buff_x + (x * 2) + i;
                                int yy_ = buff_y + (y * 2) + j;
                                buf[(yy_ * bufw + xx_) * 3 + 0] =
                                        gb_pal_colors[palette[color]][0];
                                buf[(yy_ * bufw + xx_) * 3 + 1] =
                                        gb_pal_colors[palette[color]][1];
                                buf[(yy_ * bufw + xx_) * 3 + 2] =
                                        gb_pal_colors[palette[color]][2];
                            }
                        }
                    }
                }
            }
        }
    }
}

void GB_Debug_PrintSprites(unsigned char *buf)
{
    for (int y = 0; y < 256; y++)
    {
        for (int x = 0; x < 256; x++)
        {
            buf[(y * 256 + x) * 3 + 0] = ((x ^ y) & 4) ? 192 : 128;
            buf[(y * 256 + x) * 3 + 1] = ((x ^ y) & 4) ? 192 : 128;
            buf[(y * 256 + x) * 3 + 2] = ((x ^ y) & 4) ? 192 : 128;
        }
    }

    u32 i = 0;

    for (int y = 0; y < 5; y++)
    {
        for (int x = 0; x < 8; x++)
        {
            GB_Debug_PrintSprite(buf, 256, 256,
                                 8 + (x * 32), 10 + (y * 52), i++);
        }
    }
}

void GB_Debug_PrintZoomedSprite(unsigned char *buf, int sprite)
{
    _GB_MEMORY_ *mem = &GameBoy.Memory;
    s32 spriteheight = 8 << ((mem->IO_Ports[LCDC_REG - 0xFF00] & (1 << 2)) != 0);

    _GB_OAM_ENTRY_ *GB_Sprite;
    GB_Sprite = &(((_GB_OAM_ *)(mem->ObjAttrMem))->Sprite[sprite]);

    u32 tile = GB_Sprite->Tile & ((spriteheight == 16) ? 0xFE : 0xFF);
    u8 *spr_data = &mem->VideoRAM[tile << 4]; // Bank 0

    for (int y = 0; y < 16 * 8; y++)
    {
        for (int x = 0; x < 8 * 8; x++)
        {
            buf[(y * (8 * 8) + x) * 3 + 0] = ((x ^ y) & 4) ? 192 : 128;
            buf[(y * (8 * 8) + x) * 3 + 1] = ((x ^ y) & 4) ? 192 : 128;
            buf[(y * (8 * 8) + x) * 3 + 2] = ((x ^ y) & 4) ? 192 : 128;
        }
    }

    if (GameBoy.Emulator.CGBEnabled)
    {
        if (GB_Sprite->Info & (1 << 3)) // If bank 1...
            spr_data += 0x2000;

        u32 pal_index = (GB_Sprite->Info & 7) * 8;

        u32 palette[4];
        palette[0] = GameBoy.Emulator.spr_pal[pal_index]
                     | (GameBoy.Emulator.spr_pal[pal_index + 1] << 8);
        palette[1] = GameBoy.Emulator.spr_pal[pal_index + 2]
                     | (GameBoy.Emulator.spr_pal[pal_index + 3] << 8);
        palette[2] = GameBoy.Emulator.spr_pal[pal_index + 4]
                     | (GameBoy.Emulator.spr_pal[pal_index + 5] << 8);
        palette[3] = GameBoy.Emulator.spr_pal[pal_index + 6]
                     | (GameBoy.Emulator.spr_pal[pal_index + 7] << 8);

        for (int y = 0; y < spriteheight; y++)
        {
            for (int x = 0; x < 8; x++)
            {
                u8 *data = spr_data;

                if (GB_Sprite->Info & (1 << 6)) // Flip Y
                    data += (spriteheight - y - 1) * 2;
                else
                    data += y * 2;

                int x_;

                if (GB_Sprite->Info & (1 << 5)) // Flip X
                    x_ = 7 - x;
                else
                    x_ = x;

                x_ = 7 - x_;

                u32 color =
                        ((*data >> x_) & 1) | ((((*(data + 1)) >> x_) << 1) & 2);

                for (int i = 0; i < 8; i++)
                {
                    for (int j = 0; j < 8; j++)
                    {
                        if (color != 0) // Don't display transparent color
                        {
                            int xx_ = (x * 8) + i;
                            int yy_ = (y * 8) + j;
                            buf[(yy_ * (8 * 8) + xx_) * 3 + 0] =
                                    (palette[color] & 0x1F) << 3;
                            buf[(yy_ * (8 * 8) + xx_) * 3 + 1] =
                                    ((palette[color] >> 5) & 0x1F) << 3;
                            buf[(yy_ * (8 * 8) + xx_) * 3 + 2] =
                                    ((palette[color] >> 10) & 0x1F) << 3;
                        }
                    }
                }
            }
        }
    }
    else
    {
        u32 palette[4];
        u32 objX_reg;
        if (GB_Sprite->Info & (1 << 4))
            objX_reg = mem->IO_Ports[OBP1_REG - 0xFF00];
        else
            objX_reg = mem->IO_Ports[OBP0_REG - 0xFF00];

        palette[0] = objX_reg & 0x3;
        palette[1] = (objX_reg >> 2) & 0x3;
        palette[2] = (objX_reg >> 4) & 0x3;
        palette[3] = (objX_reg >> 6) & 0x3;

        for (int y = 0; y < spriteheight; y++)
        {
            for (int x = 0; x < 8; x++)
            {
                u8 *data = spr_data;

                if (GB_Sprite->Info & (1 << 6)) // Flip Y
                    data += (spriteheight - y - 1) * 2;
                else
                    data += y * 2;

                int x_;

                if (GB_Sprite->Info & (1 << 5)) // Flip X
                    x_ = 7 - x;
                else
                    x_ = x;

                x_ = 7 - x_;

                u32 color = ((*data >> x_) & 1)
                            | ((((*(data + 1)) >> x_) << 1) & 2);

                for (int i = 0; i < 8; i++)
                {
                    for (int j = 0; j < 8; j++)
                    {
                        if (color != 0) // Don't display transparent color
                        {
                            int xx_ = (x * 8) + i;
                            int yy_ = (y * 8) + j;
                            buf[(yy_ * (8 * 8) + xx_) * 3 + 0] =
                                    gb_pal_colors[palette[color]][0];
                            buf[(yy_ * (8 * 8) + xx_) * 3 + 1] =
                                    gb_pal_colors[palette[color]][1];
                            buf[(yy_ * (8 * 8) + xx_) * 3 + 2] =
                                    gb_pal_colors[palette[color]][2];
                        }
                    }
                }
            }
        }
    }
}

//------------------------------------------------------------------------------

static void _gb_debug_print_sprite_alpha_at_position(unsigned char *buf,
                                                     int bufw, unused__ int bufh,
                                                     int buff_x, int buff_y,
                                                     int sprite)
{
    _GB_MEMORY_ *mem = &GameBoy.Memory;
    s32 spriteheight =
            8 << ((mem->IO_Ports[LCDC_REG - 0xFF00] & (1 << 2)) != 0);

    _GB_OAM_ENTRY_ *GB_Sprite;
    GB_Sprite = &(((_GB_OAM_ *)(mem->ObjAttrMem))->Sprite[sprite]);

    u32 tile = GB_Sprite->Tile & ((spriteheight == 16) ? 0xFE : 0xFF);
    u8 *spr_data = &mem->VideoRAM[tile << 4]; // Bank 0

    if (GameBoy.Emulator.CGBEnabled)
    {
        if (GB_Sprite->Info & (1 << 3)) // If bank 1...
            spr_data += 0x2000;

        u32 pal_index = (GB_Sprite->Info & 7) * 8;

        u32 palette[4];
        palette[0] = GameBoy.Emulator.spr_pal[pal_index]
                     | (GameBoy.Emulator.spr_pal[pal_index + 1] << 8);
        palette[1] = GameBoy.Emulator.spr_pal[pal_index + 2]
                     | (GameBoy.Emulator.spr_pal[pal_index + 3] << 8);
        palette[2] = GameBoy.Emulator.spr_pal[pal_index + 4]
                     | (GameBoy.Emulator.spr_pal[pal_index + 5] << 8);
        palette[3] = GameBoy.Emulator.spr_pal[pal_index + 6]
                     | (GameBoy.Emulator.spr_pal[pal_index + 7] << 8);

        for (int y = 0; y < spriteheight; y++)
        {
            for (int x = 0; x < 8; x++)
            {
                u8 *data = spr_data;

                if (GB_Sprite->Info & (1 << 6)) // Flip Y
                    data += (spriteheight - y - 1) * 2;
                else
                    data += y * 2;

                int x_;

                if (GB_Sprite->Info & (1 << 5)) // Flip X
                    x_ = 7 - x;
                else
                    x_ = x;

                x_ = 7 - x_;

                u32 color =
                        ((*data >> x_) & 1) | ((((*(data + 1)) >> x_) << 1) & 2);

                for (int i = 0; i < 2; i++)
                {
                    for (int j = 0; j < 2; j++)
                    {
                        int xx_ = buff_x + (x * 2) + i;
                        int yy_ = buff_y + (y * 2) + j;

                        if (color != 0) // Don't display transparent color
                        {
                            buf[(yy_ * bufw + xx_) * 4 + 0] =
                                    (palette[color] & 0x1F) << 3;
                            buf[(yy_ * bufw + xx_) * 4 + 1] =
                                    ((palette[color] >> 5) & 0x1F) << 3;
                            buf[(yy_ * bufw + xx_) * 4 + 2] =
                                    ((palette[color] >> 10) & 0x1F) << 3;
                            buf[(yy_ * bufw + xx_) * 4 + 3] = 255;
                        }
                        else
                        {
                            buf[(yy_ * bufw + xx_) * 4 + 0] = 0;
                            buf[(yy_ * bufw + xx_) * 4 + 1] = 0;
                            buf[(yy_ * bufw + xx_) * 4 + 2] = 0;
                            buf[(yy_ * bufw + xx_) * 4 + 3] = 0;
                        }
                    }
                }
            }
        }
    }
    else
    {
        u32 palette[4];
        u32 objX_reg;

        if (GB_Sprite->Info & (1 << 4))
            objX_reg = mem->IO_Ports[OBP1_REG - 0xFF00];
        else
            objX_reg = mem->IO_Ports[OBP0_REG - 0xFF00];

        palette[0] = objX_reg & 0x3;
        palette[1] = (objX_reg >> 2) & 0x3;
        palette[2] = (objX_reg >> 4) & 0x3;
        palette[3] = (objX_reg >> 6) & 0x3;

        for (int y = 0; y < spriteheight; y++)
        {
            for (int x = 0; x < 8; x++)
            {
                u8 *data = spr_data;

                if (GB_Sprite->Info & (1 << 6)) // Flip Y
                    data += (spriteheight - y - 1) * 2;
                else
                    data += y * 2;

                int x_;

                if (GB_Sprite->Info & (1 << 5)) // Flip X
                    x_ = 7 - x;
                else
                    x_ = x;

                x_ = 7 - x_;

                u32 color =
                        ((*data >> x_) & 1) | ((((*(data + 1)) >> x_) << 1) & 2);

                for (int i = 0; i < 2; i++)
                {
                    for (int j = 0; j < 2; j++)
                    {
                        int xx_ = buff_x + (x * 2) + i;
                        int yy_ = buff_y + (y * 2) + j;

                        if (color != 0) // Don't display transparent color
                        {
                            buf[(yy_ * bufw + xx_) * 4 + 0] =
                                    gb_pal_colors[palette[color]][0];
                            buf[(yy_ * bufw + xx_) * 4 + 1] =
                                    gb_pal_colors[palette[color]][1];
                            buf[(yy_ * bufw + xx_) * 4 + 2] =
                                    gb_pal_colors[palette[color]][2];
                            buf[(yy_ * bufw + xx_) * 4 + 3] = 255;
                        }
                        else
                        {
                            buf[(yy_ * bufw + xx_) * 4 + 0] = 0;
                            buf[(yy_ * bufw + xx_) * 4 + 1] = 0;
                            buf[(yy_ * bufw + xx_) * 4 + 2] = 0;
                            buf[(yy_ * bufw + xx_) * 4 + 3] = 0;
                        }
                    }
                }
            }
        }
    }
}

void GB_Debug_PrintSpritesAlpha(unsigned char *buf)
{
    memset(buf, 0, 256 * 256 * 4);

    u32 i = 0;
    for (int y = 0; y < 5; y++)
    {
        for (int x = 0; x < 8; x++)
        {
            _gb_debug_print_sprite_alpha_at_position(buf, 256, 256,
                    8 + (x * 32), 10 + (y * 52), i++);
        }
    }
}

void GB_Debug_PrintSpriteAlpha(unsigned char *buf, int sprite)
{
    _GB_MEMORY_ *mem = &GameBoy.Memory;
    s32 spriteheight =
            8 << ((mem->IO_Ports[LCDC_REG - 0xFF00] & (1 << 2)) != 0);

    _GB_OAM_ENTRY_ *GB_Sprite;
    GB_Sprite = &(((_GB_OAM_ *)(mem->ObjAttrMem))->Sprite[sprite]);

    u32 tile = GB_Sprite->Tile & ((spriteheight == 16) ? 0xFE : 0xFF);
    u8 *spr_data = &mem->VideoRAM[tile << 4]; // Bank 0

    if (GameBoy.Emulator.CGBEnabled)
    {
        if (GB_Sprite->Info & (1 << 3)) // If bank 1...
            spr_data += 0x2000;

        u32 pal_index = (GB_Sprite->Info & 7) * 8;

        u32 palette[4];
        palette[0] = GameBoy.Emulator.spr_pal[pal_index]
                     | (GameBoy.Emulator.spr_pal[pal_index + 1] << 8);
        palette[1] = GameBoy.Emulator.spr_pal[pal_index + 2]
                     | (GameBoy.Emulator.spr_pal[pal_index + 3] << 8);
        palette[2] = GameBoy.Emulator.spr_pal[pal_index + 4]
                     | (GameBoy.Emulator.spr_pal[pal_index + 5] << 8);
        palette[3] = GameBoy.Emulator.spr_pal[pal_index + 6]
                     | (GameBoy.Emulator.spr_pal[pal_index + 7] << 8);

        for (int y = 0; y < spriteheight; y++)
        {
            for (int x = 0; x < 8; x++)
            {
                u8 *data = spr_data;

                if (GB_Sprite->Info & (1 << 6)) // Flip Y
                    data += (spriteheight - y - 1) * 2;
                else
                    data += y * 2;

                int x_;

                if (GB_Sprite->Info & (1 << 5)) // Flip X
                    x_ = 7 - x;
                else
                    x_ = x;

                x_ = 7 - x_;

                u32 color = ((*data >> x_) & 1)
                            | ((((*(data + 1)) >> x_) << 1) & 2);

                if (color != 0) // Don't display transparent color
                {
                    buf[(y * 8 + x) * 4 + 0] =
                            (palette[color] & 0x1F) << 3;
                    buf[(y * 8 + x) * 4 + 1] =
                            ((palette[color] >> 5) & 0x1F) << 3;
                    buf[(y * 8 + x) * 4 + 2] =
                            ((palette[color] >> 10) & 0x1F) << 3;
                    buf[(y * 8 + x) * 4 + 3] = 255;
                }
                else
                {
                    buf[(y * 8 + x) * 4 + 0] = 0;
                    buf[(y * 8 + x) * 4 + 1] = 0;
                    buf[(y * 8 + x) * 4 + 2] = 0;
                    buf[(y * 8 + x) * 4 + 3] = 0;
                }
            }
        }
    }
    else
    {
        u32 palette[4];
        u32 objX_reg;
        if (GB_Sprite->Info & (1 << 4))
            objX_reg = mem->IO_Ports[OBP1_REG - 0xFF00];
        else
            objX_reg = mem->IO_Ports[OBP0_REG - 0xFF00];

        palette[0] = objX_reg & 0x3;
        palette[1] = (objX_reg >> 2) & 0x3;
        palette[2] = (objX_reg >> 4) & 0x3;
        palette[3] = (objX_reg >> 6) & 0x3;

        for (int y = 0; y < spriteheight; y++)
        {
            for (int x = 0; x < 8; x++)
            {
                u8 *data = spr_data;

                if (GB_Sprite->Info & (1 << 6)) // Flip Y
                    data += (spriteheight - y - 1) * 2;
                else
                    data += y * 2;

                int x_;

                if (GB_Sprite->Info & (1 << 5)) // Flip X
                    x_ = 7 - x;
                else
                    x_ = x;

                x_ = 7 - x_;

                u32 color =
                        ((*data >> x_) & 1) | ((((*(data + 1)) >> x_) << 1) & 2);

                if (color != 0) // Don't display transparent color
                {
                    buf[(y * 8 + x) * 4 + 0] = gb_pal_colors[palette[color]][0];
                    buf[(y * 8 + x) * 4 + 1] = gb_pal_colors[palette[color]][1];
                    buf[(y * 8 + x) * 4 + 2] = gb_pal_colors[palette[color]][2];
                    buf[(y * 8 + x) * 4 + 3] = 255;
                }
                else
                {
                    buf[(y * 8 + x) * 4 + 0] = 0;
                    buf[(y * 8 + x) * 4 + 1] = 0;
                    buf[(y * 8 + x) * 4 + 2] = 0;
                    buf[(y * 8 + x) * 4 + 3] = 0;
                }
            }
        }
    }
}

//------------------------------------------------------------------------------

void GB_Debug_TileVRAMDraw(unsigned char *buffer0, int bufw0, unused__ int bufh0,
                           unsigned char *buffer1, int bufw1, unused__ int bufh1)
{
    _GB_MEMORY_ *mem = &GameBoy.Memory;

    for (int y = 0; y < 192; y++)
    {
        for (int x = 0; x < 128; x++)
        {
            u32 tile = (x >> 3) + ((y >> 3) * 16);

            u8 *data = &mem->VideoRAM[tile << 4]; // Bank 0

            data += ((y & 7) * 2);

            u32 x_ = 7 - (x & 7);

            u32 color =
                    ((*data >> x_) & 1) | ((((*(data + 1)) >> x_) << 1) & 2);

            buffer0[(y * bufw0 + x) * 3 + 0] = gb_pal_colors[color][0];
            buffer0[(y * bufw0 + x) * 3 + 1] = gb_pal_colors[color][1];
            buffer0[(y * bufw0 + x) * 3 + 2] = gb_pal_colors[color][2];
        }
    }

    for (int y = 0; y < 192; y++)
    {
        for (int x = 0; x < 128; x++)
        {
            u32 tile = (x >> 3) + ((y >> 3) * 16);

            u8 *data = &mem->VideoRAM[tile << 4];
            data += 0x2000; // Bank 1;

            data += (y & 7) * 2;

            u32 x_ = 7 - (x & 7);

            u32 color =
                    ((*data >> x_) & 1) | ((((*(data + 1)) >> x_) << 1) & 2);

            buffer1[(y * bufw1 + x) * 3 + 0] = gb_pal_colors[color][0];
            buffer1[(y * bufw1 + x) * 3 + 1] = gb_pal_colors[color][1];
            buffer1[(y * bufw1 + x) * 3 + 2] = gb_pal_colors[color][2];
        }
    }
}

void GB_Debug_TileVRAMDrawPaletted(unsigned char *buffer0,
                                   unused__ int bufw0, unused__ int bufh0,
                                   unsigned char *buffer1,
                                   unused__ int bufw1, unused__ int bufh1,
                                   int pal, int pal_is_spr)
{
    _GB_MEMORY_ *mem = &GameBoy.Memory;

    u32 bg_pal[4];
    u32 bgp_reg = mem->IO_Ports[BGP_REG - 0xFF00];
    bg_pal[0] = gb_pal_colors[bgp_reg & 0x3][0];
    bg_pal[1] = gb_pal_colors[(bgp_reg >> 2) & 0x3][0];
    bg_pal[2] = gb_pal_colors[(bgp_reg >> 4) & 0x3][0];
    bg_pal[3] = gb_pal_colors[(bgp_reg >> 6) & 0x3][0];

    for (int y = 0; y < 192; y++)
    {
        for (int x = 0; x < 128; x++)
        {
            u32 tile = (x >> 3) + ((y >> 3) * 16);

            u8 *data = &mem->VideoRAM[tile << 4]; // Bank 0

            data += (y & 7) * 2;

            u32 x_ = 7 - (x & 7);

            u32 color = ((*data >> x_) & 1)
                        | ((((*(data + 1)) >> x_) << 1) & 2);

            if (GameBoy.Emulator.CGBEnabled)
            {
                u32 pal_index = (pal * 8) + (2 * color);

                if (pal_is_spr)
                {
                    color = GameBoy.Emulator.spr_pal[pal_index]
                            | (GameBoy.Emulator.spr_pal[pal_index + 1] << 8);
                }
                else
                {
                    color = GameBoy.Emulator.bg_pal[pal_index]
                            | (GameBoy.Emulator.bg_pal[pal_index + 1] << 8);
                }
                color = rgb16to32(color);
            }
            else
            {
                color = bg_pal[color]
                        | (bg_pal[color] << 8)
                        | (bg_pal[color] << 16);
            }

            buffer0[(y * bufw0 + x) * 3 + 0] = color & 0xFF;
            buffer0[(y * bufw0 + x) * 3 + 1] = (color >> 8) & 0xFF;
            buffer0[(y * bufw0 + x) * 3 + 2] = (color >> 16) & 0xFF;
        }
    }

    for (int y = 0; y < 192; y++)
    {
        for (int x = 0; x < 128; x++)
        {
            u32 tile = (x >> 3) + ((y >> 3) * 16);

            u8 *data = &mem->VideoRAM[tile << 4];
            data += 0x2000; // Bank 1

            data += (y & 7) * 2;

            u32 x_ = 7 - (x & 7);

            u32 color = ((*data >> x_) & 1)
                        | ((((*(data + 1)) >> x_) << 1) & 2);

            if (GameBoy.Emulator.CGBEnabled)
            {
                u32 pal_index = (pal * 8) + (2 * color);

                if (pal_is_spr)
                {
                    color = GameBoy.Emulator.spr_pal[pal_index]
                            | (GameBoy.Emulator.spr_pal[pal_index + 1] << 8);
                }
                else
                {
                    color = GameBoy.Emulator.bg_pal[pal_index]
                            | (GameBoy.Emulator.bg_pal[pal_index + 1] << 8);
                }
                color = rgb16to32(color);
            }
            else
            {
                color = bg_pal[color]
                        | (bg_pal[color] << 8)
                        | (bg_pal[color] << 16);
            }

            buffer1[(y * bufw0 + x) * 3 + 0] = color & 0xFF;
            buffer1[(y * bufw0 + x) * 3 + 1] = (color >> 8) & 0xFF;
            buffer1[(y * bufw0 + x) * 3 + 2] = (color >> 16) & 0xFF;
        }
    }
}

void GB_Debug_TileDrawZoomed64x64(unsigned char *buffer, int tile, int bank)
{
    int tiletempbuffer[8 * 8];

    u8 *tile_data = &GameBoy.Memory.VideoRAM[tile << 4]; // Bank 0
    if (bank)
        tile_data += 0x2000; // Bank 1

    for (int y = 0; y < 8; y++)
    {
        for (int x = 0; x < 8; x++)
        {
            u8 *data = tile_data + ((y & 7) * 2);
            u32 x_ = 7 - (x & 7);
            u32 color =
                    ((*data >> x_) & 1) | ((((*(data + 1)) >> x_) << 1) & 2);

            tiletempbuffer[x + y * 8] = (gb_pal_colors[color][0] << 16)
                                        | (gb_pal_colors[color][1] << 8)
                                        | gb_pal_colors[color][2];
        }
    }

    // Expand to 64x64
    for (int i = 0; i < 64; i++)
    {
        for (int j = 0; j < 64; j++)
        {
            buffer[(j * 64 + i) * 3 + 0] =
                    tiletempbuffer[(j / 8) * 8 + (i / 8)] & 0xFF;
            buffer[(j * 64 + i) * 3 + 1] =
                    (tiletempbuffer[(j / 8) * 8 + (i / 8)] >> 8) & 0xFF;
            buffer[(j * 64 + i) * 3 + 2] =
                    (tiletempbuffer[(j / 8) * 8 + (i / 8)] >> 16) & 0xFF;
        }
    }
}

void GB_Debug_TileDrawZoomedPaletted64x64(unsigned char *buffer, int tile,
                                          int bank, int palette,
                                          int is_sprite_palette)
{
    _GB_MEMORY_ *mem = &GameBoy.Memory;

    int tiletempbuffer[8 * 8];

    u8 *tile_data = &GameBoy.Memory.VideoRAM[tile << 4]; // Bank 0
    if (bank)
        tile_data += 0x2000; // Bank 1

    u32 bg_pal[4];
    u32 bgp_reg = mem->IO_Ports[BGP_REG - 0xFF00];
    bg_pal[0] = gb_pal_colors[bgp_reg & 0x3][0];
    bg_pal[1] = gb_pal_colors[(bgp_reg >> 2) & 0x3][0];
    bg_pal[2] = gb_pal_colors[(bgp_reg >> 4) & 0x3][0];
    bg_pal[3] = gb_pal_colors[(bgp_reg >> 6) & 0x3][0];

    for (int y = 0; y < 8; y++)
    {
        for (int x = 0; x < 8; x++)
        {
            u8 *data = tile_data + ((y & 7) * 2);
            u32 x_ = 7 - (x & 7);
            u32 color =
                    ((*data >> x_) & 1) | ((((*(data + 1)) >> x_) << 1) & 2);

            if (GameBoy.Emulator.CGBEnabled)
            {
                u32 pal_index = (palette * 8) + (2 * color);

                if (is_sprite_palette)
                {
                    color = GameBoy.Emulator.spr_pal[pal_index]
                            | (GameBoy.Emulator.spr_pal[pal_index + 1] << 8);
                }
                else
                {
                    color = GameBoy.Emulator.bg_pal[pal_index]
                            | (GameBoy.Emulator.bg_pal[pal_index + 1] << 8);
                }

                tiletempbuffer[x + y * 8] = rgb16to32(color);
            }
            else
            {
                tiletempbuffer[x + y * 8] = bg_pal[color] | (bg_pal[color] << 8)
                                            | (bg_pal[color] << 16);
            }
        }
    }

    // Expand to 64x64
    for (int i = 0; i < 64; i++)
    {
        for (int j = 0; j < 64; j++)
        {
            buffer[(j * 64 + i) * 3 + 0] =
                    tiletempbuffer[(j / 8) * 8 + (i / 8)] & 0xFF;
            buffer[(j * 64 + i) * 3 + 1] =
                    (tiletempbuffer[(j / 8) * 8 + (i / 8)] >> 8) & 0xFF;
            buffer[(j * 64 + i) * 3 + 2] =
                    (tiletempbuffer[(j / 8) * 8 + (i / 8)] >> 16) & 0xFF;
        }
    }
}

//------------------------------------------------------------------------------

void GB_Debug_MapPrint(unsigned char *buffer, int bufw, unused__ int bufh,
                       int map, int tile_base)
{
    _GB_MEMORY_ *mem = &GameBoy.Memory;

    u8 *tiledata = tile_base ? &mem->VideoRAM[0x0800] : &mem->VideoRAM[0x0000];
    u8 *bgtilemap = map ? &mem->VideoRAM[0x1C00] : &mem->VideoRAM[0x1800];

    if (GameBoy.Emulator.CGBEnabled)
    {
        for (int y = 0; y < 256; y++)
        {
            for (int x = 0; x < 256; x++)
            {
                u32 tile_location = (((y >> 3) & 31) * 32) + ((x >> 3) & 31);
                u32 tile = bgtilemap[tile_location];
                u32 tileinfo = bgtilemap[tile_location + 0x2000];

                if (tile_base) // If tile base is 0x8800
                {
                    if (tile & (1 << 7))
                        tile &= 0x7F;
                    else
                        tile += 128;
                }

                u8 *data = &tiledata[(tile << 4)
                         + ((tileinfo & (1 << 3)) ? 0x2000 : 0)]; // Bank 1?

                // V flip
                if (tileinfo & (1 << 6))
                    data += (((7 - y) & 7) * 2);
                else
                    data += ((y & 7) * 2);

                u32 x_;

                // H flip
                if (tileinfo & (1 << 5))
                    x_ = (x & 7);
                else
                    x_ = 7 - (x & 7);

                u32 color =
                        ((*data >> x_) & 1) | ((((*(data + 1)) >> x_) << 1) & 2);
                u32 pal_index = ((tileinfo & 7) * 8) + (2 * color);

                color = GameBoy.Emulator.bg_pal[pal_index]
                        | (GameBoy.Emulator.bg_pal[pal_index + 1] << 8);

                buffer[(y * bufw + x) * 3 + 0] = (color & 0x1F) << 3;
                buffer[(y * bufw + x) * 3 + 1] = ((color >> 5) & 0x1F) << 3;
                buffer[(y * bufw + x) * 3 + 2] = ((color >> 10) & 0x1F) << 3;
            }
        }
    }
    else
    {
        u32 bg_pal[4];
        u32 bgp_reg = mem->IO_Ports[BGP_REG - 0xFF00];
        bg_pal[0] = gb_pal_colors[bgp_reg & 0x3][0];
        bg_pal[1] = gb_pal_colors[(bgp_reg >> 2) & 0x3][0];
        bg_pal[2] = gb_pal_colors[(bgp_reg >> 4) & 0x3][0];
        bg_pal[3] = gb_pal_colors[(bgp_reg >> 6) & 0x3][0];

        for (int y = 0; y < 256; y++)
        {
            for (int x = 0; x < 256; x++)
            {
                u32 tile = bgtilemap[(((y >> 3) & 31) * 32) + ((x >> 3) & 31)];

                if (tile_base) // If tile base is 0x8800
                {
                    if (tile & (1 << 7))
                        tile &= 0x7F;
                    else
                        tile += 128;
                }

                u8 *data = (&tiledata[tile << 4]) + ((y & 7) << 1);

                u32 x_ = 7 - (x & 7);

                u32 color = ((*data >> x_) & 1)
                            | ((((*(data + 1)) >> x_) << 1) & 2);

                buffer[(y * bufw + x) * 3 + 0] = bg_pal[color];
                buffer[(y * bufw + x) * 3 + 1] = bg_pal[color];
                buffer[(y * bufw + x) * 3 + 2] = bg_pal[color];
            }
        }
    }
}

void GB_Debug_MapPrintBW(unsigned char *buffer, int bufw, unused__ int bufh,
                         int map, int tile_base)
{
    _GB_MEMORY_ *mem = &GameBoy.Memory;

    u8 *tiledata = tile_base ? &mem->VideoRAM[0x0800] : &mem->VideoRAM[0x0000];
    u8 *bgtilemap = map ? &mem->VideoRAM[0x1C00] : &mem->VideoRAM[0x1800];

    if (GameBoy.Emulator.CGBEnabled)
    {
        for (int y = 0; y < 256; y++)
        {
            for (int x = 0; x < 256; x++)
            {
                u32 tile_location = (((y >> 3) & 31) * 32) + ((x >> 3) & 31);
                u32 tile = bgtilemap[tile_location];
                u32 tileinfo = bgtilemap[tile_location + 0x2000];

                if (tile_base) // If tile base is 0x8800
                {
                    if (tile & (1 << 7))
                        tile &= 0x7F;
                    else
                        tile += 128;
                }

                u8 *data = &tiledata[(tile << 4)
                         + ((tileinfo & (1 << 3)) ? 0x2000 : 0)]; // Bank 1?

                // V flip
                if (tileinfo & (1 << 6))
                    data += (((7 - y) & 7) * 2);
                else
                    data += ((y & 7) * 2);

                u32 x_;

                // H flip
                if (tileinfo & (1 << 5))
                    x_ = x & 7;
                else
                    x_ = 7 - (x & 7);

                u32 color = ((*data >> x_) & 1)
                            | ((((*(data + 1)) >> x_) << 1) & 2);

                buffer[(y * bufw + x) * 3 + 0] = gb_pal_colors[color][0];
                buffer[(y * bufw + x) * 3 + 1] = gb_pal_colors[color][1];
                buffer[(y * bufw + x) * 3 + 2] = gb_pal_colors[color][2];
            }
        }
    }
    else
    {
        for (int y = 0; y < 256; y++)
        {
            for (int x = 0; x < 256; x++)
            {
                u32 tile = bgtilemap[(((y >> 3) & 31) * 32) + ((x >> 3) & 31)];

                if (tile_base) // If tile base is 0x8800
                {
                    if (tile & (1 << 7))
                        tile &= 0x7F;
                    else
                        tile += 128;
                }

                u8 *data = (&tiledata[tile << 4]) + ((y & 7) << 1);

                u32 x_ = 7 - (x & 7);

                u32 color = ((*data >> x_) & 1)
                            | ((((*(data + 1)) >> x_) << 1) & 2);

                buffer[(y * bufw + x) * 3 + 0] = gb_pal_colors[color][0];
                buffer[(y * bufw + x) * 3 + 1] = gb_pal_colors[color][1];
                buffer[(y * bufw + x) * 3 + 2] = gb_pal_colors[color][2];
            }
        }
    }
}

//------------------------------------------------------------------------------

void GB_Debug_GBCameraMiniPhotoPrint(unsigned char *buffer,
                                     int bufw, unused__ int bufh,
                                     int posx, int posy, int index)
{
    int ramaddr = index * 0x1000 + 0x2E00;
    int bank = (ramaddr & 0x1E000) >> 13;
    int bankaddr = ramaddr & 0x1FFF;

    _GB_MEMORY_ *mem = &GameBoy.Memory;

    for (int y = 0; y < 4 * 8; y++)
    {
        for (int x = 0; x < 4 * 8; x++)
        {
            int basetileaddr = bankaddr + (((y >> 3) * 4 + (x >> 3)) * 16);

            int baselineaddr = basetileaddr + ((y & 7) << 1);

            u8 data = mem->ExternRAM[bank][baselineaddr];
            u8 data2 = mem->ExternRAM[bank][baselineaddr + 1];

            u32 x_ = 7 - (x & 7);

            u32 color = ((data >> x_) & 1) | (((data2 >> x_) << 1) & 2);

            int bufindex = ((y + posy) * bufw + (x + posx)) * 3;
            buffer[bufindex + 0] = gb_pal_colors[color][0];
            buffer[bufindex + 1] = gb_pal_colors[color][0];
            buffer[bufindex + 2] = gb_pal_colors[color][0];
        }
    }
}

void GB_Debug_GBCameraPhotoPrint(unsigned char *buffer,
                                 int bufw, unused__ int bufh, int index)
{
    int ramaddr = index * 0x1000 + 0x2000;
    if (index == -1)
        ramaddr = 0x0100;

    int bank = (ramaddr & 0x1E000) >> 13;
    int bankaddr = ramaddr & 0x1FFF;

    _GB_MEMORY_ *mem = &GameBoy.Memory;

    for (int y = 0; y < 14 * 8; y++)
    {
        for (int x = 0; x < 16 * 8; x++)
        {
            int basetileaddr = bankaddr + (((y >> 3) * 16 + (x >> 3)) * 16);

            int baselineaddr = basetileaddr + ((y & 7) << 1);

            u8 data = mem->ExternRAM[bank][baselineaddr];
            u8 data2 = mem->ExternRAM[bank][baselineaddr + 1];

            u32 x_ = 7 - (x & 7);

            u32 color = ((data >> x_) & 1) | (((data2 >> x_) << 1) & 2);

            int bufindex = (y * bufw + x) * 3;
            buffer[bufindex + 0] = gb_pal_colors[color][0];
            buffer[bufindex + 1] = gb_pal_colors[color][0];
            buffer[bufindex + 2] = gb_pal_colors[color][0];
        }
    }
}

void GB_Debug_GBCameraMiniPhotoPrintAll(unsigned char *buf)
{
    for (int y = 0; y < 208; y++)
    {
        for (int x = 0; x < 248; x++)
        {
            buf[(y * 248 + x) * 3 + 0] = ((x ^ y) & 4) ? 192 : 128;
            buf[(y * 248 + x) * 3 + 1] = ((x ^ y) & 4) ? 192 : 128;
            buf[(y * 248 + x) * 3 + 2] = ((x ^ y) & 4) ? 192 : 128;
        }
    }

    for (int i = 0; i < 30; i++)
    {
        int x_ = (i % 6) * (32 + 8) + 8;
        int y_ = (i / 6) * (32 + 8) + 8;
        GB_Debug_GBCameraMiniPhotoPrint(buf, 248, 208, x_, y_, i);
    }
}

void GB_Debug_GBCameraWebcamOutputPrint(unsigned char *buffer, int bufw,
                                        unused__ int bufh)
{
    for (int y = 0; y < 14 * 8; y++)
    {
        for (int x = 0; x < 16 * 8; x++)
        {
            int color = GB_CameraWebcamImageGetPixel(x, y);
            int bufindex = (y * bufw + x) * 3;
            buffer[bufindex + 0] = color;
            buffer[bufindex + 1] = color;
            buffer[bufindex + 2] = color;
        }
    }
}

void GB_Debug_GBCameraRetinaProcessedPrint(unsigned char *buffer, int bufw,
                                           unused__ int bufh)
{
    for (int y = 0; y < 14 * 8; y++)
    {
        for (int x = 0; x < 16 * 8; x++)
        {
            int color = GB_CameraRetinaProcessedImageGetPixel(x, y);
            int bufindex = (y * bufw + x) * 3;
            buffer[bufindex + 0] = color;
            buffer[bufindex + 1] = color;
            buffer[bufindex + 2] = color;
        }
    }
}
