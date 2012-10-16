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
#include <math.h>

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

#define MIN_TOLERANCE	0.0005

#define MARKED			0x1

/********************************************************************
*																	*
*							Macros									*
*																	*
********************************************************************/

#define MARK_POINT(edge)	(edge)->status |= MARKED
#define UNMARK_POINT(edge)	(edge)->status &= ~MARKED

/********************************************************************
*																	*
*							Types									*
*																	*
********************************************************************/

typedef struct _FloatPoint {
	double x;	// x coordinate
	double y;	// y coordinate
	BYTE status;// Point status
} FloatPoint, * pFloatPoint;

typedef struct _EdgeInfo {
	FloatPoint loc;	// Offsets of edge
	double dy;		// Vertical expanse of edge
	double invSlope;// Inverse slope of edge
} EdgeInfo, * pEdgeInfo;

typedef struct _TriangleInfo {
	EdgeInfo edge [NUM_EDGES];	// Edges
	COLORREF color;				// color value
} TriangleInfo, * pTriangleInfo;

/********************************************************************
*																	*
*							DrawTriangleF							*
*																	*
*	Primary wrapper for triangle filler, with floating points		*
*																	*
********************************************************************/

void DrawTriangle (HDC hDc, pFloatPoint fp0, pFloatPoint fp1, pFloatPoint fp2, COLORREF color);

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

#endif // TRIANGLE_H