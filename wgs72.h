//
// WGS-72 physical and geopotential constants
//
// ck2 = 0.5 * j2 * az**2,
// ck4 = -0.375 * jr * az**4,    and 
// a3ovk2 = -xj3 / ck2 * az**3;  where az = 1.0
//
// aj2 = 1.082616e-3
// xj4 = -1.65597e-6
// qo  = 120.0
// so  = 78.0
//
// ck2		= 0.5 * aj2					= 5.4130800e-4
// ck4		= -0.375 * xj4				= 6.2238750e-7
// qoms2t	= ((qo - so) / xkmper)**4	= 1.8802768e-9
// s		= 1.0 + so / xkmper			= 1.0122293
//
#ifndef WGS72_H
#define WGS72_H

const double XKMPER	= 6378.137;
const double CK2	= 5.4130800e-4;
const double S		= 1.0122293;
const double CK4	= 6.2238750e-7;
const double QOMS2T	= 1.8802768e-9;
const double XJ3	= -0.253881e-5;
const double XKE	= 0.743669161e-1;

#endif

