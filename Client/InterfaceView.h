#if !defined(AFX_SERVEROUTPUTVIEW_H__7F6D33B6_133D_4C7A_B527_F8CBE7B0C523__INCLUDED_)
#define AFX_SERVEROUTPUTVIEW_H__7F6D33B6_133D_4C7A_B527_F8CBE7B0C523__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ServerOutputView.h : header file
//

#include "../YATH/clientclasses.h"

/////////////////////////////////////////////////////////////////////////////
// CInterfaceView window

class CMainFrame;

class CInterfaceView : public CWnd, public yathCUserInterface
{
// Construction
public:
	CInterfaceView();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInterfaceView)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CInterfaceView();
	virtual void ClearOutput();
	virtual void WriteOutput( const char *Str);
	virtual void WriteOutputToCatch( const char *Str);
	virtual void FlushOutputCatch();
	void SetMainFrame( CMainFrame *MainFrame);
	
	// Generated message map functions
protected:
	// CRichEditCtrl m_Edit;
	CFont m_OutputEditFont;
	CFont m_CommandEditFont;
	CEdit m_OutputEdit;
	CEdit m_CommandEdit;
	CMainFrame *m_MainFrame;
	CString m_OutputBuffer;

	void SubmitCommand();
	
	//{{AFX_MSG(CInterfaceView)
	afx_msg void OnPaint();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SERVEROUTPUTVIEW_H__7F6D33B6_133D_4C7A_B527_F8CBE7B0C523__INCLUDED_)
