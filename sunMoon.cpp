//
// sunMoon.cpp
//
// The purpose of sunMoon() is to compute the position of the Sun and 
// the Moon at any specified time.  The error in the output location
// of the Moon is about 1/1000 of its radial distance and the error
// in the output location for the Sun is about 1/20,000 of its radial
// distance.
//
// INPUT
//   tmjd - the current modified Julian date.  The reference for 
//          computing this date is 19600101.000000
//
// OUTPUT
//   dirCos - Direction of cosines of the Sun and the Moon plus the
//            geocentric distances of the same in equatorial Earth radii.
//
// Compute dirCos array to determine gravational pull of Sun and Moon
//
#include "stdafx.h"
#include "FuncHdr.h"
#include "qconvk.h"

void sunMoon( double tmjd, double* sunDirCos, double* sunDist, double* moonDirCos,
							double* moonDist )
{
	double alm, als, c, cam, cas, cdm, cep, d, delm, den, eps, esun, pmor, s, sam;
	double sas, sep, t, vm, xfm, xlamm, xslm, xsls, yp, zp;

	t     = tmjd - 36934.0;
	xlamm = 5.3158443 + 0.22997150230 * t;
	xslm  = 0.8532023 + 0.22802714022 * t;
	d     = 0.4347637 + 0.21276871085 * t;
	xfm   = 2.1964995 + 0.23089572140 * t;
	xsls  = 6.2380306 + 1.720196968e-2 * t;
	eps   = 0.4091835 - 6.22e-9 * t;
	sep   = sin( eps );
	cep   = cos( eps );
	den   = 206264.806;

	// for Sun

	esun = 0.01672591 - 1.144e-9 * t;
	c    = esun * cos( xsls );
	s    = esun * sin( xsls );
	vm   = s / 24.0 * (48.0 + 60.0 * c + c * c * (72.0 + 81.0 * c) - s * s * (32.0 + 125.0 * c ));
	als  = xlamm - d + vm;
	cas  = cos( als );
	sas  = sin( als );

	sunDirCos[0] = cas;
	sunDirCos[1] = sas * cep;
	sunDirCos[2] = sas * sep;
	*sunDist      = 149557.0 / 6.378165 / (1.0 + esun * cos( vm + xsls ));

	// for Moon ( reduced form )

	alm  = xlamm + (22639.5 * sin( xslm ) + 769.0 * sin( 2.0 * xslm)
		         + 2369.9 * sin( 2.0 * d ) + 4586.5 * sin( 2.0 * d - xslm )
				 - 668.1 * sin( xsls ) - 411.6 * sin( 2.0 * xfm )) / den;

	pmor = 3422.5 + 186.5 * cos( xslm ) + 10.1 * cos( 2.0 *xslm )
		          + 28.2 * cos( 2.0 * d ) + 34.3 * cos( 2.0 * d - xslm )
				  + 3.1 * cos( 2.0 * d + xslm);

	delm = (996.3 * sin( xslm - xfm ) + 621.8 * sin( 2.0 * d - xfm )
		   + 18464.1 * sin( xfm ) + 1013.1 * sin(xslm + xfm)) / den;

	cam = cos( alm );
	sam = sin( alm );
	cdm = cos( delm );
	zp  = sin( delm );
	yp  = sam * cdm;

	moonDirCos[0] = cam * cdm;
	moonDirCos[1] = yp * cep - zp * sep;
	moonDirCos[2] = zp * cep + yp * sep;
	*moonDist      = 1.315591e12 / pmor / 6378165.0;
}

