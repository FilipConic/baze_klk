#ifndef SERIAL_FILE_H
#define SERIAL_FILE_H

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#define DATE_SIZE 20
#define USER_ID_SIZE 11
#define DESC_SIZE 21

typedef enum {
	INFO,
	WARNING,
	ERROR,
} SerialEventType;

typedef struct {
	int64_t id;
	char date[DATE_SIZE];
	SerialEventType type;
	char user_id[USER_ID_SIZE];
	char description[DESC_SIZE];
	char flag;
} SerialTuple;

#define BLOCK_SIZE 3

typedef struct {
	SerialTuple data[BLOCK_SIZE];
} SerialBlock;

int serial_init(FILE* file);
int serial_fill(FILE* file, const char* file_path);
#define serial_open(file_path) fopen(file_path, "wb+")
int serial_append(FILE* file, SerialTuple t);
int serial_print(FILE* file);
SerialTuple serial_find(FILE* file, int64_t id);
int serial_update(FILE* file, SerialTuple t);
int serial_logic_delete(FILE* file, int64_t id);
int serial_delete(FILE* file, int64_t id);

#endif // SERIAL_FILE_H
