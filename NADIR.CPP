//
//	nadir.cpp - computes nadir location for vehicle
//
//  returned:
//		xlat 	- nadir lat
//		xlong	- nadir long
//		alt		- altitude of vehicle
//
#include <stdlib.h>
#include <math.h>
#include "qconvk.h"

#include "stdafx.h"

void nadir( double x, double y, double z, double zdot, double rtAscGreen,
			 double* p_alt, double* p_xlat, double* p_xlong )
{
	double r, rr, localt, loclat, loclon;

// compute the altitude

	rr = pow ( x, 2 ) + pow ( y, 2 ) + pow ( z, 2 );
	r = sqrt ( rr );
	localt = r - AE / sqrt ( 1.0 + OBLATE * pow ( z / r, 2 ) );
	*p_alt = localt / FTPERNM;

// compute the nadir latitude

	loclat = BLATE * sqrt ( pow ( x, 2 ) + pow ( y, 2 ) );
	loclat = atan ( z / loclat );
	*p_xlat = loclat * RADTODEG;

// compute the nadir longitude

	loclon = atan2 ( y, x ) - rtAscGreen;
	loclon = fmod ( loclon, M_2PI );

	if ( loclon < 0.0 )
		loclon = loclon + M_2PI;

	*p_xlong = loclon * RADTODEG;

	return;
}
