#if !defined(_LOCAL_FRAMEWORK_H)
#define _LOCAL_FRAMEWORK_H
#include <stdio.h>
#include <sys/stat.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#define HASH_TABLE_INITIAL_SZ 7
#define HASH_TABLE_MAX_LOAD (double)0.65

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))


// #define BUFFSIZE 4096

#endif