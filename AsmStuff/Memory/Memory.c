/********************************************************************
*																	*
*							Includes								*
*																	*
********************************************************************/

#include "i_Memory.h"

/********************************************************************
*																	*
*							Internals								*
*																	*
********************************************************************/

PRIVATE mMEMORY MemMgr;	// Memory manager; linkage restricted to memory library

/********************************************************************************
*																				*
*								MemInit											*
*																				*
********************************************************************************/	

// Purpose:	Initializes a memory manager
// Input:   No input
// Return:  A code indicating the results of the initialization 

RETCODE MemInit (uMCONFIG * Config)
{
	Dword PoolSize = ALIGNED(Config->PoolSize);	// Get requested size of memory frame

	MemMgr.Pool = (ptMEMBLOCK) malloc (PoolSize);
	// Allocate memory for manager object and pool

	if (MemMgr.Pool == NULL)	// Ascertain that malloc succeeded
		return RETCODE_FAILURE;	// Return failure

	MemMgr.Pool->Size = PoolSize - sizeof(tMEMBLOCK);	// Set pool amount available
	MemMgr.Pool->Prev = MemMgr.Pool->Next = MemMgr.Pool;// Link pool entries to self

	_asm {
		mov esi, [MemMgr]._Pool;/* Initialize the memory pool */
		call MemInsertIntoFreeBlocks;
	}

	return RETCODE_SUCCESS;
	// Return success
}

/********************************************************************************
*																				*
*								MemTerm											*
*																				*
********************************************************************************/	

// Purpose:	Deinitializes a memory manager
// Input:   Optional name of a log file
// Return:  A code indicating the results of the termination

RETCODE MemTerm (char const * LogFile)
{		
	if (LogFile != NULL)	// User requests diagnostics
	{
		ptMEMBLOCK MemBlock = MemMgr.Pool;	// Refer to first memory block

		FILE * fpLog;	// File used to log debug info

		char Pattern [9] = {0};	// Buffer used to retrieve memory patterns

		fpLog = fopen (LogFile, "wt");	// Create a log file

		if (fpLog != NULL)	// Log diagnostics if file creation was successful
		{
			// Output used entry/byte information
			fprintf (fpLog, "%d unfreed entries\n", MemMgr.nUsed);

			if (MemMgr.nUsed != 0) do {	// If memory is unfreed, list instances
				if (MemBlock->Size & MEM_USED)	// Check if memory block is not freed
				{
					// Retrieve memory pattern
					MemGetPattern (&MemBlock [BASE_EXTENT], Pattern);

					if (Pattern [0] == '\0')	// Check for unlabeled blocks
						fprintf (fpLog, "No pattern\n");	// Print message

					else fprintf (fpLog, "Entry: %s\n", Pattern);	// Print pattern

					fprintf (fpLog, "Bytes used: %u\n", MemBlock->Size ^ MEM_USED);	// Print memory consumption
				}

				MemBlock = MemBlock->Next;	// Go to next block in memory chain
			} while (MemBlock != MemMgr.Pool);	// Loop through all blocks

			fclose (fpLog);	// Close the log file
		}
	}

	free (MemMgr.Pool);	// Deallocate memory manager pool

	ZeroMemory(&MemMgr,sizeof(mMEMORY));// Clear manager

	return RETCODE_SUCCESS;
	// Return success
}

/********************************************************************************
*																				*
*								MemAlloc										*
*																				*
********************************************************************************/	

// Purpose:	Used to allocate memory of a given size
// Input:   Block size, and options
// Return:	Pointer to the memory, if successful; NULL otherwise

QUICK void * MemAlloc (Dword numBytes, FLAGS Options)
{
	_asm {
		mov eax, [esp+4];/* Load request size, cache base, and count of blocks to scan */
		mov esi, [MemMgr]._Cache;
		mov ecx, [MemMgr]._nUnused;
		add eax, 3;		/* Align request to next dword boundary; bits 0, 1 free */
		and eax, not 3;
$fMem:	cmp [esi]._Size, eax;	/* Scan through cache, stopping at first fit */
		jae $mGood;
		mov esi, [esi]._nFree;
		dec ecx;
		jnz $fMem;
		xor eax, eax;	/* If no blocks were found, return NULL */
		ret;
$mGood:	call MemRemoveFromFreeBlocks;	/* Extract the memory block from the pool */
		mov ecx, [esi]._Size;	/* Set the new block's size, and compute the padding left over */
		mov [esi]._Size, eax;
		sub ecx, eax;
		or [esi]._Size, MEM_USED;	/* Encode usage in bit 0 */
		cmp ecx, BLOCK_SIZE;/* If padding is inadequate to form a new block plus data, skip ahead */
		jle $mPad;
		mov edi, esi;	/* Cache old block */
		sub ecx, BLOCK_SIZE;/* Determine the size of new block */
		mov ebx, [edi]._Next;	/* Load offsets of new block and block after it */
		lea esi, [edi+eax+BLOCK_SIZE];
		mov [esi]._Size, ecx;	/* Set the new block's size */
		mov [esi]._Prev, edi;	/* Update the blocks' connections */
		mov [esi]._Next, ebx;
		mov [ebx]._Prev, esi;
		mov [edi]._Next, esi;
		call MemInsertIntoFreeBlocks;	/* Put the new block back into the free blocks */
		mov esi, edi;	/* Restore old block */
		xor ecx, ecx;	/* Zero out padding */
$mPad:	add [esi]._Size, ecx;	/* Accumulate leftover padding into allocated block */
		mov [esi]._Pattern, 0;	/* Effectively zero out block's pattern */
		test [esp+8], MEM_ZERO;	/* If requested, zero out the block's memory */
		jz $mNo0;
		cld;
		mov ecx, [esi]._Size;
		xor	eax, eax;
		shr	ecx, 2;
		lea edi, [esi]._DATA;
		rep stosd;
$mNo0:	lea eax, [esi]._DATA;	/* Load pointer to allocated memory as return value */
		inc dword ptr [MemMgr].nUsed;	/* Document addition of used memory block */
		ret;/* Return to caller */
	}
}

/********************************************************************************
*																				*
*								MemFree											*
*																				*
********************************************************************************/	

// Purpose: Used to release memory
// Input:   Context to release
// Return:  No return value

QUICK void MemFree (void * memory)
{
	_asm {
		mov esi, [esp+4];	/* Obtain the block preceding the memory variable */
		sub esi, BLOCK_SIZE;
		and [esi]._Size, not MEM_USED;	/* Remove usage encoding in bit 0 */
		call MemInsertIntoFreeBlocks;	/* Put block back into the free blocks */
		dec dword ptr [MemMgr].nUsed;	/* Document removal of used memory block */
		ret;/* Return to caller */
	}
}

/********************************************************************************
*																				*
*								MemGetPattern									*
*																				*
********************************************************************************/

// Purpose:	Used to retrieve a pattern used to identify memory
// Input:   Memory variable associated with pattern, and a buffer to load
// Return:  A code indicating the results of getting the pattern

RETCODE MemGetPattern (void * memory, char Pattern [])
{
	ptMEMBLOCK MemBlock;// Owner memory block

	int index;	// Loop variable

	MemBlock = (ptMEMBLOCK)((Pbyte) memory - sizeof(tMEMBLOCK));
	// Refer to memory's owning block

	for (index = 0; index < 8; ++index)	// Loop through pattern
	{
		Pattern [index] = MemBlock->Pattern [index];// Get byte

		if (MemBlock->Pattern [index] == '\0')	// Check for null bytes
			break;	// Pattern is complete
	}

	return RETCODE_SUCCESS;
	// Return success
}

/********************************************************************************
*																				*
*								MemSetPattern									*
*																				*
********************************************************************************/

// Purpose:	Used to associate a pattern to a memory variable
// Input:   Memory variable to associate with pattern, and pattern to assign
// Return:  A code indicating the results of setting the pattern

RETCODE MemSetPattern (void * memory, char Pattern [])
{
	ptMEMBLOCK MemBlock;// Owner memory block

	int index;	// Loop variable

	MemBlock = (ptMEMBLOCK)((Pbyte) memory - sizeof(tMEMBLOCK));
	// Refer to memory's owning block

	for (index = 0; index < 8; ++index)	// Loop through pattern
	{
		MemBlock->Pattern [index] = Pattern [index];// Set byte

		if (MemBlock->Pattern [index] == '\0')	// Check for null bytes
			break;	// Pattern is complete
	}

	return RETCODE_SUCCESS;
	// Return success
}

/********************************************************************************
*																				*
*								MemRemoveFromFreeBlocks							*
*																				*
********************************************************************************/	

// Purpose:	Used to extract a block from the free blocks
// Input:	ESI : Memory block to extract
// Return:	No return value

QUICK void MemRemoveFromFreeBlocks (void)
{
	_asm {
		mov ebx, [esi]._pFree;	/* Load last and next free blocks */
		mov edx, [esi]._nFree;
		cmp [MemMgr]._Cache, esi;	/* If block to remove is not the cache base, skip ahead */
		jne $mNo;
		mov [MemMgr]._Cache, edx;	/* Reassign cache to next free block in sequence */
$mNo:	mov [ebx]._nFree, edx;	/* Update the blocks' free block connections */
		mov [edx]._pFree, ebx;
		dec dword ptr [MemMgr]._nUnused;/* Document the removal of a free block */
		ret;/* Return to caller */
	}
}

/********************************************************************************
*																				*
*								MemInsertIntoFreeBlocks							*
*																				*
********************************************************************************/	

// Purpose:	Used to place a block within the free blocks
// Input:	ESI : Memory block to insert
// Return:	No return value

QUICK void MemInsertIntoFreeBlocks (void)
{
	_asm {
		cmp dword ptr [MemMgr]._nUnused, 0;	/* If the cache is not empty, skip ahead */
		jne $mNZ;
		mov [MemMgr]._Cache, esi;	/* Load cache base */
		mov [esi]._pFree, esi;	/* Refer cache to itself, and finish up */
		mov [esi]._nFree, esi;
		jmp $mEnd;
$mNZ:	call MemAdjoinBlocks;	/* Attempt to join new block into memory */
		mov ebx, [MemMgr]._Cache;	/* Load cache base and next free block */
		cmp eax, 2;
		mov edx, [ebx]._nFree;
		jne $mDone;	/* If block attached itself to memory, insertion is complete */
		mov [esi]._pFree, ebx;	/* Update the blocks' free connections */
		mov [esi]._nFree, edx;
		mov [ebx]._nFree, edx;
		mov [edx]._pFree, ebx;
$mEnd:	inc dword ptr [MemMgr]._nUnused;/* Document the addition of a free block */
$mDone:	ret;/* Return to caller */
	}
}

/********************************************************************************
*																				*
*								MemAdjoinBlocks									*
*																				*
********************************************************************************/	

// Purpose:	Used to adjoin memory
// Input:	ESI : Memory block to adjoin
// Return:	Amount of non-adjoinments

QUICK long MemAdjoinBlocks (void)
{
	_asm {
		mov eax, 2;	/* Set initial non-adjoinment counter */
		mov ebx, [esi]._Next;	/* Load next block, and check whether it's upper in memory and free */
		cmp ebx, esi;
		jle $uBad;
		test [ebx]._Size, MEM_USED;
		jnz $uBad;
		mov ecx, [ebx]._Size;	/* Enlarge next block; next block may be cache base, so reassign base */
		add ecx, BLOCK_SIZE;
		mov [MemMgr]._Cache, esi;
		add [esi]._Size, ecx;
		mov edx, [ebx]._Next;	/* Update blocks' connections */
		mov [esi]._Next, edx;
		mov [edx]._Prev, esi;
		mov edx, [ebx]._Next;	/* Reassign next block, and decrement non-adjoinment counter */
		dec eax;/* Decrement non-adjoinment counter */
		cmp dword ptr [MemMgr]._nUnused, 1;	/* Skip ahead if next block is not only block in cache */
		jne $More;
		mov [esi]._pFree, esi;	/* If next block is alone, bind this block to itself, and finish up */
		mov [esi]._nFree, esi;
		ret;
$More:	mov ecx, [ebx]._pFree;	/* Load last and next free blocks */
		mov edx, [ebx]._nFree;
		mov [esi]._pFree, ecx;	/* Update the blocks' free block connections */
		mov [esi]._nFree, edx;
		mov [ecx]._nFree, esi;
		mov [edx]._pFree, esi;
$uBad:	mov ebx, [esi]._Prev;	/* Load last block, and check whether it's lower in memory and free */
		cmp ebx, esi;
		jge $lBad;
		test [ebx]._Size, MEM_USED;
		jnz $lBad;
		mov ecx, [esi]._Size;	/* Enlarge last block; current block may be cache base, so reassign base */
		add ecx, BLOCK_SIZE;
		mov [MemMgr]._Cache, ebx;
		add [ebx]._Size, ecx;
		mov edx, [esi]._Next;	/* Update blocks' connections */
		mov [ebx]._Next, edx;
		mov [edx]._Prev, ebx;
		dec eax;/* Decrement non-adjoinment counter */
		jnz $lFin;	/* Skip ahead if memory was not adjoined before */
		dec dword ptr [MemMgr]._nUnused;/* Dual adjoinment reduces the block count; skip ahead if more than one block remains */
		cmp dword ptr [MemMgr]._nUnused, 1;
		jg $lFin;
		mov [ebx]._pFree, ebx;	/* If block is alone, bind last block to itself, and finish up */
		mov [ebx]._nFree, ebx;
		ret;
$lFin:	mov ecx, [esi]._pFree;	/* Load last and next free blocks */
		mov edx, [esi]._nFree;
		mov [ecx]._nFree, edx;	/* Update the blocks' free block connections */
		mov [edx]._pFree, ecx;
$lBad:	ret;/* Return to caller */
	}
}