//------------------------------------------------------------------------------
#ifndef __LIB_WEATHER_H__

//------------------------------------------------------------------------------
#define __LIB_WEATHER_H__

#include <ESP8266WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <Arduino.h>

//------------------------------------------------------------------------------
/*
	기상청 RSS 설정 및 관련 자료
	https://devweather.kma.go.kr/weather/lifenindustry/sevice_rss.jsp
	https://www.weather.go.kr/w/resources/pdf/dongnaeforecast_rss.pdf
*/
/*
	RSS xml data format
	<data seq="0">			// 데이터 package는 3시간 단위로 묶어서 내보냄 (시간으로 확인됨)
		<hour>12</hour>		// 현재로 부터 12시까지의 데이터
		<day>0</day>		// 0 현재, 1 내일 2 모래
		<temp>11.0</temp>	// 온도
		<tmx>14.0</tmx>		// 최고온도 (모든 데이터가 정상적으로 들어온 경우만 생성)
		<tmn>-999.0</tmn>	// 최저온도 (모든 데이터가 정상적으로 들어온 경우만 생성)
		<sky>3</sky>		// 하늘상태 : 1 맑음, 3 구름많음, 4 흐림
		<pty>0</pty>		// 강수상태 : 0 없음, 1 비, 2 비/눈, 3 눈, 4 소나기, 5 빗방울, 6 빗방울/눈날림, 7 눈날림
		<wfKor>구름 많음</wfKor>			// 한국어 날씨 상태
		<wfEn>Mostly Cloudy</wfEn>		// 영어 날씨 상태
		<pop>20</pop>		// 강수확률(%)
		<r12>0.0</r12>		// 12시간 예상강수량
		<s12>0.0</s12>		// 12시간 예상적설량
		<ws>4.4</ws>		// 풍속(m/s)
		<wd>6</wd>			// 풍향 코드 : 0 ~ 7 (북, 북동, 동, 남동, 남, 남서, 서, 북서)
		<wdKor>서</wdKor>	// 풍향 한글
		<wdEn>W</wdEn>		// 풍향 영어
		<reh>55</reh>		// 습도 (%)
		<r06>0.0</r06>		// 6시간 예상강수량
		<s06>0.0</s06>		// 6시간 예상 적설량
	</data>
*/
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
/* Weather ITEM List */
//------------------------------------------------------------------------------
enum	w_data_e {
	W_DATA_TEMP = 0,
	W_DATA_TEMP_MAX,
	W_DATA_TEMP_MIN,
	W_DATA_SKY,
	W_DATA_PTY,
	W_DATA_WF_KOR,
	W_DATA_WF_EN,
	W_DATA_POP,
	W_DATA_RAIN_12,
	W_DATA_SNOW_12,
	W_DATA_WS,
	W_DATA_WD,
	W_DATA_WD_KOR,
	W_DATA_WD_EN,
	W_DATA_REH,
	W_DATA_RAIN_6,
	W_DATA_SNOW_6,
	W_DATA_TM,
	W_DATA_END
};

//------------------------------------------------------------------------------
#define	HTTP_PORT	80
#define	HOST_NAME	"www.kma.go.kr"

//------------------------------------------------------------------------------
class lib_weather
{
private:
	/* data */
	uint32_t	_data_period_ms	= 1000 * 60; // default 1 min
	uint32_t	_priv_millis	= 0;

	char		*_p_rss_url = NULL;
	WiFiClient	*_p_client  = NULL;

	// 지역 정보
	String _location = "";
	// 발표시간
	String _tm = "";
	String _w_data[W_DATA_END];

	void _dataParse();
	void _getData();

public:
	lib_weather(/* args */);
	lib_weather(WiFiClient *client, const char *url);
	~lib_weather();

	String *getData(unsigned char w_item);
	void	setRssUrl(const char *url);
	void	setClient(WiFiClient *client);
	void	setDataPeriod(uint32_t period_ms);
};

//------------------------------------------------------------------------------
#endif	// __LIB_WEATHER_H__
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
