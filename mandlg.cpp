// mandlg.cpp : implementation file
//
#include <memory.h>

#include "stdafx.h"
//#include "noaa.h"
#include "mandlg.h"
#include "resource.h"
//#include "mainfrm.h"
//#include "noaaview.h"
//#include "noaadoc.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CManDlg dialog


CManDlg::CManDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CManDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CManDlg)
	m_EditAcqLocal = _T("");
	m_EditDur = 0;
	m_EditGMT = _T("");
	m_EditFilename = _T("");
	m_EditRealTime = _T("");
	m_EditSecToAcq = 0;
	m_pWave = NULL;
	m_iNumDevs = 0;
	m_pDevCaps = NULL;
	m_iMode = IDLE;
	m_CarrierDetect = FALSE;
	m_listTimeAzEl = _T("");
	//}}AFX_DATA_INIT
	
	m_NotifyObj.Attach(this);
}

CManDlg::~CManDlg()
{
    if (m_pDevCaps) delete m_pDevCaps;
}

void CManDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CManDlg)
	DDX_Control(pDX, IDC_FRAME_VU, m_wndVU);
	DDX_Control(pDX, IDC_EDIT_SEC_TO_ACQ, m_EditSecToAcqControl);
	DDX_Control(pDX, IDC_EDIT_DUR, m_EditDurControl);
	DDX_Text(pDX, IDC_EDIT_ACQ_LOCAL, m_EditAcqLocal);
	DDV_MaxChars(pDX, m_EditAcqLocal, 100);
	DDX_Text(pDX, IDC_EDIT_DUR, m_EditDur);
	DDV_MinMaxLong(pDX, m_EditDur, -1, 9999);
	DDX_Text(pDX, IDC_EDIT_ACQ_GMT, m_EditGMT);
	DDV_MaxChars(pDX, m_EditGMT, 100);
	DDX_Text(pDX, IDC_EDIT_FILENAME, m_EditFilename);
	DDV_MaxChars(pDX, m_EditFilename, 100);
	DDX_Text(pDX, IDC_EDIT_REALTIME, m_EditRealTime);
	DDV_MaxChars(pDX, m_EditRealTime, 100);
	DDX_Text(pDX, IDC_EDIT_SEC_TO_ACQ, m_EditSecToAcq);
	DDV_MinMaxLong(pDX, m_EditSecToAcq, -1000, 99999999);
	DDX_Text(pDX, IDC_LIST_TIME_AZ_EL, m_listTimeAzEl);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CManDlg, CDialog)
	//{{AFX_MSG_MAP(CManDlg)
	ON_EN_UPDATE(IDC_EDIT_SEC_TO_ACQ, OnUpdateEditSecToAcq)
	ON_EN_SETFOCUS(IDC_EDIT_SEC_TO_ACQ, OnSetfocusEditSecToAcq)
	ON_EN_SETFOCUS(IDC_EDIT_DUR, OnSetfocusEditDur)
	ON_EN_KILLFOCUS(IDC_EDIT_DUR, OnKillfocusEditDur)
	ON_EN_KILLFOCUS(IDC_EDIT_SEC_TO_ACQ, OnKillfocusEditSecToAcq)
	ON_BN_CLICKED(IDCANCELTHIS, OnCancelthis)
	ON_BN_CLICKED(IDC_RECORD, OnStopRecord)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CManDlg message handlers

BOOL CManDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// the bitmaps are reloaded below. this is mainly to size the button

	m_recordButton.AutoLoad(IDC_RECORD, this);

	// load in the proper bitmaps in the record status button and make sure it is disabled
	// the bitmap fields are: Up, Down, focus, disable

	m_recordButton.EnableWindow (TRUE);	// must be enabled to realize the bitmaps
	m_recordButton.LoadBitmaps ("RECORDU", "RECORDU", "RECORDU", "RECORDU");
	m_recordButton.EnableWindow (FALSE);	// so the user can't screw with the button
	
	// install the VU meter in its frame
	CRect rcVU;
   m_wndVU.GetWindowRect(&rcVU);
   ScreenToClient(&rcVU);
   m_VU.Create("VU",
               WS_CHILD | WS_VISIBLE,
               rcVU,
               this,
               1);
   m_VU.SetValue(0, 0);
   
   // Get the VU meter going
   SetMode(SAMPLING);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CManDlg::SetMode(MODE m)
{
    if (m == m_iMode)
    	return; // no change

    // Stop whatever we are doing now
    if (m_pWave)
    	m_pWave->Stop();
    switch (m_iMode) {
    	case SAMPLING:
    	case RECORDING:
      	m_InDev.Reset();
      	m_InDev.Close();
      	break;
    	case PLAYING:
        // nothing else to do
      	break;
    }
    m_iMode = IDLE;

    // Start up in the new mode
    switch (m) {
    case SAMPLING:
    case RECORDING:
        {
		  GetDevice();	// get the 1st wave device hanging around

        // get the device id we saved in the manufacture's id slot
        UINT uiID = m_pDevCaps[0].wMid;

        // Open the device
        PCMWAVEFORMAT fmt;
		  fmt.wf.wFormatTag = WAVE_FORMAT_PCM;
        fmt.wf.nChannels = 1;
        fmt.wf.nSamplesPerSec = 8000;
		  fmt.wf.nAvgBytesPerSec = 8000;
		  fmt.wf.nBlockAlign = 1;
        fmt.wBitsPerSample = 8;
        
        if (!m_InDev.Open(uiID, &fmt))	{
        		return;
		  }
        if (m == SAMPLING) {
            m_SampleWave.DeleteAll();
            m_SampleWave.Create(&fmt);
            m_SampleWave.Record(&m_InDev,
                                1024,  
                                &m_NotifyObj);
        } else if (m == RECORDING) {
            if (!m_pWave)
            	m_pWave = new CWave;
            ASSERT(m_pWave);
            m_pWave->Create(&fmt);
            m_pWave->Record(&m_InDev,
                            2048,  
                            &m_NotifyObj);
        }
        } break;

    case PLAYING:
        if (m_pWave) m_pWave->Play(NULL, &m_NotifyObj);
        break;
    }
    m_iMode = m;
}

void CManDlg::GetDevice()
{
	m_iNumDevs = waveInGetNumDevs();
   if (m_iNumDevs == 0) {
       AfxMessageBox("There are no suitable input devices");
       EndDialog(IDCANCEL);
       return;
   }
   if (m_pDevCaps)
   	delete m_pDevCaps;
   m_pDevCaps = new WAVEINCAPS[m_iNumDevs];
   for (int i=0; i<m_iNumDevs; i++) {
       waveInGetDevCaps(i,
                        &m_pDevCaps[i],
                        sizeof(WAVEINCAPS));
       // save the device id in the manufacturer field
       m_pDevCaps[i].wMid = i;
	}
}

void CManDlg::NewData(CWave *pWave, CWaveBlock* pBlock)
{
	// this is the "callback" routine for the CWaveNotifyObj

	static BYTE* p_file = NULL;
	ASSERT(pWave);
	ASSERT(pBlock);

	// Update the VU meter from the samples

	// get the format of the data
	PCMWAVEFORMAT* pwf = (PCMWAVEFORMAT*) pWave->GetFormat();
	ASSERT(pwf->wf.wFormatTag == WAVE_FORMAT_PCM);
	int iCount = pBlock->GetNumSamples();
	
	BYTE bMax = 0;
	int noisePk, carrierPk;

	CMainFrame* p_noaaFrame = (CMainFrame *)AfxGetApp()->m_pMainWnd;
	CNoaaDoc* pDoc = (CNoaaDoc *)p_noaaFrame->GetActiveDocument();
	ASSERT_VALID(pDoc);	// what a long way to go to get the damn document!!

	BYTE* pData = (BYTE*)pBlock->GetSamples();

	iCount = pBlock->GetNumSamples();
	if (m_iMode == SAMPLING) {
		BOOL result = pDoc->DetectCarrier(pData, iCount, 2400.0, noisePk, carrierPk);
		m_VU.SetValue( noisePk << 8, carrierPk << 8);
		if (result) {
			TRACE("Detected carrier\n");
			m_CarrierDetect = TRUE;	// which will force the record to begin
		}
		//m_CarrierDetect = TRUE;	// just for testing we don't need the 2400 Hz to record
	} 

    // If we are just sampling, nuke the wave blocks
    if (m_iMode != RECORDING) {
        pWave->GetBlockList()->FreeAll();
		  p_file = NULL;
    }	else {
		iCount = pBlock->GetNumSamples();
		if (p_file == NULL)
			p_file = m_pwavMap + 176;	// because the header is now 176 bytes, d16
		memcpy(p_file, pData, iCount);
		p_file += iCount;
		
		while (iCount--) {
			if (*pData > bMax)
				bMax = *pData;
			pData++;
		}
		// get the max positive amplitude (range 0 --> 127)
		if (bMax < 128) {
			bMax = 0;
		} else {
			bMax -= 128;
		}
		// scale the amplitude by 256 for the VU meter (max = 32768)
		m_VU.SetValue(bMax << 8, bMax << 8);
		pWave->GetBlockList()->FreeAll();	// now nuke the wav blocks	 
	 }
}

void CManDlg::EndPlayback(CWave *pWave)
{
    ASSERT(pWave);
    SetMode(SAMPLING);
}


void CManDlg::OnUpdateEditSecToAcq() 
{
	UpdateData (TRUE);
	ctSpan = (CTimeSpan)m_EditSecToAcq;
	cAcqLocal = cNowLocal + ctSpan;
	m_EditAcqLocal = cAcqLocal.Format("Day %j, %a %B %d %Y      %H:%M:%S");
	m_EditGMT = cAcqLocal.FormatGmt("Day %j, %a %B %d %Y      %H:%M:%S");
	UpdateData (FALSE);
}

void CManDlg::OnSetfocusEditSecToAcq() 
{
	// if the seconds to acqusition is > zero, kill the acqdlg timer(101)

	UpdateData(TRUE);
	if (m_EditSecToAcq != 0) {
		
		// in order to kill the acqdlg timer, get a handle to that window

		CMainFrame* p_noaaFrame = (CMainFrame *)AfxGetApp()->m_pMainWnd;
		CNoaaView* pView = (CNoaaView *)p_noaaFrame->GetActiveView();
		ASSERT_VALID(pView);

		pView->acqDlg.KillTimer(101);
	}
}

void CManDlg::OnSetfocusEditDur() 
{
		// if the acquisition > 0 just edit the duration, but if the pass duration countdown
		// timer is running, then kill that.
		
	UpdateData (TRUE);
	if (m_EditSecToAcq == 0  &&  m_EditDur != 0) {
	
		// in order to kill the acqdlg timer, get a handle to that window

		CMainFrame* p_noaaFrame = (CMainFrame *)AfxGetApp()->m_pMainWnd;
		CNoaaView* pView = (CNoaaView *)p_noaaFrame->GetActiveView();
		ASSERT_VALID(pView);
		pView->acqDlg.KillTimer(102);
	}
}

void CManDlg::OnKillfocusEditDur() 
{
	UpdateData (TRUE);

	// if the acquisition counted to zero, but the duration is non-zero, then restart the count

	if (m_EditSecToAcq == 0  &&  m_EditDur != 0) {

		// in order to set the acqdlg timer, get a handle to that window

		CMainFrame* p_noaaFrame = (CMainFrame *)AfxGetApp()->m_pMainWnd;
		CNoaaView* pView = (CNoaaView *)p_noaaFrame->GetActiveView();
		ASSERT_VALID(pView);

		pView->acqDlg.SetTimer(102, 1000, NULL);
	}
}

void CManDlg::OnKillfocusEditSecToAcq() 
{
	UpdateData (TRUE);
	if (m_EditSecToAcq != 0) {

		// in order to set the acqdlg timer, get a handle to that window

		CMainFrame* p_noaaFrame = (CMainFrame *)AfxGetApp()->m_pMainWnd;
		CNoaaView* pView = (CNoaaView *)p_noaaFrame->GetActiveView();
		ASSERT_VALID(pView);

		pView->acqDlg.SetTimer(101, 1000, NULL);
	}
}

void CManDlg::OnCancelthis() 
{
	// this is a public function whose purpose is to access a protected member function

	CDialog::OnOK();	
}

void CManDlg::OnCancel() 
{
	// this is the cancel all passes button. set a variable in the parent view class
		
	CMainFrame* p_noaaFrame = (CMainFrame *)AfxGetApp()->m_pMainWnd;
	CNoaaView* pView = (CNoaaView *)p_noaaFrame->GetActiveView();
	ASSERT_VALID(pView);
	pView->cancelPass = TRUE;
				
	CDialog::OnCancel();
}

void CManDlg::OnStopRecord() 
{
	// this button is depressed when the user wishes to prematurely end the acquisition	

	TRACE("manDlg OnStopRecord():\n");		
	CMainFrame* p_noaaFrame = (CMainFrame *)AfxGetApp()->m_pMainWnd;
	CNoaaView* pView = (CNoaaView *)p_noaaFrame->GetActiveView();
	ASSERT_VALID(pView);

	// just by setting the duration variable to zero, the acqDlg routine will think the pass
	// is over and terminate normally

	pView->manDlg.m_EditDur = 0;	// reached fade now
	pView->manDlg.UpdateData(FALSE);	// write it back into the dialog's storage
	//pView->m_cmciobjPtr->Stop();	// the MM_MCINOTIFY callback will do the save and close
}


///////////////////////////////////////////////////////////////
// CRecDlgNotifyObj

CRecDlgNotifyObj::CRecDlgNotifyObj()
{
    m_pDlg = NULL;
}

CRecDlgNotifyObj::~CRecDlgNotifyObj()
{
}

void CRecDlgNotifyObj::NewData(CWave *pWave, CWaveBlock* pBlock)
{
    ASSERT(m_pDlg);
    m_pDlg->NewData(pWave, pBlock);
}

void CRecDlgNotifyObj::EndPlayback(CWave *pWave)
{
    ASSERT(m_pDlg);
    m_pDlg->EndPlayback(pWave);
}
