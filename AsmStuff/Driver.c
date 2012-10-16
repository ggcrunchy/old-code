#include "common.h"

#include "Memory\Memory.h"
#include "List\List.h"

int I [500];

RETCODE Equal (void * This, void * Outer)
{
	return *(int*)This == *(int*)Outer;
}

RETCODE PrintMult (void * This, void * Outer)
{
	int T = *(int*) This;
	int O = *(int*) Outer;

	I [T] = T * O; 

	return RETCODE_SUCCESS;
}

void main (void)
{
	uMCONFIG M;	// Configuration structure
	int index;	// Loop variable
	LARGE_INTEGER C1, C2, D;// Profiling variables
	double seconds, Freq;	// Profiler output variables
	int * A [9000];	// Memory to allocate
	int const N = sizeof A / sizeof(int);	// Count of elements in array
	hLIST List;	// Linked list
	FILE * fp;	// Output file
	int Int = 4, * Item;// Integer data used to test list

	/* Initialize timer */
	QueryPerformanceFrequency (&D);
	Freq = 1.0 / (double)D.QuadPart;

	/* Initialize memory; allocate just enough for maximum allocation */
	M.PoolSize = N * (sizeof(int) + 0x14);
	MemInit (&M);

	fp = fopen ("Log.txt","wt");

	fprintf (fp, "%d ints:\n", N);

	/* Test speed of MemAlloc */
	QueryPerformanceCounter (&C1);
	for (index = 0; index < N; ++index)	A [index] = (int *) MemAlloc (sizeof(int), 0);
	QueryPerformanceCounter (&C2);

	seconds = (double)(C2.QuadPart - C1.QuadPart) * Freq;
	fprintf (fp, "With MemAlloc: %f seconds, %.8f p/int\n", seconds, seconds / N);

	/* Test speed of MemFree */
	QueryPerformanceCounter (&C1);
	for (index = 0; index < N; ++index)	MemFree (A [index]);
	QueryPerformanceCounter (&C2);

	seconds = (double)(C2.QuadPart - C1.QuadPart) * Freq;
	fprintf (fp, "With MemFree:  %f seconds, %.8f p/int\n", seconds, seconds / N);

	/* Test speed of malloc */
	QueryPerformanceCounter (&C1);
	for (index = 0; index < N; ++index)	A [index] = (int *) malloc (sizeof(int));
	QueryPerformanceCounter (&C2);

	seconds = (double)(C2.QuadPart - C1.QuadPart) * Freq;
	fprintf (fp, "With malloc:   %f seconds, %.8f p/int\n", seconds, seconds / N);

	/* Test speed of free */
	QueryPerformanceCounter (&C1);
	for (index = 0; index < N; ++index)	free (A [index]);
	QueryPerformanceCounter (&C2);

	seconds = (double)(C2.QuadPart - C1.QuadPart) * Freq;
	fprintf (fp, "With free:     %f seconds, %.8f p/int\n", seconds, seconds / N);

	/* Test speed of ListCreate */
	QueryPerformanceCounter (&C1);
	List = ListCreate (5, sizeof(int), L_DYNAMIC);
	QueryPerformanceCounter (&C2);

	seconds = (double)(C2.QuadPart - C1.QuadPart) * Freq;
	fprintf (fp, "With ListCreate:  %f seconds\n", seconds);

	fprintf (fp, "500 items:\n");

	/* Test speed of ListToFront */
	QueryPerformanceCounter (&C1);
	for (index = 0; index < 500; ++index) ListToFront (List, &index);
	QueryPerformanceCounter (&C2);

	seconds = (double)(C2.QuadPart - C1.QuadPart) * Freq;
	fprintf (fp, "With ListToFront:  %f seconds, %.8f p/int\n", seconds, seconds / 500);

	for (index = 0; index < 500; ++index) I [index] = 0;

	/* Test speed of ListExecute */
	QueryPerformanceCounter (&C1);
	Int = 9;	ListExecute (List, PrintMult, &Int);
	QueryPerformanceCounter (&C2);

	for (index = 0; index < 500; ++index)
	{
		if (index % 5 == 0) printf ("\n");

		printf ("%dx%d:%d\t", index, Int, I [index]);
	}

	seconds = (double)(C2.QuadPart - C1.QuadPart) * Freq;
	fprintf (fp, "With ListExecute:  %f seconds, %.8f p/int\n", seconds, seconds / 500);

	/* Test speed of ListSearch */
	QueryPerformanceCounter (&C1);
	Int = 205;	Item = ListSearch (List, Equal, &Int);
	QueryPerformanceCounter (&C2);

	printf ("Datum found: %d\n", *(int*) Item);

	seconds = (double)(C2.QuadPart - C1.QuadPart) * Freq;
	fprintf (fp, "With ListSearch:   %f seconds, %.8f p/int\n", seconds, seconds / 500);

	/* Test speed of ListDestroy */
	QueryPerformanceCounter (&C1);
	ListDestroy (List);
	QueryPerformanceCounter (&C2);

	seconds = (double)(C2.QuadPart - C1.QuadPart) * Freq;
	fprintf (fp, "With ListDestroy:  %f seconds, %.8f p/int\n", seconds, seconds / 500);

	fclose (fp);

	/* Terminate memory; output results to file */
	MemTerm ("Mem.txt");
}