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

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);

//경기도 안양시만안구 석수2동 Rss
//http://www.kma.go.kr/wid/queryDFSRSS.jsp?zone=4117160000
WiFiClient	Client;
lib_weather weather(&Client, "/wid/queryDFSRSS.jsp?zone=4117160000");

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
unsigned long priv_millis = 0;
void loop()
{
    if (millis() - priv_millis > 10000) {
        priv_millis = millis();
        timeClient.update();

        Serial.printf ( "Weather data reading...(%ld)\r\n", priv_millis);
        String *Temp = weather.getData(W_DATA_TEMP);

        Serial.printf("%s\r\n", Temp->c_str());
        Serial.println(*Temp);

        String *Ws = weather.getData(W_DATA_WS);

        Serial.printf("%3.1f\r\n", atof(Ws->c_str()));
        Serial.println(*Ws);

        String *WfKor = weather.getData(W_DATA_WF_KOR);
        Serial.println(*WfKor);
        {
            String *tm = weather.getData(W_DATA_TM);
            Serial.println(*tm);
            {
                union _tm_u {
                    struct {
                        char    min[2];
                        char    hour[2];
                        char    day[2];
                        char    month[2];
                        char    year[4];
                    }   _tm_s;
                    char tm_str[12];
                };

                union _tm_u _tm;

                memset(_tm.tm_str, 0, sizeof(_tm));
                sprintf(_tm.tm_str, "%s", tm->c_str());
                Serial.println(_tm.tm_str);
                Serial.printf("%04d-%02d-%02d, %d:%d\r\n",
                    atoi(_tm._tm_s.year),
                    atoi(_tm._tm_s.month),
                    atoi(_tm._tm_s.day),
                    atoi(_tm._tm_s.hour),
                    atoi(_tm._tm_s.min));
            }
        }
    }
}
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
