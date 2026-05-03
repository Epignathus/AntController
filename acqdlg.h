// acqdlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAcqDlg dialog

#ifndef ACQDLG_H
#define ACQDLG_H

#include "cephem.h"

class CAcqDlg : public CDialog
{
// Construction
public:
	CAcqDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CAcqDlg)
	enum { IDD = IDD_ACQ_DIALOG };
	CListBox	m_EditAcq;
	//}}AFX_DATA

	int	m_tracePtIndex;  // used to keep track of which CEphem trace points are listed
	CEphemeris* pLocEphem;  // used to keep current/next pass information

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAcqDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAcqDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg LONG OnFinishRecord(UINT wParam, LONG lParam);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif
