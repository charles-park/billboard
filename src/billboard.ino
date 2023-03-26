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

#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

//------------------------------------------------------------------------------
const char *ssid     = "charles";
const char *password = "charles.park";

const long utcOffsetInSeconds = 3600;


char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);

//------------------------------------------------------------------------------
// number of dot matrix(8x8), default
#define MATRIX_MODULE_COUNT     8 
#define MATRIX_MODULE_LINES     2

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

/* 32 x 16 */
const unsigned char MatrixMap[MATRIX_MODULE_COUNT] = {
     0,  1,  2,  3,  4,  5,  6,  7,
};

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
    delay(10);

    memset (pMatrixFb, 0x00, sizeof(pMatrixFb));
    matrix_update();
}

//------------------------------------------------------------------------------
void matrix_test (void)
{
    static unsigned char mem_value = 0x00;

    memset(pMatrixFb, mem_value--, sizeof(pMatrixFb));
    matrix_update ();
}

//------------------------------------------------------------------------------
void convert_to_matrix (fb_info_t *fb_info, int start_x, int start_y)
{
    int x, y, pos, x_bits, y_bits, module_x_cnt, matrix_fb_x, matrix_fb_y;
    fb_color_u *fb = (fb_color_u *)fb_info->data;

    unsigned char bit_mask = 0x80;

    module_x_cnt = MATRIX_MODULE_MAX / MatrixModuleLines;

    x_bits = (module_x_cnt      * 8);
    y_bits = (MatrixModuleLines * 8);

    memset(pMatrixFb, 0x00, sizeof(pMatrixFb));
    for (y = 0; y < y_bits; y++) {
        for (x = 0, bit_mask = 0x80; x < x_bits; x++) {
//          MatrixFb[(y / 8) * (x축 모듈갯수) + (x / 8)][y % 8] |= (fb[pos].uint != 0 ? bit_mask : 0);
            matrix_fb_x = (y / 8) * module_x_cnt + (x / 8);
            matrix_fb_y = (y % 8);

            /* Matrix position find from MatrixMap table */
            matrix_fb_x = MatrixMap[matrix_fb_x];
            if (((start_x + x) > fb_info->w) || ((start_x + x) < 0) ||
                ((start_y + y) > fb_info->h) || ((start_y + y) < 0)) {
                pMatrixFb[matrix_fb_x][matrix_fb_y] |= 0;
            } else {
                pos = ((y + start_y) * fb_info->w) + (x + start_x);
                fb[pos].bits.a = 0;
                pMatrixFb[matrix_fb_x][matrix_fb_y] |= (fb[pos].uint != 0 ? bit_mask : 0);
            }
            bit_mask >>= 1;
            if (!bit_mask)
                bit_mask = 0x80;
        }
    }
}

//------------------------------------------------------------------------------
int my_strlen (char *str)
{
    int cnt = 0, err = strlen(str) +1;

    /* utf-8 에서 한글표현은 3바이트 */
    while ((*str != 0x00) && err--) {
        if (*str & 0x80) {
            str += 3;   cnt += 2;
        } else {
            str += 1;   cnt++;
        }
    }
    return cnt;
}

//------------------------------------------------------------------------------//------------------------------------------------------------------------------
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

    WiFi.begin(ssid, password);

    while ( WiFi.status() != WL_CONNECTED ) {
        delay ( 500 );
        Serial.print ( "." );
    }

    timeClient.begin();                 // NTP 클라이언트 초기화
    timeClient.setTimeOffset(32400);    // 한국은 GMT+9이므로 9*3600=32400
    timeClient.update();
    FbInfo = fb_init (256, 32, 32);
}

char buf[128];
int s1 = 0, x = 0;
void loop()
{
    timeClient.update();

    memset(buf, 0x00, sizeof(buf));
    sprintf(buf, "%d시 %d분 %d초 입니다.", 
            timeClient.getHours(),
            timeClient.getMinutes(),
            timeClient.getSeconds());

    fb_clear (FbInfo);
    s1 = draw_text (FbInfo, 0, 0, 1, 0, 1, "%s", buf);
    for (x = 0; x < s1; x++) {
draw_text (FbInfo, 0, 16, 1, 0, 1, "%s", buf);
        convert_to_matrix (FbInfo, x, 0);
        matrix_update ();
        delay(30);
        digitalWrite(2, 1);
        delay(30);
        digitalWrite(2, 0);
    }
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
