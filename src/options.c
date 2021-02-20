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

	#define X(type, name, format) \
         printf("%s -> "format"\n", #name, prms.name);
	X_FIELDS
	#undef X
}
