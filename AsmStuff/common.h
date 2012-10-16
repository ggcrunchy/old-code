#ifndef COMMON_H 
#define COMMON_H

/********************************************************************
*																	*
*							Includes								*
*																	*
********************************************************************/

// Windows-specific
#include <windows.h>
#include <windowsx.h>
// Basic routines
#include <stdio.h>
#include <stdlib.h>

/********************************************************************
*																	*
*							Qualifiers								*
*																	*
********************************************************************/

#define PUBLIC				// Semantic used to indicate publicly exposed methods
#define PROTECTED			// Semantic used to indicate methods shared among implementations
#define PRIVATE		static	// Semantic used to indicate implementation-specific attributes and methods

#define QUICK	_declspec(naked)	// Semantic used to perform optimal function call

/********************************************************************
*																	*
*							Values									*
*																	*
********************************************************************/

#define BASE_AMOUNT	1	// Count of elements of base array
#define BASE_EXTENT	1	// Index of element beyond base array

/********************************************************************
*																	*
*							Return codes							*
*																	*
********************************************************************/

#define RETCODE_BREAK	(-1)// General-purpose break code
#define RETCODE_FAILURE	 0	// General-purpose failure
#define RETCODE_SUCCESS	(+1)// General-purpose success

/********************************************************************
*																	*
*							Types									*
*																	*
********************************************************************/

// Unsigned primitives/references
typedef unsigned char  Byte, * Pbyte;
typedef unsigned short Word, * Pword;
typedef unsigned long  Dword, * Pdword;

// Return/variable types
typedef long FLAGS;		// General flags type
typedef long RETCODE;	// Return code type

/********************************************************************
*																	*
*							Methods									*
*																	*
********************************************************************/

typedef RETCODE (* EQUIVAL) (void *, void *);	// Callback function for search comparisons
typedef RETCODE (* EXECUTE)	(void *, void *);	// Method used for item-wise processing

/********************************************************************
*																	*
*							Macros									*
*																	*
********************************************************************/

// Extract the aligned version of a datum
#define ALIGNED(data)	(((int)(data) + sizeof(int) - 1) & ~(sizeof(int) - 1))

#endif // COMMON_H
