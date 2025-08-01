#ifndef SEQUENTIAL_FILE_H
#define SEQUENTIAL_FILE_H

#include <stdint.h>
#include <stdio.h>

#define ISBN_SIZE 14
#define BOOK_NAME_SIZE 16
#define DATE_SIZE 17

typedef enum {
	ACTIVE,
	RETURNED,
} SeqStatus;

typedef struct {
	int64_t id;
	int32_t membership_num;
	char isbn[ISBN_SIZE];
	char book_name[BOOK_NAME_SIZE];
	char date[DATE_SIZE];
	SeqStatus stat;
	char flag;
} SeqTuple;

#define BLOCK_SIZE 3

typedef struct {
	SeqTuple data[BLOCK_SIZE];
} SeqBlock;

int seq_init(FILE* file);
#define seq_open(file_path) fopen(file_path, "wb+")
int seq_add(FILE* file, SeqTuple t);
int seq_print(FILE* file);
SeqTuple seq_find(FILE* file, int64_t id);
int seq_update(FILE* file, SeqTuple t);
int seq_logic_delete(FILE* file, int64_t id);
int seq_delete(FILE* file, int64_t id);


#endif // SEQUENTIAL_FILE_H
