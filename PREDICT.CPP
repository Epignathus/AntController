//#include "stdafx.h"
#include "Predict.h"
#include "qconvk.h"
#include "FuncHdr.h"
#include <stdio.h>
#include <iostream>

//
// sgp method for ephemeris propogation
//
int predict( int init, Vector *vec )
{
	double aooq, poq, xj2bar;

	// calculate preliminary quantities

	if (init == 1) {
		// mjd0: +15384 correlates from 1900 back to 11/18/1857(1858?)
		// NOTE:  mjd0: +36934 correlates from 1960 back to 11/18/1857(1858?)
		// OLD19980515: vec->mjd0 = 15384 + (int)(365.251*(double)(vec->epochYear - 1)) + vec->epochDay;

		vec->mjd0 = 15384 + (int)(365.251*(double)(vec->epochYear - 1 - 1900)) + vec->epochDay;
		vec->adotoa = -4.0/3.0*vec->meanMotionRate /( (double)vec->meanMotionIntegral + vec->meanMotionFraction );
		vec->ci = cos(vec->inclination * DEGTORAD);
		vec->si = sin(vec->inclination * DEGTORAD);
		aooq = pow(XMU * pow(86400.0/((double)vec->meanMotionIntegral + vec->meanMotionFraction)/M_2PI, 2.0), 1.0/3.0) / REQ;
		vec->azoq = aooq*(1.0 - 0.25 * XJ2 / aooq / aooq *(3.0*vec->ci*vec->ci - 1.0)/pow(1.0 - vec->eccentricity*vec->eccentricity, 1.5) );
		poq = vec->azoq*(1.0 - vec->eccentricity*vec->eccentricity);
		vec->bc = 0.001172535/poq*vec->si;
		vec->fai = vec->bc/2.0*(3.0 + 5.0*vec->ci)/(1.0 + vec->ci) * RADTODEG;
		xj2bar = XJ2/(poq*poq);
		vec->dwdu = -0.75*xj2bar*(1.0 - 5.0*vec->ci*vec->ci);
		vec->domdu = -1.5*xj2bar*vec->ci;
		return(0);
	}
	
	// propagate mean orbital elements from the initial epoch to the final epoch

	//OLD19980515:  int mjd = 15384 + (int)(365.251*(double)(vec->year -1)) + vec->day;
	int mjd = 15384 + (int)(365.251*(double)(vec->year -1 - 1900)) + vec->day;
	int idt = mjd - vec->mjd0;
	double fdt = vec->fracDay - vec->epochDayFraction;
	double dt = (double)idt + fdt;
	double aoq = vec->azoq*(1.0 + vec->adotoa*dt);
	double dm = ((double)vec->meanMotionIntegral*fdt + vec->meanMotionFraction*dt + vec->meanMotionRate*dt*dt)*360.0;
	double dmtot = dm + (double)vec->meanMotionIntegral*(double)idt*360.0;
	vec->w = fmod((vec->argPerigee + vec->dwdu*dmtot), 360.0);
	if (vec->w < 0.0)
		vec->w += 360.0;
	vec->om = fmod((vec->rtAscAN + vec->domdu*dmtot), 360.0);
	if (vec->om < 0.0)
		vec->om += 360.0;
	double a = vec->eccentricity*cos(vec->w * DEGTORAD);
	double b = vec->eccentricity*sin(vec->w * DEGTORAD) + vec->bc;
	vec->xm = fmod((vec->meanAnomaly + dm + a*vec->fai), 360.0);
	if (vec->xm < 0.0)
		vec->xm += 360.0;

	// now calculate the position and velocity coordinates in inertial geocentric-equtorial
	// reference frame.

	std::cout << "to Kepler: xm = " << vec->xm << ", w = " << vec->w << ", a = " << a << ", b = " << b << std::endl;
  	double u;
	int status = kepler(vec->xm + vec->w, a, b, 1.0e-6, &u);
	if (status) {
		printf ( "Function Kepler() failed to converge in 30 iterations - ABORT" );
		return(1);
	}
	std::cout << "u from Kepler = " << u << std::endl;
	double com = cos(vec->om * DEGTORAD);
	double som = sin(vec->om * DEGTORAD);
	double cu = cos(u * DEGTORAD);
	double su = sin(u * DEGTORAD);
	double ri = com*cu - som*vec->ci*su;
	double rj = som*cu + com*vec->ci*su;
	double rk = vec->si*su;
	double ui = -com*su - som*vec->ci*cu;
	double uj = -som*su + com*vec->ci*cu;
	double uk = vec->si*cu;
	double p = aoq*(1.0 - a*a - b*b) * REQ;
	double r = p/(1.0 + a*cu + b*su);
	vec->x = r * ri * FTPERKILOMETER;
	vec->y = r * rj * FTPERKILOMETER;
	vec->z = r * rk * FTPERKILOMETER;

	double rudot = sqrt( XMU * p )/r;
	double rdotor = rudot*(a*su - b*cu)/p;
	vec->xdot = rdotor*vec->x + rudot*ui;
	vec->ydot = rdotor*vec->y + rudot*uj;
	vec->zdot = rdotor*vec->z + rudot*uk;
	return(0);
}
