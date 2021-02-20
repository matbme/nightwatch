#include "options.h"

void parse_args(int argc, char **argv) {
	char ch;
	while ((ch = getopt_long(argc, argv, "d", long_options, NULL)) != -1) {
		switch (ch) {
			case 'd':
				dump_defaults();
				break;
		}
	}

	return;
}

void dump_defaults() {
	cf_params prms = initParams();
	char **ptr = &(prms.light_mode_time);

	for (int i = 0 ; i < sizeof(prms)/sizeof(char*) ; i++) {
		printf("%s -> %s\n", cf_params_names[i], *ptr);
		ptr += (sizeof(*ptr)/sizeof(char*));
	}
}
