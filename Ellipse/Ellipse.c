/********************************************************************
*																	*
*							Ellipse.c								*
*																	*
*	Author:		Steven Johnson										*
*	Purpose:	Contains implementation of ellipse algorithm		*
*																	*
********************************************************************/

/********************************************************************
*																	*
*							Includes								*
*																	*
********************************************************************/

#include "Bresenham.h"
#include "Ellipse.h"

/********************************************************************
*																	*
*							Globals									*
*																	*
********************************************************************/

static EllipseInfo ellipse;

/********************************************************************
*																	*
*							DrawEllipse								*
*																	*
*	Purpose:	Primary ellipse drawing function					*
*																	*
********************************************************************/

void DrawEllipse (HDC hDc, long centerX, long centerY, long a, long b, COLORREF color, COLORREF fillColor, BOOL fill)
{
	ellipse.centerX		= centerX;
	ellipse.centerY		= centerY;
	ellipse.x			= a > 0 ? a : -a;
	ellipse.y			= 0;
	ellipse.a_square	= a * a;
	ellipse.b_square	= b * b;
	ellipse.dx_square	= (ellipse.x << 1) * ellipse.b_square - ellipse.b_square;
	ellipse.dy_square	= ellipse.a_square;
	ellipse.r_square	= ellipse.a_square * ellipse.b_square;
	ellipse.x_square	= ellipse.a_square - ellipse.dx_square;
	ellipse.y_square	= 0;
	ellipse.Ax			= ellipse.b_square * ellipse.x;
	ellipse.Ay			= 0;
	ellipse.color		= color;
	ellipse.fill		= fillColor;

	PLOT_INITIAL_ELLIPSE_PIXELS_DIRECT(hDc,ellipse);
	
	(fill ? FilledEllipse : UnfilledEllipse) (hDc);
}

/********************************************************************
*																	*
*							UnfilledEllipse							*
*																	*
*	Purpose:	Draw an unfilled ellipse							*
*																	*
********************************************************************/

static void UnfilledEllipse (HDC hDc)
{
	long diff;
	long dHorz, dVert, dDiag;
	long a_squ2 = ellipse.a_square << 1, b_squ2 = ellipse.b_square << 1;
	long sum_squ2 = a_squ2 + b_squ2;
	long x = ellipse.x, y = ellipse.y;
	long centerX = ellipse.centerX, centerY = ellipse.centerY;
	long color = ellipse.color;

	{
		long init = -((ellipse.b_square * ellipse.x) << 1);

		diff = (init + a_squ2 + ellipse.a_square) >> 2;
		dVert = 3 * ellipse.a_square;
		dDiag = init + ellipse.b_square + a_squ2;
	}

	while (ellipse.Ax > ellipse.Ay)
	{	
		if (diff > 0)
		{
			x--;
			ellipse.Ax -= ellipse.b_square;

			diff += dDiag;

			dDiag += sum_squ2;
			dVert += a_squ2;
		}

		else
		{
			diff += dVert;

			dVert += a_squ2;
			dDiag += a_squ2;
		}

		y--;
		ellipse.Ay += ellipse.a_square;

		PLOT_ELLIPSE_PIXELS(hDc,centerX,centerY,x,y,color);
	}

	dHorz = ellipse.b_square * (-(x << 1) + 1);

	while (x--)
	{
		if (diff < 0)
		{
			y--;

			diff += dDiag;

			dDiag += sum_squ2;
			dHorz += b_squ2;
		}

		else
		{
			diff += dHorz;

			dHorz += b_squ2;
			dDiag += b_squ2;
		}

		PLOT_ELLIPSE_PIXELS(hDc,centerX,centerY,x,y,color);
	}
}

/********************************************************************
*																	*
*							FilledEllipse							*
*																	*
*	Purpose:	Draw an ellipse, filled with a certain color		*
*																	*
********************************************************************/

static void FilledEllipse (HDC hDc)
{
}
