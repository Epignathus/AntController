// mandlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CManDlg dialog

#ifndef MANDLG_H
#define MANDLG_H

#include "resource.h"
#include "usrclass.h"

class CManDlg;	// just a forward reference

class CRecDlgNotifyObj : public CWaveNotifyObj
{
public:
    CRecDlgNotifyObj();
    ~CRecDlgNotifyObj();
    void Attach(CManDlg* pDlg)
        {m_pDlg = pDlg;}


    virtual void NewData(CWave *pWave,
                         CWaveBlock* pBlock);
    virtual void EndPlayback(CWave *pWave);
    CManDlg* m_pDlg;
};


class CManDlg : public CDialog
{
// Construction
public:
	CManDlg(CWnd* pParent = NULL);   // standard constructor
	~CManDlg();
   void NewData(CWave *pWave, CWaveBlock* pBlock);
   void EndPlayback(CWave *pWave);

	CTime cNowLocal, cNowGMT;
	CTime cAcqLocal, cAcqGMT;
	CString sNowLocal, sNowGMT;
	CString sAcqLocal, sAcqGMT;
	CTimeSpan ctSpan;
	CBitmapButton m_recordButton;
	BOOL m_CarrierDetect;
	HANDLE m_wavFileHandle;
	HANDLE m_wavFileMapHandle;
	BYTE* m_pwavMap;

	void cManDlgCancel() {OnCancelthis();}

// Dialog Data
	//{{AFX_DATA(CManDlg)
	enum { IDD = IDD_DIALOG_REC_PASS };
	CStatic	m_wndVU;
	CEdit	m_EditSecToAcqControl;
	CEdit	m_EditDurControl;
	CString	m_EditAcqLocal;
	long	m_EditDur;
	CString	m_EditGMT;
	CString	m_EditFilename;
	CString	m_EditRealTime;
	long	m_EditSecToAcq;
	CString	m_listTimeAzEl;
	//}}AFX_DATA
   
   CWave* m_pWave;
	enum MODE {
      IDLE,
      SAMPLING,
      PLAYING,
      RECORDING
   };
	void SetMode(MODE m);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CManDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
private:
	CVUMeter m_VU;          // VU Meter
  CWaveInDevice m_InDev;  // Wave input device   
	CRecDlgNotifyObj m_NotifyObj;

   int m_iMode;            // current mode
   int m_iNumDevs;         // number of input devices
   WAVEINCAPS* m_pDevCaps; // ptr to caps list
	CWave m_SampleWave;     // sample wave
	void GetDevice();
protected:

	// Generated message map functions
	//{{AFX_MSG(CManDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnUpdateEditSecToAcq();
	afx_msg void OnSetfocusEditSecToAcq();
	afx_msg void OnSetfocusEditDur();
	afx_msg void OnKillfocusEditDur();
	afx_msg void OnKillfocusEditSecToAcq();
	afx_msg void OnCancelthis();
	virtual void OnCancel();
	afx_msg void OnStopRecord();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

#endif
