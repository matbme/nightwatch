#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <dirent.h>
#include "config_params.c"
#include "options.c"

#define MAX_SIZE 255
#define GEOCLUE_TIMEOUT 30

int watcher();
void saveToLog(char *msg, char *logPath);

int main(int argc, char **argv) {
	if (argc == 1) watcher();
		else parse_args(argc, argv);

	return 0;
}

int watcher() {
	cf_params prms = initParams();

	char *user = getlogin();
	time_t rtime = time(NULL);
	struct tm *ctime_t = localtime(&rtime);
	int tNextChange = 0;

	// read config file
	if (user == NULL) {
		puts("Unable to get current user!");
		exit(-1);
	}

	char *path = malloc(strlen(user) + 25);
	strcpy(path, "/home/");
	strcat(path, user);
	strcat(path, "/.config/nightwatch"); 

	char *configPath = malloc(strlen(path) + 14);
	strcpy(configPath, path);
	strcat(configPath, "/nightwatchrc");

	char *logPath = malloc(strlen(path) + 4);
	strcpy(logPath, path);
	strcat(logPath, "/log");

	FILE *cfg = fopen(configPath, "r");
	if (cfg == NULL) {
		puts("No configuration file detected! Using defaults...");
	} else {
		puts("Getting config...");
		read_config_file(configPath, &prms);
		puts("Config read!");
	}

	int nl_time = ( (prms.light_mode_time[0] - '0') * 10 + (prms.light_mode_time[1] - '0') ) * 60 +
				  ( (prms.light_mode_time[3] - '0') * 10 + (prms.light_mode_time[4] - '0') );
	int nd_time = ( (prms.dark_mode_time[0] - '0') * 10 + (prms.dark_mode_time[1] - '0') ) * 60 +
				  ( (prms.dark_mode_time[3] - '0') * 10 + (prms.dark_mode_time[4] - '0') );

	// system("/usr/lib/geoclue-2.0/demos/where-am-i"); //TODO: append path here

	while (1) {
		// read time
		rtime = time(NULL);
		ctime_t = localtime(&rtime);
		
		// determine state
		int nc_time = ctime_t -> tm_hour * 60 + ctime_t -> tm_min;

		char *modePath = malloc(strlen(path) + 11);
		if (nc_time > nl_time && nc_time < nd_time) {
			strcpy(modePath, path);
			strcat(modePath, "/LightMode/");

			tNextChange = nd_time - nc_time;
		} else {
			strcpy(modePath, path);
			strcat(modePath, "/DarkMode/");

			if (nc_time < nl_time) {
				tNextChange = nl_time - nc_time;
			} else {
				tNextChange = (1440 - nc_time) + nl_time;
			}
		}

		printf("\nTime until next change: %d minutes\n", tNextChange);

		char *filePath = calloc(strlen(modePath) + 255, sizeof(char));

		// run themes scripts
		DIR *dir;
		struct dirent *ent;
		if ((dir = opendir (modePath)) != NULL) {
			while ((ent = readdir (dir)) != NULL) {
				if (strcmp(ent->d_name, ".") && strcmp(ent->d_name, "..")) {
					strcpy(filePath, modePath);
					strcat(filePath, ent->d_name);

					memmove(filePath + 3, filePath, strlen(filePath) + 1);
					memcpy(filePath, "sh ", 3);

					printf("Executing script %s\n", filePath);
					int out = system(filePath); 
					if (out != 0) {
						char *msg = malloc(strlen(filePath) + 41);
						strcpy(msg, "Script exited with non-zero status code: ");
						strcat(msg, filePath);
						saveToLog(msg, logPath);
					}

					for(int i = 0 ; i < sizeof(filePath) ; i++) {
						filePath[i] = 0;
					}
				}
			}
			closedir(dir);
		} else {
			puts("Failed to locate scripts folder!");
			exit(-1);
		}

		free(modePath);
		free(filePath);
		fflush(stdout);
		sleep(tNextChange * 60);
	}

	return 0;
}

void saveToLog(char *msg, char *logPath) {
	FILE *log;
	time_t rtime = time(NULL);
	struct tm *ctime_t = localtime(&rtime);

	if ( (log = fopen(logPath, "a+")) != NULL ) {
		fprintf(log, "%d-%02d-%02d %02d:%02d:%02d - ",
				ctime_t->tm_year + 1900,
				ctime_t->tm_mon + 1,
				ctime_t->tm_mday,
				ctime_t->tm_hour,
				ctime_t->tm_min,
				ctime_t->tm_sec);
		fputs(msg, log);
		fputs("\n", log);
		fclose(log);
	}
}

// And now my watch has ended.
