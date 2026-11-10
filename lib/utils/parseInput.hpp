#ifndef _PARSE
#define _PARSE

#include <core.hpp>
#include <stdio.h>
#include <filesystem>
#include <iostream>
#include <cstring>
#include <fstream>

int printOpts(options *opts);

int printOptsYTA(sYTA *yta_opts);

int parseGeneralInput(options *opts);

int parseInputYTA(sYTA *yta_opts);

#endif
