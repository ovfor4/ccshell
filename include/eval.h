#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <iostream>
#include <vector>
#include <string>

#include "global.h"
#include "util.h"
#include "job.h"
#include "ground.h"
#include "parse.h"
#include "path.h"

namespace ov4
{

void eval(char *cmdline);

}
