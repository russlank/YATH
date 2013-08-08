// ConnectDlg.cpp : implementation file
//

#include "stdafx.h"
#include "YATHClient.h"
#include "ConnectDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CConnectDlg dialog


CConnectDlg::CConnectDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CConnectDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CConnectDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CConnectDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CConnectDlg)
	DDX_Control(pDX, IDC_ADDRESSEDIT, m_AddressEdit);
	DDX_Control(pDX, IDC_PORTEDIT, m_PortNoEdit);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CConnectDlg, CDialog)
	//{{AFX_MSG_MAP(CConnectDlg)
	ON_WM_SHOWWINDOW()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CConnectDlg message handlers

void CConnectDlg::OnOK() 
{
	// TODO: Add extra validation here

	CString PortStr;
	char *ScanEnd;

	m_AddressEdit.GetAddress( m_Address);
	m_PortNoEdit.GetWindowText( PortStr);
	m_Port = strtol( PortStr, &ScanEnd, 10);

	m_Address = m_Address;
	m_Port = m_Port;
	
	CDialog::OnOK();
}

void CConnectDlg::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CDialog::OnShowWindow(bShow, nStatus);
	
	// TODO: Add your message handler code here

#ifdef _DEBUG
	m_Address = 2130706433;
	m_AddressEdit.SetAddress( m_Address);
	m_PortNoEdit.SetWindowText( "4000");
#endif
	
}
