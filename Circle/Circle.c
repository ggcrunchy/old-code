/********************************************************************
*																	*
*							Circle.c								*
*																	*
*	Author:		Steven Johnson										*
*	Purpose:	Contains implementation of circle algorithm			*
*																	*
********************************************************************/

/********************************************************************
*																	*
*							Includes								*
*																	*
********************************************************************/

#include "..//Bresenham//Bresenham.h"
#include "Circle.h"

/********************************************************************
*																	*
*							Globals									*
*																	*
********************************************************************/

static CircleInfo circle;

/********************************************************************
*																	*
*							Circle									*
*																	*
*	Purpose:	Primary circle drawing function						*
*																	*
********************************************************************/

void Circle (HDC hDc, long centerX, long centerY, long radius, COLORREF color, COLORREF fillColor, BOOL fill)
{
	circle.centerX = centerX;
	circle.centerY = centerY;
	circle.x = radius;
	circle.y = 0;
	circle.dx_square = (radius << 1) - 1;
	circle.dy_square = 1;
	circle.r_square = radius * radius;
	circle.x_square = circle.r_square - circle.dx_square;
	circle.color = color;
	circle.fill = fillColor;

	PLOT_INITIAL_CIRCLE_PIXELS_DIRECT(hDc,circle);
	
	(fill ? FilledCircle : UnfilledCircle) (hDc);
}

/********************************************************************
*																	*
*							UnfilledCircle							*
*																	*
*	Purpose:	Draw an unfilled circle								*
*																	*
********************************************************************/

static void UnfilledCircle (HDC hDc)
{
	long centerX = circle.centerX, centerY = circle.centerY;
	long x = circle.x, y = circle.y;
	long color = circle.color;

	while (x > y++)
	{
		if (circle.r_square <= circle.x_square)
		{
			x--;

			circle.dx_square -= DX_SQUARE_INC;
			circle.x_square -= circle.dx_square;
		}

		circle.r_square -= circle.dy_square;
		circle.dy_square += DY_SQUARE_INC;

		PLOT_CIRCLE_PIXELS(hDc,centerX,centerY,x,y,color);
	}
}

/********************************************************************
*																	*
*							FilledCircle							*
*																	*
*	Purpose:	Draw a circle filled with a color value				*
*																	*
********************************************************************/

static void FilledCircle (HDC hDc)
{
	long centerX = circle.centerX, centerY = circle.centerY;
	long x = circle.x, y = circle.y;
	long color = circle.color;
	long fill = circle.fill;
	long x_prime1, x_prime2;

	Bresenham (hDc, centerX - x, centerY, centerX + x, centerY, fill);

	while (x > y++)
	{
		if (circle.r_square <= circle.x_square)
		{
			x--;

			circle.dx_square -= DX_SQUARE_INC;
			circle.x_square -= circle.dx_square;
		}

		circle.r_square -= circle.dy_square;
		circle.dy_square += DY_SQUARE_INC;

		PLOT_CIRCLE_PIXELS(hDc,centerX,centerY,x,y,color);
		FILL_CIRCLE(hDc,centerX,centerY,x,y,fill);
	}

	x_prime1 = centerX - x;
	x_prime2 = centerX + x;
	x++;

	FILL_CIRCLE_HULL(hDc,centerY,x_prime1,x_prime2,y,x,fill);
}