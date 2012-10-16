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

	FIXED_DIV(2,3);
	FIXED_MOD(1,2);
	FIXED_FLOOR(0);
	FIXED_CEIL(2);

	points [0] = p0;
	points [1] = p1;
	points [2] = p2;

	for (j = 0; j < NUM_POINTS; ++j)
	{
		for (i = 0; i < NUM_POINTS - 1; ++i)
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
		WriteColumn (hDc, Ceiling (points [LOW_POINT].x, RADIX), Ceiling (points [LOW_POINT].y, RADIX), Ceiling (points [HIGH_POINT].y, RADIX));
		return;
	}

	if (!longEdge->dy)					// Degenerate case
	{
		WriteRow    (hDc, Ceiling (points [LOW_POINT].x, RADIX), Ceiling (points [HIGH_POINT].x, RADIX), Ceiling (points [LOW_POINT].y, RADIX));
		return;
	}

	left = FIXED_MULT (topEdge->dx, lowEdge->dy) 
		 > FIXED_MULT (lowEdge->dx, topEdge->dy);
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
	long y		 = Ceiling (points [LOW_POINT].y, RADIX);
	long Dy;

	long lEdge	 = Ceiling (points [LOW_POINT].x, RADIX);
	long rEdge   = Ceiling (points [LOW_POINT].x, RADIX);

	long lDx;
	long lDy;
	long rDx	 = FIXED_MULT (longEdge->dx, RADIX);
	long rDy	 = FIXED_MULT (longEdge->dy, RADIX);

	long lConst;
	long rConst  = Floor (longEdge->dx, longEdge->dy);

	long lMod;
	long rMod    = Mod (rDx,rDy);

	long lNumer;  
	long rNumer  = FIXED_MULT (points [LOW_POINT].x, longEdge->dy);

	lDx    = FIXED_MULT (lowEdge->dx, RADIX);
	lDy    = FIXED_MULT (lowEdge->dy, RADIX);
	lMod   = Mod (lDx,lDy);
	lConst = Floor (lowEdge->dx, lowEdge->dy);
	lNumer = FIXED_MULT (points [LOW_POINT].x, lowEdge->dy);

	Dy = Ceiling (lowEdge->dy, RADIX);
	while (Dy--)
	{
		WriteRow (hDc, lEdge, rEdge, y++);

		lEdge += lConst + Floor (Mod (lNumer, lDy) + lMod, lDy);
		rEdge += rConst + Floor (Mod (rNumer, rDy) + rMod, rDy);

		lNumer += lDx;
		rNumer += rDx;
	}

	lDx    = FIXED_MULT (topEdge->dx, RADIX);
	lDy    = FIXED_MULT (topEdge->dy, RADIX);
	lMod   = Mod (lDx,lDy);
	lConst = Floor (topEdge->dx, topEdge->dy);
	lNumer = FIXED_MULT (points [MID_POINT].x, topEdge->dy);

	Dy = Ceiling (topEdge->dy, RADIX);
	while (Dy--)
	{
		WriteRow (hDc, lEdge, rEdge, y++);

		lEdge += lConst + Floor (Mod (lNumer, lDy) + lMod, lDy);
		rEdge += rConst + Floor (Mod (rNumer, rDy) + rMod, rDy);

		lNumer += lDx;
		rNumer += rDx;
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
	long y		 = Ceiling (points [LOW_POINT].y, RADIX);
	long Dy;

	long lEdge   = Ceiling (points [LOW_POINT].x, RADIX);
	long rEdge   = Ceiling (points [LOW_POINT].x, RADIX);

	long lDx     = FIXED_MULT (longEdge->dx, RADIX);
	long lDy     = FIXED_MULT (longEdge->dy, RADIX);
	long rDx;
	long rDy;

	long lConst  = Floor (longEdge->dx, longEdge->dy);
	long rConst;

	long lMod    = Mod (lDx, lDy);
	long rMod;

	long lNumer  = FIXED_MULT (points [LOW_POINT].x, longEdge->dy);
	long rNumer;
	
	rDx    = FIXED_MULT (lowEdge->dx, RADIX);
	rDy    = FIXED_MULT (lowEdge->dy, RADIX);
	rMod   = Mod (rDx, rDy);
	rConst = Floor (lowEdge->dx, lowEdge->dy);
	rNumer = FIXED_MULT (points [LOW_POINT].x, lowEdge->dy);

	Dy = Ceiling (lowEdge->dy, RADIX);
	while (Dy--)
	{
		WriteRow (hDc, lEdge, rEdge, y++);
		
		lEdge += lConst + Floor (Mod (lNumer, lDy) + lMod, lDy);
		rEdge += rConst + Floor (Mod (rNumer, rDy) + rMod, rDy);

		lNumer += lDx;
		rNumer += rDx;
	}

	rDx    = FIXED_MULT (topEdge->dx, RADIX);
	rDy    = FIXED_MULT (topEdge->dy, RADIX);
	rMod   = Mod (rDx, rDy);
	rConst = Floor (topEdge->dx, topEdge->dy);
	rNumer = FIXED_MULT (points [MID_POINT].x, topEdge->dy);

	Dy = Ceiling (topEdge->dy, RADIX);
	while (Dy--)
	{
		WriteRow (hDc, lEdge, rEdge, y++);
 
		lEdge += lConst + Floor (Mod (lNumer, lDy) + lMod, lDy);
		rEdge += rConst + Floor (Mod (rNumer, rDy) + rMod, rDy);

		lNumer += lDx;
		rNumer += rDx;
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
*							Ceiling                                 *
*                                                                   *
*	Computes the ceiling of a fractional value                      *
*                                                                   *
********************************************************************/

static long Ceiling (long numerator, long denominator)
{
	return Floor (numerator - 1, denominator) + 1;
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
		return numerator * denominator > 0 ? 0 : -1;	// -1 if value if less than 0, 0 otherwise
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
	return value - base * Floor (value, base);
}

/********************************************************************
*																	*
*							FixedPoint_24_8							*
*																	*
*	Used to compose 24:8 fixed point numbers						*
*																	*
********************************************************************/

FIXED_TYPE MAKE_FIXED (long Integer, long Real)
{
	FIXED_TYPE FixedPoint;
	long sign = (Integer < 0) ^ (Real < 0);

	Integer = (Integer > 0 ? Integer : -Integer) & SIGN_MASK;
	Real	= (Real	   > 0 ? Real	 : -Real)	 & REAL_MASK;

	FixedPoint = (Integer << PRECISION) | Real;
	FixedPoint = sign ? -FixedPoint : FixedPoint;

	return FixedPoint;
}

static FIXED_TYPE FIXED_MULT (FIXED_TYPE FixedOne, FIXED_TYPE FixedTwo)
{
	FIXED_TYPE Product;
	long sign = (FixedOne < 0) ^ (FixedTwo < 0);

	FixedOne = FixedOne > 0 ? FixedOne : -FixedOne;
	FixedTwo = FixedTwo > 0 ? FixedTwo : -FixedTwo;

	Product   = (FixedOne * FixedTwo) & SIGN_MASK;
	Product >>= PRECISION;

	return sign ? -Product : Product;
}

static FIXED_TYPE FIXED_DIV (FIXED_TYPE FixedOne, FIXED_TYPE FixedTwo)
{
	FIXED_TYPE Quotient;
	long sign = (FixedOne < 0) ^ (FixedTwo < 0);

	if (FixedTwo == 0)
	{
		return 0;
	}

	FixedOne = FixedOne > 0 ? FixedOne : -FixedOne;
	FixedTwo = FixedTwo > 0 ? FixedTwo : -FixedTwo;

	Quotient   = FixedOne / FixedTwo;
	Quotient <<= PRECISION;
	Quotient  &= SIGN_MASK;

	return sign ? -Quotient : Quotient;
}

static FIXED_TYPE FIXED_CEIL (FIXED_TYPE Fixed)
{
	long sign = Fixed < 0;

	long Integer;
	long Real;

	Fixed	= sign ? -Fixed : Fixed;

	Integer = Fixed >> PRECISION;
	Real	= Fixed  & SIGN_MASK;

	if (Real == 0)
	{
		return sign ? -Fixed : Fixed;
	}

	else if (Integer == 0)            // |Value| < 1
	{
		return MAKE_FIXED (sign ? 0 : 1, 0);	// -1 if value if less than 0, 0 otherwise
	}

	else if (sign)                   // Value < -1
	{
		return -(MAKE_FIXED (Integer - 1, 0));
	}

	else                             // Value > 1
	{
		return MAKE_FIXED (Integer + 1, 0);
	}
}

static FIXED_TYPE FIXED_FLOOR (FIXED_TYPE Fixed)
{
	long sign    = Fixed < 0;

	long Integer;
	long Real;

	Fixed	= sign ? -Fixed : Fixed;

	Integer = Fixed >> PRECISION;
	Real	= Fixed  & SIGN_MASK;

	if (Real == 0)
	{
		return sign ? -Fixed : Fixed;
	}

	else if (Integer == 0)            // |Value| < 1
	{
		return MAKE_FIXED (sign ? -1 : 0, 0);	// -1 if value if less than 0, 0 otherwise
	}

	else if (sign)                   // Value < -1
	{
		return -(MAKE_FIXED (Integer + 1, 0));
	}

	else                             // Value > 1
	{
		return MAKE_FIXED (Integer, 0);
	}
}

static FIXED_TYPE FIXED_MOD (FIXED_TYPE FixedOne, FIXED_TYPE FixedTwo)
{
	return FixedOne - FIXED_MULT (FixedTwo, FIXED_FLOOR (FIXED_DIV (FixedOne, FixedTwo)));
}
