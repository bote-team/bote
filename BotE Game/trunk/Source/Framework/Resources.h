#pragma once


class CBotEDoc;
class CCommandLineParameters;
class CMainFrame;
class CClientWorker;

#include "System/BuildingInfo.h"

namespace resources {

	extern CBotEDoc* pDoc;
	extern const CCommandLineParameters* pClp;
	extern CMainFrame* pMainFrame;
	extern CClientWorker* pClientWorker;
	extern const BuildingInfoArray* BuildingInfo;
}
