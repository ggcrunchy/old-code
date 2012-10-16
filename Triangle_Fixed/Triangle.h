/********************************************************************
*																	*
*							Triangle.h								*
*																	*
*	Author:		Steven Johnson										*
*	Purpose:	Header for triangle-fill algorithm implementation	*
*																	*
********************************************************************/

#ifndef TRIANGLE_H
#define TRIANGLE_H

/********************************************************************
*																	*
*							Includes								*
*																	*
********************************************************************/

#include <windows.h>
#include <assert.h>

/********************************************************************
*																	*
*							Defines									*
*																	*
********************************************************************/

#define NUM_EDGES	3
#define NUM_POINTS	3

#define LONG_EDGE	0
#define TOP_EDGE	1
#define LOW_EDGE	2

#define LOW_POINT	0
#define MID_POINT	1
#define HIGH_POINT	2

#define FIXED_TYPE	Fixed_24_8		// Generic type of fixed-point
#define MAKE_FIXED	FixedPoint_24_8	// Generic fixed-type constructor
#define FIXED_MULT	FixedMult_24_8	// Generic fixed-type multiplication
#define FIXED_DIV	FixedDiv_24_8	// Generic fixed-type division
#define FIXED_CEIL	FixedCeil_24_8	// Generic fixed-type ceiling
#define FIXED_FLOOR	FixedFloor_24_8	// Generic fixed-type floor
#define FIXED_MOD	FixedMod_24_8	// Generic fixed-type modulo

#define PRECISION	6				// Bit size of floating point portion
#define RADIX		(1<<PRECISION)	// Radix of float portion
#define REAL_MASK	(RADIX-1)		// Mask used to eliminate extraneous portions of float portion
#define SIGN_MASK	0x7FFFFFFF		// Mask used to eliminate signed part of 24-bit values

/********************************************************************
*																	*
*							Types									*
*																	*
********************************************************************/

typedef long Fixed_24_8;

typedef struct _EdgeInfo {
	FIXED_TYPE dx;	// Horizontal expanse of edge
	FIXED_TYPE dy;	// Vertical expanses of edge
} EdgeInfo, * pEdgeInfo;

typedef struct _TriangleInfo {
	EdgeInfo edge [NUM_EDGES];	// Edges
	COLORREF color;				// color value
} TriangleInfo, * pTriangleInfo;

/********************************************************************
*																	*
*							FixedPoint_24_8							*
*																	*
*	Used to compose 24:8 fixed point numbers						*
*																	*
********************************************************************/

FIXED_TYPE MAKE_FIXED (long Integer, long Real);

/********************************************************************
*																	*
*							DrawTriangle							*
*																	*
*	Primary wrapper for triangle filler, with integers				*
*																	*
********************************************************************/

void DrawTriangle (HDC hDc, POINT p0, POINT p1, POINT p2, COLORREF color);

/********************************************************************
*																	*
*							FillLeftOrientedTriangle				*
*																	*
*	Fill a triangle with long side to right of middle vertex		*
*																	*
********************************************************************/

static void FillLeftOrientedTriangle (HDC hDc);

/********************************************************************
*																	*
*							FillRightOrientedTriangle				*
*																	*
*	Fill a triangle with long side to left of middle vertex			*
*																	*
********************************************************************/

static void FillRightOrientedTriangle (HDC hDc);

/********************************************************************
*																	*
*							WriteRow								*
*																	*
*	Fill a triangle row from one x value to another, given a y		*
*																	*
********************************************************************/

static void WriteRow (HDC hDc, long x, long endX, long y);

/********************************************************************
*																	*
*							WriteColumn								*
*																	*
*	Fill a triangle column from one y value to another, given an x	*
*																	*
********************************************************************/

static void WriteColumn (HDC hDc, long x, long y, long endY);

/********************************************************************
*																	*
*							Swap points								*
*																	*
*	Exchanges two given POINTs in an array							*
*																	*
********************************************************************/

static void SwapPoints (PPOINT p1, PPOINT p2);

/********************************************************************
*                                                                   *
*							Ceiling                                 *
*                                                                   *
*	Computes the ceiling of a fractional value                      *
*                                                                   *
********************************************************************/

static long Ceiling (long numerator, long denominator);

/********************************************************************
*																	*
*							Floor									*
*																	*
*	Computes the floor of a fractional value						*
*																	*
********************************************************************/

static long Floor (long numerator, long denominator);

/********************************************************************
*																	*
*							Mod										*
*																	*
*	Computes the modulus of two values								*
*																	*
********************************************************************/

static long Mod (long a, long b);

/********************************************************************
*																	*
*							FixedMult_24_8							*
*																	*
*	Computes the product of two 24:8 fixed point numbers			*
*																	*
********************************************************************/

static FIXED_TYPE FIXED_MULT (FIXED_TYPE FixedOne, FIXED_TYPE FixedTwo);

/********************************************************************
*																	*
*							FixedDiv_24_8							*
*																	*
*	Computes the quotient of two 24:8 fixed point numbers			*
*																	*
********************************************************************/

static FIXED_TYPE FIXED_DIV (FIXED_TYPE FixedOne, FIXED_TYPE FixedTwo);

/********************************************************************
*																	*
*							FixedCeil_24_8							*
*																	*
*	Computes the ceiling of a 24:8 fixed point number				*
*																	*
********************************************************************/

static FIXED_TYPE FIXED_CEIL (FIXED_TYPE Fixed);

/********************************************************************
*																	*
*							FixedFloor_24_8							*
*																	*
*	Computes the floor of a 24:8 fixed point number					*
*																	*
********************************************************************/

static FIXED_TYPE FIXED_FLOOR (FIXED_TYPE Fixed);

/********************************************************************
*																	*
*							FixedMod_24_8							*
*																	*
*	Computes the modulo of two 24:8 fixed point numbers				*
*																	*
********************************************************************/

static FIXED_TYPE FIXED_MOD (FIXED_TYPE FixedOne, FIXED_TYPE FixedTwo);

#endif // TRIANGLE_H