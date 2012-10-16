/********************************************************************
*																	*
*							Bresenham.h								*
*																	*
*	Author:		Steven Johnson										*
*	Purpose:	Header for implementation of Bresenham algorithm	*
*																	*
********************************************************************/

#ifndef BRESENHAM_H
#define BRESENHAM_H

/********************************************************************
*																	*
*							Includes								*
*																	*
********************************************************************/

#include <windows.h>

/********************************************************************
*																	*
*							Macros									*
*																	*
********************************************************************/

#define PLOT_LINE_PIXELS_INDIRECT(hdc,line)	SetPixel (hDc, (line)->x0, (line)->y0, (line)->color);	\
											SetPixel (hDc, (line)->x1, (line)->y1, (line)->color);

#define PLOT_LINE_PIXELS_DIRECT(hDc,line)	SetPixel (hDc, (line).x0, (line).y0, (line).color);		\
											SetPixel (hDc, (line).x1, (line).y1, (line).color);

/********************************************************************
*																	*
*							Types									*
*																	*
********************************************************************/

typedef struct _LineInfo {
	long x0;		// Inital x
	long x1;		// Terminal x
	long y0;		// Initial y
	long y1;		// Terminal y
	long dx;		// Delta of x
	long dy;		// Delta of y
	long Adx;		// Absolute value of dx
	long Ady;		// Absolute value of dy
	long xStep;		// x step value
	long yStep;		// y step value
	COLORREF color;	// color value
} LineInfo, * pLineInfo;

/********************************************************************
*																	*
*							Bresenham								*
*																	*
*	Purpose:	Primary line drawing function						*
*																	*
********************************************************************/

void Bresenham (HDC hDc, long x0, long y0, long x1, long y1, COLORREF color);

/********************************************************************
*																	*
*							VertLine								*
*																	*
*	Purpose:	Draw a vertical line								*
*																	*
********************************************************************/

static void VertLine (HDC hDc, pLineInfo line);

/********************************************************************
*																	*
*							HorzLine								*
*																	*
*	Purpose:	Draw a horizontal line								*
*																	*
********************************************************************/

static void HorzLine (HDC hDc, pLineInfo line);

/********************************************************************
*																	*
*							DiagLine								*
*																	*
*	Purpose:	Draw a diagonal line								*
*																	*
********************************************************************/

static void DiagLine (HDC hDc, pLineInfo line);

/********************************************************************
*																	*
*							LowLine									*
*																	*
*	Purpose:	Draw a line such that 0 < |m| < 1					*
*																	*
********************************************************************/

static void LowLine (HDC hDc, pLineInfo line);

/********************************************************************
*																	*
*							SteepLine								*
*																	*
*	Purpose:	Draw a line such that |m| > 1						*
*																	*
********************************************************************/

static void SteepLine (HDC hDc, pLineInfo line);

#endif // BRESENHAM_H