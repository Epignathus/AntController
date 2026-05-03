//#include "stdafx.h"
#include "qconvk.h"

//
// lrlputo() converts local rectangular (xr,yr,zr) to local polar (slantRange,el,az)
//
// input: vr - see inlruto for 'vr' description
//
// output:	slant range in feet
//			elevation angle in radians
//			azimuth in radians
//
//

void lrlputo( double xr, double yr, double zr, 
						 double* p_slantRange, double* p_elevation, double* p_azimuth )
{
	*p_slantRange = sqrt(xr*xr + yr*yr + zr*zr);
	*p_elevation = asin(zr/(*p_slantRange));
	*p_azimuth = atan2(xr, yr);
	if (*p_azimuth < 0.0)
		*p_azimuth += M_2PI;
}
