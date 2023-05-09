//-----------------------------------------------------------------------------
/**
 * @file lib_font.cpp
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
#include <lib_font.h>

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define TEST_MAKE_IMG
//-----------------------------------------------------------------------------
lib_font::lib_font (enum eASCII_FONTS a_font, enum eHANGUL_FONTS h_font)
{
    set_ascii_font (a_font);    set_hangul_font (h_font);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
int lib_font::get_ascii_img_w()
{
    switch (_ascii_font) {
        default:
        case    eASCII_FONT_8x16:
        case    eASCII_FONT_8x8:    return  8;
        case    eASCII_FONT_16x32:  return  16;
    }
}

//-----------------------------------------------------------------------------
int lib_font::get_ascii_img_h()
{
    switch (_ascii_font) {
        case    eASCII_FONT_8x8:    return  8;
        case    eASCII_FONT_16x32:  return  32;
        default:
        case    eASCII_FONT_8x16:   return  16;
    }
}

//-----------------------------------------------------------------------------
void lib_font::make_ascii_img (char ascii)
{
    unsigned char _ascii = ascii;
    memset (_ascii_img, 0x00, sizeof(_ascii_img));
    switch (_ascii_font) {
        case    eASCII_FONT_8x8:
            for (int i = 0; i < 8; i ++)
                _ascii_img[i] = pgm_read_byte (FONT_ASCII_8x8[_ascii] + i);
        break;
        case    eASCII_FONT_16x32:
            for (int i = 0; i < 64; i ++)
                _ascii_img[i] = pgm_read_byte (FONT_ASCII_16x32[_ascii] + i);
        break;
        default :
        case    eASCII_FONT_8x16:
            for (int i = 0; i < 16; i ++)
                _ascii_img[i] = pgm_read_byte (FONT_ASCII_8x16[_ascii] + i);
        break;
    }
    #if defined(TEST_MAKE_IMG)
    {
        int w = get_ascii_img_w(), h = get_ascii_img_h();

        printf("\n\r");
        for (int i = 0; i < h; i++) {
            for (int j = 0; j < w; j++) {
                printf("%c", get_ascii_img_pixel (j, i) ? '*' : '.');
            }
            printf("\n\r");
        }
        printf("\n\r");
    }
    #endif
}

//-----------------------------------------------------------------------------
bool lib_font::get_ascii_img_pixel  (int w, int h)
{
    if ((w < get_ascii_img_w()) && (h < get_ascii_img_h())) {
        int offset = h * get_ascii_img_w() / 8 + (w / 8);
        unsigned char mask = 0x80 >> (w % 8);

        return (_ascii_img[offset] & mask) ? true : false;
    }
    printf("\n\r%s:wrong pixel location. w = %d, %d, h %d, %d\n\r",
        __func__, w, get_ascii_img_w(), h, get_ascii_img_w());
    return false;
}

//-----------------------------------------------------------------------------
void lib_font::_make_image (unsigned char f_m_l, int img_base)
{
    switch (_hangul_font) {
        case eHANGUL_FONT_HANGUL:
            for (int i = 0; i < 32; i++) {
                switch (f_m_l) {
                    case    0:
                        _hangul_img[i] |= pgm_read_byte (FONT_HANGUL1[img_base] + i);
                    break;
                    case    1:
                        _hangul_img[i] |= pgm_read_byte (FONT_HANGUL2[img_base] + i);
                    break;
                    case    2:
                        _hangul_img[i] |= pgm_read_byte (FONT_HANGUL3[img_base] + i);
                    break;
                }
            }
        break;
        case eHANGUL_FONT_HANBOOT:
            for (int i = 0; i < 32; i++) {
                switch (f_m_l) {
                    case    0:
                        _hangul_img[i] |= pgm_read_byte (FONT_HANBOOT1[img_base] + i);
                    break;
                    case    1:
                        _hangul_img[i] |= pgm_read_byte (FONT_HANBOOT2[img_base] + i);
                    break;
                    case    2:
                        _hangul_img[i] |= pgm_read_byte (FONT_HANBOOT3[img_base] + i);
                    break;
                }
            }
        break;
        case eHANGUL_FONT_HANGODIC:
            for (int i = 0; i < 32; i++) {
                switch (f_m_l) {
                    case    0:
                        _hangul_img[i] |= pgm_read_byte (FONT_HANGODIC1[img_base] + i);
                    break;
                    case    1:
                        _hangul_img[i] |= pgm_read_byte (FONT_HANGODIC2[img_base] + i);
                    break;
                    case    2:
                        _hangul_img[i] |= pgm_read_byte (FONT_HANGODIC3[img_base] + i);
                    break;
                }
            }
        break;
        case eHANGUL_FONT_HANPIL:
            for (int i = 0; i < 32; i++) {
                switch (f_m_l) {
                    case    0:
                        _hangul_img[i] |= pgm_read_byte (FONT_HANPIL1[img_base] + i);
                    break;
                    case    1:
                        _hangul_img[i] |= pgm_read_byte (FONT_HANPIL2[img_base] + i);
                    break;
                    case    2:
                        _hangul_img[i] |= pgm_read_byte (FONT_HANPIL3[img_base] + i);
                    break;
                }
            }
        break;
        default:
        case eHANGUL_FONT_HANSOFT:
            for (int i = 0; i < 32; i++) {
                switch (f_m_l) {
                    case    0:
                        _hangul_img[i] |= pgm_read_byte (FONT_HANSOFT1[img_base] + i);
                    break;
                    case    1:
                        _hangul_img[i] |= pgm_read_byte (FONT_HANSOFT2[img_base] + i);
                    break;
                    case    2:
                        _hangul_img[i] |= pgm_read_byte (FONT_HANSOFT3[img_base] + i);
                    break;
                }
            }
        break;
    }
}

//-----------------------------------------------------------------------------
void lib_font::make_hangul_img (unsigned char c1, unsigned char c2, unsigned char c3)
{

    unsigned char f, m, l;
    unsigned char f1, f2, f3;
    unsigned short utf16 = 0;

    memset (_hangul_img, 0x00, sizeof(_hangul_img));
    /*------------------------------
    UTF-8 을 UTF-16으로 변환한다.

    UTF-8 1110xxxx 10xxxxxx 10xxxxxx
    ------------------------------*/
    utf16 = ((unsigned short)c1 & 0x000f) << 12 |
            ((unsigned short)c2 & 0x003f) << 6  |
            ((unsigned short)c3 & 0x003f);
    utf16 -= 0xAC00;

    /* 초성 / 중성 / 종성 분리 */
    l = (utf16 % 28);
    utf16 /= 28;
    m = (utf16 % 21) +1;
    f = (utf16 / 21) +1;

    /* 초성 / 중성 / 종성 형태에 따른 이미지 선택 */
    f3 = _D_ML[m];
    f2 = _D_FM[(f * 2) + (l != 0)];
    f1 = _D_MF[(m * 2) + (l != 0)];

    if (f)  {   _make_image(0, (f1*16 + f1 *4 + f)); }
    if (m)  {   _make_image(1, (        f2*22 + m)); }
    if (l)  {   _make_image(2, (f3*32 - f3 *4 + l)); }

    #if defined(TEST_MAKE_IMG)
    {
        int w = get_hangul_img_w(), h = get_hangul_img_h();

        printf("\n\r");
        for (int i = 0; i < h; i++) {
            for (int j = 0; j < w; j++) {
                printf("%c", get_hangul_img_pixel (j, i) ? '*' : '.');
            }
            printf("\n\r");
        }
        printf("\n\r");
    }
    #endif
}

//-----------------------------------------------------------------------------
bool lib_font::get_hangul_img_pixel (int w, int h)
{
    if ((w < get_hangul_img_w()) && (h < get_hangul_img_h())) {
        int offset = h * get_hangul_img_w() / 8 + (w / 8);
        unsigned char mask = 0x80 >> (w % 8);

        return (_hangul_img[offset] & mask) ? true : false;
    }
    printf("\n\r%s:wrong pixel location. w = %d, %d, h %d, %d\n\r",
        __func__, w, get_hangul_img_w(), h, get_hangul_img_w());
    return false;
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
