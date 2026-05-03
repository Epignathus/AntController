#include "qconvk.h"

// This function returns the right ascension of Greenwich meridian for any input date and
// systems time after 1979.

// Input - modified Julian day, decimal fraction of a day

double rag( int mjd, double fmjd )
{
	double tus, gmst0, gmst;

	tus = ((double)mjd - 15019.5)/36525.0;
	gmst0 = 1.739936 + 628.331951*tus + 6.8e-6*tus*tus;
	gmst = fmod(gmst0 + 6.3003881*fmjd, M_2PI);
	return(gmst);
}
