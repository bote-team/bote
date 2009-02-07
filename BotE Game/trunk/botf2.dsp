# Microsoft Developer Studio Project File - Name="botf2" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** NICHT BEARBEITEN **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=botf2 - Win32 Debug
!MESSAGE Dies ist kein gültiges Makefile. Zum Erstellen dieses Projekts mit NMAKE
!MESSAGE verwenden Sie den Befehl "Makefile exportieren" und führen Sie den Befehl
!MESSAGE 
!MESSAGE NMAKE /f "botf2.mak".
!MESSAGE 
!MESSAGE Sie können beim Ausführen von NMAKE eine Konfiguration angeben
!MESSAGE durch Definieren des Makros CFG in der Befehlszeile. Zum Beispiel:
!MESSAGE 
!MESSAGE NMAKE /f "botf2.mak" CFG="botf2 - Win32 Debug"
!MESSAGE 
!MESSAGE Für die Konfiguration stehen zur Auswahl:
!MESSAGE 
!MESSAGE "botf2 - Win32 Release" (basierend auf  "Win32 (x86) Application")
!MESSAGE "botf2 - Win32 Debug" (basierend auf  "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "botf2 - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x407 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 winmm.lib /nologo /subsystem:windows /machine:I386
# SUBTRACT LINK32 /profile /incremental:yes /debug

!ELSEIF  "$(CFG)" == "botf2 - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x407 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# SUBTRACT BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 winmm.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "botf2 - Win32 Release"
# Name "botf2 - Win32 Debug"
# Begin Group "Quellcodedateien"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\BeamWeapons.cpp
# End Source File
# Begin Source File

SOURCE=.\botf2.cpp
# End Source File
# Begin Source File

SOURCE=.\botf2.rc
# End Source File
# Begin Source File

SOURCE=.\botf2Doc.cpp
# End Source File
# Begin Source File

SOURCE=.\botf2View.cpp
# End Source File
# Begin Source File

SOURCE=.\Building.cpp
# End Source File
# Begin Source File

SOURCE=.\BuildingInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\DBScrollView.cpp
# End Source File
# Begin Source File

SOURCE=.\Diplomacy.cpp
# End Source File
# Begin Source File

SOURCE=.\Empire.cpp
# End Source File
# Begin Source File

SOURCE=.\EnBitmap.cpp
# End Source File
# Begin Source File

SOURCE=.\Fleet.cpp
# End Source File
# Begin Source File

SOURCE=.\GenDiploMessage.cpp
# End Source File
# Begin Source File

SOURCE=.\GenSectorName.cpp
# End Source File
# Begin Source File

SOURCE=.\GenShipName.cpp
# End Source File
# Begin Source File

SOURCE=.\Hull.cpp
# End Source File
# Begin Source File

SOURCE=.\MainFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\MajorRace.cpp
# End Source File
# Begin Source File

SOURCE=.\Message.cpp
# End Source File
# Begin Source File

SOURCE=.\MinorRace.cpp
# End Source File
# Begin Source File

SOURCE=.\MySplitterWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\Planet.cpp
# End Source File
# Begin Source File

SOURCE=.\Research.cpp
# End Source File
# Begin Source File

SOURCE=.\ResearchComplex.cpp
# End Source File
# Begin Source File

SOURCE=.\ResearchInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\Sector.cpp
# End Source File
# Begin Source File

SOURCE=.\Shield.cpp
# End Source File
# Begin Source File

SOURCE=.\Ship.cpp
# End Source File
# Begin Source File

SOURCE=.\ShipInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\STARTDIALOG.cpp
# End Source File
# Begin Source File

SOURCE=.\Statistics.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\System.cpp
# End Source File
# Begin Source File

SOURCE=.\TorpedoWeapons.cpp
# End Source File
# Begin Source File

SOURCE=.\View2.cpp
# End Source File
# Begin Source File

SOURCE=.\View3.cpp
# End Source File
# Begin Source File

SOURCE=.\View4.cpp
# End Source File
# Begin Source File

SOURCE=.\Worker.cpp
# End Source File
# End Group
# Begin Group "Header-Dateien"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\BeamWeapons.h
# End Source File
# Begin Source File

SOURCE=.\botf2.h
# End Source File
# Begin Source File

SOURCE=.\botf2Doc.h
# End Source File
# Begin Source File

SOURCE=.\botf2View.h
# End Source File
# Begin Source File

SOURCE=.\Building.h
# End Source File
# Begin Source File

SOURCE=.\BuildingInfo.h
# End Source File
# Begin Source File

SOURCE=.\DBScrollView.h
# End Source File
# Begin Source File

SOURCE=.\Diplomacy.h
# End Source File
# Begin Source File

SOURCE=.\Empire.h
# End Source File
# Begin Source File

SOURCE=.\EnBitmap.h
# End Source File
# Begin Source File

SOURCE=.\Fleet.h
# End Source File
# Begin Source File

SOURCE=.\GenDiploMessage.h
# End Source File
# Begin Source File

SOURCE=.\GenSectorName.h
# End Source File
# Begin Source File

SOURCE=.\GenShipName.h
# End Source File
# Begin Source File

SOURCE=.\Hull.h
# End Source File
# Begin Source File

SOURCE=.\MainFrm.h
# End Source File
# Begin Source File

SOURCE=.\MajorRace.h
# End Source File
# Begin Source File

SOURCE=.\memdc.h
# End Source File
# Begin Source File

SOURCE=.\Message.h
# End Source File
# Begin Source File

SOURCE=.\MinorRace.h
# End Source File
# Begin Source File

SOURCE=.\MySplitterWnd.h
# End Source File
# Begin Source File

SOURCE=.\Options.h
# End Source File
# Begin Source File

SOURCE=.\Planet.h
# End Source File
# Begin Source File

SOURCE=.\Research.h
# End Source File
# Begin Source File

SOURCE=.\ResearchComplex.h
# End Source File
# Begin Source File

SOURCE=.\ResearchInfo.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\Sector.h
# End Source File
# Begin Source File

SOURCE=.\Shield.h
# End Source File
# Begin Source File

SOURCE=.\Ship.h
# End Source File
# Begin Source File

SOURCE=.\ShipInfo.h
# End Source File
# Begin Source File

SOURCE=.\STARTDIALOG.h
# End Source File
# Begin Source File

SOURCE=.\Statistics.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\System.h
# End Source File
# Begin Source File

SOURCE=.\TorpedoWeapons.h
# End Source File
# Begin Source File

SOURCE=.\View2.h
# End Source File
# Begin Source File

SOURCE=.\View3.h
# End Source File
# Begin Source File

SOURCE=.\View4.h
# End Source File
# Begin Source File

SOURCE=.\Worker.h
# End Source File
# End Group
# Begin Group "Ressourcendateien"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\botf2.ico
# End Source File
# Begin Source File

SOURCE=.\res\botf2.rc2
# End Source File
# Begin Source File

SOURCE=.\res\botf2Doc.ico
# End Source File
# Begin Source File

SOURCE=".\res\Re-exposure of MapStar03.bmp"
# End Source File
# Begin Source File

SOURCE=".\res\TNG-09.ICO"
# End Source File
# Begin Source File

SOURCE=.\res\Toolbar.bmp
# End Source File
# Begin Source File

SOURCE=.\res\TREKTH~2.ICO
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
