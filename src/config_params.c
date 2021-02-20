#ifndef CONFIG_PARAMS_C_
# define CONFIG_PARAMS_C_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "config_params.h"

#define MAX_SIZE 255
#define CONFIG_FILE_SEPARATOR " = "

#define typename(x) _Generic((x), \
    int:     getParamInt(path, var_name), \
    float:   getParamFloat(path, var_name), \
	char *:  getParamChar(path, var_name), \
	bool:	 getParamBool(path, var_name), \
    default: "other")

cf_params initParams() {
	cf_params fileConfigs = {
		.light_mode_time = "08:00",
		.dark_mode_time = "20:00",
		.latitude = 0,
		.longitude = 0,
		.use_sun_times = false
	};

	return fileConfigs;
}

char *readFile(char *path, char *param) {
	FILE *fp;
	char buf[MAX_SIZE] = {};
	char *tok;

	if ( (fp = fopen(path, "r") ) != NULL) {
		while(fgets(buf, MAX_SIZE, fp) != NULL) {
			/* Ignore comments and whitespaces */
			if ( (buf[0] == '#') || (buf[0] == '\n') ) 
				continue;

			/* Find parameter line and return its value */
			tok = strtok(buf, CONFIG_FILE_SEPARATOR);
			if ( !strcmp(param, tok) ) {
				char *val = strdup(strtok(0, CONFIG_FILE_SEPARATOR));
				fclose(fp);
				return val;
			}
		}
	}

	return 0;
}

char *getParamChar(char *path, char *param) {
	char *val = readFile(path, param);
	char filtered_val[sizeof(val) - 3] = {};
	int i = 0;

	while (i < strlen(val) - 1) { 
		if (val[i] != '"') {
			strncat(filtered_val, &val[i], 1);
		}
		i++;
	}

	return strdup(filtered_val);
}

int getParamInt(char *path, char *param) {
	// TODO: same for above but parse int
	// Don't feel like doing rn
	return 0;
}

float getParamFloat(char *path, char *param) {
	char *val = readFile(path, param);
	return atof(val);
}

bool getParamBool(char *path, char *param) {
	char *val = readFile(path, param);
	return (int) strcmp(val, "true\n") == 0 ? true: false;
}

void read_config_file(char *path, cf_params *params) {
	char *var_name;
#define X(type, name, format) \
		var_name = #name; \
		params->name = typename(params->name);
	X_FIELDS
#undef X
}
#endif
