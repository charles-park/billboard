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

#include <umm_malloc/umm_heap_select.h>

//------------------------------------------------------------------------------
#define OFFICE_BILLBOARD

#if defined(OFFICE_BILLBOARD)
    const char *ssid     = "Charles_2.4G";
    const char *password = "hard4624";
#else   // Home AP
    const char *ssid     = "charles";
    const char *password = "charles.park";
#endif

const long utcOffsetInSeconds = 3600;

const char daysOfTheWeek[7][11] = {"일요일", "월요일", "화요일", "수요일", "목요일", "금요일", "토요일"};

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);

//------------------------------------------------------------------------------
// number of dot matrix(8x8), default
#if defined(OFFICE_BILLBOARD)
    #define MATRIX_MODULE_COUNT     32
    #define MATRIX_MODULE_LINES     4
#else
    #define MATRIX_MODULE_COUNT     8
    #define MATRIX_MODULE_LINES     2
#endif

/* 32 x 16 */
const unsigned char MatrixMap[MATRIX_MODULE_COUNT] = {
     0,  1,  2,  3,  4,  5,  6,  7,
#if defined(OFFICE_BILLBOARD)
     8,  9, 10, 11, 12, 13, 14, 15,
    16, 17, 18, 19, 20, 21, 22, 23,
    24, 25, 26, 27, 28, 29, 30, 31,
#endif
};

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
    int x, y, x_bits, y_bits, module_x_cnt, matrix_fb_x, matrix_fb_y;

    unsigned char bit_mask = 0x80;

    module_x_cnt = MATRIX_MODULE_MAX / MatrixModuleLines;

    x_bits = (module_x_cnt      * 8);
    y_bits = (MatrixModuleLines * 8);

    memset(pMatrixFb, 0x00, sizeof(pMatrixFb));
    for (y = 0; y < y_bits; y++) {
        for (x = 0, bit_mask = 0x80; x < x_bits; x++) {
            // MatrixFb[(y / 8) * (x축 모듈갯수) + (x / 8)][y % 8] |= (fb[pos].uint != 0 ? bit_mask : 0);
            matrix_fb_x = (y / 8) * module_x_cnt + (x / 8);
            matrix_fb_y = (y % 8);

            /* Matrix position find from MatrixMap table */
            matrix_fb_x = MatrixMap[matrix_fb_x];
            if (((start_x + x) > fb_info->w) || ((start_x + x) < 0) ||
                ((start_y + y) > fb_info->h) || ((start_y + y) < 0)) {
                pMatrixFb[matrix_fb_x][matrix_fb_y] &= ~(bit_mask);
            } else {
                pMatrixFb[matrix_fb_x][matrix_fb_y] |=
                        (get_pixel (fb_info, x + start_x, y + start_y) ? bit_mask : 0);
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

//------------------------------------------------------------------------------
fb_info_t *FbInfo;

void setup()
{
    // Board LED초기화. 동작상황 표시함.
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
    FbInfo = fb_init (1920, 32);
}

char buf[256];
int s, s1, s2, x = 0, f = 0;
void loop()
{
    timeClient.update();

    fb_clear (FbInfo);
#if 0
    s1 = draw_text (FbInfo, 0, 0,  1, "%s", "한");
    /* if scale = 0 then ascii 8x8 font */
    s2 = draw_text (FbInfo, 0, 16, 0, "%s", "A");
    s2 = draw_text (FbInfo, 0, 24, 0, "%s", "a");
#endif

#if 1
    memset(buf, 0x00, sizeof(buf));
    sprintf(buf, "현재시간은 %d시 %d분 %d초",
            timeClient.getHours(),
            timeClient.getMinutes(),
            timeClient.getSeconds());
    s1 = draw_text (FbInfo, 0, 0, 1, "%s", buf);

    s2 = timeClient.getDay();
    memset(buf, 0x00, sizeof(buf));
    sprintf(buf, "오늘은 %s 입니다.",
            daysOfTheWeek[s2]);
    s2 = draw_text (FbInfo, 0, 16, 1, "%s", buf);
#endif

    convert_to_matrix (FbInfo, 0, 0);
    matrix_update ();
    delay(1000);
    printf ("s1 = %d, s2 = %d\r\n", s1, s2);
    {
        HeapSelectIram ephemeral;
        Serial.printf("\rIRAM free: %6d bytes\r\n", ESP.getFreeHeap());
        {
            HeapSelectDram ephemeral;
            Serial.printf("\rDRAM free: %6d bytes\r\n", ESP.getFreeHeap());
        }
    }

#if 0   /* Frmaebuffer debug */
{
    int x, y;
    printf("\n\r");
    for (y = 0; y < 32; y++) {
        for(x = 0; x < 80; x++) {
            if (get_pixel (FbInfo, x, y))   printf("#");
            else                            printf(".");
        }
        printf("\n\r");
    }
    printf("\n\r");
    for (y = 0; y < 16; y++) {
        for (x = 0x80; x != 0; x >>= 1) {
            if (pMatrixFb[0][y] & x)    printf("#");
            else                        printf(".");
        }
        printf("\n\r");
    }
}
#endif
#if 1
    s = s1 > s2 ? s1 : s2;
    for (x = 0; x < s; x++) {
        convert_to_matrix (FbInfo, x, 0);
        matrix_update ();
        delay(30);
        digitalWrite(2, 1);
        delay(30);
        digitalWrite(2, 0);
    }
#endif
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
