//------------------------------------------------------------------------------
/**
 * @file billboard.c
 * @author charles-park (charles.park@hardkernel.com)
 * @brief Dot-Matrix Billboard Application.(esp8266 d1_mini)
 * @version 0.1
 * @date 2023-03-24
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
#include <lib_fb.h>

//------------------------------------------------------------------------------
// number of dot matrix(8x8), default
#define MATRIX_MODULE_COUNT     32 
//#define MATRIX_MODULE_LINES     2
#define MATRIX_MODULE_LINES     4

static int MatrixModuleCount = MATRIX_MODULE_COUNT;
static int MatrixModuleLines = MATRIX_MODULE_LINES;

#define MATRIX_MODULE_MAX   MATRIX_MODULE_COUNT
static unsigned char    pMatrixFb[MATRIX_MODULE_COUNT][8];

/* SPI Send Buffer : MatrixModuleCount * 2 */
static unsigned char    *pSpiBuffer;

//------------------------------------------------------------------------------
/* Dot matrix (max7219) init value */
const unsigned char MatrixInitCmd[][2] = {
    /* { reg, init val }*/
    /* shutdown : operation (D0 = 0) */
    {0x0c,0x00},
    /* Decode mode : No decode for digits 7-0(dot matrix led) */
    {0x09,0x00},
    /* dot led bright 0x0f(max) = 0.5W/ea */
//    {0x0a,0x01},
    {0x0a,0x08},
//    {0x0a,0x0f},
    /* scan limit : 8 line enable */
    {0x0b,0x07},
    /* shutdown : normal operation (D0 = 1) */
    {0x0c,0x01},
    /* Display Normal Operation (D0 = 0)*/
    {0x0f,0x00},
    /* Display Test Mode Operation (D0 = 1)*/
//    {0x0f,0x01},
};   

#define ARRAY_COUNT(x)  (sizeof(x) / sizeof(x[0]))
//------------------------------------------------------------------------------
void spi_write (void *buf, int size)
{
    SPI.transfer(buf, size);
}

//------------------------------------------------------------------------------
static void matrix_update (void)
{
    unsigned char i, j;

    for (i = 0; i < 8; i++)  {
        memset (pSpiBuffer, 0x00, MatrixModuleCount * 2);
        for (j = 0; j < MatrixModuleCount; j++) {
            /* digit number set (1-8) */
            pSpiBuffer [(j * 2) + 0] = i +1;
            /* digit data set */
            pSpiBuffer [(j * 2) + 1] = pMatrixFb[j][i];
        }
        spi_write(pSpiBuffer, MatrixModuleCount * 2);
    }
}

//------------------------------------------------------------------------------
/* dot matrix init */
//------------------------------------------------------------------------------
void matrix_init (void)
{
    unsigned char i, j;

    pSpiBuffer = (unsigned char *)malloc(MatrixModuleCount * 2);

    /* dummy data send */
    memset (pSpiBuffer, 0, MatrixModuleCount * 2);
    spi_write(pSpiBuffer, MatrixModuleCount * 2);
    delay(10);

    for (i = 0; i < (int)ARRAY_COUNT(MatrixInitCmd); i++)  {
        memset (pSpiBuffer, 0x00, MatrixModuleCount * 2);
        for (j = 0; j < MATRIX_MODULE_COUNT; j++) {
            pSpiBuffer[(j * 2) + 0] = MatrixInitCmd[i][0];
            pSpiBuffer[(j * 2) + 1] = MatrixInitCmd[i][1];
        }
        spi_write(pSpiBuffer, MatrixModuleCount * 2);
    }
}

//------------------------------------------------------------------------------
void matrix_test (void)
{
    static unsigned char mem_value = 0x00;

    memset(pMatrixFb, mem_value--, sizeof(pMatrixFb));
    matrix_update ();
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
fb_info_t *FbInfo;

void setup() 
{
//    pinMode(15, OUTPUT);
//    digitalWrite(15, 1);

    pinMode(2,  OUTPUT);

    Serial.begin(115200);

    SPI.begin();
    SPI.setFrequency(1000000);
    SPI.setHwCs(true);

    matrix_init();

    FbInfo = fb_init (128, 32, 32);
}

int i = 0;
void loop()
{
    Serial.println(i);

    matrix_test();
    digitalWrite(2, 1);
    delay(500);
    digitalWrite(2, 0);
    delay(500);
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
