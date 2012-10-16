#ifndef PINBALL_H
#define PINBALL_H

/********************************************************************
*																	*
*							Includes								*
*																	*
********************************************************************/

#include <stdio.h>
#include <float.h>
#include <math.h>
#include "windows.h"

/********************************************************************
*																	*
*							Qualifiers								*
*																	*
********************************************************************/

#define PUBLIC				// Semantic: denotes interface function
#define PROTECTED			// Semantic: denotes utility function
#define PRIVATE		static	// Semantic: denotes implementation function

/********************************************************************
*																	*
*							Macros									*
*																	*
********************************************************************/

#define MAKETAG(c1,c2,c3,c4) ((((c1) & 0xFF) << 24)	| (((c2) & 0xFF) << 16)	| (((c3) & 0xFF) << 8) |  ((c4) & 0xFF))
// Convert four characters to an integer tag

/********************************************************************
*																	*
*							Values									*
*																	*
********************************************************************/

#define BACKGROUND "A:\\Pinball\\monkeyPsych.bmp"
#define DATA	   "A:\\Pinball\\Pinball.dat"

#define PADDING		0.015

#define L_TOLERANCE 0.0 - PADDING
#define U_TOLERANCE 1.0 + PADDING

/********************************************************************
*																	*
*							Types									*
*																	*
********************************************************************/

/////////////////////////////////////////////
// Vector: Used to represent displacements //
/////////////////////////////////////////////

typedef struct _Vector {
	double px;     // Horizontal position
	double py;     // Vertical position
	double vx;     // Horizontal displacement
	double vy;     // Vertical displacement
	double nx;     // Normal horizontal displacement
	double ny;     // Normal vertical displacement
	BOOL isHit;	   // Marks hit vector
	int conHits;   // Consecutive hits with vector
	double mForce; // Force given to vector
} Vector, * pVector;

////////////////////////////////////////
// tCircle: Used to represent circles //
////////////////////////////////////////

typedef struct _tCircle {
	double vx;	   // Horizontal displacement of circle
	double vy;	   // Vertical displacement of circle
	double nx; 	   // Normal horizontal displacement of circle
	double ny;	   // Normal vertical displacement of circle
	long cx;	   // Horizontal center of circle
	long cy;	   // Vertical center of circle
	long radius;   // Radius of circle
	BOOL isHit;	   // Marks hit circle
	int conHits;   // Consecutive collisions with circle
	double mForce; // Force given to circle
} tCircle, * ptCircle;

//////////////////////////////////////////
// tPolygon: Used to represent polygons //
//////////////////////////////////////////

typedef struct _tPolygon {
	int numElements;// Count of polygon elements
	PPOINT points;	// Polygon points
	pVector edges;	// Polygon edges
} tPolygon, * ptPolygon;

///////////////////////////////
// Ball: Primary ball object //
///////////////////////////////

typedef struct _Ball {
	Vector direction; // Direction of ball's travel
	double radius;	  // Radius of ball
	double dispX;	  // Unitary x displacement
	double dispY;	  // Unitary y displacement
	double mag;		  // Magnitude of ball displacement
	BOOL * marker;	  // Marks hit objects
	int  * incer;	  // Increments hit counts
} Ball, * pBall;

/////////////////////////////////////
// Pinball: Primary pinball object //
/////////////////////////////////////

typedef struct _Pinball {
	Ball ball;			  // Pinball ball
	int numCircles;		  // Count of circles
	ptCircle aCircle;	  // Array of circles
	int numInnerPolys;	  // Count of inner polygons
	ptPolygon aInnerPolys;// Array of inner polygons
	int numOuterPolys;	  // Count of outer polygons
	ptPolygon aOuterPolys;// Array of outer polygons
	double mGravity;	  // Gravity affecting ball
} Pinball, * pPinball;

/********************************************************************
*																	*
*							LoadData								*
*																	*
********************************************************************/

PUBLIC void LoadData (HDC hDc);

/********************************************************************
*																	*
*							LoadObjects								*
*																	*
********************************************************************/

PUBLIC void LoadObjects (HDC hDc);

/********************************************************************
*																	*
*							DestroyObjects							*
*																	*
********************************************************************/

PUBLIC void DestroyObjects (HDC hDc);

/********************************************************************
*																	*
*							UpdateBall								*
*																	*
********************************************************************/

PUBLIC void UpdateBall (HDC hDc);

/********************************************************************
*																	*
*							SetBallLocation							*
*																	*
********************************************************************/

PUBLIC void SetBallLocation (double cx, double cy);

/********************************************************************
*																	*
*							MovePolygon								*
*																	*
********************************************************************/

PUBLIC void MovePolygon (HDC hDc, int index, int dx, int dy);

/********************************************************************
*																	*
*							DrawObjects								*
*																	*
********************************************************************/

PRIVATE void DrawObjects (HDC hDc);

/********************************************************************
*																	*
*							LoadBall								*
*																	*
********************************************************************/

PRIVATE void LoadBall (FILE * fp);

/********************************************************************
*																	*
*							LoadCircles								*
*																	*
********************************************************************/

PRIVATE void LoadCircles (FILE * fp);

/********************************************************************
*																	*
*							LoadInnerPolys							*
*																	*
********************************************************************/

PRIVATE void LoadInnerPolys (FILE * fp);

/********************************************************************
*																	*
*							LoadOuterPolys							*
*																	*
********************************************************************/

PRIVATE void LoadOuterPolys (FILE * fp);

/********************************************************************
*																	*
*							ConstructInnerVectors					*
*																	*
********************************************************************/

PRIVATE void ConstructInnerVectors (ptPolygon polygon);

/********************************************************************
*																	*
*							ConstructOuterVectors					*
*																	*
********************************************************************/

PRIVATE void ConstructOuterVectors (ptPolygon polygon);

/********************************************************************
*																	*
*							Magnitude								*
*																	*
********************************************************************/

PRIVATE double Magnitude (pVector vector);

/********************************************************************
*																	*
*							Determinant								*
*																	*
********************************************************************/

PRIVATE double Determinant (pVector vOne, pVector vTwo);

/********************************************************************
*																	*
*							DotProduct								*
*																	*
********************************************************************/

PRIVATE double DotProduct (pVector vOne, pVector vTwo);

/********************************************************************
*																	*
*							MakeDirectionVector						*
*																	*
********************************************************************/

PRIVATE void MakeDirectionVector (pVector vector, double magnitude);

/********************************************************************
*																	*
*							StepAhead								*
*																	*
********************************************************************/

PRIVATE void StepAhead (void);

/********************************************************************
*																	*
*							CirclesIntersect						*
*																	*
********************************************************************/

PRIVATE BOOL BallCircleIntersect (pBall ball, ptCircle circle, double * s, double * t);

/********************************************************************
*																	*
*							CircleVectorIntersect					*
*																	*
********************************************************************/

PRIVATE BOOL BallVectorIntersect (pBall ball, pVector vector, double * s, double * t);

/********************************************************************
*																	*
*							ReflectOffCircle						*
*																	*
********************************************************************/

PRIVATE void ReflectOffCircle (pBall ball, ptCircle circle);

/********************************************************************
*																	*
*							ReflectOffVector						*
*																	*
********************************************************************/

PRIVATE void ReflectOffVector (pBall ball, pVector vector);

/********************************************************************
*																	*
*							MarkObject								*
*																	*
********************************************************************/

PRIVATE void MarkObject (pBall ball, int * counter, BOOL * marker);

#endif // PINBALL_H