//------------------------------------------------------------------------------
/**
 * @file test_matrix.c
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
#include <lib_matrix.h>

//------------------------------------------------------------------------------
#define OFFICE_BILLBOARD

//------------------------------------------------------------------------------
/* 32 x 16 */
const unsigned char MatrixMap[] = {
     0,  1,  2,  3,  4,  5,  6,  7, 32, 33, 34, 35, 36, 37, 38, 39, 64, 65, 66, 67, 68, 69, 70, 71,
     8,  9, 10, 11, 12, 13, 14, 15, 40, 41, 42, 43, 44, 45, 36, 37, 72, 73, 74, 75, 76, 77, 78, 79,
    16, 17, 18, 19, 20, 21, 22, 23, 48, 49, 50, 51, 52, 53, 54, 55, 80, 81, 82, 83, 84, 85, 86, 87,
    24, 25, 26, 27, 28, 29, 30, 31, 56, 57, 58, 59, 60, 61, 62, 63, 88, 89, 90, 91, 92, 93, 94, 95,
};

// #define X_DOTS  64
// #define Y_DOTS  32

//#define	X_DOTS	128
//#define	Y_DOTS	16

//#define	X_DOTS	128
#define	X_DOTS	192
#define	Y_DOTS	32

// Default SPI 1Mhz, HW cs = true
//lib_matrix  matrix (X_DOTS, Y_DOTS, MatrixMap, 300000, true);
lib_matrix  matrix (X_DOTS, Y_DOTS, MatrixMap, 2000000, true);

//------------------------------------------------------------------------------
const unsigned char MatrixTestPattern[] = {
    0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01,
};

//------------------------------------------------------------------------------
void setup()
{
    Serial.begin(115200);
    // Board LED초기화. 동작상황 표시함.
    pinMode(2,  OUTPUT);
}

//------------------------------------------------------------------------------
unsigned char i = 0;
void loop()
{
    Serial.println("Loop Start");
#if 0
matrix.brightness(0x15);
matrix.fill(0xff);  matrix.update();
while (1) {
    delay(2000);
}
#endif
//    i %= 15;
//    matrix.brightness(i);
    matrix.brightness(1);
    Serial.printf("Brightness = %d\r\n", i++);

    matrix.fill(0xff);  matrix.update();
    delay(2000);
    matrix.fill(0x00);  matrix.update();
    delay(1000);
#if 1
#if 0
    for (int y = 0; y < matrix.get_y_dots(); y++) {
        for (int x = 0; x < matrix.get_x_dots(); x++) {
            matrix.set_bit(x, y, true);     matrix.update();    delay(5);
            matrix.set_bit(x, y, false);    matrix.update();    delay(5);
        }
    }
#endif
    for (unsigned int i = 0; i < sizeof(MatrixTestPattern); i++) {
        matrix.fill(MatrixTestPattern[i]);
        matrix.update();
        delay(500);
    }
    for (unsigned int i = sizeof(MatrixTestPattern); i > 0; i--) {
        matrix.fill(MatrixTestPattern[i]);
        matrix.update();
        delay(500);
    }
    for (int i = 0; i < 8; i++) {
        matrix.fill(0x00);

        for (int j = 0; j < matrix.num_of_module(); j++)
            // fb_set(module num, module line, line data)
            matrix.set_module_byte(j, i, 0xff);
        matrix.update();
        delay(500);
    }
    for (int i = 8-1; i > 0; i--) {
        matrix.fill(0x00);

        for (int j = 0; j < matrix.num_of_module(); j++)
            // fb_set(module, line, line data)
            matrix.set_module_byte(j, i, 0xff);
        matrix.update();
        delay(500);
    }

#if 0
    for (int j = 0; j < matrix.num_of_module(); j++) {
        matrix.fill(0x00);
        // fb_set(module num, module line, line data)
        for (int i = 0; i < 8; i++)
            matrix.set_module_byte(j, i, 0xff);
        matrix.update();
        delay(500);
    }
#endif
#endif
    Serial.println("Loop End");
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
