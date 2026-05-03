#ifndef PREDICT_H
#define PREDICT_H
//
//	Predict.h
//
//	Contains data prototypes for the predict (sgp) routine.
//

//
// This struct holds the ephemeris information for the vector which was read in from
// the two line element set (tle).  This structure is used with the predict routine,
// (offically known as sgp).  This structure contains all the informtation to allow 
// propagation of the ephemeris set with out re-initializing it. This will soon be 
// converted to a C++ class.  
//
struct Vector {
	char	satelliteName[32];
	int		catalogNumber;
	int		elementSet;
	int 	launchYear;
	int		epochRev;
	int		epochYear;				// 4 digit year
	int		epochDay;				// julian day of year
	double	epochDayFraction;	
	double	inclination;			// degrees
	double	rtAscAN;				// degrees
	double	eccentricity;
	double	argPerigee;				// degrees
	double	meanAnomaly;			// degrees
	double	bStarDrag;				// Drag or Radiation Pressure Coefficient
	int		meanMotionIntegral;		// revs per day
	double	meanMotionFraction;		// revs per day
	double	meanMotionRate;			// revs per day**2
//
// parameters used by predict or returned by predict
//
	int		year;					// this is the year to propagate to
	int		day;					// this is the julian day to propagate to
	double	fracDay;				// the fractional day to propagate to

	double	om;						// rt. ascension of ascending node
	double	w; 						// argument of perigee
	double	xm;      				// mean anomaly
	double	x, y, z;				// Earth centered inertial feet
	double	xdot, ydot, zdot;		// feet per second
	double	nadirAlt;				// in equatorial Earth radii
	double	nadirLat;				// range: +- 90.0 degrees
	double	nadirLong;				// range: +- 180.0 degrees
//
// misc predict information which has to be kept around to propagate orbit
//
	int mjd0;
	double adotoa, ci, si, azoq, bc, fai, dwdu, domdu;
};

//
// subroutine prototype
//
int predict( int init, Vector *vec );


#endif