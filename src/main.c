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
  printf("example: final -n -f database.db\n");
}

int main(int argc, char *argv[]) {
  char *filepath = NULL;
  bool newfile = false;
  char *add_string = NULL;
  int c;

  int dbfd = -1;
  struct dbheader_t *dbhdr = NULL;
  struct employee_t *employees = NULL;

  while ((c = getopt(argc, argv, "nf:a:")) != -1) {
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

    printf("Database file created\n");
    if (create_db_header(&dbhdr) == STATUS_ERROR) {
      printf("Failed to create database header\n");
      return -1;
    }
    printf("Database header created\n");
    // output_file(dbfd, dbhdr_t, employees);
  } else {
    dbfd = open_db_file(filepath);
    if (dbfd == STATUS_ERROR) {
      printf("Failed to open database file\n");
      return -1;
    }
    printf("Database file opened\n");
    if (validate_db_header(dbfd, &dbhdr) == STATUS_ERROR) {
      printf("Failed to validate database header\n");
      return -1;
    }
    printf("Database header validated\n");
  }

  printf("Filepath: %s\n", filepath);
  printf("Newfile: %d\n", newfile);

  if (read_employees(dbfd, dbhdr, &employees) != STATUS_OK) {
    printf("Failed to read employees\n");
    return 0;
  }

  if (add_string) {
    dbhdr->count++;
    employees = realloc(employees, dbhdr->count*(sizeof(struct employee_t)));
    add_employee(dbhdr, employees, add_string);
  }

  if (output_file(dbfd, dbhdr, employees) == STATUS_ERROR) {
    printf("Failed to output file\n");
  }

  return 0;
}