#ifndef OPTIONS_H_
#define OPTIONS_H_

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <stdbool.h>
#include "config_params.h"

static struct option long_options[] = {
	{"defaults", no_argument, NULL, 'd'},
	{"verbose", no_argument, NULL, 'v'},
	{"configPath", required_argument, NULL, 'c'}
};

bool parse_args(int argc, char **argv, bool *v, char **c);
void dump_defaults();

#endif
