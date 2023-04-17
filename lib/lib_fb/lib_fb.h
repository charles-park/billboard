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

#ifdef  __cplusplus
extern  "C" {
#endif

//-----------------------------------------------------------------------------
// Frame buffer struct
//-----------------------------------------------------------------------------
typedef struct fb_info__t {
    int     w;
    int     h;
    int     stride;
    int     size;
    char    *data;
}   fb_info_t;

//-----------------------------------------------------------------------------
#define FONT_HANGUL_WIDTH   16
#define FONT_ASCII_WIDTH    8
#define FONT_HEIGHT         16

//------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------
extern int          get_pixel 	(fb_info_t *fb, int x, int y);
extern void         put_pixel 	(fb_info_t *fb, int x, int y, int color);
extern int          draw_text 	(fb_info_t *fb, int x, int y, int scale, char *fmt, ...);
extern void         draw_line 	(fb_info_t *fb, int x, int y, int w);
extern void         draw_rect 	(fb_info_t *fb, int x, int y, int w, int h, int lw);
extern void         draw_fill_rect (fb_info_t *fb, int x, int y, int w, int h);
extern void         fb_clear 	(fb_info_t *fb);

//-----------------------------------------------------------------------------
/* File discripter macgic number for Virutal framebuffer  */
//-----------------------------------------------------------------------------
extern fb_info_t    *fb_init 	(int width, int height);
extern void         fb_close   (fb_info_t *fb);

#ifdef  __cplusplus
}
#endif
//------------------------------------------------------------------------------------------------
#endif  // #define __LIB_FB_H__
//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------
