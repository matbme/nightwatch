#ifndef SUNTIME_H_
#define SUNTIME_H_

#include <stdio.h>
#include <time.h>
#include <math.h>
#include "config_params.h"

#define PI 3.1415926
#define ZENITH -.83

void get_sun_times(char **sunrise, char **sunset, cf_params *obj);
float riseset(int year, int month, int day, float lat, float lng, int localOffset, int set);

#endif
