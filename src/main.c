#include <stdio.h>
#include <getopt.h>
#include <stdbool.h>
#include "file.h"
#include "common.h"
#include "parse.h"

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
	int c;
    struct dbheader_t *dbhdr_t = NULL;
	while ((c = getopt(argc, argv, "nf:")) != -1) {
		switch (c) {
			case 'n':
				newfile = true;
				break;
			case 'f':
				filepath = optarg;
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
        int dbfd = create_database_file(filepath);
        if (dbfd == STATUS_ERROR) {
            printf("Failed to create database file\n");
            return -1;
        }

        printf("Database file created\n");
        if (create_db_header(&dbhdr_t) == STATUS_ERROR) {
            printf("Failed to create database header\n");
            return -1;
        }
        printf("Database header created\n");
        output_file(dbfd, dbhdr_t);
    } else {
        int dbfd = open_database_file(filepath);
        if (dbfd == STATUS_ERROR) {
            printf("Failed to open database file\n");
            return -1;
        }
        printf("Database file opened\n");
        if (validate_db_header(dbfd, &dbhdr_t) == STATUS_ERROR) {
            printf("Failed to validate database header\n");
            return -1;
        }
        printf("Database header validated\n");
    }

    printf("Filepath: %s\n", filepath);
    printf("Newfile: %d\n", newfile);
    return 0;
}