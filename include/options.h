#ifndef OPTIONS_H_
#define OPTIONS_H_

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include "config_params.h"

static struct option long_options[] = {
	{"defaults", no_argument, NULL, 'd'}
};

void parse_args(int argc, char **argv);
void dump_defaults();

#endif
