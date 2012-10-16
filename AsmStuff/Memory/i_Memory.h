#ifndef I_MEMORY_H
#define I_MEMORY_H

/********************************************************************
*																	*
*							Includes								*
*																	*
********************************************************************/

#include <malloc.h>
#include <memory.h>

#include "Memory.h"

/********************************************************************
*																	*
*							Flags									*
*																	*
********************************************************************/

#define MEM_USED 0x1	// Used memory

/********************************************************************
*																	*
*							Values									*
*																	*
********************************************************************/

/* tMEMBLOCK offsets */
#define _Prev  0x00	// Prev offset
#define _Next  0x04	// Next offset
#define _pFree 0x08	// pFree offset
#define _nFree 0x0C	// nFree offset
#define _Size  0x10	// Size offset
#define _DATA  0x14	// Data offset

#define _Pattern _pFree	// Pattern offset; coincident with pFree offset

/* tMEMBLOCK size */
#define BLOCK_SIZE _DATA	// Data begins at end of block

/* mMEMORY offsets */
#define _Pool	 0x0 // Pool offset
#define _Cache	 0x4 // Cache offset
#define _nUsed	 0x8 // nUsed offset
#define _nUnused 0xC // nUnused offset

/********************************************************************
*																	*
*							Types									*
*																	*
********************************************************************/

////////////////////////////////////////////////////////
// _tMEMBLOCK: Container for memory block information //
////////////////////////////////////////////////////////

typedef struct _tMEMBLOCK * fMEMBLOCK;	// Forward reference
typedef struct _tMEMBLOCK {
	fMEMBLOCK Prev;	// Pointer to last memory block in list
	fMEMBLOCK Next;	// Pointer to next memory block in list
	union {	// Shared memory
		struct {	// Unused-block format
			fMEMBLOCK pFree;	// Pointer to last free block
			fMEMBLOCK nFree;	// Pointer to next free block
		};	// End-struct
		struct {	// Used-block format
			Byte Pattern [8];	// Pattern used to identify memory
		};	// End-struct
	};	// End-union
	Dword Size;		// Size of memory block in bytes
	/* Byte Data []: Virtual byte stream */
} tMEMBLOCK, * ptMEMBLOCK;

///////////////////////////////////////////////////////////
// _mMEMORY: Encapsulation of memory control information //
///////////////////////////////////////////////////////////

typedef struct _mMEMORY {
	ptMEMBLOCK Pool;	// Main memory pool
	ptMEMBLOCK Cache;	// Cache of unused memory blocks
	Dword nUsed;		// Count of used memory blocks
	Dword nUnused;		// Count of unused memory blocks
} mMEMORY, * pmMEMORY;

/********************************************************************
*																	*
*							Implementation							*
*																	*
********************************************************************/

void MemRemoveFromFreeBlocks (void);

// Purpose:	Used to extract a block from the free blocks
// Input:	ESI : Memory block to extract
// Return:	No return value

void MemInsertIntoFreeBlocks (void);

// Purpose:	Used to place a block within the free blocks
// Input:	ESI : Memory block to insert
// Return:	No return value

long MemAdjoinBlocks (void);

// Purpose:	Used to adjoin memory
// Input:	ESI : Memory block to adjoin
// Return:	Amount of non-adjoinments

#endif // I_MEMORY_H