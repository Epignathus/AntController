#include "stdafx.h"
#include "csatellite.h"
#include <iostream.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>

#include "orbit.h"

int readVector(char* fileName)
{
	FILE* vectorFile;
	char buffer[133];

	vectorFile = fopen(fileName, "r");
	if (vectorFile == NULL)
		return(1);

	int foundSatellite = 0;
	int items = 0;

	do {
		items = fscanf(vectorFile, "%s\n", buffer);
		if (items > 0) {
			if (strcmp(buffer, satelliteName) == 0)
				foundSatellite = 1;
		}
	} while (foundSatellite == 0  &&  items > 0);
	if (foundSatellite == 0) {
		fclose (vectorFile);
		return(1);				// couldn't find our baby...
	}

	// we have the vehicle so read each field and decode it into the structure

	// read over "Catalog number:"

	items = fscanf(vectorFile, "%s\n", buffer);		// "Catalog"
	items = fscanf(vectorFile, "%s\n", buffer);		// "number:"
	items = fscanf(vectorFile, "%d\n", &vector.catalogNumber);

	// read over "Epoch time:"

	items = fscanf(vectorFile, "%s\n", buffer);		// "Epoch"
	items = fscanf(vectorFile, "%s\n", buffer);		// "time:"
	items = fscanf(vectorFile, "%s\n", buffer);
	char yr[3], day[4];
	yr[0] = buffer[0];
	yr[1] = buffer[1];
	yr[2] = 0;
	vector.epochYear = atoi(yr);
	day[0] = buffer[2];
	day[1] = buffer[3];
	day[2] = buffer[4];
	day[3] = 0;
	vector.epochDay = atoi(day);
	vector.epochDayFraction = atof(&buffer[5]);

	// next item is "Element set:"

	items = fscanf(vectorFile, "%s\n", buffer);		// "Element"
	items = fscanf(vectorFile, "%s\n", buffer);		// "Set"
	items = fscanf(vectorFile, "%d\n", &vector.elementSet);

	// "Inclination;"

	items = fscanf(vectorFile, "%s\n", buffer);		// "Inclination:"
	items = fscanf(vectorFile, "%lf\n", &vector.inclination);
	items = fscanf(vectorFile, "%s\n", buffer);		// "deg"

	// "RA of node:"

	items = fscanf(vectorFile, "%s\n", buffer);		// "RA"
	items = fscanf(vectorFile, "%s\n", buffer);		// "of"
	items = fscanf(vectorFile, "%s\n", buffer);		// "node:"
	items = fscanf(vectorFile, "%lf\n", &vector.rtAscAN);
	items = fscanf(vectorFile, "%s\n", buffer);		// "deg"

	// "Eccentricity:"

	items = fscanf(vectorFile, "%s\n", buffer);		// "Eccentricity:"
	items = fscanf(vectorFile, "%lf\n", &vector.eccentricity);

	// "Arg of perigee:"

	items = fscanf(vectorFile, "%s\n", buffer);		// "Arg"
	items = fscanf(vectorFile, "%s\n", buffer);		// "of"
	items = fscanf(vectorFile, "%s\n", buffer);		// "perigee:"
	items = fscanf(vectorFile, "%lf\n", &vector.argPerigee);
	items = fscanf(vectorFile, "%s\n", buffer);		// "deg"

	// "Mean anomaly:"

	items = fscanf(vectorFile, "%s\n", buffer);		// "Mean"
	items = fscanf(vectorFile, "%s\n", buffer);		// "anomaly:"
	items = fscanf(vectorFile, "%lf\n", &vector.meanAnomaly);
	items = fscanf(vectorFile, "%s\n", buffer);		// "deg"

	// "Mean motion:"

	items = fscanf(vectorFile, "%s\n", buffer);		// "Mean"
	items = fscanf(vectorFile, "%s\n", buffer);		// "motion:"
	int intMotion;
	double fractMotion;
	items = fscanf(vectorFile, "%lf\n", &fractMotion);
	intMotion = (int)fractMotion;
	fractMotion -= (double)intMotion;
	vector.meanMotionIntegral = intMotion;
	vector.meanMotionFraction = fractMotion;
	items = fscanf(vectorFile, "%s\n", buffer);		// "rev/day"

	// "Decay rate:"

	items = fscanf(vectorFile, "%s\n", buffer);		// "Decay"
	items = fscanf(vectorFile, "%s\n", buffer);		// "rate:"
	items = fscanf(vectorFile, "%lf\n", &vector.meanMotionRate);
	items = fscanf(vectorFile, "%s\n", buffer);		// "rev/day^2"

	// "Epoch rev:"

	items = fscanf(vectorFile, "%s\n", buffer);		// "Epoch"
	items = fscanf(vectorFile, "%s\n", buffer);		// "rev:"
	items = fscanf(vectorFile, "%d\n", &vector.epochRev);
	
	fclose (vectorFile);
	return(0);
}
