#ifndef CEPHEM_H
#define CEPHEM_H


#include <atltime.h>
#define MAX_TRACE_PTS 50

enum	TrackSide	{EAST, WEST};
enum	Direction	{ASCENDING, DESCENDING};

struct CEphemeris
{
	CString	satelliteName;	// simpifies code at pass selection time
	CTime		acqTime;
	CTime		maxElTime;
	CTime		fadeTime;
	CTime		traceTime[MAX_TRACE_PTS];
	double	acqAz;
	double	acqSlantRange;
	double	maxElAz, maxEl;
	double	maxElSlantRange;
	double	fadeAz;
	double	acqNadirLat, acqNadirLong;
	double	maxElNadirLong;
	double	fadeNadirLat, fadeNadirLong;
	double	fadeSlantRange;
	double  traceAz[MAX_TRACE_PTS];  // traceAz, traceEl, numTracePts used to store
	double  traceEl[MAX_TRACE_PTS];  // pointing angles during the satellite pass
	int     numTracePts;
	TrackSide	trackSide;	//EAST or WEST of track
	Direction	direction;	//ASCENDING or DESCENDING
};


void Display();

#endif