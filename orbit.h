#ifndef ORBIT_H
#define ORBIT_H

#pragma once
#include <math.h>
//#include "csatellite.h"
//#include "cephem.h"

// units conversion constants used throughout

#ifndef M_PI
	const double M_PI = 3.1415926535897932384626;
#endif
const double M_2PI = 2.0*M_PI;
const double radToDeg = 180.0 / M_PI;
const double degToRad = M_PI / 180.0;
const double req = 6378.135;
const double xmu = 398600.8;
const double xj2 = 0.001082616;
const double ftPerKilometer = 3280.839895;
const double ftPerNM = 6076.1154858;
const double mtrPerStMile = 1609.344; 
const double oblate = 0.00673943368903;	// (ae/re) squared - 1.0
const double blate = 0.993395682222;
const double ae = 20925646.0;					// equatorial radius in feet
const double aem = 6378135.0;					// equitorial radius in meters
const double rem = 6356750.5;					// polar radius in meters
const double ee = 0.0818188106627;			// Earth's eccentricity
const double omf = 1.0 - 0.00352779454;
const double omfs = omf*omf;
const double refractConst = 300.0;			// nominal value for atmospheric refraction
const double speedOfLight = 299.691162387e6;	// meters per sec

int predict(int, int, int, double, double, double, double, double, double, int, double, double,
			int, int, double, double &, double &, double &, double &, double &, double &,
			double &, double &, double &);

int kepler(double, double, double, double, double &);
double rag(int, double);
//double time2rag(CTime *);
void inlruto(double, double, double, double, double, double, double &, double &, double &);
void lrlputo(double, double, double, double &, double &, double &);
//int readVector(char *, CSatellite *);
//void julianToMonthDay(int, int, int &, char *, int &);
//void monthDayToJulian(int, int, int, int &);
double refract(double, double);
void nadir ( double, double, double, double, double, double &, double &, double & );
void latLong2ECR(double, double, double, double &, double &, double &);
//Ephemeris* orbitCalc ( char *, double, int, int, CSatellite *);
double fnsin(double, double, double, double, double);
double distance(double, double, double, double, double &);

#endif
