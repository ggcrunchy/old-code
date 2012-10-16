/********************************************************************
*																	*
*							Bresenham.c								*
*																	*
*	Author:		Steven Johnson										*
*	Purpose:	Contains implementation of Bresenham line algorithm	*
*																	*
********************************************************************/

/********************************************************************
*																	*
*							Includes								*
*																	*
********************************************************************/

#include "Bresenham.h"

/********************************************************************
*																	*
*							Bresenham								*
*																	*
*	Purpose:	Primary line drawing function						*
*																	*
********************************************************************/

void Bresenham (HDC hDc, long x0, long y0, long x1, long y1, COLORREF color)
{
	LineInfo line;

	line.x0 = x0;
	line.y0 = y0;
	line.x1 = x1;
	line.y1 = y1;
	line.dx = x1 - x0;
	line.dy = y1 - y0;
	line.Adx = abs (line.dx);
	line.Ady = abs (line.dy);
	line.xStep = (line.dx > 0) ? 1 : -1;
	line.yStep = (line.dy > 0) ? 1 : -1;
	line.color = color;

	line.Adx >>= 1;
	line.Ady >>= 1;

	PLOT_LINE_PIXELS_DIRECT(hDc,line);

	if (!line.Adx)
	{
		VertLine (hDc, &line);
	}

	else if (!line.Ady)
	{
		HorzLine (hDc, &line);
	}

	else if (line.Adx == line.Ady)
	{
		DiagLine (hDc, &line);
	}

	else
	{
		(line.Adx > line.Ady ? LowLine : SteepLine) (hDc, &line);
	}
}

/********************************************************************
*																	*
*							VertLine								*
*																	*
*	Purpose:	Draw a vertical line								*
*																	*
********************************************************************/

static void VertLine (HDC hDc, LineInfo * line)
{
	while (line->Ady--)
	{
		line->y0 += line->yStep;
		line->y1 -= line->yStep;
	
		PLOT_LINE_PIXELS_INDIRECT(hDc,line);
	}
}

/********************************************************************
*																	*
*							HorzLine								*
*																	*
*	Purpose:	Draw a horizontal line								*
*																	*
********************************************************************/

static void HorzLine (HDC hDc, LineInfo * line)
{
	while (line->Adx--)
	{
		line->x0 += line->xStep;
		line->x1 -= line->xStep;

		PLOT_LINE_PIXELS_INDIRECT(hDc,line);
	}
}

/********************************************************************
*																	*
*							DiagLine								*
*																	*
*	Purpose:	Draw a diagonal line								*
*																	*
********************************************************************/

static void DiagLine (HDC hDc, LineInfo * line)
{
	while (line->Adx--)
	{
		line->x0 += line->xStep, line->y0 += line->yStep;
		line->x1 -= line->xStep, line->y1 -= line->yStep;

		PLOT_LINE_PIXELS_INDIRECT(hDc,line);
	}
}

/********************************************************************
*																	*
*							LowLine									*
*																	*
*	Purpose:	Draw a line such that 0 < |m| < 1					*
*																	*
********************************************************************/

static void LowLine (HDC hDc, LineInfo * line)
{
	int diff = (line->Ady << 1) - line->Adx;
	int dLateral = line->Ady << 1;
	int dDiagonal = (line->Ady - line->Adx) << 1;

	while (line->Adx--)
	{
		line->x0 += line->xStep;
		line->x1 -= line->xStep;

		if (diff > 0)
		{
			line->y0 += line->yStep;
			line->y1 -= line->yStep;

			diff += dDiagonal;
		}

		else
		{
			diff += dLateral;
		}

		PLOT_LINE_PIXELS_INDIRECT(hDc,line);
	}
}

/********************************************************************
*																	*
*							SteepLine								*
*																	*
*	Purpose:	Draw a line such that |m| > 1						*
*																	*
********************************************************************/

static void SteepLine (HDC hDc, LineInfo * line)
{
	int diff = (line->Adx << 1) - line->Ady;
	int dLateral = line->Adx << 1;
	int dDiagonal = (line->Adx - line->Ady) << 1;

	while (line->Ady--)
	{
		line->y0 += line->yStep;
		line->y1 -= line->yStep;

		if (diff > 0)
		{
			line->x0 += line->xStep;
			line->x1 -= line->xStep;

			diff += dDiagonal;
		}

		else
		{
			diff += dLateral;
		}

		PLOT_LINE_PIXELS_INDIRECT(hDc,line);
	}
}
