/********************************************************************
*																	*
*							Triangulation.h							*
*																	*
*	Author:		Steven Johnson										*
*	Purpose:	Header for triangulation implementation				*
*																	*
********************************************************************/

#ifndef TRIANGULATION_H
#define TRIANGULATION_H

/********************************************************************
*																	*
*							Includes								*
*																	*
********************************************************************/

#include <windows.h>
#include <assert.h>

/********************************************************************
*																	*
*							Defines									*
*																	*
********************************************************************/

#define TRI_POINTS		3				// Number of points in a triangle

#define MAX_POINTS		87				// Maximal number of polygon points
#define MAX_VECTORS		MAX_POINTS		// Maximal number of polygon vectors

#define MAX_TRIANGLES	MAX_POINTS - 2	// Maximal number of triangles used to compose a polygon

/********************************************************************
*																	*
*							Types									*
*																	*
********************************************************************/

typedef struct _Vector {
	WORD vX; // x component of vector
	WORD vY; // y component of vector
} Vector, * pVector;
	
typedef struct _Triangle {
	POINT mPoints [TRI_POINTS];	// Triangle points
} Triangle, * pTriangle;

typedef struct _tPolygon {
	POINT mPoints	[MAX_POINTS];	// Array of point members
	Vector mVectors [MAX_VECTORS];	// Array of vector members
	WORD numPoints;					// Count of point members
	WORD numVectors;				// Count of vector members
	WORD iLowest;					// Index of lowest point in polygon
	WORD iLowestInBounds;			// Index of next lowest point in polygon that is between two x-values
} tPolygon, * ptPolygon;

typedef struct _Triangulation {
	Triangle mTriangles [MAX_TRIANGLES];// Array of triangle members
	WORD numTriangles;					// Count of triangles
	WORD padding;						// DWORD-align structure
} Triangulation, * pTriangulation;

/********************************************************************
*																	*
*							DrawPolygon								*
*																	*
*	Purpose:	Wrapper used to draw an arbitrary polygon			*
*																	*
********************************************************************/

void DrawPolygon (HDC hDc, POINT polyPoints [], int numPoints);

/********************************************************************
*																	*
*							zCross									*
*																	*
*	Purpose:	Returns the z result of crossing two 2-D vectors	*
*																	*
********************************************************************/

static int zCross (pVector vOne, pVector vTwo);

/********************************************************************
*																	*
*							IsPolygonConcave						*
*																	*
*	Purpose:	Determines polygon concavity						*
*																	*
********************************************************************/

static BOOL IsPolygonConcave (ptPolygon polygon);

/********************************************************************
*																	*
*							IsChord									*
*																	*
*	Purpose:	Determines a chord									*
*																	*
********************************************************************/

static BOOL IsChord (ptPolygon polygon, int iOne, int iTwo);

/********************************************************************
*																	*
*							GetLowest								*
*																	*
*	Purpose:	Determines index of polygon's lowest point			*
*																	*
********************************************************************/

static void GetLowest (ptPolygon polygon);

/********************************************************************
*																	*
*							GetLowestInBounds						*
*																	*
*	Purpose:	Determines index of polygon's lowest bounded point	*
*																	*
********************************************************************/

static void GetLowestInBounds (ptPolygon polygon, int xLeft, int xRight);

/********************************************************************
*																	*
*							MakeVector								*
*																	*
*	Purpose:	Constructs vectors from two points					*
*																	*
********************************************************************/

static void MakeVector (pVector vector, PPOINT start, PPOINT end);

/********************************************************************
*																	*
*							MakePolygonVectors						*
*																	*
*	Purpose:	Constructs vectors used in polygon formation		*
*																	*
********************************************************************/

static void MakePolygonVectors (ptPolygon polygon);

/********************************************************************
*																	*
*							ConstructPolygon						*
*																	*
*	Purpose:	Constructs a polygon from a temporary polygon		*
*																	*
********************************************************************/

static void ConstructPolygon (ptPolygon dstPoly, ptPolygon srcPoly, int init, int term);

/********************************************************************
*																	*
*							TriangulateConvex						*
*																	*
*	Purpose:	Performs a convex triangulation						*
*																	*
********************************************************************/

static void TriangulateConvex (ptPolygon polygon);

/********************************************************************
*																	*
*							TriangulateConcave						*
*																	*
*	Purpose:	Performs a concave triangulation					*
*																	*
********************************************************************/

static void TriangulateConcave (ptPolygon polygon);

#endif // TRIANGLE_H