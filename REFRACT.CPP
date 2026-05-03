//#include "stdafx.h"
#include "qconvk.h"

/***************************************************************************************************

		Function refract computes the delta to the true elevation angle which when
		added to true elevation, gives the apparent elevation.  This angle delta is
		due to atmospheric refraction.

		Input: true elevation in degrees
			   site altitude expected in feet

		From a Lockheed memorandum by Huntington W. Small Phd  May 07, 1984 titled:
		SIMPLE REFRACTIVE BENDING FORMULAS APPLICABLE TO BOTH LARGE & SMALL
		ELEVATION ANGLE

		Note: Resultant apparent elevation angles not valid for input true elevations less than
			  -5.0 degrees.

****************************************************************************************************/

double refract( double trueEl, double siteAlt )
{
	double capNZero, n, gOfTrueEl, rtnVal;

	trueEl *= DEGTORAD;
	capNZero = REFRACTCONST * exp( (-siteAlt/FTPERKILOMETER)/7.43);
	n = capNZero*1.0e-6;
	gOfTrueEl = 1.0/(31.6 + 400.0*tan(trueEl) + 1900.0*tan(trueEl)*tan(trueEl) );
	rtnVal = ( (n/(tan(trueEl) + gOfTrueEl) ) * RADTODEG );
	return ( rtnVal );
}
