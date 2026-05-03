//
// test program to check sun moon az el code added to orbitPropagation dll
//
#include "stdio.h"
#include "FuncHdr.h"
#include "qconvk.h"
#include "windows.h"
#include "winbase.h"

void main( int arcg, char* argv[] )
{
	double siteLat = 37.36095 * DEGTORAD;
	double siteLon = 237.9666167 * DEGTORAD;
	double siteAlt = 152.0;

	double sunAz, sunEl, moonAz, moonEl;

	time_t ltime;
	struct tm* gmt;

	while( 1 ) {
		time( &ltime );

		int status = calcSunMoonAzEl( ltime, siteLat, siteLon, siteAlt,
																	sunAz, sunEl, moonAz, moonEl );

		gmt = gmtime( &ltime );

		printf( "Time: %2.2d:%2.2d:%2.2d - Sun Az: %7.3lf, Sun El: %7.3lf,   Moon Az: %7.3lf, Moon El: %7.3lf\r",
				 gmt->tm_hour, gmt->tm_min, gmt->tm_sec, sunAz, sunEl, moonAz, moonEl );

		Sleep( 1000 );
	}
}
			
