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

#define NUM_EDGES		3
#define NUM_POINTS		3

#define LONG_EDGE		0
#define TOP_EDGE		1
#define LOW_EDGE		2

#define LOW_POINT		0
#define MID_POINT		1
#define HIGH_POINT		2

/********************************************************************
*																	*
*							Types									*
*																	*
********************************************************************/

typedef struct _EdgeInfo {
	long dx;	// Horizontal expanse of edge
	long dy;	// Vertical expanses of edge
} EdgeInfo, * pEdgeInfo;

typedef struct _TriangleInfo {
	EdgeInfo edge [NUM_EDGES];	// Edges
	COLORREF color;				// color value
} TriangleInfo, * pTriangleInfo;

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

#endif // TRIANGLE_H