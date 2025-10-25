#ifndef FILE_H
#define FILE_H

#include <stdbool.h>

int create_database_file(const char *filename);
int open_database_file(const char *filename);

#endif