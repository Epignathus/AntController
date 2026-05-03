//
// dateTimetoMJD
//
// This function takes the date and time and computes the modified Julian
// day.
//
// Inputs:
//		year - four digit year
//		month
//		day
//		hour
//		minute
//		day
//
//

double dateTimeToMJD( int year, int month, int day,
					  int hour, int minute, double seconds )
{
	int iMJD0 = 367 * (year - 1980) - 7 * (year + (month + 9)/12)/4 +
		275 * month / 9 + day + 47673;

	double dMJD = (double)iMJD0 + 
		((double)hour * 3600.0 + (double)minute * 60.0 + seconds) / 86400.0;

	return dMJD;
}


