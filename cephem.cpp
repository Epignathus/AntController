#include "stdafx.h"
#include "cephem.h"
#include <iostream>
#include <atltime.h>

void Display ()
{
	CString displayTime = CString("");
	//displayTime = Ephemeris.acqTime = CTime::FormatGmt("Day %j %a %b %d, %Y   %H:%M:%S");
	std::cout << "\nFrom CEphemeris::display(): Acquisition: \n" << displayTime;
	//displayTime = Ephemeris.maxElTime = FormatGmt("Day %j %a %b %d, %Y   %H:%M:%S");
	std::cout << "From CEphemeris::display(): Max El time: %s\n" << displayTime;
	//displayTime = Ephemeris.fadeTime = FormatGmt("Day %j %a %b %d, %Y   %H:%M:%S");
	std::cout << "From CEphemeris::display(): Fade: %s\n" << displayTime;
}
