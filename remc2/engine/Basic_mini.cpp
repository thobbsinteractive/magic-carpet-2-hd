#include "Basic_mini.h"

uint8_t* Zero_pointer = NULL;

void qmemcpy(void* a, void* b, size_t c) {
	memcpy(a, b, c);
};
