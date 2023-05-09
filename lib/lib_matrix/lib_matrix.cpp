//------------------------------------------------------------------------------
/**
 * @file lib_matrix.cpp
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
#include <SPI.h>
#include <lib_matrix.h>

//------------------------------------------------------------------------------
void lib_matrix::set_bit (int x, int y, bool onoff)
{
    unsigned int num_module_x = (_num_of_module / _num_of_module_line);
    unsigned int location_of_module = (y / 8) * num_module_x + (x / 8);
    unsigned int line_of_module = (y % 8);
    unsigned char control_bit_mask;;
    unsigned char module_line_byte = get_module_byte(location_of_module, line_of_module);

    control_bit_mask = (0x80 >> (x % 8));
    if (onoff)  module_line_byte |=  (control_bit_mask);
    else        module_line_byte &= ~(control_bit_mask);

    set_module_byte(location_of_module, line_of_module, module_line_byte);
}

//------------------------------------------------------------------------------
bool lib_matrix::get_bit (int x, int y)
{
    unsigned int num_module_x = (_num_of_module / _num_of_module_line);
    unsigned int location_of_module = (y / 8) * num_module_x + (x / 8);
    unsigned int line_of_module = (y % 8);
    unsigned char control_bit_mask;;
    unsigned char module_line_byte = get_module_byte(location_of_module, line_of_module);

    control_bit_mask = (0x80 >> (x % 8));

    return  (module_line_byte & control_bit_mask) ? true : false;
}

//------------------------------------------------------------------------------
void lib_matrix::set_module_byte (int location_of_module, int line_of_module,
                                unsigned char byte)
{
    _p_fb[(location_of_module * 8) + line_of_module] = byte;
}

//------------------------------------------------------------------------------
unsigned char lib_matrix::get_module_byte (int location_of_module,
                                        int line_of_module)
{
    return _p_fb[(location_of_module * 8) + line_of_module];
}

//------------------------------------------------------------------------------
// brightness : 0x1 ~ 0xF
//
void lib_matrix::brightness (unsigned char brightness)
{
    if (brightness && (brightness < 0x10)) {
        for (int j = 0; j < _num_of_module; j++) {
            /* Brightness address setup */
            _p_spi_buffer [(j * 2) + 0] = 0x0a;
            /* digit data set (0x01 ~ 0x0F) */
            _p_spi_buffer [(j * 2) + 1] = brightness;
        }
        _send_to_matrix();
    }
}

//------------------------------------------------------------------------------
void lib_matrix::update ()
{
    for (int i = 0; i < 8; i++)  {
        for (int j = 0; j < _num_of_module; j++) {
            /* digit line number set (1-8) */
            _p_spi_buffer [(j * 2) + 0] = i +1;
            /* digit line data set */
            _p_spi_buffer [(j * 2) + 1] = _p_fb[_p_matrix_table[j] * 8 + i];
        }
        _send_to_matrix();
    }
}

//------------------------------------------------------------------------------
void lib_matrix::_init (unsigned long spi_freq, bool hw_cs)
{
    int cmd_cnt = sizeof(_InitCmd)/sizeof(_InitCmd[0]);

    /* SPI init */
    SPI.begin();
    SPI.setFrequency(spi_freq);
    SPI.setHwCs(hw_cs);

    /* dummy data send */
    memset (_p_spi_buffer, 0, _spi_send_bytes); _send_to_matrix ();
    delay(10);

    for (int i = 0; i < cmd_cnt; i++)  {
        for (int j = 0; j < _num_of_module; j++) {
            _p_spi_buffer[(j * 2) + 0] = _InitCmd[i][0];
            _p_spi_buffer[(j * 2) + 1] = _InitCmd[i][1];
        }
        _send_to_matrix();
    }
    delay(10);

    memset (_p_fb, 0x00, _fb_size);
    update();
}

//------------------------------------------------------------------------------
void lib_matrix::_send_to_matrix ()
{
    if (_p_spi_buffer && _spi_send_bytes)
        SPI.transfer(_p_spi_buffer, _spi_send_bytes);
}

//------------------------------------------------------------------------------
lib_matrix::lib_matrix (/* args */)
{
    // default setup
    lib_matrix(8, 8, NULL);
}

//------------------------------------------------------------------------------
lib_matrix::lib_matrix (int x_dots, int y_dots, const unsigned char *matrix_table,
                        unsigned long spi_freq, bool hw_cs)
{
    _x_dots = x_dots;    _y_dots = y_dots;
    // calculate num of module (module bits 64)
    _num_of_module      = _x_dots * _y_dots / _DotsOfOneModule;
    _num_of_module_line = _y_dots / 8;

    if ((_p_matrix_table = new unsigned char[_num_of_module])!= NULL) {
        for (int i = 0; i < _num_of_module; i++) {
            _p_matrix_table[i] = (matrix_table != NULL) ? matrix_table[i] : i;
        }
    }
    // Send to Matrix buffer via SPI. Data format [Address, Data] * num of module
    _spi_send_bytes = _num_of_module * 2;
    _p_spi_buffer   = new unsigned char [_spi_send_bytes];

    // matrix data save buffer (One module requires 8 bytes.)
    _fb_size = _num_of_module * 8;
    // _p_fb[module number][column bits of module]
    _p_fb = new unsigned char [_fb_size];

    // SPI H/W init, Matrix Module init
    _init (spi_freq, hw_cs);
}

//------------------------------------------------------------------------------
lib_matrix::~lib_matrix ()
{
    if (_p_matrix_table)
        delete[]    _p_matrix_table;
    if (_p_spi_buffer)
        delete[]    _p_spi_buffer;
    if (_p_fb)
        delete[]    _p_fb;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
