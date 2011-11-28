// CombatSimulatorView.cpp : Implementierung der Klasse CCombatSimulatorView
//

#include "stdafx.h"
#include <irrlicht.h>
#include "BeamSceneNode.h"
#include "EventReceiver.h"

using namespace irr;


using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

#include "CombatSimulator.h"

#include "CombatSimulatorDoc.h"
#include "CombatSimulatorView.h"

//#include "memdc.h"
#include <afxglobals.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif



#ifdef _MSC_VER
#pragma comment(lib, "Irrlicht.lib")
#endif

// CCombatSimulatorView

IMPLEMENT_DYNCREATE(CCombatSimulatorView, CView)

BEGIN_MESSAGE_MAP(CCombatSimulatorView, CView)
	ON_WM_ERASEBKGND()
	ON_WM_MOUSEMOVE()
	ON_WM_MBUTTONDOWN()
ON_WM_MOUSEWHEEL()
END_MESSAGE_MAP()

// CCombatSimulatorView-Erstellung/Zerstörung

CCombatSimulatorView::CCombatSimulatorView()
{
	// TODO: Hier Code zur Konstruktion einfügen
	m_iCameraX=0;
	m_iCameraY=0;

}

CCombatSimulatorView::~CCombatSimulatorView()
{ 
	p_timer->SuspendThread();
	delete p_timer;
	p_smgr->clear();
	
	delete p_smgr;
	p_device->closeDevice();

}

BOOL CCombatSimulatorView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Ändern Sie hier die Fensterklasse oder die Darstellung, indem Sie
	//  CREATESTRUCT cs modifizieren.
		BOOL value = CView::PreCreateWindow(cs);


  return value;
}

/** Diese Funktion wird zum Zeichnen der 3D Grafik missbraucht, das funktioniert mehr schlecht als recht
* Alle Zeichenrelevanten dinge sind in die Klasse CombatShip verlagert worden, wo sie ja auch berechnet werden.
* Hier wird nur noch rudimänter berechnet was notwendig ist. Wenn die MFC weg ist soll auch das weg.
* @todo Aufräumen der vielen MemoryLeaks 
*/
void CCombatSimulatorView::OnDraw(CDC* pDC)
{
	//wenn der Timer steht lass in laufen
	if (!(p_timer->go))
	{
		p_timer->go = true;
		p_timer->ResumeThread() ;
	}
	//Ab hier geht nur noch ein Thread sonst gibts kuddelmuttel 
	m_lock.Lock();
	CCombatSimulatorDoc* pDoc = GetDocument();

	if (!pDoc)
		return;

  p_device->setWindowCaption(L"Hello World! - Irrlicht Engine Demo");

	bool bExit = true;
	//ist der Kampf noch nicht beendet Zeichne 3D
	if (pDoc->repeat == 0)
	{

		if (pDoc->combat.m_bReady)
		{
			bExit = false;
			
			pDoc->combat.CalculateCombat(winner);
			//Ticks zählen
			core::stringw s ;
			s += L"TICK: "; 
			s += pDoc->combat.m_iTime;
			text1->setText(s.c_str());
			//Torpedos fliegen lassen
			for (std::list<CTorpedo*>::const_iterator it = pDoc->combat.m_CT.begin(); it != pDoc->combat.m_CT.end(); ++it)
			{

				(*it)->GetNode()->addAnimator(p_smgr->createFlyStraightAnimator(p_node->getAbsolutePosition(),vector3df((*it)->m_KO.x,(*it)->m_KO.y,(*it)->m_KO.z),false,false));

			}
			//Zähler hoch
				counter++;
				if (counter%200 == 0)
					xy_ebene = !xy_ebene;
				int time = 0;

		    //hier wird gezeichnet
				p_driver->beginScene(true, true, SColor(255,100,101,140));
				ICameraSceneNode* c = p_smgr->getActiveCamera();
			//eventuell noch mal der Kamerrazoom verändert
				c->setPosition(vector3df(0,0,m_iCameraZoom));

				c->setTarget(vector3df(m_iCameraX,m_iCameraY,0));
			//Ab gehts die Szene wird gemalt
				p_smgr->drawAll();
				p_guienv->drawAll();

				p_driver->endScene();
								
            //verschossene Laser werden entfernt
			for (int q = 0; q < pDoc->combat.m_CS.GetSize(); q++)
			{
				pDoc->combat.m_CS.GetAt(q)->GetBulletParent()->removeAll();
			}
			//ab jetzt können wieder mehr Threads ran
			m_lock.Unlock();
			return;
		}
		// ************** TEST DES KAMPFMODUSES ZEICHNEN ist hier zu Ende **************

}

	//		return;
		if (!pDoc->combat.m_bReady)
		{
			bExit = true;
			for (std::map<CString, BYTE>::const_iterator it = winner.begin(); it != winner.end(); ++it)
				if (it->second == 1)
					AfxMessageBox("Race " + it->first + " wins the combat");

			// Das Log-File über die vernichteten Schiffe anlegen
			CString fileName="Combat.log";														// Name des zu Öffnenden Files 
			CStdioFile file;																	// Varibale vom Typ CStdioFile
			if (file.Open(fileName, CFile::modeCreate | CFile::modeWrite | CFile::typeText))	// Datei wird geöffnet
			{
				file.WriteString("Birth of the Empires - Combat Log file\n\n");
				for (set<CString>::const_iterator it = pDoc->combat.m_mInvolvedRaces.begin(); it != pDoc->combat.m_mInvolvedRaces.end(); ++it)
				{
					CString s;
					s.Format("----------------------------------------\nlost ships of race: %s\n", *it);
					file.WriteString(s);
					for (int j = 0; j < pDoc->m_ShipArray.GetSize(); j++)
						if (pDoc->m_ShipArray.GetAt(j).GetOwnerOfShip() == *it)
							if (pDoc->m_ShipArray.GetAt(j).GetHull()->GetCurrentHull() == 0)
								file.WriteString(pDoc->m_ShipArray.GetAt(j).GetShipClass()+"\n");
				}
			}
			file.Close();

			
			return;
		}

		p_timer->SuspendThread();
		Invalidate(FALSE);



		GetClientRect(r);

		CString fileName="Stats.log";														// Name des zu Öffnenden Files 
		CStdioFile file;																	// Varibale vom Typ CStdioFile
		if (file.Open(fileName, CFile::modeCreate | CFile::modeWrite | CFile::typeText))	// Datei wird geöffnet
		{
			CString s;
			file.WriteString("Birth of the Empires - Stats Log file\n\n");
			s.Format("overall combats: %d\n",pDoc->repeat);
			file.WriteString(s);
			for (std::map<CString, int>::const_iterator it = pDoc->wins.begin(); it != pDoc->wins.end(); ++it)
			{
				s.Format("----------------------------------------\nrace: %s wins %d combats", it->first, it->second);
				file.WriteString(s);
				s.Format(" -> %d%%\n", it->second * 100 / pDoc->repeat);
				file.WriteString(s);				
			}
		}
		file.Close();
		AfxMessageBox("Generating statsfile... ready\n\nClose this application or run a new one");
		
	
	}

	


#ifdef _DEBUG
void CCombatSimulatorView::AssertValid() const
{
	CView::AssertValid();
}

void CCombatSimulatorView::Dump(CDumpContext& dc) const
{
	
	CView::Dump(dc);
}

CCombatSimulatorDoc* CCombatSimulatorView::GetDocument() const // Nicht-Debugversion ist inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CCombatSimulatorDoc)));
	return (CCombatSimulatorDoc*)m_pDocument;
}
#endif //_DEBUG


/** Erstes erstellen des Fensters, hier werden verschiedene wichtige Intialisierungen druchgeführt
* Zum Beispiel alle 3D Objekte erstellt und die 3D Umgebung initialisiert
*/

void CCombatSimulatorView::OnInitialUpdate()
{
	CView::OnInitialUpdate();
	SIrrlichtCreationParameters ppp;
	//Events sollen nach der MFC hier ankommen 
	p_receiver = new EventReceiver();
	//Einstellungen des 3D Fensters
	ppp.AntiAlias = true;
	ppp.Bits = 16;
	ppp.DriverType = EDT_DIRECT3D9;
	ppp.EventReceiver = p_receiver;
	ppp.Fullscreen = false;
	ppp.HighPrecisionFPU = false;
	ppp.Stencilbuffer = false;
	ppp.Vsync = false;
	ppp.WindowSize = dimension2d<s32>(800, 600);
	ppp.WindowId = this->m_hWnd;


	p_device = createDeviceEx(ppp);
//Timer Thread wird initalisiert

	p_timer = new CTimerAdapter() ;
	p_timer->CreateThread(CREATE_SUSPENDED) ;
	p_timer->go = false;
	p_timer->p_view = this;
	p_timer->m_bAutoDelete = false ; // Let me delete it.

	p_timer->count = 40;
  //3D wird initalisiert
  p_driver = p_device->getVideoDriver();
  p_smgr = p_device->getSceneManager();
  p_ShipParent = p_smgr->addEmptySceneNode();
  p_guienv = p_device->getGUIEnvironment();
  //Kamera in Position 
  m_iCameraZoom = -150.0;
  m_iCameraX = 0.0;
  m_iCameraY = 5.0;
  p_camera = p_smgr->addCameraSceneNode(0, vector3df(0,0,m_iCameraZoom), vector3df(m_iCameraX,m_iCameraY,0));
  //Tickanzeige im oberen Fenster initalisieren
   text1 = p_guienv->addStaticText(L"Hello World! This is the Irrlicht Software renderer!",
		rect<s32>(10,10,260,22), true);
   text1->setOverrideColor(SColor(255,255,255,255));
   //Es werde Licht
  //ILightSceneNode* light = p_smgr->addLightSceneNode(p_camera, core::vector3df(0,20,0),
  //              video::SColorf(1.0f, 0.6f, 0.7f, 1.0f), 1800.0f);
  // Dokumentklasse für den Raumkampf hohlen
   	CCombatSimulatorDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;
	counter = 0;
	xy_ebene = TRUE;
	//Texturen laden
	Texture[""]		= p_driver->getTexture("media/snazf1.jpg");
	Texture["MAJOR1"]	= p_driver->getTexture("media/thor.jpg");
	Texture["MAJOR2"]	= p_driver->getTexture("media/robin.jpg");
	Texture["MAJOR3"]	= p_driver->getTexture("media/shadee.jpg");
	Texture["MAJOR4"]	= p_driver->getTexture("media/snaz12.jpg");
	Texture["MAJOR5"]	= p_driver->getTexture("media/pyschicparrot.jpg");
	Texture["MAJOR6"]	= p_driver->getTexture("media/psionic.jpg");
	//Farben festlegen 
	color[""]		= SColor(123,123,123,0);
	color["MAJOR1"]	= SColor(125,0,125,255);
	color["MAJOR2"]	= SColor(248,211,5,0);
	color["MAJOR3"]	= SColor(255,0,0,0);
	color["MAJOR4"]	= SColor(0,140,0,0);
	color["MAJOR5"] = SColor(125,0,125,0);
	color["MAJOR6"]	= SColor(73,240,240,0);

	// Hier werden alle am Kampf beteiligten Schiffe gezeichnet 
   for (int q = 0; q < pDoc->combat.m_CS.GetSize(); q++)
   {
			//verschiedene Routinen zum laden des Meshes für das Schiff
			IMesh* mesh = p_smgr->getMesh("media/fighter1.3ds");
			if (!mesh)
			{
				p_device->drop();
				return;
				
			}
			pDoc->combat.m_CS.GetAt(q)->SetMesh(mesh);
			pDoc->combat.m_CS.GetAt(q)->SetSceneManager(p_smgr);
			
			p_node = p_smgr->addMeshSceneNode( mesh,p_ShipParent,q );
			if (!p_node)
			{
				p_device->drop();
				return;
			}
			else
			{
				//Textur und Position für das Schiff wird gesetzt 
				p_node->setScale(vector3df(0.2f,0.2f,0.2f));
				p_node->setPosition(vector3df(pDoc->combat.m_CS.GetAt(q)->m_KO.x,pDoc->combat.m_CS.GetAt(q)->m_KO.y,pDoc->combat.m_CS.GetAt(q)->m_KO.z));
				p_node->setMaterialFlag(EMF_LIGHTING, false);
				p_node->setMaterialTexture( 0, Texture[pDoc->combat.m_CS.GetAt(q)->m_pShip->GetOwnerOfShip()] );
				pDoc->combat.m_CS.GetAt(q)->SetTexture(Texture[pDoc->combat.m_CS.GetAt(q)->m_pShip->GetOwnerOfShip()] );
				//Statustext über das Schiff zeichnen
				p_smgr->addTextSceneNode(p_guienv->getFont("media\\fontlucida.png"),L"TEST",color[pDoc->combat.m_CS.GetAt(q)->m_pShip->GetOwnerOfShip()],p_node,vector3df(40,-30,-50));
				
				//Sphere erstellen die um das Schiff gezeichnet wird wenn es mit der Mouse ausgewählt, wird später entfernt werden
				ISceneNode* sphere = p_smgr->addSphereSceneNode(30.0,16,p_node);
				sphere->setMaterialType(EMT_TRANSPARENT_ALPHA_CHANNEL);

				ITexture * target = p_driver->getTexture("media\\LaserBeam.bmp");
				sphere->setMaterialTexture(0, target);
				sphere->setVisible(false);


				//Schiffe im Raum ausrichten (ist noch etwas buggy)
				switch(pDoc->combat.m_CS.GetAt(q)->m_Race)
				{
					case 0:   p_node->setRotation(vector3df(0,0,0));  break;
					case 1:	  p_node->setRotation(vector3df(0,180,0));	break;
					case 2:   p_node->setRotation(vector3df(0,90,0));	break;
					case 3:   p_node->setRotation(vector3df(0,-90,0));	break;
					case 4:   p_node->setRotation(vector3df(0,0,90));	break;
					case 5:   p_node->setRotation(vector3df(0,0,-90));	break;	
				}

				pDoc->combat.m_CS.GetAt(q)->SetNode(p_node);
			}

			ITexture* SkyBox = p_driver->getTexture("media/background2.jpg");
			p_smgr->addSkyBoxSceneNode(SkyBox,SkyBox,SkyBox,SkyBox,SkyBox,SkyBox);
			//p_smgr->addSkyDomeSceneNode(SkyBox,32U,32U);
			
   }
}


/** Tastatureingaben werden abgefangen.
*/
BOOL  CCombatSimulatorView::PreTranslateMessage(MSG* pMsg )
{
	if(pMsg->message==WM_KEYDOWN)
    {
		WPARAM param = pMsg->wParam;
		switch (param)
		{
			case 'p':
			case 'P':
				p_timer->go = !p_timer->go;
				break;
			case 107:
				m_iCameraZoom = m_iCameraZoom + 10.0;
				break;
			case 109:
				m_iCameraZoom = m_iCameraZoom - 10.0;
				break;
			case '1':
				p_timer->count= 100;
				break;
			case '2':
				p_timer->count= 80;
				break;
			case '3':
				p_timer->count= 40;
				break;
			case '4':
				p_timer->count= 30;
				break;
			case '5':
				p_timer->count= 20;	
				break;
			case 37:
				m_iCameraX = m_iCameraX-10;
				break;
			case 39:
				m_iCameraX = m_iCameraX+10;
				break;
			case 38:
				m_iCameraY = m_iCameraY-10;
				break;
			case 40:
				m_iCameraY = m_iCameraY+10;
				break;

		}
    }	

    return CView::PreTranslateMessage(pMsg);
}

BOOL CCombatSimulatorView::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default

	return FALSE;
}
/** Timerthread wird instaziert
*/
BOOL CTimerAdapter::InitInstance()
{

    return TRUE;
}
/** Hiermit wird der Takt gegeben
*/
int CTimerAdapter::Run()
{
	CDC* pCDC = new CDC;
	while (true)
	{
		Sleep(count);
		if (go)
			p_view->OnDraw(pCDC);
	}
	delete pCDC;
	return (0) ;
}

/** Alles was beim bewegen der Maus passiert, passiert natürlich hier 
* Rechte Maustaste Kamera bewegen
* Linke Moustaste auswählen eines Schiffes mittels \ref SelectNode
*/

void CCombatSimulatorView::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Fügen Sie hier Ihren Meldungsbehandlungscode ein, und/oder benutzen Sie den Standard.
	if (nFlags == MK_RBUTTON)
	{
		m_iCameraX = m_iCameraX - (m_mouse.x - point.x);
		m_iCameraY = m_iCameraY + (m_mouse.y - point.y);
	}
	if (nFlags == MK_LBUTTON)
	{
		SelectNode(position2di(point.x,point.y));
	}

	m_mouse = point;
	CView::OnMouseMove(nFlags, point);
}




/** Zoomt beim dehen den Mousrades ran 
*/

BOOL CCombatSimulatorView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: Fügen Sie hier Ihren Meldungsbehandlungscode ein, und/oder benutzen Sie den Standard.
	m_iCameraZoom = m_iCameraZoom + zDelta/5;
	return CView::OnMouseWheel(nFlags, zDelta, pt);
}

/** Die Funktion errechtet das dem Mauszeiger am nächsten liegende Schiff und wählt es
* derzeit wird deine Elipse darum gezeichnet. Die Funktion basiert auf der Irrlicht internen Funktion 
* getSceneNodeFromScreenCoordinatesBB aber musste Aufrund von fehlerhafter Berechnung im Windows Mode
* etwas abgeändert werden.
*
* @param mouse_pos Wo liegt der Mauszeichner
* @return Den nächstliegenden Node#
* @todo Soll zur Auswahl von Schiffen genutzt werden. 
* @todo Nach entfernen der MFC eventuell wieder getSceneNodeFromScreenCoordinatesBB nutzen
*/

ISceneNode* CCombatSimulatorView::SelectNode(position2di mouse_pos)
{

	scene::ISceneCollisionManager* pCollMgr = p_smgr->getSceneCollisionManager();


	vector2d<s32> cursor = p_device->getCursorControl()->getPosition();

	core::line3d<f32> ln(0,0,0,0,0,0);

	ICameraSceneNode* camera = p_smgr->getActiveCamera();

	const scene::SViewFrustum* f = camera->getViewFrustum();

	core::vector3df farLeftUp = f->getFarLeftUp();
	core::vector3df lefttoright = f->getFarRightUp() - farLeftUp;
	core::vector3df uptodown = f->getFarLeftDown() - farLeftUp;

	RECT m_rect;
	this->GetWindowRect( &m_rect);



	f32 dx = (cursor.X + m_rect.left)  / (f32) (m_rect.right - m_rect.left) ;
	f32 dy = (cursor.Y + m_rect.top) / (f32) (m_rect.bottom - m_rect.top);

	if (camera->isOrthogonal())
		ln.start = f->cameraPosition + (lefttoright * (dx-0.5f)) + (uptodown * (dy-0.5f));
	else
		ln.start = f->cameraPosition;

	ln.end = farLeftUp + (lefttoright * dx) + (uptodown * dy);

	if ( ln.start == ln.end )
		return 0;


	ISceneNode* pNode = pCollMgr->getSceneNodeFromRayBB(ln, 0, true, p_ShipParent);

	if( pNode )
	{
		irr::core::list<ISceneNode*> list = pNode->getChildren();
		irr::core::list<ISceneNode*>::Iterator it = list.begin();
		ESCENE_NODE_TYPE t = pNode->getType();
		if (pNode->getType() == ESNT_SPHERE)
			pNode->setVisible(false);
		for (it;it != list.end(); it++)
		{
			if ((*it)->getType() ==ESNT_SPHERE)
			{
				if ( (*it)->isVisible())
					(*it)->setVisible(false);
				else
					(*it)->setVisible(true);
			}
		}

	}


	return pNode;


}
