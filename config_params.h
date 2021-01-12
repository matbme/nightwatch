typedef struct config_params {
	char *light_mode_time;
	char *dark_mode_time;
} cf_params;

char *getParamChar(char *path, char *param);
int getParamInt(char *path, char *param);

cf_params *read_config_file(char *path);
