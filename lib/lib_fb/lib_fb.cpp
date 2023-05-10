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
void lib_fb::put_pixel (int x, int y, unsigned int color)
{
    if ((x >= _w) || (y >= _h)) {
        printf ("%s(%s):Out of range.(width = %d, x = %d, height = %d, y = %d)\r\n",
            __FILE__, __func__, _w, x, _h, y);
        return;
    }
    if ((_bpp == 32) || (_bpp == 24)) {
        int offset = (y * _stride) + (x * (_bpp >> 3));
        union u_color888 c;
        c.ui = color;

        _p_mem[offset+0] = c.ub[0]; _p_mem[offset+1] = c.ub[1];
        _p_mem[offset+2] = c.ub[2]; _p_mem[offset+3] = c.ub[3];
    } else if (_bpp == 16) {
        int offset = (y * _stride) + (x * (_bpp >> 3));
        union u_color565 c;
        c.us = (unsigned short)color;

        _p_mem[offset+0] = c.ub[0]; _p_mem[offset+1] = c.ub[1];
    } else {
        int offset = (y * _stride) + (x / 8), bit_offset = x % 8;

        if (color)  _p_mem[offset] |= ( (0x01 << bit_offset));
        else        _p_mem[offset] &= (~(0x01 << bit_offset));
    }
}

//-----------------------------------------------------------------------------
unsigned int lib_fb::get_pixel (int x, int y)
{
    if ((x >= _w) || (y >= _h)) {
        printf ("%s(%s):Out of range.(width = %d, x = %d, height = %d, y = %d)\r\n",
            __FILE__, __func__, _w, x, _h, y);
        return 0;
    }
    if ((_bpp == 32) || (_bpp == 24)) {
        int offset = (y * _stride) + (x * (_bpp >> 3));
        union u_color888 c;

        c.ub[0] = _p_mem[offset+0]; c.ub[1] = _p_mem[offset+1];
        c.ub[2] = _p_mem[offset+2]; c.ub[3] = _p_mem[offset+3];
        return c.ui;
    } else if (_bpp == 16) {
        int offset = (y * _stride) + (x * (_bpp >> 3));
        union u_color565 c;

        c.ub[0] = _p_mem[offset+0]; c.ub[1] = _p_mem[offset+1];
        return (unsigned int)c.us;
    } else {
        int offset = (y * _stride) + (x / 8), bit_offset = x % 8;

        return  (_p_mem[offset] & (0x01 << bit_offset)) ? 1 : 0;
    }
}

//-----------------------------------------------------------------------------
int lib_fb::_draw_hangul_bitmap (int x, int y)
{
    int i, j, x_off, y_off, scale_y, scale_x;

    for (i = 0, y_off = 0; i < get_hangul_img_h(); i++) {
        for (scale_y = 0; scale_y < _scale; scale_y++) {
            for (x_off = 0, j = 0; j < get_hangul_img_w(); j++) {
                for (scale_x = 0; scale_x < _scale; scale_x++) {
                    unsigned int color = get_hangul_img_pixel(j, i) ?
                                            _fg_color : _bg_color;
                    put_pixel (x + x_off, y + y_off, color);
                    x_off++;
                }
            }
            y_off++;
        }
    }
    return x_off;
}

//-----------------------------------------------------------------------------
int lib_fb::_draw_ascii_bitmap (int x, int y)
{
    int i, j, x_off = 0, y_off = 0, scale_y, scale_x;

    for (i = 0, y_off = 0; i < get_ascii_img_h(); i++) {
        for (scale_y = 0; scale_y < _scale; scale_y++) {
            for (x_off = 0, j = 0; j < get_ascii_img_w(); j++) {
                for (scale_x = 0; scale_x < _scale; scale_x++) {
                    unsigned int color = get_ascii_img_pixel(j, i) ?
                                            _fg_color : _bg_color;
                    put_pixel (x + x_off, y + y_off, color);
                    x_off++;
                }
            }
            y_off++;
        }
    }
    return x_off;
}

//-----------------------------------------------------------------------------
int lib_fb::_draw_text  (int x, int y, unsigned char *p_str)
{
    unsigned char c1, c2, c3;
    int draw_bits_total = 0, draw_bits = 0;

    while(*p_str && _scale) {

        c1 = *(unsigned char *)p_str++;
        //---------- 한글 ---------
        /* 모든 문자는 기본적으로 UTF-8형태로 저장되며 한글은 3바이트를 가진다. */
        /* 한글은 3바이트를 읽어 UTF8 to UTF16으로 변환후 초/중/종성을 분리하여 조합형으로 표시한다. */
        if (c1 >= 0x80) {
            c2 = *(unsigned char *)p_str++;
            c3 = *(unsigned char *)p_str++;

            make_hangul_img (c1, c2, c3);
            draw_bits = _draw_hangul_bitmap (x, y);
        }
        //---------- ASCII 8x16 or ASCII 8x8(if scale is 0) ---------
        else {
            make_ascii_img (c1);
            draw_bits = _draw_ascii_bitmap(x, y);
        }
        x += draw_bits;
        draw_bits_total += draw_bits;
    }
    printf("\n\rdraw image x bits size = %d\n\r", draw_bits_total);
    return (draw_bits_total);
}

//-----------------------------------------------------------------------------
int lib_fb::my_strlen (char *str)
{
    int cnt = 0, err = strlen(str) +1;

    /* utf-8 에서 한글표현은 3바이트 */
    while ((*str != 0x00) && err--) {
        if (*str & 0x80) {
            str += 3;   cnt += 2;
        } else {
            str += 1;   cnt++;
        }
    }
    return cnt;
}

//------------------------------------------------------------------------------
int lib_fb::draw_text  (int x, int y, int scale, char *fmt, ...)
{
    unsigned char buf[512];
    int draw_w_bits;

    va_list va;

    memset((void *)buf, 0x00, sizeof(buf));

    va_start(va, fmt);
    vsprintf((char *)buf, fmt, va);
    va_end(va);

    set_scale (scale);
    draw_w_bits = _draw_text(x, y, buf);

    return draw_w_bits;
}

//------------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
