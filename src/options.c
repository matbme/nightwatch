#include "options.h"

bool parse_args(int argc, char **argv, bool *v, char **c) {
	char ch;
	bool retval = false;

	while ((ch = getopt_long(argc, argv, "dvc:", long_options, NULL)) != -1) {
		switch (ch) {
			case 'd':
				dump_defaults();
				break;
			case 'v':
				*v = true;
				retval = true;
				break;
			case 'c':
				*c = optarg;
				retval = true;
				break;
		}
	}

	return retval;
}

void dump_defaults() {
	cf_params prms = initParams();

	#define X(type, name, format) \
         printf("%s -> "format"\n", #name, prms.name);
	X_FIELDS
	#undef X
}
