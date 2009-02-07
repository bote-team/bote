; CLW-Datei enthält Informationen für den MFC-Klassen-Assistenten

[General Info]
Version=1
LastClass=CBotf2View
LastTemplate=generic CWnd
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "botf2.h"
LastPage=0

ClassCount=10
Class1=CBotf2App
Class2=CBotf2Doc
Class3=CBotf2View
Class4=CMainFrame

ResourceCount=3
Resource1=IDR_MAINFRAME
Class5=CAboutDlg
Class6=View2
Class7=View3
Class8=View4
Resource2=IDD_ABOUTBOX
Class9=CSTARTDIALOG
Class10=CMySplitterWnd
Resource3=IDD_GAMESTART

[CLS:CBotf2App]
Type=0
HeaderFile=botf2.h
ImplementationFile=botf2.cpp
Filter=N
LastObject=CBotf2App

[CLS:CBotf2Doc]
Type=0
HeaderFile=botf2Doc.h
ImplementationFile=botf2Doc.cpp
Filter=N
LastObject=CBotf2Doc

[CLS:CBotf2View]
Type=0
HeaderFile=botf2View.h
ImplementationFile=botf2View.cpp
Filter=C
LastObject=CBotf2View
BaseClass=CScrollView
VirtualFilter=VWC


[CLS:CMainFrame]
Type=0
HeaderFile=MainFrm.h
ImplementationFile=MainFrm.cpp
Filter=T
BaseClass=CFrameWnd
VirtualFilter=fWC
LastObject=CMainFrame




[CLS:CAboutDlg]
Type=0
HeaderFile=botf2.cpp
ImplementationFile=botf2.cpp
Filter=D
LastObject=CAboutDlg

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg
ControlCount=5
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889
Control5=IDC_STATIC,static,1342308352

[MNU:IDR_MAINFRAME]
Type=1
Class=CMainFrame
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_SAVE
Command4=ID_OPTIONEN
Command5=ID_APP_EXIT
Command6=ID_APP_ABOUT
CommandCount=6

[ACL:IDR_MAINFRAME]
Type=1
Class=CMainFrame
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_SAVE
Command4=ID_FILE_PRINT
Command5=ID_EDIT_UNDO
Command6=ID_EDIT_CUT
Command7=ID_EDIT_COPY
Command8=ID_EDIT_PASTE
Command9=ID_EDIT_UNDO
Command10=ID_EDIT_CUT
Command11=ID_EDIT_COPY
Command12=ID_EDIT_PASTE
Command13=ID_NEXT_PANE
Command14=ID_PREV_PANE
CommandCount=14

[TB:IDR_MAINFRAME]
Type=1
Class=?
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_SAVE
Command4=ID_BUTTON32771
Command5=ID_EDIT_CUT
Command6=ID_EDIT_COPY
Command7=ID_EDIT_PASTE
Command8=ID_FILE_PRINT
Command9=ID_APP_ABOUT
CommandCount=9

[DLG:IDR_MAINFRAME]
Type=1
Class=?
ControlCount=0

[CLS:View2]
Type=0
HeaderFile=View2.h
ImplementationFile=View2.cpp
BaseClass=CView
Filter=C
LastObject=ID_APP_ABOUT
VirtualFilter=VWC

[CLS:View3]
Type=0
HeaderFile=View3.h
ImplementationFile=View3.cpp
BaseClass=CView
Filter=C
LastObject=ID_APP_ABOUT
VirtualFilter=VWC

[CLS:View4]
Type=0
HeaderFile=View4.h
ImplementationFile=View4.cpp
BaseClass=CView
Filter=C
LastObject=ID_APP_ABOUT
VirtualFilter=VWC

[DLG:IDD_GAMESTART]
Type=1
Class=CSTARTDIALOG
ControlCount=14
Control1=IDC_CHOOSERACE,button,1342308359
Control2=IDC_FEDERATION,button,1342312457
Control3=IDC_FERENGI,button,1342181385
Control4=IDC_KLINGON,button,1342181385
Control5=IDC_ROMULAN,button,1342181385
Control6=IDC_CARDASSIAN,button,1342181385
Control7=IDC_DOMINION,button,1342181385
Control8=IDOK,button,1342242817
Control9=IDC_STATIC,static,1342308353
Control10=IDC_STATIC,static,1342308352
Control11=IDC_STATIC,static,1342308352
Control12=IDC_STATIC,static,1342308352
Control13=IDC_STATIC,static,1342308352
Control14=IDC_STATIC,static,1342308352

[CLS:CSTARTDIALOG]
Type=0
HeaderFile=STARTDIALOG.h
ImplementationFile=STARTDIALOG.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC
LastObject=CSTARTDIALOG

[CLS:CMySplitterWnd]
Type=0
HeaderFile=MySplitterWnd.h
ImplementationFile=MySplitterWnd.cpp
BaseClass=CSplitterWnd
Filter=W
VirtualFilter=pWC
LastObject=CMySplitterWnd

