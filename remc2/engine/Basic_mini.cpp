#include "Basic_mini.h"

bool config_EDITOR = false;
bool config_LOAD_EDITED_LEVEL = false;

uint8_t* Zero_pointer = NULL;

void qmemcpy(void* a, void* b, size_t c) {
	memcpy(a, b, c);
};
