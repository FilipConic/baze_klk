#include <stdio.h>
#include <stdlib.h>
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
	srand(123);
	for (size_t i = 0; i < 1000; ++i) {
		int curr = rand() % 1000000;
		seq_add(file, (SeqTuple){ .id = curr } );
		if ( rand() % 5 == 0 ) {
			seq_logic_delete(file, curr);
		}
	}
	// seq_print(file);
	fclose(file);
	return 0;
}
