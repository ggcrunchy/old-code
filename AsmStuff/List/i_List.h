#ifndef I_LIST_H
#define I_LIST_H

/********************************************************************
*																	*
*							Includes								*
*																	*
********************************************************************/

#include "List.h"	// Interface information

#include "..\Memory\Memory.h"	// Allocation

/********************************************************************
*																	*
*							Values									*
*																	*
********************************************************************/

/* tLISTNODE offsets */
#define _Prev 0x0	// Prev offset
#define _Next 0x4	// Next offset
#define _DATA 0x8	// Data offset

/* tLISTNODE size */
#define NODE_SIZE _DATA	// Data begins at end of node

/* tLIST offsets */
#define _nNodes		  0x00	// nNodes offset
#define _nMax		  0x04	// nMax offset
#define _Status		  0x08	// Status offset
#define _Head		  0x0C	// Head offset
#define _Free		  0x10	// Free offset
#define _SizeOfObject 0x14  // SizeOfObject offset

/* tLIST size */
#define LIST_SIZE 0x18

/********************************************************************
*																	*
*							Types									*
*																	*
********************************************************************/

/////////////////////////////////////////
// _tLISTNODE: Storage element in list //
/////////////////////////////////////////

typedef struct _tLISTNODE * fLISTNODE;	// Forward reference
typedef struct _tLISTNODE {
	fLISTNODE Prev;	// Last node in list
	fLISTNODE Next;	// Next node in list
	/* Byte Data []; Virtual byte stream */	
} tLISTNODE, * ptLISTNODE;

////////////////////////////////////////////
// _tLIST: General data storage mechanism //
////////////////////////////////////////////

typedef struct _tLIST {
	int nNodes, nMax;	// Current node count; max node count
	FLAGS Status;		// Current list settings
	ptLISTNODE Head;	// Head of linked list
	ptLISTNODE Free;	// Head of free list
	Dword SizeOfObject;	// Size of object stored in list
} tLIST, * ptLIST;

/********************************************************************
*																	*
*							Implementation							*
*																	*
********************************************************************/

ptLIST ListStaticInit (void);

// Purpose:	Used to initialize a static linked list
// Input:	ECX : Node count, EDX : Per-element datum size
// Return:	Pointer to a new list

ptLIST ListDynamicInit (void);

// Purpose:	Used to initialize a dynamic linked list
// Input:	ECX : Node count, EDX : Per-element datum size
// Return:	Pointer to a new list

#endif // I_LIST_H