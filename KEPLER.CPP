//#include "stdafx.h"
#include "qconvk.h"

// kepler calculate the position and velocity coordinates in inertial geocentric-equtorial
// reference frame.

int kepler(double rldeg, double ec, double es, double eps, double* p_upomdeg)
{
	double rl = fmod(rldeg, 360.0) * DEGTORAD;
	double cl = cos(rl);
	double sl = sin(rl);
	double c = ec*cl + es*sl;
	double s = ec*sl - es*cl;
	if (ec*ec + es*es < 0.0025) {
		*p_upomdeg = (rl + s*(48.0 + c*(60.0 + c*(72.0 + 81.0*c)) - 
					s*s*(32.0 + 125.0*c))/24.0) * RADTODEG;
		// cout << "No iterations upomdeg = " << upomdeg << endl;
		return(0);
	} else {
		int iter = 0;
		double ep = rl + s*(1.0 - s*s*(0.5 + 1.66666667*c))/(1.0 - c);
		do {
			double sep = sin(ep);
			double cep = cos(ep);
			double ec1 = 1.0 - ec*cep - es*sep;
			double esea = ec*sep - es*cep;
			double dep = (rl - ep + esea)/ec1;
			ep += dep;
			if (fabs(dep) < eps) {
				*p_upomdeg = (ep + 2.0*atan(esea/(ec1 + sqrt(1.0 - ec*ec - es*es)))) * RADTODEG;
//				cout << "Iterated " << iter << " times, upomdeg = " << upomdeg << endl;
				return(0);
			}
		} while (iter++ < 30);

		// no convergence in 30 iterations so return bad status

		return(1);
	}
}
