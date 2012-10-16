#ifndef MEMORY_H
#define MEMORY_H

/********************************************************************
*																	*
*							Includes								*
*																	*
********************************************************************/

#include "..\common.h"

/********************************************************************
*																	*
*							Flags									*
*																	*
********************************************************************/

#define MEM_ZERO 0x1	// Used to zero out allocated memory

/********************************************************************
*																	*
*							User Types								*
*																	*
********************************************************************/

// User-configuration for memory
typedef struct {
	Dword PoolSize;	// Bytes to allocate for memory pool
} uMCONFIG, * puMCONFIG;

/********************************************************************
*																	*
*							Interface								*
*																	*
********************************************************************/

PUBLIC RETCODE MemInit (puMCONFIG Config);

// Purpose:	Initializes a memory manager
// Input:   Pointer to a configuration structure
// Return:  A code indicating the results of the initialization    

PUBLIC RETCODE MemTerm (char const * LogFile);

// Purpose: Deinitializes a memory manager
// Input:   Optional name of a log file
// Return:  A code indicating the results of the termination

PUBLIC void * MemAlloc (Dword numBytes, FLAGS Options);

// Purpose:	Used to allocate memory of a given size
// Input:   Block size, and options
// Return:	Pointer to the memory, if successful; NULL otherwise

PUBLIC void MemFree (void * memory);

// Purpose: Used to release memory
// Input:   Context to release
// Return:  No return value

PUBLIC RETCODE MemGetPattern (void * memory, char Pattern []);

// Purpose:	Used to retrieve a pattern used to identify memory
// Input:   Memory variable associated with pattern, and a buffer to load
// Return:  A code indicating the results of getting the pattern

PUBLIC RETCODE MemSetPattern (void * memory, char Pattern []);

// Purpose:	Used to associate a pattern to a memory variable
// Input:   Memory variable to associate with pattern, and pattern to assign
// Return:  A code indicating the results of setting the pattern

#endif // MEMORY_H