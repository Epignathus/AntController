#ifndef CSATELLITE_H
#define CSATELLITE_H

//#include "stdafx.h"
#include "cephem.h"
#include <stdio.h>

struct Vector {
	int		catalogNumber;
	int		elementSet;
	int		launchYear;
	int		epochRev;
	int		epochYear;				// 2 digit year
	int		epochDay;				// julian day of year
	double	epochDayFraction;
	double	inclination;			// degrees
	double	rtAscAN;					// degrees
	double	eccentricity;
	double	argPerigee;				// degrees
	double	meanAnomaly;			// degrees
	int		meanMotionIntegral;	// revs per day
	double	meanMotionFraction;	// revs per day
	double	meanMotionRate;		// revs per day**2
};


/**********************************************************************************
class CSatellite : public CObject {
public:
	CString satelliteName;
	CObList* pEphem;				// a list of ephemeris objects
	CTime		spanStartTime;	// predict ephemeris interval
	CTime		spanStopTime;
	double	dCalcInterval;	// // delta t in seconds for orbit calculations

	int orbitCalc (CString, double, int, int, double, double, double);
	int readVector (char *);
//private:
	Vector vector;
};
**************************************************************************************/


#endif