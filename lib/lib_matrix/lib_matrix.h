//------------------------------------------------------------------------------
/**
 * @file lib_matrix.h
 * @author charles-park (charles.park@hardkernel.com)
 * @brief Dot-Matrix(MAX7219) Billboard Application.(esp8266 d1_mini)
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
#ifndef __LIB_MATRIX_H__
#define __LIB_MATRIX_H__

#include <Arduino.h>
//------------------------------------------------------------------------------
class lib_matrix
{
private:
    // x=8Dots, y=8Dots, total=64Dots
    const unsigned char _DotsOfOneModule = 64;
    const unsigned char _LineOfOneModule = 8;
    const unsigned char _InitCmd[6][2] = {
        /* { reg, init val }*/
        /* shutdown : operation (D0 = 0) */
        {0x0c,0x00},
        /* Decode mode : No decode for digits 7-0(dot matrix led) */
        {0x09,0x00},
        /* dot led bright 0x0f(max) = 0.5W/ea */
//        {0x0a,0x08},
        {0x0a,0x01},
        /* scan limit : 8 line enable */
        {0x0b,0x07},
        /* shutdown : normal operation (D0 = 1) */
        {0x0c,0x01},
        /* Display Normal Operation (D0 = 0)*/
        {0x0f,0x00},
        /* Display Test Mode Operation (D0 = 1)*/
    //    {0x0f,0x01},
    };

    /* data */
    int _num_of_module;         // total module count
    int _num_of_module_line;    // module line

    // Matrix dots
    int _x_dots, _y_dots;
    // Matrix control table
    unsigned char   *_p_matrix_table;
    // Mattrix Frame buffer
    unsigned char   *_p_fb;
    int _fb_size;

    // SPI buffer : Send to Matrix(MAX7219)
    unsigned char   *_p_spi_buffer;
    int _spi_send_bytes;

    void _init (unsigned long spi_freq , bool hw_cs);
    void _send_to_matrix ();

public:

    lib_matrix (/* args */);
    // SPI Default freq 1Mhz
    lib_matrix (const int x_bits, const int y_bits, const unsigned char *matrix_table,
                unsigned long spi_freq = 1000000, bool hw_cs = true);
    ~lib_matrix ();

    void update ();
    void brightness (unsigned char brightness);

    void fill (unsigned char fill) {
        memset (_p_fb, fill, _fb_size);
    }
    void set_bit (int x, int y, bool onoff);
    bool get_bit (int x, int y);

    void set_module_byte (int location_of_module, int line_of_module,
                        unsigned char byte);
    unsigned char get_module_byte (int location_of_module, int line_of_module);
    int num_of_module () {
        return  _num_of_module;
    }
    int get_x_dots() {
        return  _x_dots;
    }
    int get_y_dots() {
        return  _y_dots;
    }
};

//------------------------------------------------------------------------------
#endif  // __LIB_MATRIX_H__
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
