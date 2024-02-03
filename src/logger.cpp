#include "logger.h"

#include "FreeRTOS.h"
#include "task.h"

#include <stdarg.h>
#include <stdio.h>

void logger(const char *fmt, ...) {
    printf("%s on core%u: ", pcTaskGetName(NULL), get_core_num());
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
}
