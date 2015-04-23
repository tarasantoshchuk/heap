/**
* @file		Heap.c
* @brief	Declarations of additional functions and implementation of Heap.h
*/
#include "Heap.h"

HeapInfo* heap;

struct HeapInfo
{
	int sizeOfFreeMem;
	int sizeOfTwoBytesFreeMem;
	int sizeOfFourBytesFreeMem;
	char* pBitBlock;
	char* pCheckBitBlock;
	char* pStart;
	char* pEnd;
	char* pTwoBytesBitBlock;
	char* pTwoBytesStart;
	char* pTwoBytesEnd;
	char* pFourBytesBitBlock;
	char* pFourBytesStart;
	char* pFourBytesEnd;
	char* pFourBytesBitBlockEnd;
	char isPrecached;
};

/**
* @brief	modyfies parameter to match requirements of my heap
* @param	[in, out]	value to be modyfied
* @remark	value of *size is increased or the same after function
* @return	void
*/
void normalizeSize(size_t* size);

/**
* @brief	count full size of memory needed for heap
* @param	[in]	size of free (non-service) heap memory
* @return	size_t	full size of heap
*/
size_t fullSize(size_t size);

/**
* @brief	count size of service part of heap
* @param	[in]	size of free (non-service) heap memory
* @return	size_t	size of service part of heap
*/
size_t serviceSize(size_t size);

/**
* @brief	invert specified bit in byte pointed by pServiceByte
* @param	[in, out]	pointer to byte to be changed
* @param	[in]		number of bit to change
* @return	void
*/
void changeState(char* pServiceByte, int bitNumber);

/**
* @brief	checks if specified bit is equal to 0
* @param	[in]	pointer to service byte
* @param	[in]	number of bit to change
* @return	int		0		if specified bit is equal to 0
*					!= 0	if specified bit is not equal to 0
*/
int isFree(char* pServiceByte, int bitNumber);

/**
* @brief	find service byte for memory block in 2-byte precached memory
* @param	[in, out]	pointer to memory block
* @param	[out]		number of control bit
* @return	char*		pointer to control byte
*/
char* getServiceByte2(void* ptr, int* pBitNumber);

/**
* @brief	find service byte for memory block in 4-byte precached memory
* @param	[in, out]	pointer to memory block
* @param	[out]		number of control bit
* @return	char*		pointer to control byte
*/
char* getServiceByte4(void* ptr, int* pBitNumber);

/**
* @brief	allocates memory in 2-byte cache
* @return	void*	pointer to the beginning of allocated memory
* @remark	returns valid pointer only if heap->sizeOfTwoBytesFreeMem > 0
*/
void* mymalloc2();

/**
* @brief	allocates memory in 4-byte cache
* @return	void*	pointer to the beginning of allocated memory
* @remark	returns valid pointer only if heap->sizeOfFourBytesFreeMem > 0
*/
void* mymalloc4();

/**
* @brief	deallocates memory in 2-byte cache
* @return	void
*/
void myfree2(void* ptr);

/**
* @brief	deallocates memory in 4-byte cache
* @return	void
*/
void myfree4(void* ptr);

/**
* @brief	prints binary representation of byte to console
* @param	byte to print
* @return	void
*/
void printBin(char ch);



void normalizeSize(size_t* pSize)
{
	*pSize += *pSize % 8;
}

size_t fullSize(size_t size)
{
	return size + serviceSize(size);
}

size_t serviceSize(size_t size)
{
	return  sizeof(HeapInfo) + size / 4;
}

void printBin(char ch)
{
	char scaner = SCANER_BYTE;
	while (scaner != 0)
	{
		if (scaner & ch)
		{
			putchar('1');
		}
		else
		{
			putchar('0');
		}
		scaner <<= 1;
	}
	putchar('\t');
}

void map()
{
	printf("Common service part:\n");
	char* pCurrent = heap->pBitBlock;
	int i = 0;
	while (pCurrent != heap->pTwoBytesBitBlock)
	{

		printBin(*pCurrent);
		++pCurrent;
		++i;
		if (!(i % 4))
		{
			putchar('\n');
		}
	}
	i = 0;
	printf("\n");
	printf("Common check service part:\n");
	pCurrent = heap->pCheckBitBlock;
	while (pCurrent != heap->pStart)
	{
		printBin(*pCurrent);
		++pCurrent;
		++i;
		if (!(i % 4))
		{
			putchar('\n');
		}
	}
	i = 0;
	printf("\n");
	printf("2 bytes service part:\n");
	pCurrent = heap->pTwoBytesBitBlock;
	while (pCurrent != heap->pFourBytesBitBlock)
	{
		printBin(*pCurrent);
		++pCurrent;
		++i;
		if (!(i % 4))
		{
			putchar('\n');
		}
	}
	i = 0;
	printf("\n");
	printf("4 bytes service part:\n");
	pCurrent = heap->pFourBytesBitBlock;
	while (pCurrent != heap->pFourBytesBitBlockEnd)
	{
		printBin(*pCurrent);
		++pCurrent;
		++i;
		if (!(i % 4))
		{
			putchar('\n');
		}
	}
	printf("\n____________________________________________________________________\n");
}

void fullMap()
{
	printf("Common service part:\n");
	char* pCurrent = heap->pBitBlock;
	int i = 0;
	while (pCurrent != heap->pCheckBitBlock)
	{

		printBin(*pCurrent);
		++pCurrent;
		++i;
		if (!(i % 4))
		{
			putchar('\n');
		}
	}
	i = 0;
	printf("\n");
	printf("Common check service part:\n");
	while (pCurrent != heap->pStart)
	{
		printBin(*pCurrent);
		++pCurrent;
		++i;
		if (!(i % 4))
		{
			putchar('\n');
		}
	}
	printf("\n____________________________________________________________________\n");
}


void heapInit(size_t size)
{
	normalizeSize(&size);
	heap = (HeapInfo*)calloc(fullSize(size), sizeof(char));
	heap->sizeOfTwoBytesFreeMem = 16 * (size / (16 * 8));
	heap->sizeOfFourBytesFreeMem = 32 * (size / (32 * 8));
	heap->sizeOfFreeMem = size - heap->sizeOfTwoBytesFreeMem - heap->sizeOfFourBytesFreeMem;
	heap->pBitBlock = heap + 1;
	heap->pCheckBitBlock = heap->pBitBlock + size / 8;
	heap->pStart = (char*)heap + serviceSize(size);
	heap->pEnd = heap->pStart + heap->sizeOfFreeMem;
	heap->pTwoBytesBitBlock = heap->pBitBlock + heap->sizeOfFreeMem / 8;
	heap->pTwoBytesStart = heap->pEnd;
	heap->pTwoBytesEnd = heap->pTwoBytesStart + heap->sizeOfTwoBytesFreeMem;
	heap->pFourBytesBitBlock = heap->pTwoBytesBitBlock + heap->sizeOfTwoBytesFreeMem / 16;
	heap->pFourBytesStart = heap->pTwoBytesEnd;
	heap->pFourBytesEnd = heap->pFourBytesStart + heap->sizeOfFourBytesFreeMem;
	heap->pFourBytesBitBlockEnd = heap->pFourBytesBitBlock + heap->sizeOfFourBytesFreeMem / 32;
	heap->isPrecached = 1;
}

void heapDealloc()
{
	free(heap);
}

void heapReinit()
{
	size_t size = heap->pEnd - heap->pStart;
	heapDealloc();
	heapInit(size);
}

void* mymalloc(size_t size)
{
	if (size == 0)
	{
		return NULL;
	}
	// if not enough memory
	if (size > heap->sizeOfFourBytesFreeMem + heap->sizeOfTwoBytesFreeMem + heap->sizeOfFreeMem)
	{
		return NULL;
	}
	// if we can alloc memory in 2-byte block
	if ((size <= 2) && heap->sizeOfTwoBytesFreeMem)
	{
		return mymalloc2();
	}
	// if we can alloc memory in 4-byte block
	if ((size <= 4) && heap->sizeOfFourBytesFreeMem)
	{
		return mymalloc4();
	}
	char* pCurrent = heap->pBitBlock;
	char* pAllocStart = pCurrent;
	int startBit = 0;
	int endBit = 0;
	// search free block of bytes
	while (pCurrent != heap->pTwoBytesBitBlock)
	{
		char scaner = SCANER_BYTE;
		char curChar = *pCurrent;
		size_t tSize = size;
		//search free byte
		while (((scaner & curChar) != 0) && (pCurrent != heap->pTwoBytesBitBlock))
		{
			scaner <<= 1;
			if (scaner == 0)
			{
				scaner = SCANER_BYTE;
				curChar = *++pCurrent;
			}
			++startBit;
		}
		startBit %= BITS_IN_BYTE;
		endBit = startBit;
		pAllocStart = pCurrent;
		//check if block is long enough
		while ((--tSize) && (pCurrent != heap->pTwoBytesBitBlock))
		{
			scaner <<= 1;
			if (!scaner)
			{
				scaner = SCANER_BYTE;
				curChar = *++pCurrent;
			}
			if ((scaner & curChar) != 0)
			{
				break;
			}
			++endBit;
		}
		if (!tSize)
		{
			break;
		}
	}
	endBit %= BITS_IN_BYTE;
	//if we found block - change bits in service part
	if (pCurrent != heap->pTwoBytesBitBlock)
	{
		*pAllocStart ^= (FILLED_BYTE << startBit);
		if (pCurrent > pAllocStart)
		{
			memset(pAllocStart + 1, FILLED_BYTE, pCurrent - pAllocStart - 1);
		}
		*pCurrent ^= (FILLED_BYTE >> (BITS_IN_BYTE - endBit - 1));
		if (pAllocStart == pCurrent)
		{
			*pCurrent ^= FILLED_BYTE;
		}
		pCurrent += heap->pCheckBitBlock - heap->pBitBlock;
		*pCurrent |= (SCANER_BYTE << endBit);
		return heap->pStart + (pAllocStart - heap->pBitBlock) * BITS_IN_BYTE + startBit;
	}
	// break precaching and try to allocate memory again
	else if (heap->isPrecached)
	{
		heap->isPrecached = 0;
		char* pBitByte = heap->pFourBytesBitBlockEnd - 1;
		char* pServiceByte = heap->pCheckBitBlock;
		char* pCheckByte = heap->pStart;
		char scaner = SCANER_BYTE;
		// rewrite 2- and 4-bytes bit block into common service and check part
		while (pBitByte >= heap->pFourBytesBitBlock)
		{
			pServiceByte -= 4;
			pCheckByte -= 4;
			char bitByte = *pBitByte;
			*pBitByte = 0;
			for (int i = 0; i < BITS_IN_BYTE; i++)
			{
				if ((scaner << i) & bitByte)
				{
					pServiceByte[i / 2] |= (HALF_FILLED_BYTE << (4 * (i % 2)));
					pCheckByte[i / 2] |= (FOURTH_BIT << (4 * (i % 2)));
				}
				fullMap();
			}
			--pBitByte;
		}
		while (pBitByte >= heap->pTwoBytesBitBlock)
		{
			pServiceByte -= 2;
			pCheckByte -= 2;
			char bitByte = *pBitByte;
			*pBitByte = 0;
			for (int i = 0; i < BITS_IN_BYTE; i++)
			{
				if ((scaner << i) & bitByte)
				{
					pServiceByte[i / 4] |= (QUARTER_FILLED_BYTE << (2 * (i % 4)));
					pCheckByte[i / 4] |= (SECOND_BIT << (2 * (i % 4)));
				}
				fullMap();
			}
			--pBitByte;
		}
		// change some fields of HeapInfo
		heap->pTwoBytesBitBlock = heap->pCheckBitBlock;
		heap->pFourBytesBitBlock = heap->pCheckBitBlock;
		heap->pFourBytesBitBlockEnd = heap->pCheckBitBlock;
		heap->pFourBytesStart = heap->pFourBytesEnd;
		heap->pTwoBytesStart = heap->pTwoBytesEnd;
		heap->sizeOfFreeMem += heap->sizeOfTwoBytesFreeMem + heap->sizeOfFourBytesFreeMem;
		heap->sizeOfTwoBytesFreeMem = 0;
		heap->sizeOfFourBytesFreeMem = 0;
		// try to allocate memory again
		return mymalloc(size);
	}
	return NULL;
}

void* mymalloc2()
{
	heap->sizeOfTwoBytesFreeMem -= 2;
	int bitNumber = 0;
	char* pServiceByte = heap->pTwoBytesBitBlock;
	while(!isFree(pServiceByte, bitNumber))
	{
		++bitNumber;
		if (bitNumber == BITS_IN_BYTE)
		{
			bitNumber = 0;
			pServiceByte++;
		}
	}
	changeState(pServiceByte, bitNumber);
	return heap->pTwoBytesStart + (pServiceByte - heap->pTwoBytesBitBlock) * BITS_IN_BYTE * 2 + bitNumber * 2;
}

void* mymalloc4()
{
	heap->sizeOfFourBytesFreeMem -= 4;
	int bitNumber = 0;
	char* pServiceByte = heap->pFourBytesBitBlock;
	while (!isFree(pServiceByte, bitNumber))
	{
		++bitNumber;
		if (bitNumber == BITS_IN_BYTE)
		{
			bitNumber = 0;
			pServiceByte++;
		}
	}
	changeState(pServiceByte, bitNumber);
	return heap->pFourBytesStart + (pServiceByte - heap->pFourBytesBitBlock) * BITS_IN_BYTE * 4 + bitNumber * 4;
}


void myfree(void* ptr)
{
	// check if ptr is indeed in heap and locate it
	// (common part, 2-byte part, 4-byte part)
	if (ptr < heap->pStart || ptr > heap->pFourBytesEnd)
	{
		return;
	}
	else if (ptr >= heap->pTwoBytesStart && ptr < heap->pTwoBytesEnd)
	{
		myfree2(ptr);
	}
	else if (ptr >= heap->pFourBytesStart && ptr < heap->pFourBytesEnd)
	{
		myfree4(ptr);
	}
	else
	{
		char* pAllocStart = heap->pCheckBitBlock + ((char*)ptr - heap->pStart) / BITS_IN_BYTE;
		int bitNum = ((char*)ptr - heap->pStart) % BITS_IN_BYTE;
		char* pCurrent = pAllocStart;
		int curBitNum = bitNum;
		char scaner = SCANER_BYTE << bitNum;
		char curChar = *pCurrent;
		// search for end of block
		while (!(scaner & curChar) && (pCurrent != heap->pStart))
		{
			scaner <<= 1;
			if (!scaner)
			{
				scaner = SCANER_BYTE;
				curChar = *++pCurrent;
			}
			++curBitNum;
		}
		curBitNum %= BITS_IN_BYTE;
		*pCurrent ^= scaner;
		// set bits in service part to 0
		pCurrent += heap->pBitBlock - heap->pCheckBitBlock;
		pAllocStart += heap->pBitBlock - heap->pCheckBitBlock;
		*pAllocStart ^= (FILLED_BYTE << bitNum);
		if (pCurrent > pAllocStart)
		{
			memset(pAllocStart + 1, EMPTY_BYTE, pCurrent - pAllocStart - 1);
		}
		*pCurrent ^= (FILLED_BYTE >> (BITS_IN_BYTE - curBitNum - 1));
		if (pAllocStart == pCurrent)
		{
			*pCurrent ^= FILLED_BYTE;
		}
	}
}

void myfree2(void* ptr)
{
	int bitNumber;
	char* pServiceByte = getServiceByte2(ptr, &bitNumber);
	if (!isFree(pServiceByte, bitNumber))
	{
		changeState(pServiceByte, bitNumber);
	}
	heap->sizeOfTwoBytesFreeMem += 2;
}

void myfree4(void* ptr)
{
	int bitNumber;
	char* pServiceByte = getServiceByte4(ptr, &bitNumber);
	if (!isFree(pServiceByte, bitNumber))
	{
		changeState(pServiceByte, bitNumber);
	}
	heap->sizeOfTwoBytesFreeMem += 4;
}


char* getServiceByte2(void* pMemoryBlock, int* pBitNumber)
{
	int shift = ((char*)pMemoryBlock - heap->pTwoBytesStart) / 2;
	*pBitNumber = shift % 8;
	return heap->pTwoBytesBitBlock + shift / 8;
}

char* getServiceByte4(void* pMemoryBlock, int* pBitNumber)
{
	int shift = ((char*)pMemoryBlock - heap->pFourBytesStart) / 4;
	*pBitNumber = shift % 8;
	return heap->pFourBytesBitBlock + shift / 8;
}


int isFree(char* pServiceByte, int bitNumber)
{
	return !(*pServiceByte & (SCANER_BYTE << bitNumber));
}

void changeState(char* pServiceByte, int bitNumber)
{
	*pServiceByte ^= (SCANER_BYTE << bitNumber);
}

int toVirtualAddress(void* ptr)
{
	return (char*)ptr - heap->pStart;
}
