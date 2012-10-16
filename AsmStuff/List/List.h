#ifndef LIST_H
#define LIST_H

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

#define L_DYNAMIC	0x1	// Dynamic list

/********************************************************************
*																	*
*							Handles									*
*																	*
********************************************************************/

typedef struct _tLIST * hLIST;	// Handle to a linked list

/********************************************************************
*																	*
*							Interface								*
*																	*
********************************************************************/

PUBLIC hLIST ListCreate (int nNodes, Dword SizeOfObject, FLAGS Settings);

// Purpose:	Creates a list object
// Input:	A node count, per-element size, and list settings
// Return:	A handle to the new list

PUBLIC RETCODE ListDestroy (hLIST List);

// Purpose:	Destroys a list object
// Input:	A list handle
// Return:	A code indicative of the results of the destruction

PUBLIC void * ListFront (hLIST List);

// Purpose:	Retrieves the datum at the list's front
// Input:	A list handle
// Return:	Pointer to datum at front of list

PUBLIC void * ListBack (hLIST List);

// Purpose:	Retrieves the datum at the list's back
// Input:	A list handle
// Return:	Pointer to datum at back of list

PUBLIC void * ListPrev (hLIST List, void * Datum);

// Purpose:	Retrieves datum before given datum in list
// Input:	A list handle, and pointer to reference datum
// Return:	Pointer to datum in list before reference datum

PUBLIC void * ListNext (hLIST List, void * Datum);

// Purpose:	Retrieves datum after given datum in list
// Input:	A list handle, and pointer to reference datum
// Return:	Pointer to datum in list after reference datum

PUBLIC RETCODE ListToFront (hLIST List, void * Datum);

// Purpose:	Adds an entry to the front of the list
// Input:	A list handle, and pointer to datum to add
// Return:	A code indicating the results of the addition

PUBLIC RETCODE ListToBack (hLIST List, void * Datum);

// Purpose:	Adds an entry to the back of the list
// Input:	A list handle, and pointer to datum to add
// Return:	A code indicating the results of the addition

PUBLIC void ListDelete (hLIST List, void * Datum);

// Purpose:	Deletes entry from list
// Input:	A list handle, and pointer to datum to delete
// Return:	No value is returned

PUBLIC void ListFlush (hLIST List);

// Purpose:	Flushes all entries from list
// Input:	A list handle
// Return:	No value is returned

PUBLIC void * ListSearch (hLIST List, EQUIVAL Callback, void * Context);

// Purpose:	Searches a linked list for a given datum
// Input:	A list handle, an equivalence routine, and context to equat
// Return:	Pointer to the datum if it exists; NULL otherwise

PUBLIC BOOL ListIsEmpty (hLIST List);

// Purpose:	Used to determine whether list is empty
// Input:	A list handle
// Return:	A boolean indicating whether list is empty

PUBLIC BOOL ListIsFull (hLIST List);

// Purpose:	Used to determine whether list if full
// Input:	A list handle
// Return:	A boolean indicating whether list is full

PUBLIC int ListNodeCount (hLIST List);

// Purpose:	Retrieves a list's current node count
// Input:	A list handle
// Return:	Count of nodes in list

PUBLIC void ListExecute (hLIST List, EXECUTE Callback, void * Context);

// Purpose:	Performs an operation on all list elements
// Input:	A list handle, an operation to execute, and optional context to pass to routine
// Return:	No value is returned

#endif // LIST_H