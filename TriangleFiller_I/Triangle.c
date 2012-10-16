/********************************************************************
*                                                                   *
*							Triangle.c								*
*																	*
*	Author:         Steven Johnson                                  *
*   Purpose:        Implementation of triangle-fill algorithm       *
*                                                                   *
********************************************************************/

/********************************************************************
*                                                                   *
*							Includes                                *
*                                                                   *
********************************************************************/

#include "Triangle.h"
#include "MyWin.h"

/********************************************************************
*                                                                   *
*                           Globals                                 *
*                                                                   *
********************************************************************/

static TriangleInfo triangle;
static POINT points [NUM_POINTS];

static pEdgeInfo lowEdge  = triangle.edge + LOW_EDGE;
static pEdgeInfo topEdge  = triangle.edge + TOP_EDGE;
static pEdgeInfo longEdge = triangle.edge + LONG_EDGE;

/********************************************************************
*                                                                   *
*                            DrawTriangle                           *
*                                                                   *
*	Primary wrapper for triangle filler, with integers				*
*                                                                   *
********************************************************************/

void DrawTriangle (HDC hDc, POINT p0, POINT p1, POINT p2, COLORREF color)
{
	int i, j;
	BOOL left;

	points [0] = p0;
	points [1] = p1;
	points [2] = p2;

	for (j = 0; j < NUM_POINTS; j++)
	{
		for (i = 0; i < NUM_POINTS - 1; i++)
		{
			if (points [i].y > points [i + 1].y)    // Sort y values in ascending order
			{
				SwapPoints (&points [i], &points [i + 1]);
			}
		}
	}

	longEdge->dx	= points [HIGH_POINT].x - points [LOW_POINT].x;
	longEdge->dy    = points [HIGH_POINT].y - points [LOW_POINT].y;

	topEdge->dx     = points [HIGH_POINT].x - points [MID_POINT].x;
	topEdge->dy     = points [HIGH_POINT].y - points [MID_POINT].y;

	lowEdge->dx     = points [MID_POINT].x  - points [LOW_POINT].x;
	lowEdge->dy     = points [MID_POINT].y  - points [LOW_POINT].y;

	triangle.color  = color;

	if (!topEdge->dx && !lowEdge->dx)	// Degenerate case
	{
		WriteColumn (hDc, points [LOW_POINT].x, points [LOW_POINT].y, points [HIGH_POINT].y);
		return;
	}

	if (!longEdge->dy)					// Degenerate case
	{
		WriteRow (hDc, points [LOW_POINT].x, points [HIGH_POINT].x, points [LOW_POINT].y);
		return;
	}

	left = topEdge->dx * lowEdge->dy > lowEdge->dx * topEdge->dy;
	// Find common denominator and compare inverse slopes

	(left ? FillLeftOrientedTriangle : FillRightOrientedTriangle) (hDc);
}

/********************************************************************
*                                                                   *
*							FillLeftOrientedTriangle                *
*                                                                   *
*	Fill a triangle with long side to right of middle vertex		*
*                                                                   *
********************************************************************/

static void FillLeftOrientedTriangle (HDC hDc)
{
	long y          = points [LOW_POINT].y;

	long leftEdge   = points [LOW_POINT].x;
	long rightEdge  = points [LOW_POINT].x;

	long leftDx;
	long leftDy;
	long rightDx    = longEdge->dx;
	long rightDy    = longEdge->dy;

	long leftConst;
	long rightConst = Floor (longEdge->dx, longEdge->dy);

	long leftMod;
	long rightMod   = Mod (longEdge->dx, longEdge->dy);

	long leftNumer  = -1;
	long rightNumer = -1;
	
	leftConst = Floor (lowEdge->dx, lowEdge->dy);
	leftDx    = lowEdge->dx;
	leftDy    = lowEdge->dy;
	leftMod   = Mod (leftDx, leftDy);

	while (lowEdge->dy--)
	{
		WriteRow (hDc, leftEdge, rightEdge, y++);

		leftEdge    += leftConst  + Floor (Mod (leftNumer,  leftDy)  + leftMod,  leftDy);
		rightEdge	+= rightConst + Floor (Mod (rightNumer, rightDy) + rightMod, rightDy);

		leftNumer   += leftDx;
		rightNumer  += rightDx;
	}

	leftConst = Floor (topEdge->dx, topEdge->dy);
	leftDx    = topEdge->dx;
	leftDy    = topEdge->dy;
	leftMod   = Mod (leftDx, leftDy);

	while (topEdge->dy--)
	{
		WriteRow (hDc, leftEdge, rightEdge, y++);

		leftEdge    += leftConst  + Floor (Mod (leftNumer,  leftDy)  + leftMod,  leftDy);
		rightEdge	+= rightConst + Floor (Mod (rightNumer, rightDy) + rightMod, rightDy);

		leftNumer   += leftDx;
		rightNumer  += rightDx;
	}
}

/********************************************************************
*                                                                   *
*							FillRightOrientedTriangle               *
*                                                                   *
*	Fill a triangle with long side to left of middle vertex         *
*                                                                   *
********************************************************************/

static void FillRightOrientedTriangle (HDC hDc)
{
	long y          = points [LOW_POINT].y;

	long leftEdge   = points [LOW_POINT].x;
	long rightEdge  = points [LOW_POINT].x;

	long leftDx     = longEdge->dx;
	long leftDy     = longEdge->dy;
	long rightDx;
	long rightDy;

	long leftConst  = Floor (longEdge->dx, longEdge->dy);
	long rightConst;

	long leftMod    = Mod (longEdge->dx, longEdge->dy);
	long rightMod;

	long leftNumer  = -1;
	long rightNumer = -1;

	rightConst = Floor (lowEdge->dx, lowEdge->dy);
	rightDx    = lowEdge->dx;
	rightDy    = lowEdge->dy;
	rightMod   = Mod (rightDx, rightDy);

	while (lowEdge->dy--)
	{
		WriteRow (hDc, leftEdge, rightEdge, y++);

		leftEdge    += leftConst  + Floor (Mod (leftNumer,  leftDy)  + leftMod,  leftDy);
		rightEdge   += rightConst + Floor (Mod (rightNumer, rightDy) + rightMod, rightDy);
		
		leftNumer   += leftDx;
		rightNumer	+= rightDx;
	}

	rightConst = Floor (topEdge->dx, topEdge->dy);
	rightDx    = topEdge->dx;
	rightDy    = topEdge->dy;
	rightMod   = Mod (rightDx, rightDy);

	while (topEdge->dy--)
	{
		WriteRow (hDc, leftEdge, rightEdge, y++);

		leftEdge    += leftConst  + Floor (Mod (leftNumer,  leftDy)  + leftMod,  leftDy);
		rightEdge   += rightConst + Floor (Mod (rightNumer, rightDy) + rightMod, rightDy);  

		leftNumer   += leftDx;
		rightNumer	+= rightDx;

	}
}

/********************************************************************
*                                                                   *
*							WriteRow                                *
*                                                                   *
*	Fill a triangle row from one x value to another, at a given y   *
*                                                                   *
********************************************************************/

static void WriteRow (HDC hDc, long x, long endX, long y)
{
	while (x < endX)
	{
		SetPixel (hDc, x++, y, triangle.color);
	}
}

/********************************************************************
*																	*
*							WriteColumn								*
*																	*
*	Fill a triangle column from one y value to another, given an x	*
*																	*
********************************************************************/

static void WriteColumn (HDC hDc, long x, long y, long endY)
{
	while (y < endY)
	{
		SetPixel (hDc, x, y++, triangle.color);
	}
}

/********************************************************************
*                                                                   *
*							SwapPoints                              *
*                                                                   *
*	Exchanges two given POINTs in an array                          *
*                                                                   *
********************************************************************/

static void SwapPoints (POINT * p1, POINT * p2)
{
	POINT temp;

	temp = *p2;
	*p2  = *p1;
	*p1  = temp;
}

/********************************************************************
*                                                                   *
*							Floor                                   *
*                                                                   *
*	Computes the floor of a fractional value                        *
*                                                                   *
********************************************************************/

static long Floor (long numerator, long denominator)
{
	long div;
	
	if (!denominator)   // Zero denominators are meaningless
	{
		return 0;
	}

	div = numerator / denominator;    // Integer divide
	
	if (div * denominator == numerator)     // Integer value
	{
		return div;
	}

	else if (div == 0)                      // |Value| < 1
	{
		return (numerator * denominator) > 0 ? 0 : -1;  // -1 if value if less than 0, 0 otherwise
	}

	else if (div < 0)                       // Value < -1
	{
		return div - 1;
	}

	else                                    // Value > 1
	{
		return div;
	}
}

/********************************************************************
*                                                                   *
*							Mod                                     *
*                                                                   *
*	Computes the modulo of two values                               *
*                                                                   *
********************************************************************/

static long Mod (long value, long base)
{
	if (base <= 0)
	{
		return 0;
	}

	return value - base * Floor (value, base);
}
