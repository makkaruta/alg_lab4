#include <iostream>
#include <locale.h>
#include "FSA.h"
#include <vector>
using namespace std;

void show_data(vector<int*>data_temp) {
	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
	cout << "Data: ";
	for (int j = 0; j < data_temp.size() - 1; j++)
	{
		if (data_temp[j] != nullptr)
			cout << *data_temp[j] << ", ";
		else
		{
			SetConsoleTextAttribute(handle, FOREGROUND_RED | FOREGROUND_INTENSITY);
			cout << -1;
			SetConsoleTextAttribute(handle, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
			cout << ", ";
		}
	}
	cout << *data_temp[data_temp.size() - 1] << endl << endl;
};

int main(){
	setlocale(LC_ALL, "RUS"); 
	/*FSA allocator;
	vector<int*> data;
	allocator.init(512);
	allocator.show();
	cout << endl;

	for (int i = 0; i < 6; i++)
	{
		int* temp = (int*)allocator.alloc();
		data.push_back(temp);
		*(data.back()) = i;
		allocator.show();
		show_data(data);
	}
	allocator.free(data[3]);
	data[3] = nullptr;

	allocator.show();
	show_data(data);

	for (int i = 6; i < 10; i++)
	{
		int* temp = (int*)allocator.alloc();
		data.push_back(temp);
		*(data.back()) = i;
		allocator.show();
		show_data(data);
	}

	allocator.free(data[7]);
	data[7] = nullptr;

	allocator.free(data[1]);
	data[1] = nullptr;

	allocator.show();
	show_data(data);

	for (int i = 10; i < 17; i++)
	{
		int* temp = (int*)allocator.alloc();
		data.push_back(temp);
		*(data.back()) = i;
		allocator.show();
		show_data(data);
	}

	allocator.free(data[11]);
	data[11] = nullptr;

	allocator.free(data[5]);
	data[5] = nullptr;

	allocator.free(data[14]);
	data[14] = nullptr;

	allocator.free(data[8]);
	data[8] = nullptr;

	allocator.show();
	show_data(data);

	for (int i = 17; i < 25; i++)
	{
		int* temp = (int*)allocator.alloc();
		data.push_back(temp);
		*(data.back()) = i;
		allocator.show();
		show_data(data);
	}
	*/
	return 0;
}