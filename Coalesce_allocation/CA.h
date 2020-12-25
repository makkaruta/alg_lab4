#pragma once
#include <windows.h>
#include <cassert>
#include <vector>
#define BUFFER (1024 * 1024 * 10) // 10 Мб
#define SERV 2
#define MIN_BLOCK 12
#define DEBUG 1

using namespace std;

struct BlockData {
	BlockData* next;
	BlockData* prev;
	size_t size;
};

class CA {
	void* head_page; // тык
	BlockData* FH;
#ifdef DEBUG
	vector<void*> engaged_ptr; 
	vector<size_t> engaged_size;
#endif
public:
	CA() {
		FH = nullptr; 
		head_page = nullptr;
	};

	~CA() {
#ifdef DEBUG
		assert(head_page == nullptr);
#endif
	};

	void init() {
		void* temp = VirtualAlloc(nullptr, BUFFER + SERV * sizeof(void*), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
#ifdef DEBUG
		assert(temp != nullptr);
#endif
		*((void**)temp) = nullptr; // тык
		temp = (void*)((void**)temp + SERV);
		head_page = temp;
		((BlockData*)temp)->next = nullptr;
		((BlockData*)temp)->prev = nullptr;
		((BlockData*)temp)->size = BUFFER;
		FH = (BlockData*)temp;
	};

	void destroy() {
#ifdef DEBUG
		vector<void*> pages;
		assert(head_page != nullptr);
		pages.push_back(head_page);
		void* next;
		while (true)
		{
			next = *((void**)pages.back() - SERV);
			if (next == nullptr)
				break;
			else
				pages.push_back(next);
		}
		size_t check = 0;
		for (BlockData* i = FH; i != nullptr; i = i->next)
			check += i->size;
		assert(check == pages.size() * BUFFER);
		for (int i = 0; i < pages.size(); i++)
			assert(VirtualFree((void*)((void**)pages[i] - SERV), 0, MEM_RELEASE));
		pages.clear();
#else
		void* page = head_page; // тык
		while (page != nullptr)
		{
			VirtualFree((void*)((void**)page - SERV), 0, MEM_RELEASE);
			page = *((void**)page - SERV);
		}
#endif
		head_page = nullptr;
	};

	void* alloc(size_t size) {
#ifdef DEBUG
		assert(head_page != nullptr);
#endif
		size = size + sizeof(size_t);
		size = size + (8 - size % 8);
		if (size < MIN_BLOCK)
			size = MIN_BLOCK;
		BlockData* temp = nullptr;
		for (BlockData* i = FH; i != nullptr; i = i->next)
		{
			if (i->size >= size)
			{
				temp = i;
				break;
			}
		}
		if (temp != nullptr)
		{
			if (temp->size - size < MIN_BLOCK)
				size = temp->size;
			if (temp->size - size != 0)
			{
				BlockData* remain = (BlockData*)((char*)temp + size);
				remain->next = temp->next;
				remain->prev = temp->prev;
				remain->size = temp->size - size;
				if (temp->next != nullptr)
					temp->next->prev = remain;
				if (temp->prev != nullptr)
					temp->prev->next = remain;
				if (FH == temp)
					FH = remain;
			}
			else
			{
				if (temp->next != nullptr)
					temp->next->prev = temp->prev;
				if (temp->prev != nullptr)
					temp->prev->next = temp->next;
				if (FH == temp)
					FH = temp->next;
			}
		}
		else
		{
			void* new_page = VirtualAlloc(nullptr, BUFFER + SERV * sizeof(void*), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
#ifdef DEBUG
			assert(temp != nullptr);
#endif
			*((void**)new_page) = nullptr; // тык
			new_page = (void*)((void**)new_page + SERV);
			void* page = head_page;
			void* next;
			while (true)
			{
				next = *((void**)page - SERV);
				if (next == nullptr)
					break;
				else
					page = next;
			}
			page = (void*)((void**)page - SERV);
			*((void**)page) = new_page;
			if (BUFFER - size < MIN_BLOCK)
				size = BUFFER;
			if (BUFFER - size != 0)
			{
				BlockData* remain = (BlockData*)((char*)new_page + size);
				assert(remain != nullptr);
				remain->next = FH;
				remain->prev = nullptr;
				remain->size = BUFFER - size;
				if (remain->next != nullptr)
					remain->next->prev = remain;
				FH = remain;
			}
			temp = (BlockData*)new_page;
		}
#ifdef DEBUG
		assert(temp != nullptr);
		engaged_ptr.push_back((void*)temp);
		engaged_size.push_back(size);
#endif
		*((size_t*)temp) = size;
		temp = (BlockData*)((size_t*)temp + SERV);
		return (void*)temp;
	};

	void free(void* p) {
#ifdef DEBUG
		assert(head_page != nullptr);
#endif
		p = (void*)((size_t*)p - SERV);
		size_t size = *((size_t*)p);
		BlockData* left = nullptr;
		BlockData* right = nullptr;
		for (BlockData* i = FH; i != nullptr; i = i->next)
		{
			if ((char*)i + i->size == (char*)p)
				left = i;
			if ((char*)i == (char*)p + size)
				right = i;
		}
		if (left != nullptr)
		{
			left->size = left->size + size;
			if (right != nullptr)
			{
				left->size = left->size + right->size;
				if (right->prev != nullptr)
					right->prev->next = right->next; 
				if (right->next != nullptr)
					right->next->prev = right->prev;
				if (FH == right)
					FH = right->next;
			}
		}
		else if (right != nullptr)
		{
			((BlockData*)p)->next = right->next;
			((BlockData*)p)->prev = right->prev;
			((BlockData*)p)->size = right->size + size;
			if (right->prev != nullptr)
				right->prev->next = (BlockData*)p;
			if (right->next != nullptr)
				right->next->prev = (BlockData*)p;
			if (FH == right)
				FH = (BlockData*)p;
		}
		else
		{
			((BlockData*)p)->next = FH;
			((BlockData*)p)->prev = nullptr;
			((BlockData*)p)->size = size;
			if (((BlockData*)p)->next != nullptr)
				((BlockData*)p)->next->prev = ((BlockData*)p);
			FH = (BlockData*)p;
		}
		for (int i = 0; i < engaged_ptr.size(); i++)
		{
			if (engaged_ptr[i] == p)
			{
				engaged_ptr.erase(engaged_ptr.begin() + i);
				engaged_size.erase(engaged_size.begin() + i);
			}
		}
	};

	vector<void*> controlled_memory() {
		vector<void*> temp;
#ifdef DEBUG
		assert(head_page != nullptr);
#endif
		temp.push_back(head_page);
		void* next;
		while (true)
		{
			next = *((void**)temp.back() - SERV);
			if (next == nullptr)
				break;
			else
				temp.push_back(next);
		}
		return temp;
	};

#ifdef DEBUG
	virtual void dumpStat() const {
		cout << "\tCoalesce Allocator: " << endl;
		cout << "\t\tFree: ";
		int num = 0;
		for (BlockData* i = FH; i != nullptr; i = i->next)
			num++;
		cout << num << " blocks";
		cout << "\t\tEngaged: " << engaged_ptr.size() << " blocks" << endl;
	};

	virtual void dumpBlocks() const {
		cout << "\tCoalesce Allocator: " << endl;
		for (int i = 0; i < engaged_ptr.size(); i++)
			cout << "\t\t" << engaged_ptr[i] << ", size: " << engaged_size[i] << endl;
	};
#endif

	/*void show() {
		cout << "Страниц: " << pages.size() << endl;
		cout << "Границы страниц:" << endl;
		for (int i = 0; i < pages.size(); i++)
			cout << "\t" << pages[i] << " - " << (void*)((char*)pages[i] + BUFFER) << endl;
		cout << "Free-list: " << endl;
		for (BlockData* i = FH; i != nullptr; i = i->next)
		{
			cout << "\t" << i << " - " << (void*)((char*)i + i->size) << ", size: " << i->size << endl;
		}
		cout << endl;
	};*/
};