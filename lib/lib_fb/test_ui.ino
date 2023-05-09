//------------------------------------------------------------------------------
/**
 * @file test_fb.c
 * @author charles-park (charles.park@hardkernel.com)
 * @brief Dot-Matrix Billboard Application.(esp8266 d1_mini)
 * @version 0.1
 * @date 2023-04-27
 *
 * @copyright Copyright (c) 2022
 *
 * Control Item
 * https://ko.aliexpress.com/item/4001131640516.html?spm=a2g0o.order_list.order_list_main.10.6268140fqgaC4g&gatewayAdapt=glo2kor
 *
 * Data sheet
 * https://www.analog.com/media/en/technical-documentation/data-sheets/max7219-max7221.pdf
*/
//------------------------------------------------------------------------------
#include <lib_fb.h>
#include <lib_font.h>
#include <umm_malloc/umm_heap_select.h>

#define FB_W    32
#define FB_H    32
#define FB_BPP  32

//lib_fb fb(16, 16, 1);
lib_font font (eASCII_FONT_8x16, eHANGUL_FONT_HANPIL);
//lib_font font (eASCII_FONT_16x32, eHANGUL_FONT_HANPIL);

//------------------------------------------------------------------------------
void setup()
{
    // Board LED초기화. 동작상황 표시함.
    pinMode(2,  OUTPUT);

    Serial.begin(115200);
}

//------------------------------------------------------------------------------
void display_memory_usage()
{
    HeapSelectIram ephemeral;
    Serial.printf("\r\nIRAM free: %6d bytes\r\n", ESP.getFreeHeap());
    {
        HeapSelectDram ephemeral;
        Serial.printf("\r\nDRAM free: %6d bytes\r\n", ESP.getFreeHeap());
    }
}
//------------------------------------------------------------------------------
void fb_console_display(int w, int h)
{
    lib_fb fb(FB_W, FB_H, FB_BPP);
    printf ("\n\r*** after lib_fb class define ***\n\r");
    display_memory_usage();

    fb.set_color (COLOR_WHITE, COLOR_BLACK);
    fb.clear ();
    fb.put_pixel(w, h);
    printf("\r\n0x%08x\n\r", fb.get_pixel(w, h));

    for (int i = 0; i < fb.get_height(); i++) {
        printf ("\n\r");
        for (int j = 0; j < fb.get_width(); j++) {
            if (fb.get_pixel(j , i))    printf("*");
            else                        printf(".");
        }
    }
    printf ("\n\r");
}

//------------------------------------------------------------------------------
void test_draw_text (char *p_str)
{
    unsigned char c1, c2, c3;

    while(*p_str) {
        c1 = *(unsigned char *)p_str++;

        //---------- 한글 ---------
        /* 모든 문자는 기본적으로 UTF-8형태로 저장되며 한글은 3바이트를 가진다. */
        /* 한글은 3바이트를 일어 UTF8 to UTF16으로 변환후 초/중/종성을 분리하여 조합형으로 표시한다. */
        if (c1 >= 0x80){
            c2 = *(unsigned char *)p_str++;
            c3 = *(unsigned char *)p_str++;

            font.make_hangul_img(c1, c2, c3);
        }
        //---------- ASCII ---------
        else {
            font.make_ascii_img(c1);
        }
    }
}

//------------------------------------------------------------------------------
void loop()
{
    for (int i = 0; i < FB_H; i++) {
        for (int j = 0; j < FB_W; j++) {
            printf ("\n\r*** before lib_fb class define ***\n\r");
            display_memory_usage();
            switch (j % eHANGUL_FONT_END) {
                case    1:
                    font.set_hangul_font (eHANGUL_FONT_HANGUL);
                    font.make_hangul_img("한");
                    font.set_ascii_font (eASCII_FONT_8x8);
                    font.make_ascii_img('A');
                break;
                case    2:
                    font.set_hangul_font (eHANGUL_FONT_HANBOOT);
                    font.make_hangul_img("글");
                    font.set_ascii_font (eASCII_FONT_8x16);
                    font.make_ascii_img('B');
                break;
                case    3:
                    font.set_hangul_font (eHANGUL_FONT_HANGODIC);
                    font.make_hangul_img("테");
                    font.set_ascii_font (eASCII_FONT_16x32);
                    font.make_ascii_img('C');
                break;
                case    4:
                    font.set_hangul_font (eHANGUL_FONT_HANPIL);
                    font.make_hangul_img("스");
                    font.set_ascii_font (eASCII_FONT_8x8);
                    font.make_ascii_img('D');
                break;
                default :
                case    0:
                    font.set_hangul_font (eHANGUL_FONT_HANSOFT);
                    font.make_hangul_img("트");
                    font.set_ascii_font (eASCII_FONT_8x16);
                    font.make_ascii_img('E');
                break;
            }
            fb_console_display(j, i);
            delay(1000);
        }
    }
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
