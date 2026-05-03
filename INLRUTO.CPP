//
// inlruto will convert a vector 'vi' from inertial to local rectangular
// coordinates 'vr'.
//
// input:	xi - inertial x
//			yi - inertial y
//			zi - inertial z
//			rtAscSite - right ascension of site in radians
//			rgdl - geodetic latitude of site in radians
//			alt - altitude of site in feet
//
// output:	xr - completes a right handed system
//			yr - due north
//			zr - normal to earth model at the surface
//
//

#include "stdafx.h"
#include <math.h>
#include "qconvk.h"

void inlruto( double xi, double yi, double zi, double rtAscSite, double rgdl, double alt,
					double* p_xr, double* p_yr, double* p_zr )
{
	double rgcl, rec;

	double ss = sin(rtAscSite);
	double cs = cos(rtAscSite);
	double sr = sin(rgdl);
	double cr = cos(rgdl);

	*p_xr = -ss*xi + cs*yi;
	*p_yr = -cs*sr*xi - ss*sr*yi + cr*zi;
	*p_zr = cs*cr*xi + ss*cr*yi + sr*zi;

	rgcl = atan( (1.0 - EE * EE) * tan(rgdl) );
	rec = AE/sqrt(1.0 + OBLATE * sin(rgcl)*sin(rgcl));
	*p_yr += rec*sin(rgdl - rgcl);
	*p_zr = *p_zr - rec*cos(rgdl - rgcl) - alt;
}
