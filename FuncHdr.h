//
//	FunctHdr.h
//
//	Contains data prototypes for the predict (sgp) routine.
//
//
#ifndef _FUNCTIONS_H
#define _FUNCTIONS_H

#include "FuncHdr.h"

//
// subroutine prototypes
//

// kepler calculate the position and velocity coordinates in inertial geocentric-equtorial
// reference frame.
int kepler( double rldeg, double ec, double es, double eps, double* p_upomdeg );

// This function returns the right ascension of Greenwich meridian for any input date and
// systems time after 1979.
double rag( int mjd, double fmjd );

// inlruto will convert a vector 'vi' from inertial to local rectangular coordinates 'vr'.
void inlruto( double xi, double yi, double zi, double rtAscSite, double rgdl,
					    double alt, double* p_xr, double* p_yr, double* p_zr );

// lrlputo() converts local rectangular (xr,yr,zr) to local polar (slantRange,el,az)
void lrlputo( double xr, double yr, double zr, double* p_slantRange, double* p_elevation,
					    double* p_azimuth );

// Refract computes the delta to the true elevation angle which when added to true elevation,
// gives the apparent elevation.
double refract( double trueEl, double siteAlt );

// nadir.cpp - computes nadir location for vehicle
void nadir( double x, double y, double z, double zdot, double rtAscGreen,
					  double* p_alt, double* p_xlat, double* p_xlong );

// sunMoon() is to compute the position of the Sun and the Moon at any specified time.
void sunMoon( double tmjd, double sunDirCos[], double* p_sunDist, double moonDirCos[], 
						double* p_moonDist );

// calcSunMoonAzEl computes the sun and moon azimuth and elevation angles for a observation point
int calcSunMoonAzEl( time_t ltime, double siteLat, double siteLon, double siteAlt,
					 double* p_sunAz, double* p_sunEl, double* p_moonAz, double* p_moonEl );


//int readVector(char*, Vector*);

//void monthDayToJulian(int, int, int, int &);

#endif