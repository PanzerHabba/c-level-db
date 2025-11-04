#include "common.h"
#include "file.h"
#include "parse.h"
#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

void print_usage() {
  printf("Usage: final -n -f <file> -p <port> -l\n");
  printf("Options:\n");
  printf("  -n      Create a new database file\n");
  printf("  -f      File path to the database file\n");
  printf("  -a      Add via CSV list of (name,address,hours)\n");
  printf("example: final -n -f database.db\n");
}

int main(int argc, char *argv[]) {
  char *filepath = NULL;
  bool newfile = false;
  bool list = false;
  char *add_string = NULL;
  int c;

  int dbfd = -1;
  struct dbheader_t *dbhdr = NULL;
  struct employee_t *employees = NULL;

  while ((c = getopt(argc, argv, "nf:a:l")) != -1) {
    switch (c) {
    case 'n':
      newfile = true;
      break;
    case 'f':
      filepath = optarg;
      break;
    case 'a':
      add_string = optarg;
      break;
    case 'l':
      list = true;
      break;
    case '?':
      printf("Unknown option -%c\n", c);
      break;
    default:
      return -1;
    }
  }

  if (filepath == NULL) {
    printf("File path is required\n");
    print_usage();
    return 0;
  }

  if (newfile) {
    dbfd = create_db_file(filepath);
    if (dbfd == STATUS_ERROR) {
      printf("Failed to create database file\n");
      return -1;
    }

    
    if (create_db_header(&dbhdr) == STATUS_ERROR) {
      printf("Failed to create database header\n");
      return -1;
    }

  } else {
    dbfd = open_db_file(filepath);
    if (dbfd == STATUS_ERROR) {
      printf("Failed to open database file\n");
      return -1;
    }

    if (validate_db_header(dbfd, &dbhdr) == STATUS_ERROR) {
      printf("Failed to validate database header\n");
      return -1;
    }
    
  }

  if (read_employees(dbfd, dbhdr, &employees) != STATUS_OK) {
    printf("Failed to read employees\n");
    return 0;
  }

  if (add_string) {
    add_employee(dbhdr, &employees, add_string);
  }

  if (list) {
    list_employees(dbhdr, employees);
  }

  if (output_file(dbfd, dbhdr, employees) == STATUS_ERROR) {
    printf("Failed to output file\n");
  }

  return 0;
}