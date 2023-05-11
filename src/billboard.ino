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
//------------------------------------------------------------------------------
// Reference Web-site
//------------------------------------------------------------------------------
// TCP Server / TCP Client communication example.
// http://sigmaprj.com/esp8266-tcp-client-server.html
//------------------------------------------------------------------------------
// Soft-AP Example
// https://arduino-esp8266.readthedocs.io/en/latest/esp8266wifi/soft-access-point-examples.html
//------------------------------------------------------------------------------
// OTA Example
// https://arduino-esp8266.readthedocs.io/en/latest/ota_updates/readme.html
// https://github.com/esp8266/Arduino/tree/master/libraries/ArduinoOTA
// https://community.platformio.org/t/esp8266-ota-partition-tables/24802
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
#define OFFICE_BILLBOARD

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
// weather check
#include <lib_weather.h>

WiFiClient	Client;

// 기상청 RSS 서비스 : https://www.weather.go.kr/w/pop/rss-guide.do
// 경기도 안양시만안구 석수2동 Rss
// http://www.kma.go.kr/wid/queryDFSRSS.jsp?zone=4117160000
lib_weather weather(&Client, "안양시 만안구 석수2동", "/wid/queryDFSRSS.jsp?zone=4117160000");
// lib_weather weather(&Client, "석수2동", "/wid/queryDFSRSS.jsp?zone=4117160000");
// 경기도 의왕시 오전동
// http://www.kma.go.kr/wid/queryDFSRSS.jsp?zone=4143053000
// lib_weather weather(&Client, "의왕시 오전동", "/wid/queryDFSRSS.jsp?zone=4143053000");

//------------------------------------------------------------------------------
// SPI Matrix(MAX7219) lib
#include <lib_matrix.h>

#if defined(OFFICE_BILLBOARD)
    #define X_DOTS  128
    #define Y_DOTS  16

    const unsigned char MatrixMap[] = {
        0,  1,  2,  3,  4,  5,  6,  7,
        16, 17, 18, 19, 20, 21, 22, 23,
        8,  9, 10, 11, 12, 13, 14, 15,
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
lib_matrix  matrix (X_DOTS, Y_DOTS, MatrixMap, 2000000, true);

//------------------------------------------------------------------------------
// Text Draw framebuffer setup
#include <lib_fb.h>

#define FB_W    1920
#define FB_H    16
#define FB_BPP  1

lib_fb fb(FB_W, FB_H, FB_BPP);

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void matrix_msg (char *msg)
{
    static unsigned char mem_value = 0x00;

    matrix.fill(mem_value--);
    matrix.update();
    delay(500);
}

//------------------------------------------------------------------------------
struct tm *get_tm (NTPClient *pNTPClient)
{
    unsigned long long epochTime = pNTPClient->getEpochTime();
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
    return gmtime ((time_t *)&epochTime);
}

//------------------------------------------------------------------------------
void copy_matrix_to_fb (int x_offset, int y_offset)
{
    matrix.fill(0x0);
    for (int y = 0; y < matrix.get_y_dots(); y++) {
        for (int x = 0; x < matrix.get_x_dots(); x++) {
            matrix.set_bit (x, y, fb.get_pixel(x_offset + x, y_offset + y));
        }
    }
    matrix.update();
    if (!x_offset)
        delay(1000);
}

//------------------------------------------------------------------------------
void setup()
{
    // Board LED초기화. 동작상황 표시함.
    pinMode(2,  OUTPUT);

    Serial.begin(115200);
    Serial.println ( "START" );

    timeClient.begin();                 // NTP 클라이언트 초기화
    timeClient.setTimeOffset(32400);    // 한국은 GMT+9이므로 9*3600=32400
    timeClient.update();

    // Dot matrix brightness (1 ~ 15)
    matrix.brightness(3);
    // weather request period 5 min.
    weather.set_period_ms(5 * 60 * 1000);

    fb.draw_text(0, 0, 1, "WIFI Init...");
    copy_matrix_to_fb (0, 0);

    WiFi.begin(ssid, password);

    while ( WiFi.status() != WL_CONNECTED ) {
        delay ( 500 );
        Serial.print ( "." );
    }
    Serial.print ( "\r\n" );
    Serial.printf( "WIFI Setup Complete. %s \r\n", ssid );
}

//------------------------------------------------------------------------------
bool loc = false;
void loop()
{
    timeClient.update();
    {
        struct tm *ptm = get_tm(&timeClient);
        int draw_x_w;

        fb.clear();

        draw_x_w = fb.draw_text(0, 0, 1,
            "NTP Server 현재시간 : %d년 %d월 %d일, %d시 %d분 %d초 %s. ",
            ptm->tm_year + 1900,
            ptm->tm_mon+1,
            ptm->tm_mday,
            timeClient.getHours(),
            timeClient.getMinutes(),
            timeClient.getSeconds(),
            daysOfTheWeek[(int)timeClient.getDay()]);

        if (weather.request_data()) {
            String *Temp = weather.get_data(W_DATA_TEMP);
            String *Reh = weather.get_data(W_DATA_REH);
            String *WfKor = weather.get_data(W_DATA_WF_KOR);
            String *WdKor = weather.get_data(W_DATA_WD_KOR);
            String *Ws = weather.get_data(W_DATA_WS);
            String *Pop = weather.get_data(W_DATA_POP);

            draw_x_w += fb.draw_text(draw_x_w, 0, 1,
                "%s 날씨 : 온도 %s도, 습도 %s%%, 풍향 %s, 풍속 %3.1fm/s, 강수확률 %s%%, 하늘 %s.",
                weather.get_location_str(),
                Temp->c_str(),
                Reh->c_str(),
                WdKor->c_str(),
                atof(Ws->c_str()),
                Pop->c_str(),
                WfKor->c_str());

            for (int i = 0; i < draw_x_w; i++) {
                copy_matrix_to_fb (i, 0);
                digitalWrite(2, 1);
                delay(5);
                digitalWrite(2, 0);
                delay(5);
            }

            fb.clear();
            fb.draw_text(0, 0, 1, "날씨 로딩중..");
            copy_matrix_to_fb (0, 0);

            if ((loc = !loc)) {
                weather.set_rss_url("의왕시 오전동", "/wid/queryDFSRSS.jsp?zone=4143053000");
            }
            else {
                weather.set_rss_url("안양시 만안구 석수2동", "/wid/queryDFSRSS.jsp?zone=4117160000");
            }
        }
    }
    {
        HeapSelectIram ephemeral;
        Serial.printf("\rIRAM free: %6d bytes\r\n", ESP.getFreeHeap());
        {
            HeapSelectDram ephemeral;
            Serial.printf("\rDRAM free: %6d bytes\r\n", ESP.getFreeHeap());
        }
    }
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
