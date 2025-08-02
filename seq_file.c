#include "seq_file.h"
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#define block_write(block, file) fwrite(block, sizeof(SeqTuple), BLOCK_SIZE, file)
#define block_read(block, file) fread(block, sizeof(SeqTuple), BLOCK_SIZE, file)
static const SeqTuple empty_tuple = { .id = -1 };
static const SeqBlock empty_block = { empty_tuple };

int seq_init(FILE* file) {
	if (!file) {
		return -1;
	}
	rewind(file);

	SeqBlock block = { 0 };
	block.data[0].id = -1;
	block_write(&block, file);

	return 0;
}
static void seq_move(FILE* file, SeqTuple t) {
	if (t.id == -1) {
		block_write(&empty_block, file);
		return;
	}

	SeqTuple saved = { 0 };
	
	SeqBlock block = { 0 };
	block_read(&block, file);
	fseek(file, -sizeof(SeqBlock), SEEK_CUR);
	for (size_t i = 0; i < BLOCK_SIZE; ++i) {
		saved = block.data[i];
		memcpy(&block.data[i], &t, sizeof(SeqTuple));
		if (saved.flag) {
			block_write(&block, file);
			return;
		}
		t = saved;
		if (t.id == -1) {
			if (i + 1 == BLOCK_SIZE) {
				block_write(&block, file);
				block_write(&empty_block, file);
				return;
			}
			memcpy(&block.data[i + 1], &empty_tuple, sizeof(SeqTuple));
			block_write(&block, file);
			return;
	  	}
	}
	block_write(&block, file);
	seq_move(file, saved);
}
int seq_add(FILE* file, SeqTuple t) {
	if (!file) {
		return -1;
	}
	rewind(file);

	SeqBlock block = { 0 };
	int read_num = block_read(&block, file);
	do {
		for (size_t i = 0; i < BLOCK_SIZE; ++i) {
			SeqTuple* temp = &block.data[i];
			if (temp->id == -1) {
				memcpy(temp, &t, sizeof(SeqTuple));
				fseek(file, -sizeof(SeqBlock), SEEK_CUR);
				if (i + 1 == BLOCK_SIZE) {
					block_write(&block, file);
					block_write(&empty_block, file);
				} else {
					memcpy(&block.data[i + 1], &empty_tuple, sizeof(SeqTuple));
					block_write(&block, file);
				}
				return 0;
			}
			if (t.id == temp->id) {
				return -1;
			}
			if (t.id > temp->id) {
				continue;
			}

			SeqTuple save = *temp;
			memcpy(&block.data[i], &t, sizeof(SeqTuple));
			if (save.flag) {
				fseek(file, -sizeof(SeqBlock), SEEK_CUR);
				block_write(&block, file);
				return 0;
			}
			if (i + 1 == BLOCK_SIZE) {
				fseek(file, -sizeof(SeqBlock), SEEK_CUR);
				block_write(&block, file);
				seq_move(file, save);
				return 0;
			}
			t = save;
		}
	} while ((read_num = block_read(&block, file)));

	return 0;
}
int seq_print(FILE* file) {
	if (!file) {
		return -1;
	}
	rewind(file);

	SeqBlock block = { 0 };
	int read_num = block_read(&block, file);
	printf("|%-10s|%-10s|%-13s|%-15s|%-16s|%-4s|\n", "id", "membership", "isbn", "book_name", "date", "stat");
	printf("|----------|----------|-------------|---------------|----------------|----|\n");
	do {
		for (size_t i = 0; i < BLOCK_SIZE; ++i) {
			SeqTuple* temp = &block.data[i];
			if (temp->id == -1) {
				return 0;
			}
			if (!temp->flag)
				printf("|%10ld|%10d|%-13s|%-15s|%16s|%4d|\n", temp->id, temp->membership_num, temp->isbn, temp->book_name, temp->date, (int)temp->stat);
		}
	} while ((read_num = block_read(&block, file)));
	return 0;
}
SeqTuple seq_find(FILE* file, int64_t id) {
	if (!file) {
		return empty_tuple;
	}
	rewind(file);

	SeqBlock block = { 0 };
	int read_num = block_read(&block, file);
	do {
		for (size_t i = 0; i < BLOCK_SIZE; ++i) {
			SeqTuple* temp = &block.data[i];
			if (temp->id == -1 || id > temp->id) {
				return empty_tuple;
			}
			if (temp->id == id) {
				return *temp;
			}
		}
	} while ((read_num = block_read(&block, file)));

	return empty_tuple;
}
int seq_update(FILE* file, SeqTuple t) {
	if (!file) {
		return -1;
	}
	rewind(file);

	SeqBlock block = { 0 };
	int read_num = block_read(&block, file);
	do {
		for (size_t i = 0; i < BLOCK_SIZE; ++i) {
			SeqTuple* temp = &block.data[i];
			if (temp->id == -1 || t.id > temp->id) {
				return -1;
			}
			if (t.id == temp->id) {
				memcpy(temp, &t, sizeof(SeqTuple));
				fseek(file, -sizeof(SeqBlock), SEEK_CUR);
				block_write(&block, file);
				return 0;
			}
		}
	} while ((read_num = block_read(&block, file)));

	return -1;
}
int seq_logic_delete(FILE* file, int64_t id) {
	if (!file) {
		return -1;
	}
	rewind(file);

	SeqBlock block = { 0 };
	int read_num = block_read(&block, file);
	do {
		for (size_t i = 0; i < BLOCK_SIZE; ++i) {
			SeqTuple* temp = &block.data[i];
			if (temp->id == -1 || temp->id > id) {
				return -1;
			}
			if (temp->id == id) {
				temp->flag = 1;
				fseek(file, -sizeof(SeqBlock), SEEK_CUR);
				block_write(&block, file);
				return 0;
			}
		}
	} while ((read_num = block_read(&block, file)));

	return -1;
}
int seq_delete(FILE* file, int64_t id);
