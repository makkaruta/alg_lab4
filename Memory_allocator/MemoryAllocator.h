#pragma once
#include <windows.h>
#include <cassert>
#include "..//MemoryAllocator/FSA.h"
#include "..//Coalesce_allocation/CA.h"
#define UPPER_BOUND_CA (1024 * 1024 * 10 - 8)
#define DEBUG 1

using namespace std;

class MemoryAllocator {
	FSA fsa16;
	FSA fsa32;
	FSA fsa64;
	FSA fsa128;
	FSA fsa256;
	FSA fsa512;
	CA ca;
#ifdef DEBUG
	vector<void*> blocks;
	vector<size_t> blocks_size;
#endif
public:

	virtual ~MemoryAllocator() {
#ifdef DEBUG
		assert(blocks.empty());
#endif
	};

	virtual void init() {
		fsa16.init(16);
		fsa32.init(32);
		fsa64.init(64);
		fsa128.init(128);
		fsa256.init(256);
		fsa512.init(512);
		ca.init();
	};

	virtual void destroy() {
		fsa16.destroy();
		fsa32.destroy();
		fsa64.destroy();
		fsa128.destroy();
		fsa256.destroy();
		fsa512.destroy();
		ca.destroy();
	};

	virtual void* alloc(size_t size) {
		void* temp;
		if (size <= 16)
			temp = fsa16.alloc();
		else if (size <= 32)
			temp = fsa32.alloc();
		else if (size <= 64)
			temp = fsa64.alloc();
		else if (size <= 128)
			temp = fsa128.alloc();
		else if (size <= 256)
			temp = fsa256.alloc();
		else if (size <= 512)
			temp = fsa512.alloc();
		else if (size <= UPPER_BOUND_CA)
			temp = ca.alloc(size);
		else
		{
			size = size + (8 - size % 8);
			temp = VirtualAlloc(nullptr, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
#ifdef DEBUG
			blocks.push_back(temp);
			blocks_size.push_back(size);
#endif
		}
		return temp;
	};

	virtual void free(void* p) {
		vector<void*> revise = fsa16.controlled_memory();
		for (int i = 0; i < revise.size(); i++)
		{
			if ((p >= revise[i]) && (p < (void*)((char*)revise[i] + PAGE_SIZE)))
			{
				fsa16.free(p);
				return;
			}
		}
		revise.clear();
		assert(revise.empty());
		revise = fsa32.controlled_memory();
		for (int i = 0; i < revise.size(); i++)
		{
			if ((p >= revise[i]) && (p < (void*)((char*)revise[i] + PAGE_SIZE)))
			{
				fsa32.free(p);
				return;
			}
		}
		revise.clear();
		assert(revise.empty());
		revise = fsa64.controlled_memory();
		for (int i = 0; i < revise.size(); i++)
		{
			if ((p >= revise[i]) && (p < (void*)((char*)revise[i] + PAGE_SIZE)))
			{
				fsa64.free(p);
				return;
			}
		}
		revise.clear();
		assert(revise.empty());
		revise = fsa128.controlled_memory();
		for (int i = 0; i < revise.size(); i++)
		{
			if ((p >= revise[i]) && (p < (void*)((char*)revise[i] + PAGE_SIZE)))
			{
				fsa128.free(p);
				return;
			}
		}
		revise.clear();
		assert(revise.empty());
		revise = fsa256.controlled_memory();
		for (int i = 0; i < revise.size(); i++)
		{
			if ((p >= revise[i]) && (p < (void*)((char*)revise[i] + PAGE_SIZE)))
			{
				fsa256.free(p);
				return;
			}
		}
		revise.clear();
		assert(revise.empty());
		revise = fsa512.controlled_memory();
		for (int i = 0; i < revise.size(); i++)
		{
			if ((p >= revise[i]) && (p < (void*)((char*)revise[i] + PAGE_SIZE)))
			{
				fsa512.free(p);
				return;
			}
		}
		revise.clear();
		assert(revise.empty());
		revise = ca.controlled_memory();
		for (int i = 0; i < revise.size(); i++)
		{
			if ((p >= revise[i]) && (p < (void*)((char*)revise[i] + PAGE_SIZE)))
			{
				ca.free(p);
				return;
			}
		}
#ifdef DEBUG
		for (int i = 0; i < blocks.size(); i++)
		{
			if (blocks[i] == p)
			{
				assert(VirtualFree(p, 0, MEM_RELEASE));
				blocks.erase(blocks.begin() + i);
				blocks_size.erase(blocks_size.begin() + i);
				return;
			}
		}
#else
		VirtualFree(p, 0, MEM_RELEASE);
#endif
		return;
	};

#ifdef DEBUG
	virtual void dumpStat() const {
		cout << "dumpStat:" << endl;
		fsa16.dumpStat();
		fsa32.dumpStat();
		fsa64.dumpStat();
		fsa128.dumpStat();
		fsa256.dumpStat();
		fsa512.dumpStat();
		ca.dumpStat();
		cout << "\tFrom OS: " << endl;
		cout << "\t\tEngaged: " << blocks.size() << " blocks" << endl;
	};

	virtual void dumpBlocks() const {
		cout << "dumpBlocks:" << endl;
		fsa16.dumpBlocks();
		fsa32.dumpBlocks();
		fsa64.dumpBlocks();
		fsa128.dumpBlocks();
		fsa256.dumpBlocks();
		fsa512.dumpBlocks();
		ca.dumpBlocks();
		cout << "\tFrom OS: " << endl;
		for (int i = 0; i < blocks.size(); i++)
			cout << "\t\t" << blocks[i] << ", size: " << blocks_size[i] << endl;
	};
#endif
	
};