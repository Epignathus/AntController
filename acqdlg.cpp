// acqdlg.cpp : implementation file
//

#include "stdafx.h"
//#include "noaaview.h
//#include "noaa.h"
#include "acqdlg.h"
#include "csatellite.h"
//#include "mainfrm.h"
//#include "noaadoc.h"
#include "mandlg.h"
#include "resource.h"
//#include "sortCObList.h"
//#include "waitdlg.h"
//#include "MCIObj.h"
//#include "cObListCleanup.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

// global variable for current satellite name
CString currentSatName;

/////////////////////////////////////////////////////////////////////////////
// CAcqDlg dialog


CAcqDlg::CAcqDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAcqDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAcqDlg)
	//}}AFX_DATA_INIT
}


void CAcqDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAcqDlg)
	DDX_Control(pDX, IDC_LIST_ACQ, m_EditAcq);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAcqDlg, CDialog)
	//{{AFX_MSG_MAP(CAcqDlg)
	ON_WM_TIMER()
	ON_MESSAGE(MM_MCINOTIFY, OnFinishRecord)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CAcqDlg message handlers

BOOL CAcqDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// load the list box with all possible acquisitions for the previously selected satellites.
	// these have to be in time order or acquisitons will record out of order and get missed.

	// a CObList ptr to the satellite list was declared in the noaadoc constructor.
	
	CMainFrame* p_noaaFrame = (CMainFrame *)AfxGetApp()->m_pMainWnd;
	CNoaaDoc* pDoc = (CNoaaDoc *)p_noaaFrame->GetActiveDocument();
	ASSERT_VALID(pDoc);	// what a long way to go to get the damn document!!

	pDoc->p_combinedEphem = new CObList;	// get a default blocksize of initialized space

	// get a ptr to the next satellite on the list

	CSatellite* p_localSat;
	POSITION posSat;
	CEphemeris* pLocEphem;
	POSITION pos;
	
	for (posSat = pDoc->p_satelliteList->GetHeadPosition(); posSat != NULL;) {
		p_localSat = (CSatellite *)pDoc->p_satelliteList->GetNext(posSat);

		// get one easy ptr to the COblist linked list of ephemeris

		CObList* pCobList = p_localSat->pEphem;

		// Iterate through the list in head-to-tail order.

		for( pos = pCobList->GetHeadPosition(); pos != NULL; ) {
  	 		pLocEphem = (CEphemeris *)pCobList->GetNext( pos );
			
			// store this acquisition for the vehicle at hand in the combined ephemeris list

			pDoc->p_combinedEphem->AddTail ( (CObject *)pLocEphem );
	#ifdef _DEBUG	
			afxDump.SetDepth( 1 );
			afxDump << "Combined Ephemeris AddTail: " << pDoc->p_combinedEphem << "\n";	// in debug only
	#endif
		}	// end loop through acquisitions
	}	// end loop through all vehicles

	// if the CObList is to be sorted in time order, here is the place
	
	CNoaaView* pView = (CNoaaView *)p_noaaFrame->GetActiveView();
	ASSERT_VALID(pView);
	CWaitDlg cwaitDlg;	// this could take some time so prepare the user

	if (pView->m_acqListSort) {
		cwaitDlg.Create(IDD_DIALOG_WAIT, NULL);
		CWnd* waitWnd = cwaitDlg.GetDlgItem (IDC_STATIC_WAIT);
		VERIFY (waitWnd);
		waitWnd->SetWindowText ("Sorting the combined ephemeris");

		sortCObList ( (CObList *)pDoc->p_combinedEphem );
#ifdef _DEBUG
		afxDump.SetDepth( 1 );
		afxDump << "After sort: " << pDoc->p_combinedEphem << "\n";	// in debug only
#endif
	}
 
 	// now that the combined ephemeris list has been sorted, stuff it into the listbox
	// Iterate through the list in head-to-tail order.

	CString cLine, cTemp;
	for( pos = pDoc->p_combinedEphem->GetHeadPosition(); pos != NULL; ) {
  		pLocEphem = (CEphemeris *)pDoc->p_combinedEphem->GetNext( pos );
		cLine = pLocEphem->satelliteName;
		for (int i = cLine.GetLength(); i <= 9; i++)
			cLine += " ";	// to fill it out to uniform size
		cLine += pLocEphem->acqTime.Format("   %b %d %a  %H:%M:%S ");
		cLine += pLocEphem->acqTime.FormatGmt(" (%H:%M:%S) ");
		cLine += pLocEphem->fadeTime.Format("  %H:%M:%S  ");
		cTemp.Format("  %5.1lf  ", pLocEphem->maxEl);
		cLine += cTemp;
		if (pLocEphem->direction == ASCENDING)
			cTemp.Format(" Ascending   ");
		else
			cTemp.Format(" Descending ");
		cLine += cTemp;
		if (pLocEphem->trackSide == EAST)
			cTemp.Format(" East ");
		else
			cTemp.Format(" West ");
		cLine += cTemp;
		cTemp.Format("    (%05.1lf    %05.1lf    %05.1lf) ", pLocEphem->acqAz, pLocEphem->maxElAz, pLocEphem->fadeAz);  
		cLine += cTemp;

		m_EditAcq.AddString ( cLine );
	}	// end loop through all sorted acquisitions
	
	cwaitDlg.DestroyWindow();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CAcqDlg::OnOK() 
{
		
	// a CObList ptr to the combined ephemeris list was declared in the noaadoc constructor.
	
	CMainFrame* p_noaaFrame = (CMainFrame *)AfxGetApp()->m_pMainWnd;
	CNoaaDoc* pDoc = (CNoaaDoc *)p_noaaFrame->GetActiveDocument();
	ASSERT_VALID(pDoc);	// what a long way to go to get the damn document!!

	CObList* pCobList = pDoc->p_combinedEphem;

	CNoaaView* pView = (CNoaaView *)p_noaaFrame->GetActiveView();
	ASSERT_VALID(pView);

	// see if any passes were selected and if so, get the ephemeris for those selections
	// they must be retrived in time order so no passes get missed
	
	int selectedItemCount = m_EditAcq.GetSelCount();
	if (selectedItemCount <= 0) {
		AfxMessageBox ("You didn't select a pass ???", MB_OK, 0);
		return;
	}
	CTime thisTime, highTime = 0;
	int selectedIndex = 0;
	POSITION pos;
	CString cLine;
	pView->cancelPass = FALSE;	// this allows user to cancel everything
	int* itemList = new int[selectedItemCount + 1];				// the +1 because I'm paranoid	
	m_EditAcq.GetSelItems (selectedItemCount + 1, itemList);	// the +1 because I'm still paranoid

	TRACE ("Acquisition selected item count = %d\n", selectedItemCount);
	TRACE (" itemList[0] = %d\n", itemList[0]);
	do {
		thisTime = 999999999;	// to guarantee we find the smallest non-zero acq time
		for (int i = 0; i < selectedItemCount; i++) {
			pos = pCobList->FindIndex(itemList[i]);
			pLocEphem = (CEphemeris *)pCobList->GetNext( pos );
			if (pLocEphem->acqTime != 0  &&  thisTime >= pLocEphem->acqTime) {
				thisTime = pLocEphem->acqTime;
				selectedIndex = i;
			}
			if (pLocEphem->acqTime != 0  &&  highTime < pLocEphem->acqTime) {
				highTime = pLocEphem->acqTime;
			}	
		}
		pos = pCobList->FindIndex(itemList[selectedIndex]);
		pLocEphem = (CEphemeris *)pCobList->GetNext( pos );

		// load the record dialog with the pass information. the manDlg is a child to the view
		// there is enough information here to compose the *.wav file name
 		int position;

		pView->manDlg.m_EditFilename = (CString)(pLocEphem->acqTime.Format("%b%d"));
		pView->manDlg.m_EditFilename.MakeLower();

		currentSatName = pLocEphem->satelliteName;
		cLine = pLocEphem->satelliteName;

		if (( position = cLine.Find ((TCHAR)'/')) != -1 )
			cLine.SetAt (position, '-');

		pView->manDlg.m_EditFilename += cLine;

		if (pLocEphem->direction == ASCENDING)
			pView->manDlg.m_EditFilename += "a";
		else
			pView->manDlg.m_EditFilename += "d";
  	
		if (pLocEphem->trackSide == EAST)
			pView->manDlg.m_EditFilename += "e";
		else
			pView->manDlg.m_EditFilename += "w";

		pView->manDlg.m_EditFilename += ".wav";

		pView->manDlg.cNowLocal = CTime::GetCurrentTime();
		pView->manDlg.cAcqLocal = pLocEphem->acqTime;		// needed for timer callbacks below
		pView->manDlg.m_EditAcqLocal = pLocEphem->acqTime.Format("Day %j, %a %B %d %Y      %H:%M:%S");
		pView->manDlg.m_EditGMT = pLocEphem->acqTime.FormatGmt("Day %j, %a %B %d %Y      %H:%M:%S");

		pView->manDlg.ctSpan = pLocEphem->fadeTime - pLocEphem->acqTime;	
		pView->manDlg.m_EditDur = pView->manDlg.ctSpan.GetTotalSeconds();
		TRACE("%ld duration\n", pView->manDlg.m_EditDur);

		pView->manDlg.m_EditRealTime = pView->manDlg.cNowLocal.FormatGmt("Day %j, %a %B %d %Y      %H:%M:%S");

		pView->manDlg.ctSpan = pLocEphem->acqTime - pView->manDlg.cNowLocal;
		pView->manDlg.m_EditSecToAcq = pView->manDlg.ctSpan.GetTotalSeconds();

		m_tracePtIndex = 0;  // set initial antenna pointing index to 0

		// now what if the pass has already started? there is a negative time in this field
		// decrease the duration by the amount we already missed.

		if (pView->manDlg.m_EditSecToAcq < 0L) {
			pView->manDlg.m_EditDur += pView->manDlg.m_EditSecToAcq;	// remember it's negative
			pView->manDlg.m_EditSecToAcq = 0L;

			// now use the current time to find the correct pointing angle information
			for ( m_tracePtIndex = 0; m_tracePtIndex < pLocEphem->numTracePts - 3; m_tracePtIndex++ ) {
				if (( pLocEphem->numTracePts >= 3 ) &&
					( pLocEphem->traceTime[m_tracePtIndex] > pView->manDlg.cNowLocal ))
					break;
			}
		}

		// format the text for the antenna pointing angles
		if ( pLocEphem->numTracePts >= 3 ) {
			pView->manDlg.m_listTimeAzEl.Format("%s        %5.1lf         %5.1lf                %5.1lf\r%s        %5.1lf         %5.1lf\r%s        %5.1lf         %5.1lf",
				pLocEphem->traceTime[m_tracePtIndex].Format("%H:%M:%S"), pLocEphem->traceAz[m_tracePtIndex], pLocEphem->traceEl[m_tracePtIndex],
				pLocEphem->maxEl,
				pLocEphem->traceTime[m_tracePtIndex+1].Format("%H:%M:%S"), pLocEphem->traceAz[m_tracePtIndex+1], pLocEphem->traceEl[m_tracePtIndex+1], 
				pLocEphem->traceTime[m_tracePtIndex+2].Format("%H:%M:%S"), pLocEphem->traceAz[m_tracePtIndex+2], pLocEphem->traceEl[m_tracePtIndex+2]); 
		}

		if ( m_tracePtIndex < pLocEphem->numTracePts - 3 )
			m_tracePtIndex++;  // set index to next antenna data entry
		
		if ( pView->manDlg.m_EditDur > 0L ) {
			SetTimer(101, 1000, NULL);	// update the real time clock every second. timer 101 is coundown to acq
			pView->manDlg.DoModal();	// return from here for the next acquisition
		}
		else {
			AfxMessageBox("Pass is completely over");
		}

		// set the just recorded pass's acq time to zero. this insures it won't get picked again

		pLocEphem->acqTime = 0;

	} while (pView->cancelPass == FALSE  &&  thisTime != highTime);

	// at this point all passes on the schedule have been canceled or recorded. cleanup the CObLists.

//	cObListCleanup (pCobList);
	delete itemList;
	
	CDialog::OnOK();
}

void CAcqDlg::OnTimer(UINT nIDEvent) 
{
	extern void initWaveHeader(BYTE*);
	CMainFrame* p_noaaFrame = (CMainFrame *)AfxGetApp()->m_pMainWnd;
	CNoaaView* pView = (CNoaaView *)p_noaaFrame->GetActiveView();
	ASSERT_VALID(pView);
	pView->manDlg.UpdateData(TRUE);
	CTime currentTime = CTime::GetCurrentTime();

	if (nIDEvent == 101)	{			// countdown to acquisition
		pView->manDlg.cNowLocal = CTime::GetCurrentTime();
		pView->manDlg.m_EditRealTime = pView->manDlg.cNowLocal.FormatGmt("Day %j, %a %B %d %Y      %H:%M:%S");
		
		pView->manDlg.ctSpan = pView->manDlg.cAcqLocal - pView->manDlg.cNowLocal;
		if (pView->manDlg.ctSpan.GetTotalSeconds() >= 0)
			pView->manDlg.m_EditSecToAcq = pView->manDlg.ctSpan.GetTotalSeconds();
		if (pView->manDlg.m_EditSecToAcq <= 0L) {
			if (pView->manDlg.m_CarrierDetect == TRUE) {	// acq time may arrive but we need a signal too
				
				KillTimer(101);
				pView->manDlg.m_EditSecToAcq = 0;

				// load the proper bitmaps into the record status button. fields are: up, down, focus, disable

				pView->manDlg.m_recordButton.LoadBitmaps ("RECORDD", "RECORDS", "RECORDD", "RECORDS");
				pView->manDlg.m_recordButton.EnableWindow(TRUE);

				// now redefine some of the fields in the dialog for the pass and restart the duration timer

				CWnd* durWnd = pView->manDlg.GetDlgItem (IDC_STATIC_DURATION);
				VERIFY(durWnd);
				durWnd->SetWindowText ("Seconds to Fade (edit)");

				// open a memory mapped file to receive the wav data sans header

				pView->manDlg.m_wavFileHandle = CreateFile(pView->manDlg.m_EditFilename, GENERIC_READ | GENERIC_WRITE,
																		FILE_SHARE_READ, 0, OPEN_ALWAYS, FILE_FLAG_SEQUENTIAL_SCAN,
																		0);
				if (pView->manDlg.m_wavFileHandle == INVALID_HANDLE_VALUE) {
					TRACE("Could not create %s file handle.\n", pView->manDlg.m_EditFilename);
					CDialog::OnTimer(nIDEvent);
					return;
				}
				// create the file mapping. duration*sample rate megabytes max
				// I've seen this be too small and lose the whole pass. take a conservative approach.
//d16				LONG estimatedFileSize = (pView->manDlg.m_EditDur + 120) * 8000 + 16000;
				LONG estimatedFileSize = pView->manDlg.m_EditDur * 8000 + 16000;
				pView->manDlg.m_wavFileMapHandle = CreateFileMapping(pView->manDlg.m_wavFileHandle, 0, PAGE_READWRITE,
																						0, estimatedFileSize, "wavMap");
				if (pView->manDlg.m_wavFileMapHandle <= 0) {
					TRACE("File mapping error for file %s\n", pView->manDlg.m_EditFilename);
					TRACE("Error was %d\n", GetLastError());
					CDialog::OnTimer(nIDEvent);
					return;
				}
				// map the view to an address
				pView->manDlg.m_pwavMap = (BYTE *)MapViewOfFile(pView->manDlg.m_wavFileMapHandle, FILE_MAP_WRITE, 0, 0, 0);
				if (pView->manDlg.m_pwavMap == NULL) {
					TRACE("Map view of file error = %d\n", GetLastError());
					CDialog::OnTimer(nIDEvent);
					return;
				}
				// form the 144 BYTE wav header, the contents of which we have interest in is:
				// the sample rate, the ephemeris, and the start time of the record

				time_t ltime;
				time_t* timePtr;    		// used for time calculations 
				BYTE wavHeader[(58 + sizeof(Vector) + sizeof(time_t))]; // this is 144 bytes
				Vector* vectorPtr;
				vectorPtr = (Vector*)&wavHeader[58];
				timePtr = (time_t*)&wavHeader[(58 + sizeof(Vector))];

				memset(wavHeader, 0, (58 + sizeof(Vector) + sizeof(time_t)));
				initWaveHeader(wavHeader);	// this loads a real *.wav header so we can play the file

				// go and find the satellite in the list of sats selected and get the vector info stored there

				CMainFrame* p_noaaFrame = (CMainFrame *)AfxGetApp()->m_pMainWnd;
				CNoaaDoc* pDoc = (CNoaaDoc *)p_noaaFrame->GetActiveDocument();
				ASSERT_VALID(pDoc);	// what a long way to go to get the damn document, no shit!!

				// get a ptr to the next satellite on the list
				CSatellite* p_localSat;
				POSITION posSat;
	
				for (posSat = pDoc->p_satelliteList->GetHeadPosition(); posSat != NULL;) {
					p_localSat = (CSatellite *)pDoc->p_satelliteList->GetNext(posSat);

					if ( currentSatName.Compare ( p_localSat->satelliteName ) == 0 ) {
						*vectorPtr = p_localSat->vector;
						break;
					}
				}

				wavHeader[24] = 0x40;		// this is 8000 Hz
				wavHeader[25] = 0x1f;

				time ( &ltime );
				*timePtr = ltime;  

				memcpy(pView->manDlg.m_pwavMap, wavHeader, (58 + sizeof(Vector) + sizeof(time_t)));

				pView->manDlg.SetMode(pView->manDlg.RECORDING);

				//if (!pView->m_cmciobjPtr->OpenDevice("waveaudio")) {
				//	AfxMessageBox("waveaudio device is non-existant or busy", MB_OK, 0);
				//	return;
				//}
				// record for the requested duration in seconds. return is to a MM_MCINOTIFY message callback
				// function	called OnFinishRecord().

				//pView->m_cmciobjPtr->Record(this->m_hWnd, pView->manDlg.m_EditDur);
	
				SetTimer(102, 1000, NULL);	// update the real time clock every second
			}
			else {	// still no carrier, trimming duration on acq. side
				pView->manDlg.m_EditDur -= 1;	// changed by d16 because the pass could just start late

				// check if antenna pointing information needs to be updated
				if (( pLocEphem->numTracePts >= 3 ) &&
					( pLocEphem->traceTime[m_tracePtIndex] <= currentTime )) {

					// format the text for the antenna pointing angles
					pView->manDlg.m_listTimeAzEl.Format("%s        %5.1lf         %5.1lf                %5.1lf\r%s        %5.1lf         %5.1lf\r%s        %5.1lf         %5.1lf",
						pLocEphem->traceTime[m_tracePtIndex].Format("%H:%M:%S"), pLocEphem->traceAz[m_tracePtIndex], pLocEphem->traceEl[m_tracePtIndex],
						pLocEphem->maxEl,
						pLocEphem->traceTime[m_tracePtIndex+1].Format("%H:%M:%S"), pLocEphem->traceAz[m_tracePtIndex+1], pLocEphem->traceEl[m_tracePtIndex+1], 
						pLocEphem->traceTime[m_tracePtIndex+2].Format("%H:%M:%S"), pLocEphem->traceAz[m_tracePtIndex+2], pLocEphem->traceEl[m_tracePtIndex+2]); 

					if ( m_tracePtIndex < pLocEphem->numTracePts - 3 )
						m_tracePtIndex++;  // set index to next antenna data entry
				}
		
				if (pView->manDlg.m_EditDur <= 0) {
					KillTimer(101);

					// this pass is over.  However, comma, no carrier was detected. so get out of here. 
					// if the bozo is watchihg the screen give him about 5 seconds to admire
					// his little wonder and then move on to the next acquisition if there is one.

					SetTimer (103, 5000, NULL);	// just 5 seconds till showtime
				}
			}	 
		}
		pView->manDlg.UpdateData(FALSE);

	} else if (nIDEvent == 102) {

		// this timer is counting down the duration while the sound card is recording

		pView->manDlg.m_EditDur -= 1;
		pView->manDlg.m_EditRealTime = currentTime.FormatGmt("Day %j, %a %B %d %Y      %H:%M:%S");

		// check if antenna pointing information needs to be updated
		if (( pLocEphem->numTracePts >= 3 ) &&
			( pLocEphem->traceTime[m_tracePtIndex] <= currentTime )) {

			// format the text for the antenna pointing angles
			pView->manDlg.m_listTimeAzEl.Format("%s        %5.1lf         %5.1lf                %5.1lf\r%s        %5.1lf         %5.1lf\r%s        %5.1lf         %5.1lf",
				pLocEphem->traceTime[m_tracePtIndex].Format("%H:%M:%S"), pLocEphem->traceAz[m_tracePtIndex], pLocEphem->traceEl[m_tracePtIndex],
				pLocEphem->maxEl,
				pLocEphem->traceTime[m_tracePtIndex+1].Format("%H:%M:%S"), pLocEphem->traceAz[m_tracePtIndex+1], pLocEphem->traceEl[m_tracePtIndex+1], 
				pLocEphem->traceTime[m_tracePtIndex+2].Format("%H:%M:%S"), pLocEphem->traceAz[m_tracePtIndex+2], pLocEphem->traceEl[m_tracePtIndex+2]); 

			if ( m_tracePtIndex < pLocEphem->numTracePts - 3 )
				m_tracePtIndex++;  // set index to next antenna data entry
		}
		
		if (pView->manDlg.m_EditDur <= 0) {
			KillTimer(102);
			pView->manDlg.m_EditDur = 0;

			// load the proper bitmaps into the record status button. fields are: up, down, focus, disable

			pView->manDlg.m_recordButton.LoadBitmaps ("RECORDS", "RECORDS", "RECORDS", "RECORDS");
			pView->manDlg.m_recordButton.EnableWindow(FALSE);	// so the bozo can't dick with it

			// cleanup the memory mapped file
			if (!UnmapViewOfFile(pView->manDlg.m_pwavMap)) 
				TRACE("Problem unmapping the wav file.\n");
			CloseHandle(pView->manDlg.m_wavFileMapHandle);
			CloseHandle(pView->manDlg.m_wavFileHandle);

			pView->manDlg.SetMode(pView->manDlg.IDLE);

			// this pass is over. if the bozo is watchihg the screen give him about 5 seconds to admire
			// his little wonder and then move on to the next acquisition if there is one.

			SetTimer (103, 5000, NULL);	// just 5 seconds till showtime
		}

		pView->manDlg.UpdateData(FALSE);	// update the screen
	
	} else if (nIDEvent == 103) {

		// this timer separates acquisitions. if there is another, then get it started, else quit

		KillTimer(103);

		CMainFrame* p_noaaFrame = (CMainFrame *)AfxGetApp()->m_pMainWnd;
		CNoaaView* pView = (CNoaaView *)p_noaaFrame->GetActiveView();
		ASSERT_VALID(pView);
		pView->manDlg.cManDlgCancel();	// get rid of it
		pView->manDlg.m_CarrierDetect = FALSE;
	}
		
	CDialog::OnTimer(nIDEvent);
}

LONG CAcqDlg::OnFinishRecord (UINT wParam, LONG lParam)
{
	CMainFrame* p_noaaFrame = (CMainFrame *)AfxGetApp()->m_pMainWnd;
	CNoaaView* pView = (CNoaaView *)p_noaaFrame->GetActiveView();
	ASSERT_VALID(pView);

	TRACE("AcqDlg finished Recording\n");
	if (wParam == MCI_NOTIFY_ABORTED) {
		TRACE("   Aborted...\n");
	} else if (wParam == MCI_NOTIFY_SUCCESSFUL) {
		TRACE("   Success...\n");
	} else if (wParam == MCI_NOTIFY_FAILURE) {
		TRACE("   Failure...\n");
	}
	// no matter what the notification message was, save to the requested file name, the recorded
	// satellite data

	//pView->m_cmciobjPtr->Save ( pView->manDlg.m_EditFilename );

	// close the wave audio file. this is safe because the close does a safety stop first

	//pView->m_cmciobjPtr->Close();
	   
	return(0);
}

void initWaveHeader(BYTE* p)
{
	// this is a sound recorder generated header which will have the sample rate
	// overwritten later. with this header a regular media player will play the
	// generated *.wav files without complaining.

	*(p+ 0) = 0x52; *(p+ 1) = 0x49; *(p+ 2) = 0x46; *(p+ 3) = 0x46; *(p+ 4) = 0xCA;
	*(p+ 5) = 0x55; *(p+ 6) = 0x02; *(p+ 7) = 0x00; *(p+ 8) = 0x57; *(p+ 9) = 0x41;
	*(p+10) = 0x56; *(p+11) = 0x45; *(p+12) = 0x66; *(p+13) = 0x6D; *(p+14) = 0x74;
	*(p+15) = 0x20; *(p+16) = 0x12; *(p+17) = 0x00; *(p+18) = 0x00; *(p+19) = 0x00;
	*(p+20) = 0x01; *(p+21) = 0x00; *(p+22) = 0x01; *(p+23) = 0x00; *(p+24) = 0x22;
	*(p+25) = 0x56; *(p+26) = 0x00; *(p+27) = 0x00; *(p+28) = 0x22; *(p+29) = 0x56;
	*(p+30) = 0x00; *(p+31) = 0x00; *(p+32) = 0x01; *(p+33) = 0x00; *(p+34) = 0x08;
	*(p+35) = 0x00; *(p+36) = 0x00; *(p+37) = 0x00;	*(p+38) = 0x66; *(p+39) = 0x61;
	*(p+40) = 0x63; *(p+41) = 0x74; *(p+42) = 0x04; *(p+43) = 0x00; *(p+44) = 0x00;
	*(p+45) = 0x00; *(p+46) = 0x98; *(p+47) = 0x55;	*(p+48) = 0x02; *(p+49) = 0x00;
	*(p+50) = 0x64; *(p+51) = 0x61; *(p+52) = 0x74; *(p+53) = 0x61; *(p+54) = 0x98;
	*(p+55) = 0x55; *(p+56) = 0x02; *(p+57) = 0x00;
}
