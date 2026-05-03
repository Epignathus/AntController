// ConsoleApplication1.cpp : Defines the entry point for the console application.
//
// Notes: You must place the Scientific Atlanta in REM (Remote) both axes, and have
// both axzes in standby before executing. The software will command the axis into 
// Manual Positioin and set the commanded angles which the Rabbit controller should
// obey.
// As of Nov 3rd, 2025, there is no safeguard limits on either Az or El.
// Be sure to start the Rabbit application before running this application.
//
// On December 28, 2025 attempting to track the moon, Az axis commanded to 105 degrees,
// stopped at 111 degrees - Why? The belt didn't break, and it wasn't at a hard stop.

#include "stdafx.h"
#include "qconvk.h"
#include "FuncHdr.h"
#include <Windows.h>
#include <iostream>
#include <string>
#include <cstdlib>
#include <ctype.h>
#include <stdio.h>
#include <time.h>
#include <chrono>
#include <thread>

//#include "vehdlg.h"
#include "cephem.h"

int hexChar2Int(char);
char intToHexChar(long value);

double siteLat = 38.946310 * DEGTORAD; // Yorkville WeatherWatch Dish
double siteLon = 236.709613 * DEGTORAD;
double siteAlt = 2330.0; // feet

double sunAz, sunEl, moonAz, moonEl;
double f_ElAngle, f_AzAngle;

time_t ltime;
struct tm* gmt;

int status;

bool vehdlg();

int main()
{
	long i_hold, num[100];
	long i_Byte;
	char dataToSend[100];
	char buffer[256];
	DWORD bytesWritten, bytesRead;

	//vehdlg();
	//Display();

/****************************************************************************************************************/	
	HANDLE hComm = CreateFile(
		L"\\\\.\\COM12", // COM port number
		GENERIC_READ | GENERIC_WRITE, // Read/Write access
		0,                            // No sharing
		NULL,                         // No security attributes
		OPEN_EXISTING,                // Open existing port only
		0,                            // Non-overlapped I/O
		NULL                          // Null for comm devices
	);

	if (hComm == INVALID_HANDLE_VALUE) {
		printf("Could not get a handle to a com port\n");
		std::cout << "Enter a digit to release hold";
		std::cin >> i_hold;
		return 0;
	}

	DCB dcbSerialParams = { 0 };
	dcbSerialParams.DCBlength = sizeof(dcbSerialParams);

	if (!GetCommState(hComm, &dcbSerialParams)) {
		printf("Could not get com state\n");
		std::cout << "Enter a digit to release hold";
		std::cin >> i_hold;
		CloseHandle(hComm);
		return 0;
	}

	dcbSerialParams.BaudRate = CBR_9600; // Example baud rate
	dcbSerialParams.ByteSize = 8;
	dcbSerialParams.Parity = NOPARITY;
	dcbSerialParams.StopBits = ONESTOPBIT;

	if (!SetCommState(hComm, &dcbSerialParams)) {
		std::cout << "Enter a digit to release hold";
		std::cin >> i_hold;
		CloseHandle(hComm);
		return 0;
	}
	COMMTIMEOUTS timeouts = { 0 };
	timeouts.ReadIntervalTimeout = 500;
	timeouts.ReadTotalTimeoutConstant = 500;
	timeouts.ReadTotalTimeoutMultiplier = 10;
	timeouts.WriteTotalTimeoutConstant = 500;
	timeouts.WriteTotalTimeoutMultiplier = 10;

	if (!SetCommTimeouts(hComm, &timeouts)) {
		std::cout << "Enter a digit to release hold";
		std::cin >> i_hold;
		CloseHandle(hComm);
		return 0;
	}
	// set the controller to Manual Antenna mode.
	strcpy_s(dataToSend, " P00 W 04\r"); //Azimuth
	if (WriteFile(hComm, dataToSend, 10, &bytesWritten, NULL)) {
		printf("%s\n, %d\n", dataToSend, bytesWritten);
	}
	// just check what might have come back from the controller.
	if (ReadFile(hComm, buffer, 100, &bytesRead, NULL)) {
		printf("Read %d bytes after command to go Manual mode.\n", bytesRead);
		for (int i = 0; i < (int)bytesRead; i++)
		{
			printf("%c ", buffer[i]);
		}
	}
	strcpy_s(dataToSend, " P01 W 04\r"); //Elevation
	if (WriteFile(hComm, dataToSend, 10, &bytesWritten, NULL)) {
		printf("%s\n, %d\n", dataToSend, bytesWritten);
	}
	// just check what might have come back from the controller.
	if (ReadFile(hComm, buffer, 100, &bytesRead, NULL)) {
		printf("Read %d bytes after command to go Manual mode.\n", bytesRead);
		for (int i = 0; i < (int)bytesRead; i++)
		{
			printf("%c ", buffer[i]);
		}
	}
	strcpy_s(dataToSend, " P0F R6\r"); // get AZ and EL positions in binary
	if (WriteFile(hComm, dataToSend, 8, &bytesWritten, NULL)) {
		printf("%s\n, %d\n", dataToSend, bytesWritten);
	}

	if (ReadFile(hComm, buffer, 100, &bytesRead, NULL)) 
	{
		printf("Read %d bytes, expecting ?\n", bytesRead);
		printf("\n Now the char:");
		for (int i = 0; i < (int)bytesRead; i++)
		{
			printf("%c ", buffer[i]);
		}
		printf("\n Now the hex:");
		for (int i = 0; i < (int)bytesRead; i++)
		{
			printf("%02x ", buffer[i]);
		}
		printf("\n Now the hex data only:");
		for (int i = 1; i <= 12; i++)
		{
			printf("%02x ", buffer[i]);
		}
		printf("\n Now the decimal equivalents:");
		for (int i = 1; i <= 12; i++)
		{
			num[i - 1] = hexChar2Int(buffer[i]);
			printf("%d ", num[i - 1]);
		}
		printf("\n");
			
	}
	// calculate the elevation angle in degrees
	// compose a hex number from the data
	i_Byte = (num[10] << 20) | (num[11] << 16) | (num[8] << 12) | (num[9] << 8) | (num[6] << 4) | num[7];
	f_ElAngle = ((double)i_Byte / pow(2, 24)) * 360.0;

	// calculate the azimuth angle in degrees
	// compose a hex number from the data
	i_Byte = (num[4] << 20) | (num[5] << 16) | (num[2] << 12) | (num[3] << 8) | (num[0] << 4) | num[1];
	f_AzAngle = ((double)i_Byte / pow(2, 24)) * 360.0;
	printf("AZ: %7.3lf, EL: %7.3lf\n", f_AzAngle, f_ElAngle);
	
	while (1) {
		time(&ltime);
		status = calcSunMoonAzEl(ltime, siteLat, siteLon, siteAlt, &sunAz, &sunEl, &moonAz, &moonEl);
		gmt = gmtime(&ltime);
		printf("\nDate: %4.4d/%2.2d/%2.2d Time: %2.2d:%2.2d:%2.2d - Sun Az: %7.3lf, Sun El: %7.3lf,   Moon Az: %7.3lf, Moon El: %7.3lf\n\n",
			gmt->tm_year + 1900, gmt->tm_mon + 1, gmt->tm_mday, gmt->tm_hour, gmt->tm_min, gmt->tm_sec, sunAz, sunEl, moonAz, moonEl);

		//f_AzAngle = sunAz + 2.411; // 2.411 degrees is a bias in the az axis synchros mis-positioning.
		//f_ElAngle = sunEl + 0.025;
		f_AzAngle = moonAz + 2.411; // 2.411 degrees is a bias in the az axis synchros.
		f_ElAngle = moonEl + 0.025;
		printf("New Commanded Angles AZ: %7.3lf, EL: %7.3lf\n", f_AzAngle, f_ElAngle);

		// Get a new set of angles to command the antenna controller to slew the antenna to.
		// write data to the antenna controller. First elevation.
		i_Byte = (f_ElAngle / 360.0)*pow(2, 24);
		printf("El Byte: %ld\n", i_Byte);
		num[0] = i_Byte & 0x0F;
		num[1] = (i_Byte >> 4) & 0x0F;
		num[2] = (i_Byte >> 8) & 0x0F;
		num[3] = (i_Byte >> 12) & 0x0F;
		num[4] = (i_Byte >> 16) & 0x0F;
		num[5] = (i_Byte >> 20) & 0x0F;
		for (int i = 5; i >= 0; i--)
		{
			printf("num[%d] %02x ", i, num[i]);
		}
		printf("\n");
		// arrange the string to send to the controller in hex
		buffer[7] = intToHexChar(num[0]);
		buffer[6] = intToHexChar(num[1]);
		buffer[9] = intToHexChar(num[2]);
		buffer[8] = intToHexChar(num[3]);
		buffer[11] = intToHexChar(num[4]);
		buffer[10] = intToHexChar(num[5]);
		
		buffer[12] = '\r';
		buffer[13] = 0;
		printf("%s\n", buffer);


		// now decompose azimuth
		i_Byte = (f_AzAngle / 360.0)*pow(2, 24);
		printf("Az Byte: %ld\n", i_Byte);
		num[0] = i_Byte & 0x0F;
		num[1] = (i_Byte >> 4) & 0x0F;
		num[2] = (i_Byte >> 8) & 0x0F;
		num[3] = (i_Byte >> 12) & 0x0F;
		num[4] = (i_Byte >> 16) & 0x0F;
		num[5] = (i_Byte >> 20) & 0x0F;
		for (int i = 5; i >= 0; i--)
		{
			printf("num[%d] %02x ", i, num[i]);
		}
		printf("\n");
		// arrange the string to send to the controller in hex
		buffer[1] = intToHexChar(num[0]);
		buffer[0] = intToHexChar(num[1]);
		buffer[3] = intToHexChar(num[2]);
		buffer[2] = intToHexChar(num[3]);
		buffer[5] = intToHexChar(num[4]);
		buffer[4] = intToHexChar(num[5]);
		//buffer[6] = NULL;
		printf("%s\n", buffer);

		strcpy_s(dataToSend, " P60 W "); // write AZ and EL positions in binary
		strcat_s(dataToSend, buffer);
		//strcat_s(dataToSend, "\r");
		printf("String to send to P60 W xxx: %s\n", dataToSend);
		if (WriteFile(hComm, dataToSend, 20, &bytesWritten, NULL)) {
			printf("%s:, Bytes sent: %d\n", dataToSend, bytesWritten);
		}
		// just check what might have come back from the controller.
		if (ReadFile(hComm, buffer, 100, &bytesRead, NULL)) {
			printf("Read %d bytes after command to go new angle.\n", bytesRead);
			for (int i = 0; i < (int)bytesRead; i++)
			{
				printf("%c ", buffer[i]);
			}
		}
		for (int i = 0; i < 100; i++)
			dataToSend[i] = 0;

		std::this_thread::sleep_for(std::chrono::seconds(1));
	} // end while(1)
/*******************************************************************************************************/




	std::cout << "\nEnter a digit to release hold  ";
	std::cin >> i_hold;
	//CloseHandle(hComm);
	return 0;
}

