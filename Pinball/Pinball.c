/********************************************************************
*																	*
*							Includes								*
*																	*
********************************************************************/

#include "Pinball.h"

/********************************************************************
*																	*
*							Globals									*
*																	*
********************************************************************/

PRIVATE Pinball pinball;
// Primary pinball object

PRIVATE INT dummy;
// Used only to initialize pointers to a value

PRIVATE double lastX, lastY;
// Used to eject the ball when stuck

/********************************************************************
*																	*
*							LoadData								*
*																	*
********************************************************************/

PUBLIC void LoadData (HDC hDc)
{
	BITMAPINFO bInfo;
	BITMAPFILEHEADER bFileHeader;

	PBYTE BitArray;
	FILE * fp;

	HBITMAP hBitmap;
	HBRUSH hBrush;
	HPEN hPen;

	fp = fopen (BACKGROUND, "r");

	if (fp == NULL)
	{
		MessageBox (NULL, BACKGROUND " not found", "Missing bitmap", MB_OK);
		return;
	}

	fread (&bFileHeader, sizeof(BITMAPFILEHEADER), 1, fp);
	fread (&bInfo, sizeof(BITMAPINFO), 1, fp);

	hBitmap = CreateDIBSection (NULL, &bInfo, DIB_RGB_COLORS, &BitArray, NULL, 0);
	SelectObject (hDc, hBitmap);

	hPen	= CreatePen (PS_SOLID, 2, RGB(0xFF,0x7D,0x2B));
	SelectObject (hDc, hPen);

	hBrush	= CreateHatchBrush (HS_CROSS, RGB(0xCF,0xD7,0x3B));
	SelectObject (hDc, hBrush);

	fseek (fp, bFileHeader.bfOffBits, SEEK_SET);
	fread (BitArray, bInfo.bmiHeader.biSizeImage, 1, fp);

	fclose (fp);
}

/********************************************************************
*																	*
*							LoadObjects								*
*																	*
********************************************************************/

PUBLIC void LoadObjects (HDC hDc)
{
	FILE * fp;

	BOOL isLoading = FALSE;

	char c1, c2, c3, c4;
	int tag;

	fp = fopen (DATA, "r");
	
	if (fp == NULL)
	{
		MessageBox (NULL, DATA " found", "Missing data file", MB_OK);
		return;
	}

	do {
		fscanf (fp, "%c %c %c %c\n", &c1, &c2, &c3, &c4);
		tag = MAKETAG(c1,c2,c3,c4);

		switch(tag)
		{
		case MAKETAG('S','T','R','T'):
			isLoading = TRUE;
			break;

		case MAKETAG('B','A','L','L'):
			if (isLoading)
				LoadBall (fp);

			break;

		case MAKETAG('C','I','R','C'):
			if (isLoading)
				LoadCircles (fp);

			break;

		case MAKETAG('I','P','O','L'):
			if (isLoading)
				LoadInnerPolys (fp);

			break;

		case MAKETAG('O','P','O','L'):
			if (isLoading)
				LoadOuterPolys (fp);

			break;

		case MAKETAG('E','N','D','L'):
			isLoading = FALSE;
			break;

		default:
			MessageBox (NULL, "Unsupported tag: Error reading data file", "Error", MB_OK);
			break;
		}
	} while (isLoading);

	fclose (fp);

	DrawObjects (hDc);
}

/********************************************************************
*																	*
*							DestroyObjects							*
*																	*
********************************************************************/

PUBLIC void DestroyObjects (HDC hDc)
{
	int index;

	free (pinball.aCircle);

	for (index = 0; index < pinball.numInnerPolys; ++index)
	{
		free (pinball.aInnerPolys [index].points);
		free (pinball.aInnerPolys [index].edges);
	}

	free (pinball.aInnerPolys);

	for (index = 0; index < pinball.numOuterPolys; ++index)
	{
		free (pinball.aOuterPolys [index].points);
		free (pinball.aOuterPolys [index].edges);
	}

	free (pinball.aOuterPolys);

	DeleteDC (hDc);
}

/********************************************************************
*																	*
*							UpdateBall								*
*																	*
********************************************************************/

PUBLIC void UpdateBall (HDC hDc)
{
	double s, t;
	int index, loop;

	int pass, amount = (int) floor (pinball.ball.mag);

	for (pass = 0; pass < amount; ++pass)
	{
		for (index = 0; index < pinball.numCircles; ++index)
			if (BallCircleIntersect (&pinball.ball, &pinball.aCircle [index], &s, &t))
				ReflectOffCircle (&pinball.ball, &pinball.aCircle [index]);

		for (index = 0; index < pinball.numInnerPolys; ++index)
			for (loop = 0; loop < pinball.aInnerPolys [index].numElements; ++loop)
				if (BallVectorIntersect (&pinball.ball, &pinball.aInnerPolys [index].edges [loop], &s, &t))
					ReflectOffVector (&pinball.ball, &pinball.aInnerPolys [index].edges [loop]);

		for (index = 0; index < pinball.numOuterPolys; ++index)
			for (loop = 0; loop < pinball.aOuterPolys [index].numElements; ++loop)
				if (BallVectorIntersect (&pinball.ball, &pinball.aOuterPolys [index].edges [loop], &s, &t))
					ReflectOffVector (&pinball.ball, &pinball.aOuterPolys [index].edges [loop]);

		StepAhead ();

		Ellipse (hDc, (int) (pinball.ball.direction.px - pinball.ball.radius),
					  (int) (pinball.ball.direction.py - pinball.ball.radius),
					  (int) (pinball.ball.direction.px + pinball.ball.radius),
					  (int) (pinball.ball.direction.py + pinball.ball.radius));
	}
}

/********************************************************************
*																	*
*							SetBallLocation							*
*																	*
********************************************************************/

PUBLIC void SetBallLocation (double cx, double cy)
{
	pinball.ball.direction.px = cx;
	pinball.ball.direction.py = cy;
}

/********************************************************************
*																	*
*							MovePolygon								*
*																	*
********************************************************************/

PUBLIC void MovePolygon (HDC hDc, int index, int dx, int dy)
{
	int loop;

	for (loop = 0; loop < pinball.aInnerPolys [index].numElements; ++loop)
	{
		pinball.aInnerPolys [index].points [loop].x += dx;
		pinball.aInnerPolys [index].points [loop].y += dy;

		pinball.aInnerPolys [index].edges [loop].px += dx;
		pinball.aInnerPolys [index].edges [loop].py += dy;
	}

	LoadData	(hDc);
	DrawObjects (hDc);
}

/********************************************************************
*																	*
*							DrawObjects								*
*																	*
********************************************************************/

PRIVATE void DrawObjects (HDC hDc)
{
	int index, loop;

	for (index = 0; index < pinball.numOuterPolys; ++index)
	{
		MoveToEx (hDc, pinball.aOuterPolys [index].points->x, pinball.aOuterPolys [index].points->y, NULL);

		for (loop = 0; loop < pinball.aOuterPolys [index].numElements; ++loop)
			LineTo (hDc, pinball.aOuterPolys [index].points [loop + 1].x, pinball.aOuterPolys [index].points [loop + 1].y);
	}

	for (index = 0; index < pinball.numInnerPolys; ++index)
	{
		MoveToEx (hDc, pinball.aInnerPolys [index].points->x, pinball.aInnerPolys [index].points->y, NULL);

		for (loop = 0; loop < pinball.aInnerPolys [index].numElements; ++loop)
			LineTo (hDc, pinball.aInnerPolys [index].points [loop + 1].x, pinball.aInnerPolys [index].points [loop + 1].y);
	}

	for (index = 0; index < pinball.numCircles; ++index)
		Ellipse (hDc, pinball.aCircle [index].cx - pinball.aCircle [index].radius,
					  pinball.aCircle [index].cy - pinball.aCircle [index].radius,
					  pinball.aCircle [index].cx + pinball.aCircle [index].radius,
					  pinball.aCircle [index].cy + pinball.aCircle [index].radius);
}

/********************************************************************
*																	*
*							LoadBall								*
*																	*
********************************************************************/

PRIVATE void LoadBall (FILE * fp)
{
	fscanf (fp, "Location x=%lf y=%lf r=%lf\n",
			&pinball.ball.direction.px,
			&pinball.ball.direction.py,
			&pinball.ball.radius);

	fscanf (fp, "Disp x=%lf y=%lf\n",
			&pinball.ball.dispX,
			&pinball.ball.dispY);

	pinball.ball.direction.vx = pinball.ball.dispX;
	pinball.ball.direction.vy = pinball.ball.dispY;

	pinball.ball.mag = Magnitude (&pinball.ball.direction);

	MakeDirectionVector (&pinball.ball.direction, pinball.ball.mag);

	pinball.ball.marker = &dummy;
	pinball.ball.incer  = &dummy;
}

/********************************************************************
*																	*
*							LoadCircles								*
*																	*
********************************************************************/

PRIVATE void LoadCircles (FILE * fp)
{
	int index;

	fscanf (fp, "Circles=%d\n", &pinball.numCircles);

	pinball.aCircle = (ptCircle) calloc (pinball.numCircles, sizeof(tCircle));

	for (index = 0; index < pinball.numCircles; ++index)
	{
		fscanf (fp, "cx=%d cy=%d r=%d\n",
				&pinball.aCircle [index].cx,
				&pinball.aCircle [index].cy,
				&pinball.aCircle [index].radius);
	}
}

/********************************************************************
*																	*
*							LoadInnerPolys							*
*																	*
********************************************************************/

PRIVATE void LoadInnerPolys (FILE * fp)
{
	int index, loop;

	fscanf (fp, "InnerPolys=%d\n", &pinball.numInnerPolys);

	pinball.aInnerPolys = (ptPolygon) calloc (pinball.numInnerPolys, sizeof(tPolygon));

	for (index = 0; index < pinball.numInnerPolys; ++index)
	{
		fscanf (fp, "Elements=%d\n", &pinball.aInnerPolys [index].numElements);

		pinball.aInnerPolys [index].points = (PPOINT)  calloc (pinball.aInnerPolys [index].numElements + 1, sizeof(POINT));
		pinball.aInnerPolys [index].edges  = (pVector) calloc (pinball.aInnerPolys [index].numElements,     sizeof(Vector));

		for (loop = 0; loop < pinball.aInnerPolys [index].numElements; ++loop)
		{
			fscanf (fp, "x=%d y=%d\n",
					&pinball.aInnerPolys [index].points [loop].x,
					&pinball.aInnerPolys [index].points [loop].y);
		}

		pinball.aInnerPolys [index].points [loop].x = pinball.aInnerPolys [index].points->x;
		pinball.aInnerPolys [index].points [loop].y = pinball.aInnerPolys [index].points->y;

		ConstructInnerVectors (&pinball.aInnerPolys [index]);
	}
}

/********************************************************************
*																	*
*							LoadOuterPolys							*
*																	*
********************************************************************/

PRIVATE void LoadOuterPolys (FILE * fp)
{
	int index, loop;

	fscanf (fp, "OuterPolys=%d\n", &pinball.numOuterPolys);

	pinball.aOuterPolys = (ptPolygon) calloc (pinball.numOuterPolys, sizeof(tPolygon));

	for (index = 0; index < pinball.numOuterPolys; ++index)
	{
		fscanf (fp, "Elements=%d\n", &pinball.aOuterPolys [index].numElements);

		pinball.aOuterPolys [index].points = (PPOINT)  calloc (pinball.aOuterPolys [index].numElements + 1, sizeof(POINT));
		pinball.aOuterPolys [index].edges  = (pVector) calloc (pinball.aOuterPolys [index].numElements,     sizeof(Vector));

		for (loop = 0; loop < pinball.aOuterPolys [index].numElements; ++loop)
		{
			fscanf (fp, "x=%d y=%d\n",
					&pinball.aOuterPolys [index].points [loop].x,
					&pinball.aOuterPolys [index].points [loop].y);
		}

		pinball.aOuterPolys [index].points [loop].x = pinball.aOuterPolys [index].points->x;
		pinball.aOuterPolys [index].points [loop].y = pinball.aOuterPolys [index].points->y;

		ConstructOuterVectors (&pinball.aOuterPolys [index]);
	}
}

/********************************************************************
*																	*
*							ConstructInnerVectors					*
*																	*
********************************************************************/

PRIVATE void ConstructInnerVectors (tPolygon * polygon)
{
	int index;

	if (polygon->numElements == 0)
		return;

	for (index = 0; index < polygon->numElements; ++index)
	{
		polygon->edges [index].vx = polygon->points [index + 1].x - polygon->points [index].x;
		polygon->edges [index].vy = polygon->points [index + 1].y - polygon->points [index].y;

		polygon->edges [index].nx = -polygon->edges [index].vy;
		polygon->edges [index].ny =  polygon->edges [index].vx;

		polygon->edges [index].px = polygon->points [index].x;
		polygon->edges [index].py = polygon->points [index].y;
	}
}

/********************************************************************
*																	*
*							ConstructOuterVectors					*
*																	*
********************************************************************/

PRIVATE void ConstructOuterVectors (tPolygon * polygon)
{
	int index;

	if (polygon->numElements == 0)
		return;

	for (index = 0; index < polygon->numElements; ++index)
	{
		polygon->edges [index].vx  = polygon->points [index + 1].x - polygon->points [index].x;
		polygon->edges [index].vy  = polygon->points [index + 1].y - polygon->points [index].y;

		polygon->edges [index].nx  =  polygon->edges [index].vy;
		polygon->edges [index].ny  = -polygon->edges [index].vx;

		polygon->edges [index].px = polygon->points [index].x;
		polygon->edges [index].py = polygon->points [index].y;
	}
}

/********************************************************************
*																	*
*							Magnitude								*
*																	*
********************************************************************/

PRIVATE double Magnitude (Vector * vector)
{
	return sqrt (vector->vx * vector->vx + vector->vy * vector->vy);
}

/********************************************************************
*																	*
*							Determinant								*
*																	*
********************************************************************/

PRIVATE double Determinant (Vector * vOne, Vector * vTwo)
{
	return vOne->vx * vTwo->vy - vTwo->vx * vOne->vy;
}

/********************************************************************
*																	*
*							DotProduct								*
*																	*
********************************************************************/

PRIVATE double DotProduct (Vector * vOne, Vector * vTwo)
{
	return vOne->vx * vTwo->vx + vOne->vy * vTwo->vy;
}

/********************************************************************
*																	*
*							MakeDirectionVector						*
*																	*
********************************************************************/

PRIVATE void MakeDirectionVector (Vector * vector, double magnitude)
{
	vector->vx /= magnitude;
	vector->vy /= magnitude;
}

/********************************************************************
*																	*
*							StepAhead								*
*																	*
********************************************************************/

PRIVATE void StepAhead (void)
{
	pinball.ball.direction.px += pinball.ball.direction.vx;
	pinball.ball.direction.py += pinball.ball.direction.vy;
}

/********************************************************************
*																	*
*							BallCircleIntersect						*
*																	*
********************************************************************/

PRIVATE BOOL BallCircleIntersect (Ball * ball, tCircle * circle, double * s, double * t)
{
	double dx = circle->cx - ball->direction.px;
	double dy = circle->cy - ball->direction.py;

	if (sqrt (dx * dx + dy * dy) <= ball->radius + circle->radius)
	{
		double dx2 = -dx;
		double dy2 = -dy;

		double a = dx * dx + dy * dy;
		double b = -2.0 * a;
		double c = a - circle->radius * circle->radius;

		double Discriminant = (b * b) - (4.0 * a * c);
		double denom		= 2.0 * a;

		double det = ball->dispX * circle->vy - ball->dispY * circle->vx;

		double x, y;

		if (Discriminant == 0.0)
			*t = -b / denom;

		else
		{
			double root = sqrt (Discriminant);

			*t = (det > 0.0) ? (-b - root) / denom 
							 : (-b + root) / denom;
		} 

		*s  = *t * dx - dx2;
		*s /= ball->mag;

		x = ball->direction.px + *t * dx;
		y = ball->direction.py + *t * dy;

		circle->nx = dx2 - (x - circle->cx);
		circle->ny = dy2 - (y - circle->cy);

		if (circle->isHit)
			++circle->conHits;

		else
			MarkObject (ball, &circle->conHits, &circle->isHit);

		lastX = ball->direction.px + circle->nx;
		lastY = ball->direction.py + circle->ny;

		return TRUE;
	}

	return FALSE;
}

/********************************************************************
*																	*
*							BallVectorIntersect						*
*																	*
********************************************************************/

PRIVATE BOOL BallVectorIntersect (Ball * ball, Vector * vector, double * s, double * t)
{
	double dx = vector->px - ball->direction.px;
	double dy = vector->py - ball->direction.py;

	double a = DotProduct (vector, vector);
	double b = 2.0 * (dx * vector->vx + dy * vector->vy);
	double c = (dx * dx) + (dy * dy) - (ball->radius * ball->radius);

	double Discriminant = (b * b) - (4.0 * a * c);
	double denom		= 2.0 * a;

	double det = Determinant (&ball->direction, vector);

	if (Discriminant < 0.0)
		return FALSE;

	if (Discriminant == 0.0)
		*t = -b / denom;

	else
	{
		double root = sqrt (Discriminant);

		*t = (det < 0.0) ? (-b - root) / denom
						 : (-b + root) / denom;
	}

	if (*t < L_TOLERANCE || *t > U_TOLERANCE)
		return FALSE;

	*s  = *t * vector->vx + dx;
	*s /= ball->mag;

	if (vector->isHit)
		++vector->conHits;

	else
		MarkObject (ball, &vector->conHits, &vector->isHit);

	lastX = ball->direction.px + vector->nx;
	lastY = ball->direction.py + vector->ny;

	return TRUE;
}

/********************************************************************
*																	*
*							ReflectOffCircle						*
*																	*
********************************************************************/

PRIVATE void ReflectOffCircle (Ball * ball, tCircle * circle)
{
	double projScalar;

	projScalar = -2.0 * (ball->dispX * circle->nx + ball->dispY * circle->ny) 
					  / (circle->nx  * circle->nx + circle->ny  * circle->ny);

	ball->dispX += projScalar * circle->nx;
	ball->dispY += projScalar * circle->ny;

	pinball.ball.direction.vx = pinball.ball.dispX;
	pinball.ball.direction.vy = pinball.ball.dispY;

	pinball.ball.mag = Magnitude (&pinball.ball.direction);

	MakeDirectionVector (&pinball.ball.direction, pinball.ball.mag);
}

/********************************************************************
*																	*
*							ReflectOffVector						*
*																	*
********************************************************************/

PRIVATE void ReflectOffVector (Ball * ball, Vector * vector)
{
	double projScalar;

	projScalar = -2.0 * (ball->dispX * vector->nx + ball->dispY * vector->ny)
					  / (vector->nx  * vector->nx + vector->ny  * vector->ny); 

	ball->dispX += projScalar * vector->nx;
	ball->dispY += projScalar * vector->ny;

	pinball.ball.direction.vx = pinball.ball.dispX;
	pinball.ball.direction.vy = pinball.ball.dispY;

	pinball.ball.mag = Magnitude (&pinball.ball.direction);

	MakeDirectionVector (&pinball.ball.direction, pinball.ball.mag);
}

/********************************************************************
*																	*
*							MarkObject								*
*																	*
********************************************************************/

PRIVATE void MarkObject (Ball * ball, int * counter, BOOL * marker)
{
	*ball->incer  = 0;
	*ball->marker = FALSE;

	ball->incer  = counter;
	ball->marker = marker;

	++*ball->incer;
	  *ball->marker = TRUE;
}