#include "pch.h"
#include "..//Memory_allocator/MemoryAllocator.h"

TEST(TestCase1, TestName) {
	MemoryAllocator allocator;
	allocator.init();
	int* pi = (int*)allocator.alloc(sizeof(int));
	double* pd = (double*)allocator.alloc(sizeof(double));
	int* pa = (int*)allocator.alloc(10 * sizeof(int));
	allocator.dumpStat();
	allocator.dumpBlocks();
	allocator.free(pa);
	allocator.free(pd);
	allocator.free(pi);
	allocator.destroy();
}

TEST(TestCase2, TestName) {
	MemoryAllocator allocator;
	allocator.init();
	char* a = (char*)allocator.alloc(7);
	char* b = (char*)allocator.alloc(128);
	char* c = (char*)allocator.alloc(680);
	char* d = (char*)allocator.alloc(1024);
	char* e = (char*)allocator.alloc(1024 * 1024 * 10);
	allocator.dumpStat();
	allocator.dumpBlocks();
	allocator.free(a);
	allocator.free(b);
	allocator.free(c);
	allocator.free(d);
	allocator.free(e);
	allocator.destroy();
}

TEST(TestCase3, TestName) {
	MemoryAllocator allocator;
	allocator.init();
	char* a = (char*)allocator.alloc(23);
	char* b = (char*)allocator.alloc(73);
	allocator.free(a);
	char* c = (char*)allocator.alloc(376);
	char* d = (char*)allocator.alloc(1480);
	char* e = (char*)allocator.alloc(3640);
	vector<char*> data;
	for (int i = 0; i < 300; i++)
	{
		char* temp = (char*)allocator.alloc(16);
		data.push_back(temp);
	}
	allocator.free(data[210]);
	data.erase(data.begin() + 210);
	allocator.free(data[14]);
	data.erase(data.begin() + 14);
	allocator.free(data[135]);
	data.erase(data.begin() + 135);
	allocator.dumpStat();
	allocator.dumpBlocks();
	allocator.free(b);
	allocator.free(c);
	allocator.free(d);
	allocator.free(e);
	for(int i = 0; i < data.size(); i++)
		allocator.free(data[i]);
	allocator.destroy();
}