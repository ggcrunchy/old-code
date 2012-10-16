/********************************************************************
*																	*
*							Triangle.c								*
*																	*
*	Author:		Steven Johnson										*
*	Purpose:	Implementation of triangle-fill algorithm			*
*																	*
********************************************************************/

/********************************************************************
*																	*
*							Includes								*
*																	*
********************************************************************/

#include "Triangle.h"
#include "MyWin.h"

/********************************************************************
*																	*
*							Globals									*
*																	*
********************************************************************/

static TriangleInfo triangle;

static pEdgeInfo longEdge = triangle.edge + LONG_EDGE;
static pEdgeInfo topEdge = triangle.edge + TOP_EDGE;
static pEdgeInfo lowEdge = triangle.edge + LOW_EDGE;

/********************************************************************
*																	*
*							DrawTriangleF							*
*																	*
*	Primary wrapper for triangle filler, with floating points		*
*																	*
********************************************************************/

void DrawTriangle (HDC hDc, FloatPoint * fp0, FloatPoint * fp1, FloatPoint * fp2, COLORREF color)
{
	int i;
	FloatPoint * points [NUM_POINTS] = {fp0, fp1, fp2};
	pFloatPoint temp, low, mid, high;

	temp = (fp0->y < fp1->y) ? fp0 : fp1, low = (temp->y < fp2->y) ? temp : fp2;
	temp = (fp0->y > fp1->y) ? fp0 : fp1, high = (temp->y > fp2->y) ? temp : fp2;

	MARK_POINT(low), MARK_POINT(high);

	for (i = 0; i < NUM_POINTS; i++)
	{
		if (~points [i]->status & MARKED)
		{
			mid = points [i];
			break;
		}
	}

	UNMARK_POINT(low), UNMARK_POINT(high);

	longEdge->loc = topEdge->loc = *high;

	longEdge->dy = high->y - low->y;
	longEdge->invSlope = (high->x - low->x) / longEdge->dy;

	topEdge->dy = high->y - mid->y;
	topEdge->invSlope = (high->x - mid->x) / topEdge->dy;

	lowEdge->dy = mid->y - low->y;
	lowEdge->invSlope = (mid->x - low->x) / lowEdge->dy;

	triangle.color = color;

	((topEdge->invSlope > lowEdge->invSlope) ? FillLeftOrientedTriangle : FillRightOrientedTriangle) (hDc);
}

/********************************************************************
*																	*
*							FillLeftOrientedTriangle				*
*																	*
*	Fill a triangle with long side to right of middle vertex		*
*																	*
********************************************************************/

static void FillLeftOrientedTriangle (HDC hDc)
{
	double primeEdge = longEdge->loc.x, sideEdge = topEdge->loc.x;
	long color = triangle.color;
	long x, endX;
	long y = (long) ceil (longEdge->loc.y), dy;

	dy = (long) ceil (topEdge->dy);
	while (dy--)
	{
		for (x = (long) ceil (sideEdge), endX = (long) ceil (primeEdge); x < endX;)
		{
			SetPixel (hDc, x++, y, color);
		}

		sideEdge -= topEdge->invSlope;
		primeEdge -= longEdge->invSlope;

		y--;
	}

	dy = (long) ceil (lowEdge->dy);
	while (dy--)
	{
		for (x = (long) ceil (sideEdge), endX = (long) ceil (primeEdge); x < endX;)
		{
			SetPixel (hDc, x++, y, color);
		}

		sideEdge -= lowEdge->invSlope;
		primeEdge -= longEdge->invSlope;

		y--;
	}
}

/********************************************************************
*																	*
*							FillRightOrientedTriangle				*
*																	*
*	Fill a triangle with long side to left of middle vertex			*
*																	*
********************************************************************/

static void FillRightOrientedTriangle (HDC hDc)
{
	double primeEdge = longEdge->loc.x, sideEdge = topEdge->loc.x;
	long color = triangle.color;
	long x, endX;
	long y = (long) ceil (longEdge->loc.y), dy;

	dy = (long) ceil (topEdge->dy);
	while (dy--)
	{
		for (x = (long) ceil (primeEdge), endX = (long) ceil (sideEdge); x < endX;)
		{
			SetPixel (hDc, x++, y, color);
		}

		sideEdge -= topEdge->invSlope;
		primeEdge -= longEdge->invSlope;

		y--;
	}

	dy = (long) ceil (lowEdge->dy);
	while (dy--)
	{
		for (x = (long) ceil (primeEdge), endX = (long) ceil (sideEdge); x < endX;)
		{
			SetPixel (hDc, x++, y, color);
		}

		sideEdge -= lowEdge->invSlope;
		primeEdge -= longEdge->invSlope;

		y--;
	}
}
