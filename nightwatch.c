#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <dirent.h>

int main() {
	char *user = getlogin();
	char ltime[6] = {};
	char dtime[6] = {};
	time_t rtime = time(NULL);
	struct tm *ctime_t = localtime(&rtime);
	int tNextChange = 0;

	// read config file
	if (user == NULL) {
		puts("Unable to get current user!");
		exit(-1);
	}

	char path[] = "/home/";
	strcat(path, user);
	strcat(path, "/.config/nightwatch");

	char configPath[strlen(path) + 14];
	strcpy(configPath, path);
	strcat(configPath, "/nightwatchtrc");

	FILE *cfg = fopen(configPath, "r");
	if (cfg == NULL) {
		puts("No configuration file detected! Using defaults...");
		strcpy(ltime, "08:00");
		strcpy(dtime, "19:36");
	} else {
		puts("Getting config...");
	}

	int nl_time = ( (ltime[0] - '0') * 10 + (ltime[1] - '0') ) * 60 +
				  ( (ltime[3] - '0') * 10 + (ltime[4] - '0') );
	int nd_time = ( (dtime[0] - '0') * 10 + (dtime[1] - '0') ) * 60 +
				  ( (dtime[3] - '0') * 10 + (dtime[4] - '0') );

	while (1) {
		// read time
		rtime = time(NULL);
		ctime_t = localtime(&rtime);
		
		// determine state
		int nc_time = ctime_t -> tm_hour * 60 + ctime_t -> tm_min;

		puts(path);
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
					system(filePath);

					for(int i = 0 ; i < sizeof(filePath) ; i++) {
						filePath[i] = 0;
					}
				}
			}
			closedir(dir);
		} else {
			puts("Failed to locate themes folder!");
			exit(-1);
		}

		free(modePath);
		free(filePath);
		fflush(stdout);
		sleep(tNextChange * 60);
	}

	exit(0);
}
