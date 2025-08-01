#include "serial_file.h"
#include <stdint.h>
#include <string.h>
#include <stdio.h>

#define tuple_print(tup) printf("|%12ld|%-19s|%4d|%-10s|%-20s|\n", (tup)->id, (tup)->date, (tup)->type, (tup)->user_id, (tup)->description);

int serial_init(FILE* file) {
	if (!file) {
		return -1;
	}
	rewind(file);

	SerialBlock block = { 0 };
	block.data[0].id = -1;
	fwrite(&block, BLOCK_SIZE, sizeof(SerialTuple), file);

	return 0;
}
int serial_fill(FILE* file, const char* file_path) {
	if (!file) {
		return -1;
	}

	FILE* read_file = fopen(file_path, "r+");
	SerialTuple t = { 0 };
	int type;
	while (fscanf(read_file, "%ld %s %d %s %s\n", &t.id, t.date, &type, t.user_id, t.description) != EOF) {
		t.type = (SerialEventType)type;
		serial_append(file, t);
	}

	fclose(read_file);
	return 0;
}
int serial_append(FILE* file, SerialTuple t) {
	if (!file) {
		return -1;
	}
	rewind(file);

	SerialBlock block = { 0 };

	int read_num = fread(&block, sizeof(SerialTuple), BLOCK_SIZE, file);

	do {
		for (size_t i = 0; i < BLOCK_SIZE; ++i) {
			SerialTuple* read_tup = &block.data[i];

			if (read_tup->id == t.id) {
				return -1;
			}
			char flag = read_tup->flag;
			if (read_tup->id == -1 || flag) {
				memcpy(read_tup, &t, sizeof(SerialTuple));
				fseek(file, -sizeof(SerialBlock), SEEK_CUR);
				if (i + 1 < BLOCK_SIZE) {
					block.data[i + 1].id = -1;
				}
				fwrite(&block, sizeof(SerialTuple), BLOCK_SIZE, file);
				if (!flag && i + 1 == BLOCK_SIZE) {
					memset(&block, 0, sizeof(SerialBlock));
					block.data[0].id = -1;
					fwrite(&block, sizeof(SerialTuple), BLOCK_SIZE, file);
				}
				break;
			}
			
		}
	} while ((read_num = fread(&block, sizeof(SerialTuple), BLOCK_SIZE, file)));

	return 0;
}
int serial_print(FILE* file) {
	if (!file) {
		return -1;
	}
	rewind(file);

	printf("|%-12s|%-19s|type|%-10s|%-20s|\n", "id", "date-time", "user id", "description");
	printf("|------------|-------------------|----|----------|--------------------|\n");
	SerialBlock block = { 0 };
	int read_num = fread(&block, sizeof(SerialTuple), BLOCK_SIZE, file);
	do {
		for (size_t i = 0; i < BLOCK_SIZE; ++i) {
			SerialTuple* tup = &block.data[i];

			if (tup->id == -1) break;
			if (!tup->flag)
				tuple_print(tup);
		}
	} while ((read_num = fread(&block, sizeof(SerialTuple), BLOCK_SIZE, file)));

	return 0;
}
SerialTuple serial_find(FILE* file, int64_t id) {
	if (!file) {
		return (SerialTuple){ .id = -1 };
	}
	rewind(file);

	SerialTuple* t;
	SerialBlock block = { 0 };
	int read_num = fread(&block, sizeof(SerialTuple), BLOCK_SIZE, file);
	do {
		for (size_t i = 0; i < BLOCK_SIZE; ++i) {
			t = &block.data[i];
			if (!t->flag && (t->id == -1 || t->id == id)) {
				return *t;
			}
		}
	} while ((read_num = fread(&block, sizeof(SerialTuple), BLOCK_SIZE, file)));

	return *t;
}
int serial_update(FILE* file, SerialTuple t) {
	if (!file) {
		return -1;
	}
	rewind(file);

	SerialBlock block = { 0 };

	int read_num = fread(&block, sizeof(SerialTuple), BLOCK_SIZE, file);
	do {
		for (size_t i = 0; i < BLOCK_SIZE; ++i) {
			SerialTuple* read_tup = &block.data[i];

			if (read_tup->id == t.id) {
				memcpy(read_tup, &t, sizeof(SerialTuple));
				fseek(file, -sizeof(SerialBlock), SEEK_CUR);
				fwrite(&block, sizeof(SerialTuple), BLOCK_SIZE, file);
			}
			break;
		}
	} while ((read_num = fread(&block, sizeof(SerialTuple), BLOCK_SIZE, file)));

	return 0;
}
int serial_logic_delete(FILE* file, int64_t id) {
	if (!file) {
		return -1;
	}
	rewind(file);

	SerialBlock block = { 0 };
	int read_num = fread(&block, sizeof(SerialTuple), BLOCK_SIZE, file);
	do {
		for (size_t i = 0; i < BLOCK_SIZE; ++i) {
			SerialTuple* t = &block.data[i];

			if (t->id == id) {
				t->flag = 1;
			}
			break;
		}
	} while ((read_num = fread(&block, sizeof(SerialTuple), BLOCK_SIZE, file)));

	return 0;
}
int serial_delete(FILE* file, int64_t id) {
	if (!file) {
		return -1;
	}
	rewind(file);

	SerialBlock block = { 0 };
	int read_num = fread(&block, sizeof(SerialTuple), BLOCK_SIZE, file);
	char start_moving = 0;
	do {
		for (size_t i = 0; i < BLOCK_SIZE; ++i) {
			SerialTuple* t = &block.data[i];

			if (t->id == -1) {
				return -1;
			}

			if (start_moving) {
				int pos = ftell(file);
				SerialBlock saved = { 0 };
				if (!i) {
					fseek(file, -sizeof(SerialBlock), SEEK_CUR);
					read_num = fread(&saved, sizeof(SerialTuple), BLOCK_SIZE, file);
				}

				serial_append(file, *t);

				fseek(file, pos - sizeof(SerialBlock), SEEK_SET);
				if (read_num || !i) {
					saved.data[0].id = -1;
					fwrite(&saved, sizeof(SerialTuple), BLOCK_SIZE, file);
				}
			} else {
				if (t->id == id) {
					fseek(file, -sizeof(SerialBlock), SEEK_CUR);
					t->id = -1;
					fwrite(&block, sizeof(SerialTuple), BLOCK_SIZE, file);

					start_moving = 1;
				}
			}
		}
	} while ((read_num = fread(&block, sizeof(SerialTuple), BLOCK_SIZE, file)));

	return 0;
}

