//------------------------------------------------------------------------------
#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include "lib_weather.h"

//------------------------------------------------------------------------------
const char *ssid     = "Charles_2.4G";
const char *password = "hard4624";
const long utcOffsetInSeconds = 3600;
const char daysOfTheWeek[7][11] = {"일요일", "월요일", "화요일", "수요일", "목요일", "금요일", "토요일"};

// RSS 서비스
// https://www.weather.go.kr/w/pop/rss-guide.do

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);

WiFiClient	Client;
// 경기도 안양시만안구 석수2동 Rss
// http://www.kma.go.kr/wid/queryDFSRSS.jsp?zone=4117160000
lib_weather weather(&Client, "안양시 만안구 석수2동", "/wid/queryDFSRSS.jsp?zone=4117160000");

// 경기도 의왕시 오전동
// http://www.kma.go.kr/wid/queryDFSRSS.jsp?zone=4143053000
//lib_weather weather(&Client, "의왕시 오전동", "/wid/queryDFSRSS.jsp?zone=4143053000");

//------------------------------------------------------------------------------
void setup()
{
    // Board LED초기화. 동작상황 표시함.
    pinMode(2,  OUTPUT);

    Serial.begin(115200);

    WiFi.begin(ssid, password);

    while ( WiFi.status() != WL_CONNECTED ) {
        delay ( 500 );
        Serial.print ( "." );
    }
	Serial.printf("\r\nWIFI Connected. AP=%s\r\n", ssid);

    timeClient.begin();                 // NTP 클라이언트 초기화
    timeClient.setTimeOffset(32400);    // 한국은 GMT+9이므로 9*3600=32400
    timeClient.update();
}

//------------------------------------------------------------------------------
unsigned long priv_millis = 0, i = 0;
void loop()
{
    if (millis() - priv_millis > 10000) {
        priv_millis = millis();
        timeClient.update();
        Serial.printf ( "Weather data reading...(%ld) %d\r\n", priv_millis, weather.get_data_valid());
        if (weather.request_data()) {
            printf("%s\r\n", "안양시 만안구 석수2동을 표시함");
            printf("%s\r\n", weather.get_location_str());

            Serial.printf("%s\r\n", weather.get_data_str(W_DATA_TEMP));
            Serial.printf("%s\r\n", weather.get_data_str(W_DATA_TEMP_MAX));
            Serial.printf("%3.1f\r\n", atof(weather.get_data_str(W_DATA_WS)));
            Serial.println(*(weather.get_data(W_DATA_WF_KOR)));
            printf("%s\r\n", weather.get_data_str(W_DATA_WF_KOR));
            {
                String *tm = weather.get_data(W_DATA_TM);
                Serial.println(*tm);
                String split_tm;

                Serial.println(atoi(tm->substring(0, 4).c_str()));
                Serial.println(tm->substring(0, 4));    // year
                Serial.println(tm->substring(4, 6));    // month
                Serial.println(tm->substring(6, 8));    // day
                Serial.println(tm->substring(8, 10));   // hour
                Serial.println(tm->substring(10, 12));  // min
            }
            if (i++ % 2)
                weather.set_rss_url("의왕시 오전동", "/wid/queryDFSRSS.jsp?zone=4143053000");
            else
                weather.set_rss_url("안양시 만안구 석수2동", "/wid/queryDFSRSS.jsp?zone=4117160000");
        }
        else
            Serial.println("Weather data not valid.");
    }
}
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
