/********************************************************************
*																	*
*							Ellipse.h								*
*																	*
*	Author:		Steven Johnson										*
*	Purpose:	Header for implementation of ellipse algorithm		*
*																	*
********************************************************************/

#ifndef ELLIPSE_H
#define ELLIPSE_H

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

#define PLOT_INITIAL_ELLIPSE_PIXELS_INDIRECT(hDc,ellipse)	SetPixel (hDc, (ellipse)->centerX + (ellipse)->x, (ellipse)->centerY + (ellipse)->y, (ellipse)->color),	\
															SetPixel (hDc, (ellipse)->centerX - (ellipse)->x, (ellipse)->centerY + (ellipse)->y, (ellipse)->color)

#define PLOT_INITIAL_ELLIPSE_PIXELS_DIRECT(hDc,ellipse)	SetPixel (hDc, (ellipse).centerX + (ellipse).x, (ellipse).centerY + (ellipse).y, (ellipse).color),	\
														SetPixel (hDc, (ellipse).centerX - (ellipse).x, (ellipse).centerY + (ellipse).y, (ellipse).color)

#define PLOT_ELLIPSE_PIXELS(hDc,centerX,centerY,x,y,color)	SetPixel (hDc, centerX + x, centerY + y, color),	\
															SetPixel (hDc, centerX - x, centerY + y, color),	\
															SetPixel (hDc, centerX + x, centerY - y, color),	\
															SetPixel (hDc, centerX - x, centerY - y, color)		

/********************************************************************
*																	*
*							Types									*
*																	*
********************************************************************/

typedef struct _EllipseInfo {
	long centerX;	// Central x
	long centerY;	// Central y
	long x;			// x offset
	long y;			// y offset
	long Ax;		// Absolute value of x
	long Ay;		// Absolute value of y
	long dx_square;	// Change in x^2
	long dy_square;	// Change in y^2
	long x_square;	// x^2
	long y_square;	// y^2
	long r_square;	// r^2
	long a_square;	// a^2
	long b_square;	// b^2
	long color;		// Edge color
	long fill;		// Fill color
} EllipseInfo, * pEllipseeInfo;

/********************************************************************
*																	*
*							DrawEllipse								*
*																	*
*	Purpose:	Primary ellipse drawing function					*
*																	*
********************************************************************/

void DrawEllipse (HDC hDc, long centerX, long centerY, long a, long b, COLORREF color, COLORREF fillColor, BOOL fill);

/********************************************************************
*																	*
*							UnfilledEllipse							*
*																	*
*	Purpose:	Draw an unfilled ellipse							*
*																	*
********************************************************************/

static void UnfilledEllipse (HDC hDc);

/********************************************************************
*																	*
*							FilledEllipse							*
*																	*
*	Purpose:	Draw an ellipse, filled with a certain color		*
*																	*
********************************************************************/

static void FilledEllipse (HDC hDc);

#endif // ELLIPSE_H