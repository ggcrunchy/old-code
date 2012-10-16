/********************************************************************
*																	*
*							Includes								*
*																	*
********************************************************************/

#include "i_List.h"

/********************************************************************
*																	*
*							ListCreate								*
*																	*
********************************************************************/

// Purpose:	Creates a list object
// Input:	A node count, per-element size, and list settings
// Return:	A handle to the new list

QUICK ptLIST ListCreate (int nNodes, Dword SizeOfObject, FLAGS Settings)
{
	_asm {
		mov eax, [esp+12];	/* Load settings */
		mov ecx, [esp+4];	/* Load arguments */
		mov edx, [esp+8];
		push eax;	/* Save settings */
		test eax, L_DYNAMIC;	/* Check what type of list to load */
		jz $cStat;
		call ListDynamicInit;	/* Dynamic initialization */
		jmp $cDone;
$cStat:	call ListStaticInit;/* Static initialization */
$cDone:	pop dword ptr [eax]._Status;/* Load list status */
		ret;/* Return to caller */
	}
}

/********************************************************************
*																	*
*							ListDestroy								*
*																	*
********************************************************************/

// Purpose:	Destroys a list object
// Input:	A list handle
// Return:	A code indicative of the results of the destruction

QUICK RETCODE ListDestroy (tLIST * List)
{
	_asm {
		push [esp+4];/* Load argument onto stack */
		call ListFlush;	/* Empty list */
		call MemFree;	/* Free list memory; keep argument on stack */
		add esp, 4;	/* Restore stack */
		mov eax, RETCODE_SUCCESS;	/* Load success return value */
		ret;/* Return to caller */
	}
}

/********************************************************************
*																	*
*							ListFront								*
*																	*
********************************************************************/

// Purpose:	Retrieves the datum at the list's front
// Input:	A list handle
// Return:	Pointer to datum at front of list

QUICK void * ListFront (tLIST * List)
{
	_asm {
		mov ebx, [esp+4];	/* Load list head */
		mov ebx, [ebx]._Head;
		lea eax, [ebx]._DATA;	/* Load node data as return value */
		ret;/* Return to caller */
	}
}

/********************************************************************
*																	*
*							ListBack								*
*																	*
********************************************************************/

// Purpose:	Retrieves the datum at the list's back
// Input:	A list handle
// Return:	Pointer to datum at back of list

QUICK void * ListBack (tLIST * List)
{
	_asm {
		mov ebx, [esp+4];	/* Load node before list head */
		mov ebx, [ebx]._Head;
		mov ebx, [ebx]._Prev;
		lea eax, [ebx]._DATA;	/* Load node data as return value */
		ret;/* Return to caller */
	}
}

/********************************************************************
*																	*
*							ListPrev								*
*																	*
********************************************************************/

// Purpose:	Retrieves datum before given datum in list
// Input:	A list handle, and pointer to reference datum
// Return:	Pointer to datum in list before reference datum

QUICK void * ListPrev (tLIST * List, void * Datum)
{
	_asm {
		mov ebx, [esp+8];	/* Load node before datum */
		sub ebx, NODE_SIZE;
		mov ebx, [ebx]._Prev;	/* Load last node */
		lea eax, [ebx]._DATA;	/* Load node data as return value */
		ret;/* Return to caller */
	}
}

/********************************************************************
*																	*
*							ListNext								*
*																	*
********************************************************************/

// Purpose:	Retrieves datum after given datum in list
// Input:	A list handle, and pointer to reference datum
// Return:	Pointer to datum in list after reference datum

QUICK void * ListNext (tLIST * List, void * Datum)
{
	_asm {
		mov ebx, [esp+8];	/* Load node before datum */
		sub ebx, NODE_SIZE;
		mov ebx, [ebx]._Prev;	/* Load next node */
		lea eax, [ebx]._DATA;	/* Load node data as return value */
		ret;/* Return to caller */
	}
}

/********************************************************************
*																	*
*							ListToFront								*
*																	*
********************************************************************/

// Purpose:	Adds an entry to the front of the list
// Input:	A list handle, and pointer to datum to add
// Return:	A code indicating the results of the addition

QUICK RETCODE ListToFront (tLIST * List, void * Datum)
{
	_asm {
		mov ebx, [esp+4];	/* Load list */
		test [ebx]._Status, L_DYNAMIC;	/* If list is dynamic, allocate a node */
		jz $lStat;
		push ebx;	/* Save list */
		mov ecx, [ebx]._SizeOfObject;	/* Load arguments */
		push 0;
		add ecx, NODE_SIZE;
		push ecx;
		call MemAlloc;	/* Allocate node */
		add esp, 8;	/* Remove arguments from stack */
		pop ebx;/* Restore list */
		mov [ebx]._Free, eax; /* Set allocated node on free list */
$lStat:	mov eax, [ebx]._Free;	/* Refer to first node in free list */
		mov edx, [eax]._Next;	/* Reassign free list head */
		mov [ebx]._Free, edx;
		cmp dword ptr [ebx]._nNodes, 0;	/* Handle list according to whether it is empty */
		jne $pFull;
		mov [eax]._Prev, eax;	/* Bind node to itself */
		mov [eax]._Next, eax;
		jmp $Load;	/* Proceed to load data into list */
$pFull:	mov ecx, [ebx]._Head;	/* Load head and node before head */
		mov edx, [ecx]._Prev;
		mov [eax]._Next, ecx;	/* Update nodes' connections */
		mov [eax]._Prev, edx;
		mov [edx]._Next, eax;
		mov [ecx]._Prev, eax;
$Load:	mov [ebx]._Head, eax;	/* Assign list head */
		inc dword ptr [ebx]._nNodes;/* Document addition of node */
		mov ecx, [ebx]._SizeOfObject;	/* Load per-object size for copy */
		mov esi, [esp+8];	/* Load datum and node's data section */
		lea edi, [eax]._DATA;
		test ecx, 3;/* Attempt to copy exact dwords */
		jz $Dword;
		mov edx, ecx;	/* Save counter */
		and ecx, 3;	/* Isolate bits 0 and 1 */
		rep movsb;	/* Copy 1, 2, or 3 bytes */
		mov ecx, edx;	/* Restore counter */
$Dword:	shr ecx, 2;	/* Convert counter to dwords */
		rep movsd;	/* Copy memory over */
		mov eax, RETCODE_SUCCESS;	/* Load success return value */
		ret;/* Return to caller */
	}
}

/********************************************************************
*																	*
*							ListToBack								*
*																	*
********************************************************************/

// Purpose:	Adds an entry to the back of the list
// Input:	A list handle, and pointer to datum to add
// Return:	A code indicating the results of the addition

QUICK RETCODE ListToBack (tLIST * List, void * Datum)
{
	_asm {
		mov ebx, [esp+4];/* Load list */
		test [ebx]._Status, L_DYNAMIC;	/* If list is dynamic, allocate a node */
		jz $lStat;
		push ebx;	/* Save list */
		mov ecx, [ebx]._SizeOfObject;	/* Load arguments */
		push 0;
		add ecx, NODE_SIZE;
		push ecx;
		call MemAlloc;	/* Allocate node */
		add esp, 8;	/* Remove arguments from stack */
		pop ebx;/* Restore list */
		mov [ebx]._Free, eax; /* Set allocated node on free list */
$lStat:	mov eax, [ebx]._Free;	/* Refer to first node in free list */
		mov edx, [eax]._Next;	/* Reassign free list head */
		mov [ebx]._Free, edx;
		cmp dword ptr [ebx]._nNodes, 0;	/* Handle list according to whether it is empty */
		jne $pFull;
		mov [eax]._Prev, eax;	/* Bind node to itself */
		mov [eax]._Next, eax;
		mov [ebx]._Head, eax;	/* Assign list head */
		jmp $Load;	/* Proceed to load data into list */
$pFull:	mov ecx, [ebx]._Head;	/* Load head and node before head */
		mov edx, [ecx]._Prev;
		mov [eax]._Next, ecx;	/* Update nodes' connections */
		mov [eax]._Prev, edx;
		mov [edx]._Next, eax;
		mov [ecx]._Prev, eax;
$Load:	inc dword ptr [ebx]._nNodes;/* Document addition of node */
		mov ecx, [ebx]._SizeOfObject;	/* Load per-object size for copy */
		mov esi, [esp+8];	/* Load datum and node's data section */
		lea edi, [eax]._DATA;
		test ecx, 3;/* Attempt to copy exact dwords */
		jz $Dword;
		mov edx, ecx;	/* Save counter */
		and ecx, 3;	/* Isolate bits 0 and 1 */
		rep movsb;	/* Copy 1, 2, or 3 bytes */
		mov ecx, edx;	/* Restore counter */
$Dword:	shr ecx, 2;	/* Convert counter to dwords */
		rep movsd;	/* Copy memory over */
		mov eax, RETCODE_SUCCESS;	/* Load success return value */
		ret;/* Return to caller */
	}
}

/********************************************************************
*																	*
*							ListDelete								*
*																	*
********************************************************************/

// Purpose:	Deletes entry from list
// Input:	A list handle, and pointer to datum to delete
// Return:	No value is returned

QUICK void ListDelete (tLIST * List, void * Datum)
{
	_asm {
		mov eax, [esp+4];	/* Load list and datum */
		mov ebx, [esp+8];	
		sub ebx, NODE_SIZE;	/* Obtain the node preceding the datum */
		dec dword ptr [eax]._nNodes;/* Document removal of node */
		mov ecx, [ebx]._Prev;	/* Load pointers to last and next nodes */
		mov edx, [ebx]._Next;
		cmp [eax]._Head, ebx;	/* If node is not the list's head, skip ahead */
		jne $Inner;
		mov [eax]._Head, edx;	/* Reassign head */
$Inner:	mov [ecx]._Next, edx;	/* Update nodes' connections */
		mov [edx]._Prev, ecx;
		test [eax]._Status, L_DYNAMIC;	/* Process deletion according to list type */
		jnz $Dynam;
		mov ecx, [eax]._Free;	/* Bind node to free list */
		mov [ebx]._Next, ecx;
		mov [eax]._Free, ebx;
		ret;/* Return to caller */
$Dynam:	push ebx;	/* Load argument */
		call MemFree;	/* Release node memory */
		add esp, 4;	/* Remove argument from stack */
		ret;/* Return to caller */
	}
}

/********************************************************************
*																	*
*							ListFlush								*
*																	*
********************************************************************/

// Purpose:	Flushes all entries from list
// Input:	A list handle
// Return:	No value is returned

QUICK void ListFlush (tLIST * List)
{
	_asm {
		mov eax, [esp+4];	/* Load list */
		cmp dword ptr [eax]._nNodes, 0;	/* If list is empty, return trivially */
		jne $Flush;
		ret;/* Return to caller */
$Flush:	test [eax]._Status, L_DYNAMIC;	/* If list is dynamic, process it */
		jnz $Purge;
		mov ecx, [eax]._nMax;	/* Load max node count */
		mov ebx, [eax]._Head;	/* Load list head */
		cmp [eax]._nNodes, ecx;	/* If list is full, ignore maintenance */
		je $Full;
		mov ecx, [ebx]._Prev;	/* Bind final node to free list */
		mov edx, [eax]._Free;
		mov [ecx]._Next, edx;
$Full:	mov [eax]._Free, ebx;	/* Reassign free list and document flush */
		mov dword ptr [eax]._nNodes, 0;
		ret;/* Return to caller */
$Purge:	push ebp;	/* Save ebp */
		mov ebp, [eax]._Head;	/* Load list head */
$Loop:	push eax;	/* Save pointer to list */
		push ebp;	/* Load argument */
		mov ebp, [ebp]._Next;	/* Move to next element in list */
		call MemFree;	/* Delete the head */
		add esp, 4;	/* Remove argument from stack */
		pop eax;
		dec dword ptr [eax]._nNodes;/* Update node count, and quit if zero */
		jnz $Loop;
		pop ebp;/* Restore ebp */
		ret;/* Return to caller */
	}
}

/********************************************************************
*																	*
*							ListSearch								*
*																	*
********************************************************************/

// Purpose:	Searches a linked list for a given datum
// Input:	A list handle, an equivalence routine, and context to equat
// Return:	Pointer to the datum if it exists; NULL otherwise

QUICK void * ListSearch (tLIST * List, EQUIVAL Callback, void * Context)
{
	_asm {
		mov eax, [esp+4];	/* Load list */
		mov edx, [esp+12];	/* Load context */
		mov ecx, [eax]._nNodes;	/* Refer to count of list items */
		mov esi, [eax]._Head;	/* Refer to front of list */
		or ecx, ecx;/* Check if list is empty, and quit if so */
		jz $None;
		mov ebx, ecx;	/* Save loop variable */
		mov edi, [esp+8];	/* Load functor */
		push edx;	/* Load argument 2 */
$Scan:	lea edx, [esi]._DATA;	/* Refer to node's data */
		push edx;	/* Load argument 1 */
		call edi;	/* Call equivalence test */
		or eax, eax;/* If test did not fail, item was found */
		jnz $Found;
		add esp, 4;	/* Remove argument from stack */
		dec ebx;/* Update loop variable and quit if zero */
		mov esi, [esi]._Next;	/* Advance to next node */
		jnz $Scan;
		add esp, 4;	/* Remove arguments from stack */
$None:	xor eax, eax;	/* No item was found, so return NULL */
		ret;/* Return to caller */
$Found:	pop eax;/* Load node's data as return value */
		add esp, 4;	/* Remove arguments from stack */
		ret;/* Return to caller */
	}
}

/********************************************************************
*																	*
*							ListIsEmpty								*
*																	*
********************************************************************/

// Purpose:	Used to determine whether list is empty
// Input:	A list handle
// Return:	A boolean indicating whether list is empty

QUICK BOOL ListIsEmpty (tLIST * List)
{
	_asm {
		mov ebx, [esp+4];	/* Load list */
		xor eax, eax;	/* Clear test/return value */
		cmp [ebx]._nNodes, eax;	/* Check if list is empty */
		sete al;/* If so, return true */
		ret;/* Return to caller */
	}
}

/********************************************************************
*																	*
*							ListIsFull								*
*																	*
********************************************************************/

// Purpose:	Used to determine whether list if full
// Input:	A list handle
// Return:	A boolean indicating whether list is full

QUICK BOOL ListIsFull (tLIST * List)
{
	_asm {
		mov ebx, [esp+4];	/* Load list */
		xor eax, eax;	/* Clear return value */
		mov ecx, [ebx]._nNodes;	/* Compare node count to max */
		cmp [ebx]._nMax, ecx;
		sete al;/* If equal, return true */
		ret;/* Return to caller */
	}
}

/********************************************************************
*																	*
*							ListNodeCount							*
*																	*
********************************************************************/

// Purpose:	Retrieves a list's current node count
// Input:	A list handle
// Return:	Count of nodes in list

QUICK int ListNodeCount (tLIST * List)
{
	_asm {
		mov eax, [esp+4];	/* Load list */
		mov eax, [eax]._nNodes;	/* Load return value */
		ret;/* Return to caller */
	}
}

/********************************************************************
*																	*
*							ListExecute								*
*																	*
********************************************************************/

// Purpose:	Performs an operation on all list elements
// Input:	A list handle, an operation to execute, and optional context to pass to routine
// Return:	No value is returned

QUICK void ListExecute (tLIST * List, EXECUTE Callback, void * Context)
{
	_asm {
		mov eax, [esp+4];	/* Load list */
		mov edx, [esp+12];	/* Load context */
		mov ecx, [eax]._nNodes;	/* Refer to count of list items */
		mov esi, [eax]._Head;	/* Refer to front of list */
		or ecx, ecx;/* Check if list is empty, and quit if so */
		jz $None;
		mov ebx, ecx;	/* Save loop variable */
		mov edi, [esp+8];	/* Load functor */
		push edx;	/* Load argument 2 */
$Exe:	lea edx, [esi]._DATA;	/* Refer to node's data */
		push edx;	/* Load argument 1 */
		call edi;	/* Call functor */
		add esp, 4;	/* Remove argument from stack */
		dec ebx;/* Update loop variable and quit if zero */
		mov esi, [esi]._Next;	/* Advance to next node */
		jnz $Exe;
		add esp, 4;	/* Remove arguments from stack */
$None:	ret;/* Return to caller */
	}
}

/********************************************************************
*																	*
*							ListStaticInit							*
*																	*
********************************************************************/

// Purpose:	Used to initialize a static linked list
// Input:	ECX : Node count, EDX : Per-element datum size
// Return:	EAX : Pointer to a new list

QUICK ptLIST ListStaticInit (void)
{
	_asm {
		push ecx;	/* Save node count and object size */
		push edx;
		lea eax, [edx+NODE_SIZE];	/* Obtain the sum = Nodes * (NODE_SIZE + object size) + list size */
		mul ecx;
		add eax, LIST_SIZE;
		push 0;	/* Load arguments */
		push eax;
		call MemAlloc;	/* Allocate space for new list */
		add esp, 8;	/* Remove arguments from stack */
		mov dword ptr [eax]._nNodes, 0;	/* Zero out node counter */
		pop edx;/* Reload input values */
		pop ecx;
		mov [eax]._nMax, ecx;	/* Set per-object size and node max fields in list */
		mov [eax].SizeOfObject, edx;
		lea ebx, [eax+LIST_SIZE];	/* Point to node bank */
		mov [eax]._Free, ebx;	/* Refer free list to node bank */
$Loop:	dec ecx;/* Update loop variable, and quit if zero */
		jz $Done;
		lea edi, [ebx+edx+NODE_SIZE];	/* Bind to next node in bank */
		mov [ebx]._Next, edi;
		mov ebx, edi;	/* Move to next node in list */
		jmp $Loop;	/* Iterate again */
$Done:	ret;/* Return to caller */
	}
}

/********************************************************************
*																	*
*							ListDynamicInit							*
*																	*
********************************************************************/

// Purpose:	Used to initialize a dynamic linked list
// Input:	ECX : Node count, EDX : Per-element datum size
// Return:	EAX : Pointer to a new list

QUICK ptLIST ListDynamicInit (void)
{
	_asm {
		push edx;	/* Save object size */
		push 0;	/* Load arguments */
		push LIST_SIZE;
		call MemAlloc;	/* Allocate space for new list */
		add esp, 8;	/* Remove arguments from stack */
		mov dword ptr [eax]._nNodes, 0;	/* Zero out node counter */
		mov dword ptr [eax]._nMax, not 0;	/* Set max value */
		pop [eax].SizeOfObject;	/* Set per-object size */
		ret;/* Return to caller */
	}
}