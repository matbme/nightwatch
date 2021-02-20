/*
 * Adapted from https://edwilliams.org/sunrise_sunset_algorithm.htm
 */

#include "suntime.h"

void get_sun_times(char **sunrise, char **sunset, cf_params *obj) {
	float c_time;
	double h;
	float m;

	time_t rtime = time(NULL);
	struct tm ctime_t = {0};
	localtime_r(&rtime, &ctime_t);

	for (int i = 0 ; i < 2 ; i++) {
		c_time = riseset(ctime_t.tm_year + 1900,
						 ctime_t.tm_mon + 1,
						 ctime_t.tm_mday,
						 obj->latitude,
						 obj->longitude,
						 atol(ctime_t.tm_zone),
						 i);
		m = modf(c_time, &h) * 60;
		if (m > 59.5) { h++; m = 0; }

		if (!i) snprintf(*sunrise, 6, "%02.0f:%02.0f", h, m);
			else snprintf(*sunset, 6, "%02.0f:%02.0f", h, m);
	}
}

float riseset(int year, int month, int day, float lat, float lng, int localOffset, int set) {
    float N1 = floor(275 * month / 9);
    float N2 = floor((month + 9) / 12);
    float N3 = (1 + floor((year - 4 * floor(year / 4) + 2) / 3));
    float N = N1 - (N2 * N3) + day - 30;

    float lngHour = lng / 15.0;      

	float t;
	if (!set) t = N + ((6 - lngHour) / 24);
    	else t = N + ((18 - lngHour) / 24);

    float M = (0.9856 * t) - 3.289;
    float L = fmod(M + (1.916 * sin((PI/180)*M)) + (0.020 * sin(2 *(PI/180) * M)) + 282.634,360.0);
    float RA = fmod(180/PI*atan(0.91764 * tan((PI/180)*L)),360.0);

    float Lquadrant  = floor( L/90) * 90;
    float RAquadrant = floor(RA/90) * 90;
    RA = RA + (Lquadrant - RAquadrant);
    RA = RA / 15;

    float sinDec = 0.39782 * sin((PI/180)*L);
    float cosDec = cos(asin(sinDec));
    float cosH = (sin((PI/180)*ZENITH) - (sinDec * sin((PI/180)*lat))) / (cosDec * cos((PI/180)*lat));

	float H;
	if (!set) H = 360 - (180/PI)*acos(cosH);
		else H = (180/PI)*acos(cosH);

    H = H / 15;

    float T = H + RA - (0.06571 * t) - 6.622;
    float UT = fmod(T - lngHour,24.0);

    return UT + localOffset;

}


