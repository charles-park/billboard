//-----------------------------------------------------------------------------
/**
 * @file lib_fb.cpp
 * @author charles-park (charles.park@hardkernel.com)
 * @brief Framebuffer control library
 * @version 0.1
 * @date 2023-04-10
 *
 * @copyright Copyright (c) 2022
 *
 */
//-----------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <lib_fb.h>

//-----------------------------------------------------------------------------
lib_fb::lib_fb (int w, int h, int bpp,
            unsigned int fg_color, unsigned int bg_color)
{
    _w = w;
    _h = h;
    _bpp = bpp;

    _stride = (_w * _bpp) / 8;
    _size   = (_h * _stride);

    set_color (fg_color, bg_color);

    _p_mem = new unsigned char [_size];
}

//-----------------------------------------------------------------------------
lib_fb::~lib_fb ()
{
    if (_p_mem)
        delete[]    _p_mem;
}

//-----------------------------------------------------------------------------
void lib_fb::put_pixel (int w, int h)
{
    if ((w > _w) || (h > _h)) {
        printf ("Out of range.(width = %d, x = %d, height = %d, y = %d)\r\n",
            _w, w, _h, h);
        return;
    }
    if ((_bpp == 32) || (_bpp == 24)) {
        int offset = (h * _stride) + (w * (_bpp >> 3));
        union u_color888 c;
        c.ui = _fg_color;

        _p_mem[offset+0] = c.ub[0]; _p_mem[offset+1] = c.ub[1];
        _p_mem[offset+2] = c.ub[2]; _p_mem[offset+3] = c.ub[3];
    } else if (_bpp == 16) {
        int offset = (h * _stride) + (w * (_bpp >> 3));
        union u_color565 c;
        c.us = (unsigned short)_fg_color;

        _p_mem[offset+0] = c.ub[0]; _p_mem[offset+1] = c.ub[1];
    } else {
        int offset = (h * _stride) + (w / 8), bit_offset = w % 8;

        if (_fg_color)  _p_mem[offset] |= ( (0x01 << bit_offset));
        else            _p_mem[offset] &= (~(0x01 << bit_offset));
    }
}

//-----------------------------------------------------------------------------
unsigned int lib_fb::get_pixel (int w, int h)
{
    if ((w > _w) || (h > _h)) {
        printf ("Out of range.(width = %d, x = %d, height = %d, y = %d)\r\n",
            _w, w, _h, h);
        return 0;
    }
    if ((_bpp == 32) || (_bpp == 24)) {
        int offset = (h * _stride) + (w * (_bpp >> 3));
        union u_color888 c;

        c.ub[0] = _p_mem[offset+0]; c.ub[1] = _p_mem[offset+1];
        c.ub[2] = _p_mem[offset+2]; c.ub[3] = _p_mem[offset+3];
        return c.ui;
    } else if (_bpp == 16) {
        int offset = (h * _stride) + (w * (_bpp >> 3));
        union u_color565 c;

        c.ub[0] = _p_mem[offset+0]; c.ub[1] = _p_mem[offset+1];
        return (unsigned int)c.us;
    } else {
        int offset = (h * _stride) + (w / 8), bit_offset = w % 8;

        return  (_p_mem[offset] & (0x01 << bit_offset)) ? 1 : 0;
    }
    return 0;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
