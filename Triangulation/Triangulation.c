/********************************************************************
*                                                                   *
*							Triangulation.c                         *
*                                                                   *
*	Author:         Steven Johnson                                  *
*   Purpose:        Implementation of triangulation algorithm       *
*                                                                   *
********************************************************************/

/********************************************************************
*                                                                   *
*					Includes										*
*                                                                   *
********************************************************************/

#include "Triangulation.h"
#include "MyWin.h"

/********************************************************************
*                                                                   *
*                           Globals                                 *
*                                                                   *
********************************************************************/

static Triangulation triangulation;     // Global triangulation structure

/********************************************************************
*                                                                   *
*                           DrawPolygon                             *
*                                                                   *
*	Purpose:	Wrapper used to draw an arbitrary polygon           *
*                                                                   *
********************************************************************/

void DrawPolygon (HDC hDc, POINT polyPoints [], int numPoints)
{
	tPolygon poly;  // Master polygon
	int index;      // Loop variable

	assert(hDc);					// Verify that hDc is valid
	assert(polyPoints);				// Verify that polyPoints points to valid memory
	assert(numPoints < MAX_POINTS); // Verify that number of points is acceptable

	if (numPoints < 3)  // Check whether polygon is valid
		return;			// No result

	ZeroMemory(&triangulation,sizeof(Triangulation));
	// Reset triangulation

	poly.numPoints  = numPoints;// Set polygon point count
	poly.numVectors = numPoints;// Set polygon vector count

	memcpy (poly.mPoints, polyPoints, numPoints * sizeof(POINT));
	// Copy points into polygon

	MakePolygonVectors (&poly);
	// Construct vectors
	
	(IsPolygonConcave (&poly) ? TriangulateConcave : TriangulateConvex) (&poly);
	// Triangulate according to polygon type

	for (index = 0; index < triangulation.numTriangles; ++index)    // Loop through triangles
		Polygon (hDc, triangulation.mTriangles [index].mPoints, TRI_POINTS);
}

/********************************************************************
*                                                                   *
*							zCross                                  *
*                                                                   *
*	Purpose:	Returns the z result of crossing two vectors        *
*                                                                   *
********************************************************************/

static int zCross (Vector * vOne, Vector * vTwo)
{
	return vOne->vX * vTwo->vY - vOne->vY * vTwo->vX;	// Return z component of vectors' cross product
}

/********************************************************************
*                                                                   *
*							IsPolygonConcave                        *
*                                                                   *
*	Purpose:	Determines polygon concavity                        *
*                                                                   *
********************************************************************/

static BOOL IsPolygonConcave (tPolygon * polygon)
{
	int index;                          // Loop variable
	int amount = polygon->numPoints - 1;// Loop control: 1 less than final element to prevent out of bounds access

	if (polygon->numPoints == 3)	// Triangle must be convex
		return FALSE;

	for (index = 0; index < amount; ++index)	// Loop through vectors
		if (zCross (polygon->mVectors + index, polygon->mVectors + (index + 1)) < 0)	// Determine vertex concavity
			return TRUE;// Concave vertex implies concave plygon: return true
			
	if (zCross (polygon->mVectors + index, polygon->mVectors) < 0)  // Determine vertex concavity
		return TRUE;// Concave vertex implies concave polygon: return true
	
	return FALSE;   // No concavity detected implies convex polygon: return false
}

/********************************************************************
*                                                                   *
*							IsChord                                 *
*                                                                   *
*	Purpose:	Determines a chord                                  *
*                                                                   *
********************************************************************/

static BOOL IsChord (tPolygon * polygon, int iOne, int iTwo)
{
	Vector Chord;   // Chord vector
	Vector Check;   // Check vector

	if (polygon->iLowest == polygon->iLowestInBounds)
		return TRUE;

	MakeVector (&Chord, polygon->mPoints + iOne, polygon->mPoints + iTwo);
	MakeVector (&Check, polygon->mPoints + iOne, polygon->mPoints + polygon->iLowestInBounds);

	return zCross (&Chord, &Check) > 0;
}

/********************************************************************
*                                                                   *
*							GetLowest                               *
*                                                                   *
*	Purpose:	Determines index of polygon's lowest point			*
*                                                                   *
********************************************************************/

static void GetLowest (tPolygon * polygon)
{
	int index;							// Loop variable
	int lowestX = polygon->mPoints->x;  // Default lowest x
	int lowestY = polygon->mPoints->y;  // Default lowest y

	polygon->iLowest = 0;   // Set an initial index

	for (index = 1; index < polygon->numPoints; ++index)    // Loop through remaining points
	{
		if (polygon->mPoints [index].y < lowestY)       // Check for a y value that is less than current lowest y
		{
			lowestX          = polygon->mPoints [index].x;  // Set new lowest x
			lowestY          = polygon->mPoints [index].y;  // Set new lowest y
			polygon->iLowest = index;                       // Set new index of lowest value
		}

		else if (polygon->mPoints [index].y == lowestY) // When y values compare equal, check for x values
		{
			if (polygon->mPoints [index].x > lowestX)       // Check for an x value that is greater than current lowest x
			{
				lowestX          = polygon->mPoints [index].x;  // Set new lowest x
				lowestY          = polygon->mPoints [index].y;  // Set new lowest y
				polygon->iLowest = index;                       // Set new index of lowest value
			}
		}
	}
}

/********************************************************************
*                                                                   *
*							GetLowestInBounds                       *
*                                                                   *
*	Purpose:	Determines index of polygon's lowest bounded point  *
*                                                                   *
********************************************************************/

static void GetLowestInBounds (tPolygon * polygon, int xLeft, int xRight)
{
	int index;          // Loop variable
	int lowestX;        // Lowest x
	int lowestY;        // Lowest y
	BOOL first = TRUE;  // Necessary for finding first valid point

	if (xLeft > xRight)
	{
		_asm {
			mov  eax, xLeft		; Move xLeft value into eax
			xchg eax, xRight	; Swap eax and xRight
			xchg eax, xLeft		; Swap eax and xLeft
		}
	}

	for (index = 0; index < polygon->numPoints; ++index)    // Loop through first set of points
	{
		if (index == polygon->iLowest)  // Skip lowest point
			continue;

		if (polygon->mPoints [index].x > xLeft && polygon->mPoints [index].x < xRight)  // Check for an x value within the proper bounds
		{
			switch (first)  // Check whether point is first valid point
			{
			case TRUE:
				first = FALSE;  // Turn check off

				lowestX                  = polygon->mPoints [index].x;  // Set new lowest x
				lowestY                  = polygon->mPoints [index].y;  // Set new lowest y
				polygon->iLowestInBounds = index;                       // Set new index of lowest value

				break;

			case FALSE:
				if (polygon->mPoints [index].y < lowestY)       // Check for a y value that is less than current lowest y
				{
					lowestX                  = polygon->mPoints [index].x;  // Set new lowest x
					lowestY                  = polygon->mPoints [index].y;  // Set new lowest y
					polygon->iLowestInBounds = index;                       // Set new index of lowest value
				}

				else if (polygon->mPoints [index].y == lowestY && polygon->mPoints [index].x > lowestX) // When y values compare equal, check for x values
				{
					lowestX                  = polygon->mPoints [index].x;  // Set new lowest x
					lowestY                  = polygon->mPoints [index].y;  // Set new lowest y
					polygon->iLowestInBounds = index;                       // Set new index of lowest value
				}

				break;
			}
		}
	}

	if (first == TRUE)	// No valid value was found
		polygon->iLowestInBounds = polygon->iLowest;
}

/********************************************************************
*                                                                   *
*							MakeVector                              *
*                                                                   *
*	Purpose:	Constructs vectors from two points					*
*                                                                   *
********************************************************************/

static void MakeVector (Vector * vector, POINT * start, POINT * end)
{
	vector->vX = (WORD) (end->x - start->x);// Form vector's x component as end x - start x
	vector->vY = (WORD) (end->y - start->y);// Form vector's y component as end y - start y
}

/********************************************************************
*                                                                   *
*							MakePolygonVectors                      *
*                                                                   *
*	Purpose:	Constructs vectors used in polygon formation		*
*                                                                   *
********************************************************************/

static void MakePolygonVectors (tPolygon * polygon)
{
	int index;                          // Loop variable
	int amount = polygon->numPoints - 1;// Loop control: 1 less than final element to prevent out of bounds access

	for (index = 0; index < amount; ++index)	// Loop trhough vectors                                                                                                                                         
		MakeVector (&polygon->mVectors [index], polygon->mPoints + index, polygon->mPoints + (index + 1));
		// Construct vector

	MakeVector (&polygon->mVectors [index], polygon->mPoints + index, polygon->mPoints);
	// Construct final vector
}

/********************************************************************
*                                                                   *
*							ConstructPolygon                        *
*                                                                   *
*	Purpose:	Constructs a polygon from a temporary polygon       *
*                                                                   *
********************************************************************/

static void ConstructPolygon (tPolygon * dstPoly, tPolygon * srcPoly, int init, int term)
{
	int amount = srcPoly->numPoints - init; // Amount used to prevent formation of disjoint polygons
	int modify = term + 1;					// Consistent index modifier

	if (init < term)        // Check whether polygon ends between first and last point
	{
		dstPoly->numPoints  = modify - init;  // Set number of points
		dstPoly->numVectors = modify - init;  // Set number of vectors

		memcpy (dstPoly->mPoints, srcPoly->mPoints + init, dstPoly->numPoints * sizeof(POINT));
		// Copy directly into dstPolygon
	}

	else                    // Check for other cases
	{
		dstPoly->numPoints  = amount + modify;  // Set number of points
		dstPoly->numVectors = amount + modify;  // Set number of vectors

		memcpy (dstPoly->mPoints,          srcPoly->mPoints + init, amount * sizeof(POINT)); // Copy first set of points into dstPolygon
		memcpy (dstPoly->mPoints + amount, srcPoly->mPoints,        modify * sizeof(POINT)); // Copy second set of points into dstPolygon
	}

	MakePolygonVectors (dstPoly);
	// Construct vectors
}

/********************************************************************
*                                                                   *
*							TriangulateConvex                       *
*                                                                   *
*	Purpose:	Performs a convex triangulation						*
*                                                                   *
********************************************************************/

static void TriangulateConvex (tPolygon * polygon)
{
	int index;                          // Loop variable
	int amount = polygon->numPoints - 2;// Loop control: 1 less than final element to prevent out of bounds access
	pTriangle tri;                      // Contains the address of triangulation->mTriangles

	tri = triangulation.mTriangles + triangulation.numTriangles;

	for (index = 0; index < amount; ++index)	// Loop through triangles
	{
		tri [index].mPoints [0] = polygon->mPoints [0]; // Set triangle's first point to initial point

		tri [index].mPoints [1] = polygon->mPoints [index + 1]; // Set point 1 to next clockwise point
		tri [index].mPoints [2] = polygon->mPoints [index + 2]; // Set point 2 to next clockwise point
	}

	triangulation.numTriangles += amount;   // Increment triangle count
}

/********************************************************************
*                                                                   *
*							TriangulateConcave                      *
*                                                                   *
*	Purpose:	Performs a concave triangulation					*
*                                                                   *
********************************************************************/

static void TriangulateConcave (tPolygon * polygon)
{
	tPolygon pOne, pTwo;
	int prev, next;

	GetLowest (polygon);

	prev = polygon->iLowest == 0 ? polygon->numPoints - 1 
								 : polygon->iLowest   - 1;
	next = polygon->iLowest + 1 < polygon->numPoints ? polygon->iLowest + 1
													 : 0;

	GetLowestInBounds (polygon, polygon->mPoints [prev].x, polygon->mPoints [next].x);

	if (IsChord (polygon, prev, next))
	{
		ConstructPolygon (&pOne, polygon, prev, next);
		ConstructPolygon (&pTwo, polygon, next, prev);

		(IsPolygonConcave (&pTwo) ? TriangulateConcave : TriangulateConvex) (&pTwo);
		(IsPolygonConcave (&pOne) ? TriangulateConcave : TriangulateConvex) (&pOne);

	}

	else
	{
		ConstructPolygon (&pOne, polygon, polygon->iLowestInBounds, polygon->iLowest);
		ConstructPolygon (&pTwo, polygon, polygon->iLowest,         polygon->iLowestInBounds);

		(IsPolygonConcave (&pOne) ? TriangulateConcave : TriangulateConvex) (&pOne);
		(IsPolygonConcave (&pTwo) ? TriangulateConcave : TriangulateConvex) (&pTwo);
	}
}
