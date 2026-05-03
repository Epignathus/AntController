// vehdlg.cpp : implementation file
//

#include "stdafx.h"
#include "vehdlg.h"
#include "orbit.h"
#include "cephem.h"
#include "csatellite.h"
//#include "acqdlg.h"
//#include "waitdlg.h"

#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

//#ifdef _DEBUG
//#undef THIS_FILE
//static char BASED_CODE THIS_FILE[] = __FILE__;
//#endif
	
// open the vectors.log file and read all of the satellite names. then go through them one-by-one.

bool vehdlg()
{
	vectorFile = fopen ("vectors.log", "rb");
	if (vectorFile == NULL) {
		std::cout << "Unable to open vectors.log file...";
		return false;
	}
	int length = 0;
	char buffer[133];							// a lines worth
	char satelliteName[128];  // names are only this big
	string s_input;

	while ( fgets( buffer, 75, vectorFile ) != NULL ) {
		if ((strncmp (buffer, "1 ", 2) != 0) && (strncmp (buffer, "2 ", 2) != 0)) {
			length = strlen ( buffer );
			strcpy ( satelliteName, buffer );

			length--;
			do {	// trim the extra spaces that are in the name off
				satelliteName[length-1] = NULL;
				length--;
			} while ( strncmp ( &satelliteName[length-1], " ", 1 ) == 0 );

			//m_ListVehicles.AddString (satelliteName);
			std::cout << "Satellite Name: " << satelliteName << "\n";
		}
	}
	// set the offset to begin the predict ephemeris to zero days
	m_EditOffset = 0;

	// set the number of days to generate predict ephemeris to one day
	m_EditPredict = 1;

	// set the minimum maximum elevation for a valid acquisition to 25.0 degrees
	m_EditMinEl = 5.0;

	// set the default lat, long, and altitude
	m_EditAlt = 2335.0;				// altitude in feet at W6DSR		
	m_EditLat = 38.950487;			// latitude in degrees at W6DSR
	m_EditLong = 236.707839;		// longitude in degrees east at W6DSR 	
	
	// ask the user to select a satellite to track.
	cout << "Select a vehicle to track.\n";
	getline(cin, s_input);
	cout << "You selected vehicle: " << s_input << "\n";

	// now find this vehicle in the vectors.log file and collect the state vector information.



	fclose(vectorFile);
	return TRUE;
					  
}
/********************************************************************************************************************
void CVehDlg::OnOK() 
{
	UpdateData (TRUE);	// catch all those user edits

	// see if any satellites were selected and if so, dig their vectors out of the vectors.log file
	
	int selectedItemCount = m_ListVehicles.GetSelCount();
	if (selectedItemCount <= 0) {
		AfxMessageBox ("You didn't select a vehicle ???", MB_OK, 0);
		return;
	}
	int* itemList = new int[selectedItemCount + 1];						// the +1 because I'm paranoid	
	m_ListVehicles.GetSelItems (selectedItemCount + 1, itemList);	// the +1 because I'm still paranoid
	
	// the CObList ptr p_SatelliteList has been initialized in the noaadoc constructor. request heap for the
	// next satellite instance
	
	CMainFrame* p_noaaFrame = (CMainFrame *)AfxGetApp()->m_pMainWnd;
	CNoaaDoc* pDoc = (CNoaaDoc *)p_noaaFrame->GetActiveDocument();
	ASSERT_VALID(pDoc);	// what a long way to go to get the damn document!!

	for (int vehIndex = 0; vehIndex < selectedItemCount; vehIndex++) {
		CSatellite* p_satellite = new CSatellite;												// deleted after acquisition over
		m_ListVehicles.GetText (itemList[vehIndex], p_satellite->satelliteName);	// Nth satellite name

		int status = p_satellite->readVector ("vectors.log");
		if (status) {
			MessageBox ("Unable to find satellite in the vectors.log file", NULL, MB_OK);
//			delete itemList;
			delete p_satellite;	// this needs work...
			CDialog::OnOK();
		}
		// there is a ptr to a CObList list of ephemeris for this vehicle in the CSatellite class definition.
		// request off the heap, the 1st block of ptrs to these objects.

		p_satellite->pEphem = new CObList;	// gets default blocksize number to start

		// now call orbitCalc. it will request heap for each acquisition as a ptr to class CEphemeris.
		// orbitCalc() calls new for each ptr so after the pass is recorded, they must be deleted.

		CWaitDlg cwaitDlg;	// this could take some time so prepare the user
		cwaitDlg.Create(IDD_DIALOG_WAIT, NULL);
		CWnd* waitWnd = cwaitDlg.GetDlgItem (IDC_STATIC_WAIT);
		VERIFY (waitWnd);
		waitWnd->SetWindowText ("Computing Ephemeris for " + p_satellite->satelliteName);

		status = p_satellite->orbitCalc ( p_satellite->satelliteName, m_EditMinEl, m_EditPredict,
																	 m_EditOffset, m_EditLat, m_EditLong, m_EditAlt );
		if (status) {	
			AfxMessageBox ("orbitCalc() failed...", MB_OK, 0);	// something went wrong
		} else {
		
			// we have all of the ephemeris for this vehicle in a linked list. move on to the next vehicle

			TRACE ("All ephemeris for this vehicle has been generated.\n");
		
		}
		cwaitDlg.DestroyWindow();

		pDoc->p_satelliteList->AddTail ( (CObject *)p_satellite );	// adds this ptr to the linked list
#ifdef _DEBUG		
		afxDump.SetDepth( 1 );
		afxDump << "Satellite AddTail: " << pDoc->p_satelliteList << "\n";	// in debug only
#endif
	}	// end loop through selected satellites

	// throw up one more dialog to let him select those passes to be recorded. this dialog is in the view
	// class so that the manual dialog can get a handle to its timer.
	
	CNoaaView* pView = (CNoaaView *)p_noaaFrame->GetActiveView();
	ASSERT_VALID(pView);

	pView->acqDlg.DoModal();	// the acqDlg gets its list box loaded up in OnInitDialog()

	delete itemList;
	CDialog::OnOK();
}
*************************************************************************************************************************/
