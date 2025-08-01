#include <stdio.h>
#include "serial_file.h"
#include "seq_file.h"

int main() {
	FILE* file = serial_open("data.bin");

	serial_init(file);
	serial_fill(file, "./data.csv");
	serial_delete(file, 12);
	// serial_print(file);

	fclose(file);
	file = seq_open("seq.bin");
	seq_init(file);
	seq_print(file);
	seq_add(file, (SeqTuple){ .id = 20 });
	seq_add(file, (SeqTuple){ .id = 95 });
	seq_add(file, (SeqTuple){ .id = 25 });
	seq_add(file, (SeqTuple){ .id = 15 });
	seq_print(file);
	fclose(file);
	return 0;
}
