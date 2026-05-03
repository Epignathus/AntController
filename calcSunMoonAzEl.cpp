//
// calcSunMoonAzEl
// 
// This function takes the input time and the observers position
// on the earth and calculates the azimuth and elevation for the 
// sun and moon.
//
// Inputs:
//		ltime - time of request in time_t format
//		siteLat	- latitude of observers position in radians
//		siteLon -	longitude of observers position in radians
//		siteAlt -	altitude of observers position in feet
//
//	Outputs:
//		sunAz  - sun azimuth
//		sunEl  - sun elevation
//		moonAz - moon azimuth
//		moonEl - moon elevation
//

#include <math.h>
#include <time.h>
#include "FuncHdr.h"
#include "dateTimeToMJD.h"
#include "qconvk.h"


int calcSunMoonAzEl( time_t ltime, double siteLat, double siteLon, double siteAlt,
					 double* p_sunAz, double* p_sunEl, double* p_moonAz, double* p_moonEl )
{
	struct tm* gmt;

	gmt = gmtime( &ltime );
	
	int iYear	= gmt->tm_year + 1900;
	int iMonth	= gmt->tm_mon + 1;
	int iDay	= gmt->tm_mday;
	int iHour	= gmt->tm_hour;
	int iMin	= gmt->tm_min;
	int iYday	= gmt->tm_yday + 1;
	double dSec = (double)gmt->tm_sec;
	double fracDay = (double)(( gmt->tm_hour*3600.0 + gmt->tm_min*60.0 + gmt->tm_sec ) / 86400.0 );

	double dMJD = dateTimeToMJD( iYear, iMonth, iDay, iHour, iMin, dSec );

	// find the direction cosines and distance in earth radii to the sun and moon
	double dSunDirCos[3];
	double dMoonDirCos[3];
	double dSunDist, dMoonDist;

	sunMoon( dMJD, dSunDirCos, &dSunDist, dMoonDirCos, &dMoonDist );

	dSunDist *= AE;
	dMoonDist *= AE;

//	double dRtAscGrn = rtAscGreen( iYear, iMonth, iDay, iHour, iMin, dSec );
	double dRtAscGrn = rag(15384 + (int)(365.251*(double)(iYear -1 - 1900)) + iYday, fracDay );
	double dRtAscSite = fmod( dRtAscGrn + siteLon, M_2PI );
	//printf("MJD = %lf, rt asc of site = %lf\n", dMJD, dRtAscSite);
	
	double dX = dSunDirCos[0] * dSunDist;
	double dY = dSunDirCos[1] * dSunDist;
	double dZ = dSunDirCos[2] * dSunDist;

	double xr, yr, zr;
	double dSlantRange;

	// perform coordinate transformation for sun
	// inertial-to-local-rectangular
	inlruto( dX, dY, dZ, dRtAscSite, siteLat, siteAlt, &xr, &yr, &zr );
	// local-rectangular-to-local-polar
	lrlputo( xr, yr, zr, &dSlantRange, p_sunEl, p_sunAz );

	*p_sunAz *= RADTODEG;
	*p_sunEl *= RADTODEG;

	// perform coordinate transformation for moon
	dX = dMoonDirCos[0] * dMoonDist;
	dY = dMoonDirCos[1] * dMoonDist;
	dZ = dMoonDirCos[2] * dMoonDist;

	// inertial-to-local-rectangular
	inlruto( dX, dY, dZ, dRtAscSite, siteLat, siteAlt, &xr, &yr, &zr );
	// local-rectangular-to-local-polar
	lrlputo( xr, yr, zr, &dSlantRange, p_moonEl, p_moonAz );

	*p_moonAz *= RADTODEG;
	*p_moonEl *= RADTODEG;

	return 0;
}
