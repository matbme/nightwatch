#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "config_params.h"

#define MAX_SIZE 255
#define CONFIG_FILE_SEPARATOR " = "

char *getParamChar(char *path, char *param) {
	FILE *fp;
	char buf[MAX_SIZE];
	char *tok;

	if ( (fp = fopen(path, "r") ) != NULL) {
		while(fgets(buf, MAX_SIZE, fp) != NULL) {
			tok = strtok(buf, CONFIG_FILE_SEPARATOR);
			if ( !strcmp(param, tok) ) {
				return strdup(strtok(0, CONFIG_FILE_SEPARATOR));
			}
		}
	}

	return 0;
}

int getParamInt(char *path, char *param) {
	// TODO: same for above but parse int
	// Don't feel like doing rn
	return 0;
}

void read_config_file(char *path, cf_params params) {
	// TODO: Check if value is not in config file
	params.light_mode_time = getParamChar(path, "light_mode_time");
	params.dark_mode_time = getParamChar(path, "dark_mode_time");
}

