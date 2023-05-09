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

//------------------------------------------------------------------------------
// weather check
#include <lib_weather.h>

//경기도 안양시만안구 석수2동 Rss
//http://www.kma.go.kr/wid/queryDFSRSS.jsp?zone=4117160000
WiFiClient	Client;
lib_weather weather(&Client, "석수2동", "/wid/queryDFSRSS.jsp?zone=4117160000");

//------------------------------------------------------------------------------
// SPI Matrix(MAX7219) lib
#include <lib_matrix.h>

#if defined(OFFICE_BILLBOARD)
    #define X_DOTS  64
    #define Y_DOTS  32

    const unsigned char MatrixMap[] = {
        0,  1,  2,  3,  4,  5,  6,  7,
        8,  9, 10, 11, 12, 13, 14, 15,
        16, 17, 18, 19, 20, 21, 22, 23,
        24, 25, 26, 27, 28, 29, 30, 31,
    };
#else
    #define X_DOTS  32
    #define Y_DOTS  16

    const unsigned char MatrixMap[] = {
        0,  1,  2,  3,
        4,  5,  6,  7,
    };
#endif

// Default SPI 1Mhz, HW cs = true
lib_matrix  matrix (X_DOTS, Y_DOTS, MatrixMap, 1000000, true);

//------------------------------------------------------------------------------
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
//------------------------------------------------------------------------------
void matrix_test (void)
{
    static unsigned char mem_value = 0x00;

    matrix.fill(mem_value--);
    matrix.update();
    delay(500);
}

//------------------------------------------------------------------------------
#if 0
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
#endif
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
void setup()
{
    // Board LED초기화. 동작상황 표시함.
    pinMode(2,  OUTPUT);

    Serial.begin(115200);

#if 0
    SPI.begin();
    SPI.setFrequency(10000000);
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
#endif
}

//------------------------------------------------------------------------------
char buf[512];
int s, s1, s2, x = 0, f = 0;
fb_info_t *FbInfo;

//------------------------------------------------------------------------------
void loop()
{
    matrix_test ();
#if 0
    timeClient.update();
    /* fixed fb test */
    FbInfo = fb_init (1920, 32);
    {
        unsigned long long epochTime = timeClient.getEpochTime();
        /*
            tm_sec: seconds after the minute;
            tm_min: minutes after the hour;
            tm_hour: hours since midnight;
            tm_mday: day of the month;
            tm_year: years since 1900;
            tm_wday: days since Sunday;
            tm_yday: days since January 1;
            tm_isdst: Daylight Saving Time flag;
        */
        struct tm *ptm = gmtime ((time_t *)&epochTime);

        memset(buf, 0x00, sizeof(buf));
        sprintf(buf, "NTP Server 현재시간 : %d년 %d월 %d일, %d시 %d분 %d초 %s.",
                ptm->tm_year + 1900,
                ptm->tm_mon+1,
                ptm->tm_mday,
                timeClient.getHours(),
                timeClient.getMinutes(),
                timeClient.getSeconds(),
                daysOfTheWeek[(int)timeClient.getDay()]);
        s1 = draw_text (FbInfo, 0, 0, 1, "%s", buf);
    }
    {
        String *Temp = weather.getData(W_DATA_TEMP);
        String *Reh = weather.getData(W_DATA_REH);
        String *WfKor = weather.getData(W_DATA_WF_KOR);
        String *WdKor = weather.getData(W_DATA_WD_KOR);
        String *Ws = weather.getData(W_DATA_WS);
        String *Pop = weather.getData(W_DATA_POP);

        memset(buf, 0x00, sizeof(buf));
        sprintf(buf, "%s 날씨 : 온도 %s도, 습도 %s%%, 풍향 %s, 풍속 %3.1fm/s, 강수확률 %s%%, 하늘 %s.",
                weather.getLocation(),
                Temp->c_str(),
                Reh->c_str(),
                WdKor->c_str(),
                atof(Ws->c_str()),
                Pop->c_str(),
                WfKor->c_str()
                );
        s2 = draw_text (FbInfo, 0, 16, 1, "%s", buf);
    }
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

    s = s1 > s2 ? s1 : s2;
    for (x = 0; x < s; x++) {
        if (FbInfo)     convert_to_matrix (FbInfo , x, 0);
        matrix_update ();
        delay(10);
        digitalWrite(2, 1);
        delay(10);
        digitalWrite(2, 0);
    }
    if (FbInfo)     fb_close(FbInfo);
#endif
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
