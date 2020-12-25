#pragma once
#include <windows.h>
#include <cassert>
#include <vector>
#define PAGE_SIZE (512 * 8)
#define SERV 2
#define DEBUG 1

using namespace std;

class FSA {
	void* head_page; // тык
	size_t block_size;
	int num_init;
	int FH; 
#ifdef DEBUG
	vector<void*> engaged_ptr;
#endif
public:
	FSA() {
		block_size = 0;
		num_init = 0;
		FH = -1; 
		head_page = nullptr;
	};

	~FSA() {
#ifdef DEBUG
		assert(head_page == nullptr);
#endif
	};

	void init(int bSize = 16) {
		block_size = bSize;
		void* temp = VirtualAlloc(nullptr, PAGE_SIZE + SERV * sizeof(void*), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
#ifdef DEBUG
		assert(temp != nullptr);
#endif
		*((void**)temp) = nullptr; // тык
		temp = (void*)((void**)temp + SERV);
		head_page = temp;
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
		int i = FH;
		while (i != -1)
		{
			int indx = i % (PAGE_SIZE / block_size);
			int ind_page = i / (PAGE_SIZE / block_size);
			i = *((int*)((char*)pages[ind_page] + indx * block_size));
			check += block_size;
		}
		check += (PAGE_SIZE / block_size - num_init) * block_size;
		assert(check == pages.size() * PAGE_SIZE);
		for (int i = 0; i < pages.size(); i++)
			assert(VirtualFree((void*)((void**)pages[i] - SERV), 0, MEM_RELEASE));
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

	void* alloc() {
#ifdef DEBUG
		assert(head_page != nullptr);
#endif
		if (num_init < PAGE_SIZE / block_size)
		{
			void* page = head_page; // тык
			void* next;
			while(true)
			{
				next = *((void**)page - SERV);
				if (next == nullptr)
					break;
				else
					page = next;
			}
			char* temp = (char*)page + num_init * block_size;
			num_init++; 
#ifdef DEBUG
			engaged_ptr.push_back((void*)temp);
#endif
			return (void*)temp;
		}
		else if (FH != -1)
		{
			int indx = FH % (PAGE_SIZE / block_size);
			int ind_page = FH / (PAGE_SIZE / block_size); 
			void* page = head_page; // тык
			for (int i = 0; i < ind_page; i++) 
				page = *((void**)page - SERV);
#ifdef DEBUG
			assert(page != nullptr);
#endif
			FH = *((int*)((char*)page + indx * block_size));
			char* temp = (char*)page + indx * block_size; 
#ifdef DEBUG
				engaged_ptr.push_back((void*)temp);
#endif
			return (void*)temp;
		}
		else
		{
			void* temp = VirtualAlloc(nullptr, PAGE_SIZE + SERV * sizeof(void*), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
#ifdef DEBUG
			assert(temp != nullptr);
#endif
			*((void**)temp) = nullptr; // тык
			temp = (void*)((void**)temp + SERV);
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
			*((void**)page) = temp;
			num_init = 1;
#ifdef DEBUG
			engaged_ptr.push_back(temp);
#endif
			return temp;
		}
	};

	void free(void* p) {
#ifdef DEBUG
		assert(head_page != nullptr);
#endif
		*((int*)p) = FH;
		int indx = -1;
		int ind_page = 0;
		void* page = head_page; // тык
		while (page != nullptr)
		{
			if ((p >= page) && (p < (void*)((char*)page + PAGE_SIZE)))
			{
				indx = ((char*)p - (char*)page) / block_size;
				break;
			}
			ind_page++;
			page = *((void**)page - SERV);
		}
#ifdef DEBUG
		assert(indx != -1);
#endif
		FH = ind_page * (PAGE_SIZE / block_size) + indx; 
#ifdef DEBUG
		for (int i = 0; i < engaged_ptr.size(); i++)
			if (engaged_ptr[i] == p)
				engaged_ptr.erase(engaged_ptr.begin() + i);
#endif
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
		cout << "\tFixed-size Allocator " << block_size << " bytes:" << endl;
		cout << "\t\tFree: ";
		vector<void*> pages;
#ifdef DEBUG
		assert(head_page != nullptr);
#endif
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
		int num = 0;
		int i = FH;
		while (i != -1)
		{
			num++;
			int indx = i % (PAGE_SIZE / block_size);
			int ind_page = i / (PAGE_SIZE / block_size);
			i = *((int*)((char*)pages[ind_page] + indx * block_size));
		}
		num += PAGE_SIZE / block_size - num_init;
		cout << num << " blocks";
		cout << "\t\tEngaged: " << pages.size() * (PAGE_SIZE / block_size) - num << " blocks" << endl;
	};

	virtual void dumpBlocks() const {
		cout << "\tFixed-size Allocator " << block_size << " bytes:" << endl;
		for (int i = 0; i < engaged_ptr.size(); i++)
			cout << "\t\t" << engaged_ptr[i] << ", size: " << block_size << endl;
	};
#endif

	/*void show() {
		cout << "Страниц: " << pages.size() << endl;
		cout << "Инициализированных блоков: " << num_init << endl;
		cout << "Free-list: " << FH;
		int i = FH;
		while (i != -1)
		{
			int indx = i % (PAGE_SIZE / block_size);
			int ind_page = i / (PAGE_SIZE / block_size);
			i = *((int*)((char*)pages[ind_page] + indx * block_size));
			cout << ", " << i;
		}
		cout << endl;
	};*/
};