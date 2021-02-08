#ifndef CONFIG_PARAMS_H_
# define CONFIG_PARAMS_H_

typedef struct config_params {
	char *light_mode_time;
	char *dark_mode_time;
} cf_params;

static const char *cf_params_names[] = {"light_mode_time", "dark_mode_time"};

cf_params initParams();

char *getParamChar(char *path, char *param);
int getParamInt(char *path, char *param);

void read_config_file(char *path, cf_params *params);

#endif
