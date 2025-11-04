#include "parse.h"
#include "common.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

    int real_count = dbhdr->count;

    dbhdr->magic = htonl(dbhdr->magic);
    dbhdr->version = htons(dbhdr->version);
    dbhdr->count = htons(dbhdr->count);
    dbhdr->filesize = htonl(sizeof(struct dbheader_t) + (sizeof(struct employee_t) * real_count));

    lseek(fd, 0, SEEK_SET);

    if (write(fd, dbhdr, sizeof(struct dbheader_t)) == STATUS_ERROR) {
        perror("write");
        return STATUS_ERROR;
    }

    for (int i=0; i<real_count; i++) {
        employees[i].hours = htonl(employees[i].hours);
        write(fd, &employees[i], sizeof(struct employee_t));
    }

    return STATUS_OK;
}

int read_employees(int fd, struct dbheader_t *dbhdr, struct employee_t **employeesOut) {
    if (fd < 0) {
        printf("Bad file descriptor\n");
        return STATUS_ERROR;
    }

    if (dbhdr == NULL) {
        printf("db-header is NULL!\n");
        return STATUS_ERROR;
    }

    int count = dbhdr->count;

    struct employee_t *employees = calloc(count, sizeof(struct employee_t));
    if (employees == NULL) {
        printf("Failed to allocate memory\n");
        return STATUS_ERROR;
    }

    if (read(fd, employees, count*sizeof(struct employee_t)) < 0) {
        printf("Failed read employees from file\n");
        return STATUS_ERROR;
    }

    for (int i = 0; i < count; i++) {
        employees[i].hours = ntohl(employees[i].hours);
    }

    *employeesOut = employees;
    return STATUS_OK;
}

int add_employee(struct dbheader_t *dbhdr, struct employee_t **employees, char *add_string) {
    
    if (dbhdr == NULL) return STATUS_ERROR;
    if (employees == NULL) return STATUS_ERROR;
    if (*employees == NULL) return STATUS_ERROR;
    if (add_string == NULL) return STATUS_ERROR;

    char *name = strtok(add_string, ",");
    if (name == NULL) return STATUS_ERROR;

    char *addr = strtok(NULL, ",");
    if (addr == NULL) return STATUS_ERROR;

    char *hours = strtok(NULL, ",");
    if (hours == NULL) return STATUS_ERROR;

    struct employee_t *local_employees = *employees;
    local_employees = realloc(local_employees, sizeof(struct employee_t)*dbhdr->count+1);
    if (local_employees == NULL) {
        printf("Could not reallocate employees\n");
        return STATUS_ERROR;
    }

    dbhdr->count++;
    
    strncpy(local_employees[dbhdr->count-1].name, name, sizeof(local_employees[dbhdr->count-1].name)-1);
    strncpy(local_employees[dbhdr->count-1].address, addr, sizeof(local_employees[dbhdr->count-1].address)-1);
    local_employees[dbhdr->count-1].hours = atoi(hours);

    *employees = local_employees;

    return STATUS_OK;
}