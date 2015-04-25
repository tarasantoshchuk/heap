/**
* @file		Heap.h
* @brief	Declarations of functionality of my heap
*/
#ifndef _HEAP_H_
#define _HEAP_H_

#include <string.h>

#define NULL				0		
#define BITS_IN_BYTE		8
#define FILLED_BYTE			0xff	/* 11111111 */
#define HALF_FILLED_BYTE	0x0f	/* 00001111 */
#define QUARTER_FILLED_BYTE	0x03	/* 00000011 */
#define SCANER_BYTE			0x1		/* 00000001 */
#define FOURTH_BIT			0x08	/* 00001000 */
#define SECOND_BIT			0x02	/* 00000010 */
#define EMPTY_BYTE			0		/* 00000000 */


typedef unsigned int size_t;		/* unsigned integral type*/

typedef struct HeapInfo HeapInfo;	/* struct with basic info about my heap*/

extern HeapInfo* heap;	/**< Pointer to beginning of my heap*/

/**
* @brief	allocates memory in real heap, initialize HeapInfo
* @return	void
*/
void heapInit(size_t size);

/**
* @brief	deallocates memory used by my heap
* @return	void
*/
void heapDealloc();

/**
* @brief	reallocates memory used by my heap
* @return	void
* @remark	all pointer to memory blocks allocated in my heap
*			are no longer valid
*/
void heapReinit();

/**
* @brief	allocates memory of specified size
* @param	[in]	size (in bytes) to allocate
* @return	void*	pointer to the beginning of allocated memory
*			or NULL if failed to allocate
*/
void* mymalloc(size_t size);

/**
* @brief	deallocates memory in heap
* @return	void
*/
void myfree(void* ptr);

/**
* @brief	prints state of my heap to console
* @return	void
* @remark	takes into consideration precached blocks
*/
void map();

/**
* @brief	prints state of my heap to console
* @return	void
* @remark	does not take into consideration precached blocks
*/
void fullMap();

/**
* @brief	counts virtual address of memory block pointed by ptr
* @param	[in]	pointer to convert
* @return	int		virtual address - shift in my heap
*/
int toVirtualAddress(void* ptr);


#endif /* _HEAP_H_ */
