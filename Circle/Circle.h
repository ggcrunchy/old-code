/********************************************************************
*																	*
*							Circle.h								*
*																	*
*	Author:		Steven Johnson										*
*	Purpose:	Header for implementation of circle algorithm		*
*																	*
********************************************************************/

#ifndef CIRCLE_H
#define CIRCLE_H

/********************************************************************
*																	*
*							Includes								*
*																	*
********************************************************************/

#include <windows.h>

/********************************************************************
*																	*
*							Defines									*
*																	*
********************************************************************/

#define DX_SQUARE_INC	2
#define DY_SQUARE_INC	2

/********************************************************************
*																	*
*							Macros									*
*																	*
********************************************************************/

#define PLOT_INITIAL_CIRCLE_PIXELS_INDIRECT(hDc,circle)	SetPixel (hDc, (circle)->centerX + (circle)->x, (circle)->centerY + (circle)->y, (circle)->color),	\
														SetPixel (hDc, (circle)->centerX - (circle)->x, (circle)->centerY + (circle)->y, (circle)->color)

#define PLOT_INITIAL_CIRCLE_PIXELS_DIRECT(hDc,circle)	SetPixel (hDc, (circle).centerX + (circle).x, (circle).centerY + (circle).y, (circle).color),	\
														SetPixel (hDc, (circle).centerX - (circle).x, (circle).centerY + (circle).y, (circle).color)

#define PLOT_CIRCLE_PIXELS(hDc,centerX,centerY,x,y,color)	SetPixel (hDc, centerX + x, centerY + y, color),	\
															SetPixel (hDc, centerX - x, centerY + y, color),	\
															SetPixel (hDc, centerX + x, centerY - y, color),	\
															SetPixel (hDc, centerX - x, centerY - y, color),	\
															SetPixel (hDc, centerX + y, centerY + x, color),	\
															SetPixel (hDc, centerX - y, centerY + x, color),	\
															SetPixel (hDc, centerX + y, centerY - x, color),	\
															SetPixel (hDc, centerX - y, centerY - x, color)

#define FILL_CIRCLE(hDc,centerX,centerY,x,y,color)	Bresenham (hDc, centerX + x - 1, centerY + y - 1, centerX + x - 1, centerY - y + 1, color),	\
													Bresenham (hDc, centerX - x + 1, centerY + y - 1, centerX - x + 1, centerY - y + 1, color),	\
													Bresenham (hDc, centerX + y - 1, centerY + x - 1, centerX - y + 1, centerY + x - 1, color),	\
													Bresenham (hDc, centerX - y + 1, centerY - x + 1, centerX + y - 1, centerY - x + 1, color)

#define FILL_CIRCLE_HULL(hDc,centerY,x1,x2,y,dim,color)	while (dim--)													\
															Bresenham (hDc, x1, centerY + y, x2, centerY + y,color),	\
															Bresenham (hDc, x1, centerY - y, x2, centerY - y,color),	\
															y--;															

/********************************************************************
*																	*
*							Types									*
*																	*
********************************************************************/

typedef struct _CircleInfo {
	long centerX;	// Central x
	long centerY;	// Central y
	long x;			// x offset
	long y;			// y offset
	long dx_square;	// Change in x^2
	long dy_square;	// Change in y^2
	long x_square;	// x^2
	long r_square;	// r^2
	long radius;	// Radius
	long color;		// Edge color
	long fill;		// Fill color
} CircleInfo, * pCircleInfo;

/********************************************************************
*																	*
*							Circle									*
*																	*
*	Purpose:	Primary circle drawing function						*
*																	*
********************************************************************/

void Circle (HDC hDc, long centerX, long centerY, long radius, COLORREF color, COLORREF fillColor, BOOL fill);

/********************************************************************
*																	*
*							UnfilledCircle							*
*																	*
*	Purpose:	Draw an unfilled circle								*
*																	*
********************************************************************/

static void UnfilledCircle (HDC hDc);

/********************************************************************
*																	*
*							FilledCircle							*
*																	*
*	Purpose:	Draw a circle filled with a color value				*
*																	*
********************************************************************/

static void FilledCircle (HDC hDc);

#endif // CIRCLE_H