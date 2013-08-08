; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CConnectDlg
LastTemplate=generic CWnd
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "YATHClient.h"
LastPage=0

ClassCount=7
Class1=CYATHClientApp
Class3=CMainFrame
Class4=CAboutDlg

ResourceCount=7
Resource1=IDD_CONNECTDIALOG (English (U.S.))
Class2=CMainToolBar
Resource2=IDD_ABOUTBOX (English (U.S.))
Class5=CConnectDlg
Class6=CCommandEdit
Resource3=IDD_ABOUTBOX
Resource4=IDR_MAINFRAME
Resource5=IDR_COMMANDDIALOG (English (U.S.))
Resource6=IDR_MAINFRAME (English (U.S.))
Class7=CInterfaceView
Resource7=IDD_CONNECTDIALOG

[CLS:CYATHClientApp]
Type=0
HeaderFile=YATHClient.h
ImplementationFile=YATHClient.cpp
Filter=N
LastObject=ID_CONNECTBTTN
BaseClass=CWinApp
VirtualFilter=AC

[CLS:CMainFrame]
Type=0
HeaderFile=MainFrm.h
ImplementationFile=MainFrm.cpp
Filter=T
LastObject=CMainFrame
BaseClass=CFrameWnd
VirtualFilter=fWC




[CLS:CAboutDlg]
Type=0
HeaderFile=YATHClient.cpp
ImplementationFile=YATHClient.cpp
Filter=D
LastObject=CAboutDlg

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889

[MNU:IDR_MAINFRAME]
Type=1
Class=CMainFrame
Command1=ID_APP_EXIT
Command2=ID_EDIT_UNDO
Command3=ID_EDIT_CUT
Command4=ID_EDIT_COPY
Command5=ID_EDIT_PASTE
Command6=ID_VIEW_TOOLBAR
Command7=ID_VIEW_STATUS_BAR
Command8=ID_APP_ABOUT
CommandCount=8

[ACL:IDR_MAINFRAME]
Type=1
Class=CMainFrame
Command1=ID_EDIT_COPY
Command2=ID_EDIT_PASTE
Command3=ID_EDIT_UNDO
Command4=ID_EDIT_CUT
Command5=ID_NEXT_PANE
Command6=ID_PREV_PANE
Command7=ID_EDIT_COPY
Command8=ID_EDIT_PASTE
Command9=ID_EDIT_CUT
Command10=ID_EDIT_UNDO
CommandCount=10

[TB:IDR_MAINFRAME]
Type=1
Class=CMainFrame
Command1=ID_CONNECTBTTN
Command2=ID_CONFIGBTTN
Command3=ID_CLEAROUTPUTBTTN
CommandCount=3

[CLS:CCommandEdit]
Type=0
HeaderFile=CommandEdit.h
ImplementationFile=CommandEdit.cpp
BaseClass=CEdit
Filter=W

[CLS:CMainToolBar]
Type=0
HeaderFile=MainToolBar.h
ImplementationFile=MainToolBar.cpp
BaseClass=CToolBarCtrl
Filter=W
LastObject=CMainToolBar

[DLG:IDD_CONNECTDIALOG]
Type=1
Class=CConnectDlg
ControlCount=5
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_STATIC,static,1342308864
Control4=IDC_ADDRESSEDIT,SysIPAddress32,1342242816
Control5=IDC_PORTEDIT,edit,1350631552

[CLS:CConnectDlg]
Type=0
HeaderFile=ConnectDlg.h
ImplementationFile=ConnectDlg.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC
LastObject=IDOK

[DLG:IDR_COMMANDDIALOG (English (U.S.))]
Type=1
Class=?
ControlCount=2
Control1=IDC_COMMANDEDIT,edit,1350631552
Control2=IDC_EXECUTEBTTN,button,1342242817

[TB:IDR_MAINFRAME (English (U.S.))]
Type=1
Class=?
Command1=ID_CONNECTBTTN
Command2=ID_CONFIGBTTN
CommandCount=2

[MNU:IDR_MAINFRAME (English (U.S.))]
Type=1
Class=?
Command1=ID_APP_EXIT
Command2=ID_EDIT_UNDO
Command3=ID_EDIT_CUT
Command4=ID_EDIT_COPY
Command5=ID_EDIT_PASTE
Command6=ID_VIEW_TOOLBAR
Command7=ID_VIEW_STATUS_BAR
Command8=ID_APP_ABOUT
CommandCount=8

[ACL:IDR_MAINFRAME (English (U.S.))]
Type=1
Class=?
Command1=ID_EDIT_COPY
Command2=ID_EDIT_PASTE
Command3=ID_EDIT_UNDO
Command4=ID_EDIT_CUT
Command5=ID_NEXT_PANE
Command6=ID_PREV_PANE
Command7=ID_EDIT_COPY
Command8=ID_EDIT_PASTE
Command9=ID_EDIT_CUT
Command10=ID_EDIT_UNDO
CommandCount=10

[DLG:IDD_ABOUTBOX (English (U.S.))]
Type=1
Class=CAboutDlg
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889

[DLG:IDD_CONNECTDIALOG (English (U.S.))]
Type=1
Class=CConnectDlg
ControlCount=5
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_STATIC,static,1342308864
Control4=IDC_ADDRESSEDIT,SysIPAddress32,1342242816
Control5=IDC_PORTEDIT,edit,1350631552

[CLS:CInterfaceView]
Type=0
HeaderFile=interfaceview.h
ImplementationFile=interfaceview.cpp
BaseClass=CWnd
LastObject=ID_CONNECTBTTN
Filter=W
VirtualFilter=WC

