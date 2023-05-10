//-----------------------------------------------------------------------------
/**
 * @file lib_font.h
 * @author charles-park (charles-park@hardkernel.com)
 * @brief hangul, ascii font control library header file.
 * @version 0.1
 * @date 2023-05-10
 *
 * @copyright Copyright (c) 2022
 *
 */
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#ifndef __LIB_FONT_H__
#define __LIB_FONT_H__

#include <Arduino.h>

#include "fonts/FontHanboot.h"
#include "fonts/FontHangodic.h"
#include "fonts/FontHansoft.h"
#include "fonts/FontHangul.h"
#include "fonts/FontHanpil.h"
#include "fonts/FontHansoft.h"

#include "fonts/FontAscii_16x32.h"
#include "fonts/FontAscii_8x16.h"
#include "fonts/FontAscii_8x8.h"

//-----------------------------------------------------------------------------
enum eASCII_FONTS {
    eASCII_FONT_8x8 = 0,
    eASCII_FONT_8x16,
    eASCII_FONT_16x32,
    eASCII_FONT_END
};

enum eHANGUL_FONTS {
    eHANGUL_FONT_HANGUL = 0,
    eHANGUL_FONT_HANBOOT,
    eHANGUL_FONT_HANGODIC,
    eHANGUL_FONT_HANPIL,
    eHANGUL_FONT_HANSOFT,
    eHANGUL_FONT_END
};

//-----------------------------------------------------------------------------
class lib_font
{
private:
    /* 한글 조합형 테이블 (초성/중성/종성) */
    const char _D_ML[22] = { 0, 0, 2, 0, 2, 1, 2, 1, 2, 3, 0, 2, 1, 3, 3, 1, 2, 1, 3, 3, 1, 1 };
    const char _D_FM[40] = { 1, 3, 0, 2, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 0, 2, 1, 3, 1, 3, 1, 3  };
    const char _D_MF[44] = { 0, 0, 0, 5, 0, 5, 0, 5, 0, 5, 0, 5, 0, 5, 0, 5, 0, 5, 1, 6, 3, 7, 3, 7, 3, 7, 1, 6, 2, 6, 4, 7, 4, 7, 4, 7, 2, 6, 1, 6, 3, 7, 0, 5 };

    enum eASCII_FONTS  _ascii_font;
    enum eHANGUL_FONTS _hangul_font;

    /* Font image */
    /* max size 16(w)x32(h) = 2byte(w)x32byte(h) = 64 bytes */
    unsigned char _ascii_img[64];
    /* fix size 16(w)x16(h) = 2byte(w)x16byte(h) = 32 bytes */
    unsigned char _hangul_img[32];

    /* make hangul image */
    void _make_image (unsigned char f_m_l, int img_base);

public:
    lib_font (/* args */):_ascii_font(eASCII_FONT_8x16), _hangul_font(eHANGUL_FONT_HANSOFT) {};

    /* width (x bits), height (y bits), bits per pixel(1, 16, 32) */
    lib_font (  enum eASCII_FONTS  a_font,
                enum eHANGUL_FONTS h_font);
    ~lib_font() {};

    void set_hangul_font(enum eHANGUL_FONTS font)   { _hangul_font = font; };
    // parse hangul UTF8 to UTF16
    void make_hangul_img (unsigned char c1, unsigned char c2, unsigned char c3);
    void make_hangul_img (const char *hangul) {
        unsigned char *_hangul = (unsigned char *)hangul;
        if (_hangul[0] & 0x80)
            make_hangul_img (_hangul[0], _hangul[1], _hangul[2]);
    };
    /* fixed size hangul font (16x16) */
    int get_hangul_img_w()  { return 16; };
    int get_hangul_img_h()  { return 16; };
    unsigned char *get_hangul_img_p()   { return &_hangul_img[0]; };
    bool get_hangul_img_pixel (int w, int h);

    void set_ascii_font (enum eASCII_FONTS  font)   { _ascii_font  = font; };
    void make_ascii_img  (const char ascii);
    int get_ascii_img_w();
    int get_ascii_img_h();
    unsigned char *get_ascii_img_p()    { return &_ascii_img[0]; };
    bool get_ascii_img_pixel  (int w, int h);
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif  // #define __LIB_FONT_H__
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
