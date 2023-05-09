//-----------------------------------------------------------------------------
/**
 * @file lib_fb.h
 * @author charles-park (charles-park@hardkernel.com)
 * @brief dotmatrix framebuffer library header file.
 * @version 0.1
 * @date 2022-05-10
 *
 * @copyright Copyright (c) 2022
 *
 */
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#ifndef __LIB_FB_H__
#define __LIB_FB_H__

#include <Arduino.h>

//-----------------------------------------------------------------------------
// Color table & convert macro
//-----------------------------------------------------------------------------
#define UINT_TO_B(i)        ((i      ) & 0xFF)
#define UINT_TO_G(i)        ((i >>  8) & 0xFF)
#define UINT_TO_R(i)        ((i >> 16) & 0xFF)
#define UINT_TO_A(i)        ((i >> 24) & 0xFF)

#define RGB_TO_UINT(r,g,b)  (((r << 16) | (g << 8) | b) & 0xFFFFFF)
#define ARGB_TO_UINT(a,rgb) ((a << 24) | (rgb)) & 0xFFFFFFFF
/*
    https://www.rapidtables.com/web/color/RGB_Color.html
*/
#define COLOR_BLACK         RGB_TO_UINT(0, 0, 0)
#define COLOR_WHITE         RGB_TO_UINT(255,255,255)
#define COLOR_RED	        RGB_TO_UINT(255,0,0)
#define COLOR_LIME	        RGB_TO_UINT(0,255,0)
#define COLOR_BLUE	        RGB_TO_UINT(0,0,255)
#define COLOR_YELLOW        RGB_TO_UINT(255,255,0)
#define COLOR_GREEN	        RGB_TO_UINT(0,128,0)
#define COLOR_CYAN          RGB_TO_UINT(0,255,255)
#define COLOR_MAGENTA       RGB_TO_UINT(255,0,255)
#define COLOR_SILVER        RGB_TO_UINT(192,192,192)
#define COLOR_DIM_GRAY      RGB_TO_UINT(105,105,105)
#define COLOR_GRAY          RGB_TO_UINT(128,128,128)
#define COLOR_DARK_GRAY     RGB_TO_UINT(169,169,169)
#define COLOR_LIGHT_GRAY    RGB_TO_UINT(211,211,211)
#define COLOR_MAROON        RGB_TO_UINT(128,0,0)
#define COLOR_OLIVE         RGB_TO_UINT(128,128,0)
#define COLOR_PURPLE        RGB_TO_UINT(128,0,128)
#define COLOR_TEAL          RGB_TO_UINT(0,128,128)
#define COLOR_NAVY          RGB_TO_UINT(0,0,128)

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
union u_color565 {
    unsigned short  us;
    unsigned char   ub[sizeof(unsigned short)];
    struct bits
    {
        /* data */
        unsigned short  b:5;    // lsb
        unsigned short  g:6;
        unsigned short  r:5;
    };
};

union u_color888 {
    unsigned int    ui;
    unsigned char   ub[sizeof(unsigned int)];
    struct bits
    {
        /* data */
        unsigned int    b:8;    // lsb
        unsigned int    g:8;
        unsigned int    r:8;
        unsigned int    a:8;
    };
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class lib_fb
{
private:
    int             _w, _h, _bpp, _stride, _size, _bgr;
    unsigned char   *_p_mem;
    unsigned int    _bg_color, _fg_color;

public:
    lib_fb (/* args */);
    /* width (x bits), height (y bits), bits per pixel(1, 16, 32) */
    lib_fb (int w = 8, int h = 8, int bpp = 1,
            unsigned int fg_color = COLOR_WHITE,
            unsigned int bg_color = COLOR_BLACK);
    ~lib_fb();

    void set_color (unsigned int fg_color, unsigned int bg_color) {
        _fg_color = fg_color;   _bg_color = bg_color;
    }
    unsigned int get_fg_color () { return _fg_color; }
    unsigned int get_bg_color () { return _bg_color; }
    int get_width () { return _w; }
    int get_height() { return _h; }

    void clear () {
        memset (_p_mem, COLOR_BLACK, _size);
    }

    void put_pixel (int w, int h);
    unsigned int get_pixel (int w, int h);
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif  // #define __LIB_FB_H__
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
