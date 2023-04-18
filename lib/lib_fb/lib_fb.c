//-----------------------------------------------------------------------------
/**
 * @file lib_fb.c
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
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#include "lib_fb.h"
//-----------------------------------------------------------------------------
// Fonts
//-----------------------------------------------------------------------------
#if defined(FONT_HANBOOT)
    #include "fonts/FontHanboot.h"
    #define SET_FONT1   FONT_HANBOOT1
    #define SET_FONT2   FONT_HANBOOT2
    #define SET_FONT3   FONT_HANBOOT3
#elif defined(FONT_HANGODIC)
    #include "fonts/FontHangodic.h"
    #define SET_FONT1   FONT_HANGODIC1
    #define SET_FONT2   FONT_HANGODIC2
    #define SET_FONT3   FONT_HANGODIC3
#elif defined(FONT_HANSOFT)
    #include "fonts/FontHansoft.h"
    #define SET_FONT1   FONT_HANSOFT1
    #define SET_FONT2   FONT_HANSOFT2
    #define SET_FONT3   FONT_HANSOFT3
#elif defined(FONT_HANGUL1)
    #include "fonts/FontHangul.h"
    #define SET_FONT1   FONT_HANGUL1
    #define SET_FONT2   FONT_HANGUL2
    #define SET_FONT3   FONT_HANGUL3
#elif defined(FONT_HANPIL)
    #include "fonts/FontHanpil.h"
    #define SET_FONT1   FONT_HANPIL1
    #define SET_FONT2   FONT_HANPIL2
    #define SET_FONT3   FONT_HANPIL3
#else
    #include "fonts/FontHansoft.h"
    #define SET_FONT1   FONT_HANSOFT1
    #define SET_FONT2   FONT_HANSOFT2
    #define SET_FONT3   FONT_HANSOFT3
#endif

#include "fonts/FontAscii_8x16.h"
#include "fonts/FontAscii_8x8.h"

#define dmsg(fmt, args...) printf("\r%s(%d):"fmt"\r", __func__, __LINE__, ##args)
//#define dmsg

//-----------------------------------------------------------------------------
// Function prototype define.
//-----------------------------------------------------------------------------
static void _make_image (   unsigned char f_m_l, // first_middle_last
                            unsigned char *dest,
                            int img_base);
static void _get_hangul_image(  unsigned char *p_img,
                                unsigned char HAN1,
                                unsigned char HAN2,
                                unsigned char HAN3);
static int  _draw_hangul_bitmap (fb_info_t *fb, int x, int y,
            unsigned char c1, unsigned char c2, unsigned char c3, int scale);
static void _get_ascii_image    (unsigned char *p_img, unsigned char c, int f_size);
static int  _draw_ascii_bitmap  (fb_info_t *fb, int x, int y,
            unsigned char c, int scale);
static int  _draw_text  (fb_info_t *fb, int x, int y, char *p_str, int scale);
int         get_pixel   (fb_info_t *fb, int x, int y);
void        put_pixel   (fb_info_t *fb, int x, int y, int color);
int         draw_text   (fb_info_t *fb, int x, int y, int scale, char *fmt, ...);
void        fb_clear    (fb_info_t *fb);

/* for virutal frame buffer */
fb_info_t   *fb_init (int width, int height);
void        fb_close (fb_info_t *fb);
//-----------------------------------------------------------------------------
// hangul image base 16x16
//-----------------------------------------------------------------------------
const char D_ML[22] = { 0, 0, 2, 0, 2, 1, 2, 1, 2, 3, 0, 2, 1, 3, 3, 1, 2, 1, 3, 3, 1, 1 };
const char D_FM[40] = { 1, 3, 0, 2, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 0, 2, 1, 3, 1, 3, 1, 3  };
const char D_MF[44] = { 0, 0, 0, 5, 0, 5, 0, 5, 0, 5, 0, 5, 0, 5, 0, 5, 0, 5, 1, 6, 3, 7, 3, 7, 3, 7, 1, 6, 2, 6, 4, 7, 4, 7, 4, 7, 2, 6, 1, 6, 3, 7, 0, 5 };

//-----------------------------------------------------------------------------
static void _make_image (   unsigned char f_m_l, // first_middle_last
                            unsigned char *dest,
                            int img_base)
{
    int i;

    for (i = 0; i < 32; i++) {
        switch(f_m_l) {
            default :
            case    0:
                dest[i]  |= pgm_read_byte (SET_FONT1[img_base] + i);    break;
            case    1:
                dest[i]  |= pgm_read_byte (SET_FONT2[img_base] + i);    break;
            case    2:
                dest[i]  |= pgm_read_byte (SET_FONT3[img_base] + i);    break;
        }
    }
}

//-----------------------------------------------------------------------------
static void _get_hangul_image(  unsigned char *p_img,
                                unsigned char HAN1,
                                unsigned char HAN2,
                                unsigned char HAN3)
{
    unsigned char f, m, l;
    unsigned char f1, f2, f3;
    unsigned short utf16 = 0;

    /*------------------------------
    UTF-8 을 UTF-16으로 변환한다.

    UTF-8 1110xxxx 10xxxxxx 10xxxxxx
    ------------------------------*/
    utf16 = ((unsigned short)HAN1 & 0x000f) << 12 |
            ((unsigned short)HAN2 & 0x003f) << 6  |
            ((unsigned short)HAN3 & 0x003f);
    utf16 -= 0xAC00;

    /* 초성 / 중성 / 종성 분리 */
    l = (utf16 % 28);
    utf16 /= 28;
    m = (utf16 % 21) +1;
    f = (utf16 / 21) +1;

    /* 초성 / 중성 / 종성 형태에 따른 이미지 선택 */
    f3 = D_ML[m];
    f2 = D_FM[(f * 2) + (l != 0)];
    f1 = D_MF[(m * 2) + (l != 0)];

    if (f)  {   _make_image(0, p_img, (f1*16 + f1 *4 + f)); }
    if (m)  {   _make_image(1, p_img, (        f2*22 + m)); }
    if (l)  {   _make_image(2, p_img, (f3*32 - f3 *4 + l)); }

    #if 0
    {
        unsigned char i, mask;

        for (i = 0; i < 32 ; i++) {
            if ((i % 2) == 0)   printf ("\n\r");
            for (mask = 0x80; mask != 0; mask >>= 1) {
                if (p_img[i] & mask)    printf("*");
                else                    printf(".");
            }
        }
        printf ("\n\r");
    }
    #endif
}

//-----------------------------------------------------------------------------
static int _draw_hangul_bitmap (fb_info_t *fb, int x, int y,
            unsigned char c1, unsigned char c2, unsigned char c3, int scale)
{
    unsigned char p_img[32];
    int pos, i, j, mask, x_off = 0, y_off, scale_y, scale_x;

    memset (p_img, 0x00, sizeof(p_img));
    _get_hangul_image(p_img, c1, c2, c3);

    for (i = 0, y_off = 0, pos = 0; i < 16; i++) {
        for (scale_y = 0; scale_y < scale; scale_y++) {
            if (scale_y)
                pos -= 2;
            for (x_off = 0, j = 0; j < 2; j++) {
                for (mask = 0x80; mask > 0; mask >>= 1) {
                    for (scale_x = 0; scale_x < scale; scale_x++) {
                        put_pixel(fb, x + x_off, y + y_off,
                                    (p_img[pos] & mask) ? 1 : 0);
                        x_off++;
                    }
                }
                pos++;
            }
            y_off++;
        }
    }
    return x_off;
}

//-----------------------------------------------------------------------------
static void _get_ascii_image (unsigned char *p_img, unsigned char c, int f_size)
{
    int i, img_cnt = f_size ? 16 : 8;

    for (i = 0; i < img_cnt; i++) {
        p_img[i] = f_size ?
            pgm_read_byte(FONT_ASCII[c] + i) : pgm_read_byte(FONT_ASCII_8x8[c] + i);
    }
    #if 0
    {
        unsigned char i, mask;

        for (i = 0; i < img_cnt ; i++) {
            printf ("\n\r");
            for (mask = 0x80; mask != 0; mask >>= 1) {
                if (p_img[i] & mask)    printf("*");
                else                    printf(".");
            }
        }
        printf ("\n\r");
    }
    #endif
}

//-----------------------------------------------------------------------------
static int _draw_ascii_bitmap (fb_info_t *fb,
                    int x, int y, unsigned char c, int scale)
{
    unsigned char p_img[16];
    int pos, mask, x_off = 0, y_off, scale_y, scale_x, y_max;

    memset (p_img, 0, sizeof(p_img));
    _get_ascii_image (p_img, c, scale);

    /* ascii 8x8 display */
    if (!scale) {   y_max = 8;  scale = 1; }
    else            y_max = 16;

    for (pos = 0, y_off = 0; pos < y_max; pos++) {
        for (scale_y = 0; scale_y < scale; scale_y++) {
            for (x_off = 0, mask = 0x80; mask > 0; mask >>= 1) {
                for (scale_x = 0; scale_x < scale; scale_x++) {
                    put_pixel(fb, x + x_off, y + y_off,
                                (p_img[pos] & mask) ? 1 : 0);
                    x_off++;
                }
            }
            y_off++;
        }
    }
    return x_off;
}

//-----------------------------------------------------------------------------
static int _draw_text (fb_info_t *fb, int x, int y, char *p_str, int scale)
{
    unsigned char c1, c2, c3;
    int draw_bits_total = 0, draw_bits = 0;

    while(*p_str) {

        c1 = *(unsigned char *)p_str++;
        //---------- 한글 ---------
        /* 모든 문자는 기본적으로 UTF-8형태로 저장되며 한글은 3바이트를 가진다. */
        /* 한글은 3바이트를 읽어 UTF8 to UTF16으로 변환후 초/중/종성을 분리하여 조합형으로 표시한다. */
        if (c1 >= 0x80 && scale) {
            c2 = *(unsigned char *)p_str++;
            c3 = *(unsigned char *)p_str++;

            draw_bits = _draw_hangul_bitmap(fb, x, y, c1, c2, c3, scale);
        }
        //---------- ASCII 8x16 or ASCII 8x8(if scale is 0) ---------
        else {
            draw_bits = _draw_ascii_bitmap(fb, x, y, c1, scale);
        }
        x += draw_bits;
        draw_bits_total += draw_bits;
    }
    dmsg("draw image x bits size = %d\n", draw_bits_total);
    return (draw_bits_total);
}

//-----------------------------------------------------------------------------
int get_pixel (fb_info_t *fb, int x, int y)
{
    int offset = (y * fb->stride) + (x / 8), bit_offset = x % 8;

    if (offset > fb->size)
        return 0;

    return  (fb->data[offset] & (0x01 << bit_offset)) ? 1 : 0;
}

//-----------------------------------------------------------------------------
void put_pixel (fb_info_t *fb, int x, int y, int color)
{
    int offset = (y * fb->stride) + (x / 8), bit_offset = x % 8;

    if ((x < fb->w) && (y < fb->h)) {
        fb->data[offset] |= color ? (0x01 << bit_offset) : 0;
    } else {
        dmsg ("Out of range.(width = %d, x = %d, height = %d, y = %d)\n",
            fb->w, x, fb->h, y);
    }
}

//-----------------------------------------------------------------------------
int draw_text (fb_info_t *fb, int x, int y, int scale, char *fmt, ...)
{
    char buf[256];
    va_list va;

    memset(buf, 0x00, sizeof(buf));

    va_start(va, fmt);
    vsprintf(buf, fmt, va);
    va_end(va);

    return _draw_text(fb, x, y, buf, scale);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void fb_clear (fb_info_t *fb)
{
    memset(fb->data, 0x00, fb->size);
}

//-----------------------------------------------------------------------------
fb_info_t *fb_init (int width, int height)
{
    fb_info_t   *fb = (fb_info_t *)malloc(sizeof(fb_info_t));

    if (fb == NULL) {
        dmsg("ERROR: framebuffer malloc error!\n");
        return NULL;
    }
    memset(fb, 0, sizeof(fb_info_t));

    fb->w       = width;
    fb->h       = height;
    fb->stride  = fb->w / 8;
    fb->size    = fb->stride * fb->h;

    fb->data = (char *)malloc(fb->size);
    if (fb->data == NULL)
        goto out;

    fb_clear (fb);
    return  fb;
out:
    fb_close(fb);
    return  NULL;
}

//-----------------------------------------------------------------------------
void fb_close (fb_info_t *fb)
{
    if (fb->data)
        free (fb->data);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
