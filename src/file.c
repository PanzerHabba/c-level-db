#include "file.h"
#include "common.h"
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

int create_db_file(const char *filename) {

  // check if file exists
  if (open(filename, O_RDONLY) != -1) {
    printf("Database file already exists\n");
    return STATUS_ERROR;
  }

  // create file
  int fd = open(filename, O_CREAT | O_RDWR, 0644);
  if (fd == -1) {
    perror("open");
    return STATUS_ERROR;
  }
  return fd;
}

int open_db_file(const char *filename) {
  int fd = open(filename, O_RDWR, 0644);
  if (fd == -1) {
    perror("open");
    return STATUS_ERROR;
  }
  return fd;
}