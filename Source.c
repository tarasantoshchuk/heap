#include <stdlib.h>
#include "Heap.h"

#define HEAP_SIZE 0x100

int main()
{
	heapInit(HEAP_SIZE);
	int i = 0;
	map();
	while (mymalloc(HEAP_SIZE - 1))
	{
		map();
	}
	_getch();
}
