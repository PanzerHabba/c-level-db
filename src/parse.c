#include "parse.h"
#include "common.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <netinet/in.h>
#include <unistd.h>

int create_db_header(struct dbheader_t **headerOut) {
    struct dbheader_t *header = calloc(1, sizeof(struct dbheader_t));
    if (header == NULL) {
        printf("Failed to allocate memory for header\n");
        return STATUS_ERROR;
    }
    header->version = 0x1;
    header->count = 0;
    header->magic = HEADER_MAGIC;
    header->filesize = sizeof(struct dbheader_t);

    *headerOut = header;
    return STATUS_OK;
}

int validate_db_header(int fd, struct dbheader_t **headerOut) {
    if (fd < 0) {
        printf("Invalid file descriptor\n");
        return STATUS_ERROR;
    }

    struct dbheader_t *header = calloc(1, sizeof(struct dbheader_t));
    if (header == NULL) {
        printf("Failed to allocate memory for header\n");
        free(header);
        return STATUS_ERROR;
    }
    printf("allocated memory for header\n");
    
    if (read(fd, header, sizeof(struct dbheader_t)) == STATUS_ERROR) {
        perror("read");
        free(header);
        return STATUS_ERROR;
    }
    printf("Header read from db file\n");
    
    header->version = ntohs(header->version);
    header->count = ntohs(header->count);
    header->magic = ntohl(header->magic);
    header->filesize = ntohl(header->filesize);

    if (header->version != 0x1) {
        printf("Invalid database version\n");
        free(header);
        return STATUS_ERROR;
    }

    if (header->magic != HEADER_MAGIC) {
        printf("Invalid database magic\n");
        free(header);
        return STATUS_ERROR;
    }

    struct stat dbstat = {0};
    if (fstat(fd, &dbstat) == STATUS_ERROR) {
        printf("Failed to get file status\n");
        free(header);
        return STATUS_ERROR;
    }

    if (header->filesize != dbstat.st_size) {
        printf("Corrupted database file\n");
        free(header);
        return STATUS_ERROR;
    }
    
    *headerOut = header;
    return STATUS_OK;
}

int output_file(int fd, struct dbheader_t *dbhdr, struct employee_t *employees) {
    printf("Outputting file\n");

    if (fd < 0) {
        printf("Invalid file descriptor\n");
        return STATUS_ERROR;
    }

    dbhdr->magic = htonl(dbhdr->magic);
    dbhdr->version = htons(dbhdr->version);
    dbhdr->count = htons(dbhdr->count);
    dbhdr->filesize = htonl(dbhdr->filesize);

    lseek(fd, 0, SEEK_SET);

    if (write(fd, dbhdr, sizeof(struct dbheader_t)) == STATUS_ERROR) {
        perror("write");
        return STATUS_ERROR;
    }

    return STATUS_OK;
}

// int read_employees(int fd, struct dbheader_t *, struct employee_t **employeesOut) {
    
// }