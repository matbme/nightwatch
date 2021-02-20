#ifndef CONFIG_PARAMS_H_
# define CONFIG_PARAMS_H_

#include <stdbool.h>

#define X_FIELDS \
	X(char *, light_mode_time, "%s") \
	X(char *, dark_mode_time, "%s") \
	X(float, latitude, "%f") \
	X(float, longitude, "%f") \
	X(bool, use_sun_times, "%1d")

typedef struct config_params {
#define X(type, name, format) type name;
	X_FIELDS
#undef X
} cf_params;

static const char *cf_params_names[] = {"light_mode_time", "dark_mode_time", "latitude", "longitude", "sunrise_time", "sunset_time", "use_sun_times"};

cf_params initParams();

char *readFile(char *path, char *param);

char *getParamChar(char *path, char *param);
int getParamInt(char *path, char *param);
float getParamFloat(char *path, char *param);
bool getParamBool(char *path, char *param);

void read_config_file(char *path, cf_params *params);

#endif
