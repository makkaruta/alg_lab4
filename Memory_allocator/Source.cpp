#include <windows.h>
#include <iostream>
#include <locale.h>
#include "MemoryAllocator.h"

int main() {
	MemoryAllocator allocator;
	allocator.init();
	char* a = (char*)allocator.alloc(7);
	char* b = (char*)allocator.alloc(128);
	char* c = (char*)allocator.alloc(680);
	char* d = (char*)allocator.alloc(1024);
	char* e = (char*)allocator.alloc(1024 * 1024 * 10 -4);
	char* f = (char*)allocator.alloc(1024 * 1024 * 10);
	allocator.free(a);
	allocator.free(b);
	allocator.free(c);
	allocator.free(d);
	allocator.free(e);
	allocator.free(f);
	allocator.destroy();
	return 0;
}