//
//
// math constants
//
#ifndef QCONVK_H
#define QCONVK_H

#include <math.h>

// units conversion constants used throughout

const double M_PI = 3.14159265359;
const double M_2PI = 2.0*M_PI;
const double PIOVR2 = 1.5707963267949;  // pi / 2
const double RADTODEG = 180.0 / M_PI;
const double DEGTORAD = M_PI / 180.0;
const double REQ = 6378.135;
const double XMU = 398600.8;
const double XJ2 = 0.001082616;
const double FTPERKILOMETER = 3280.839895;
const double FTPERNM = 6076.1154858;
const double OBLATE = 0.00673943368903;		// (AE/re) squared - 1.0
const double BLATE = 0.993395682222;
const double AE = 20925646.0;				// equatorial radius in feet
const double EE = 0.0818188106627;			// Earth's eccentricity
const double REFRACTCONST = 300.0;			// nominal value for atmospheric refraction
const double MTRPERSTMILE = 1609.344; 
const double AEM = 6378135.0;				// equitorial radius in meters
const double REM = 6356750.5;				// polar radius in meters
const double OMF = 1.0 - 0.00352779454;
const double OMFS = OMF * OMF;
const double SPEEDOFLIGHT = 299.691162387e6;// meters per sec


#endif
