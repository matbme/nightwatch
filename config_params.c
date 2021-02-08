#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "config_params.h"

#define MAX_SIZE 255
#define CONFIG_FILE_SEPARATOR " = "

#ifndef CONFIG_PARAMS_C_
# define CONFIG_PARAMS_C_

cf_params initParams() {
	cf_params fileConfigs = {
		.light_mode_time = "08:00",
		.dark_mode_time = "20:00"
	};

	return fileConfigs;
}

char *getParamChar(char *path, char *param) {
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
				char *param = strdup(strtok(0, CONFIG_FILE_SEPARATOR));

				char filtered_param[sizeof(param) - 3] = {};
				int i = 0;
				while (param[i] != '\n') { 
					if (param[i] != '"')
						strcat(filtered_param, &param[i]);
					i++;
				}

				free(param);
				fclose(fp);
				return strdup(filtered_param);
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

void read_config_file(char *path, cf_params *params) {
	params->light_mode_time = getParamChar(path, "light_mode_time");
	params->dark_mode_time = getParamChar(path, "dark_mode_time");
}
#endif
