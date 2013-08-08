// YATHClient.h : main header file for the YATHCLIENT application
//

#if !defined(AFX_YATHCLIENT_H__FBA50B34_184A_42B7_94A5_F2D87EF35F7A__INCLUDED_)
#define AFX_YATHCLIENT_H__FBA50B34_184A_42B7_94A5_F2D87EF35F7A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CYATHClientApp:
// See YATHClient.cpp for the implementation of this class
//

class CYATHClientApp : public CWinApp
{
public:
	CYATHClientApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CYATHClientApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

public:
	//{{AFX_MSG(CYATHClientApp)
	afx_msg void OnAppAbout();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_YATHCLIENT_H__FBA50B34_184A_42B7_94A5_F2D87EF35F7A__INCLUDED_)
