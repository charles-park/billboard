//------------------------------------------------------------------------------
#include "lib_weather.h"

//------------------------------------------------------------------------------
void lib_weather::set_rss_url (char *location, char *rss_url)
{
    _data_valid = false;
    _url = " ";    _url = rss_url;

    // 한글 사용시 String class를 사용하는 경우 문제 발생됨.(정상적인 사이즈 계산이 안됨.)
    memset(_location, 0, sizeof(_location));
    memcpy(_location, location, strlen(location));
}

//------------------------------------------------------------------------------
bool lib_weather::request_data ()
{
    // WIFI가 연결되어있는지 확인
    if ((WiFi.status() != WL_CONNECTED)) {
        Serial.println("WIFI not connected.");
        return false;
    }
    if ((_url == NULL) || (_p_client == NULL)) {
        Serial.println("empty rss or wifi client.");
        return false;
    }
    _get_data();
    return  _data_valid;
}

//------------------------------------------------------------------------------
char* lib_weather::get_data_str (unsigned char w_item)
{
    if (!_data_valid)   return NULL;
    return	w_item < W_DATA_END ? (char *)_w_data[w_item].c_str() : NULL;
}

//------------------------------------------------------------------------------
String* lib_weather::get_data (unsigned char w_item)
{
    if (!_data_valid)   return NULL;
    return	w_item < W_DATA_END ? &_w_data[w_item] : NULL;
}

//------------------------------------------------------------------------------
void lib_weather::_data_parse()
{
    const String w_data_idx[] = {
        "temp", "tmx", "tmn", "sky", "pty", "wfKor", "wfEn", "pop",
        "r12", "s12", "ws", "wd", "wdKor", "wdEn", "reh", "r06", "s06", "tm"
    };

    while(_p_client->available()){
        String line = _p_client->readStringUntil('\e');

        for (int i = 0; i < W_DATA_END; i++) {
            _w_data[i] = " ";
            String idx_e = String("</") + w_data_idx[i] + String(">");
            int idx_l = line.indexOf(idx_e);
            if(idx_l > 0) {
                String idx_s = String("<") + w_data_idx[i] + String(">");
                /* 한글의 경우 정상적으로 표현되지 않는 경우가 생김 3바이트가 아닌 2바이트로 인식?? */
                _w_data[i]   = line.substring(line.indexOf(idx_s) + idx_s.length(), idx_l);

                /* 1개라도 정상적인 데이터가 들어온 경우 */
                _data_valid = true;
                // for debug
                // Serial.println(_w_data[i]);
            }
        }
    }
    Serial.println("Completed");
}

//------------------------------------------------------------------------------
void lib_weather::_get_data()
{
    uint32_t cur_millis = millis();

    // Data가 정상적으로 읽히지 않은 경우 계속하여 자료를 읽음.
    if (_data_valid) {
        // millis()함수의 경우 timer0를 사용하며 uint32 값을 1ms마다 계속 증가시킴.
        // 따라서 언젠가는 0으로 돌아가게 되어 값의 역전현상이 발생할 수 있음.
        // extern volatile unsigned long timer0_millis;
        if (cur_millis < _priv_millis)	{
            _priv_millis = cur_millis;
            return;
        }

        // 현재시간 - 데이터 읽어온시간 >= 1분인 경우 데이터 Refresh.
        // 처음 시작의 경우 (_priv_millis == 0) 바로 데이터 읽음.
        if (_priv_millis && ((cur_millis - _priv_millis) < _data_period_ms))
            return;
    }

    // 새로운 데이터가 읽기위하여 data 상태를 초기화함.
    _data_valid = false;

    // 데이터 읽어온 시간 저장
    _priv_millis = cur_millis;

    // 연결되어있고 시간이 지난경우 데이터 읽어 데이터값 기록.
    if(_p_client->connect(HOST_NAME, HTTP_PORT)) {
        _p_client->print(String("GET ") + _url + " HTTP/1.1\r\n" +
                    "Host: " + HOST_NAME + "\r\n" +
                    "Connection: close\r\n\r\n");
        delay(10);
        Serial.println("Client connecting....");
    } else {
        Serial.println("Client connect failed.");
        return;
    }
    while(_p_client->connected()){
        String line = _p_client->readStringUntil('\n');
        if(line == "\r") {
            Serial.println("Client connected... data parsing...");
            _data_parse();
            return;
        }
    }
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
lib_weather::lib_weather (WiFiClient *client, char *location, char *rss_url)
{
    _p_client = client;    set_rss_url(location, rss_url);
}

//------------------------------------------------------------------------------
lib_weather::~lib_weather ()
{
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
