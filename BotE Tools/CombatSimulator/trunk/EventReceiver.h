#ifndef EVENTRECEIVER_H
#define EVENTRECEIVER_H


#include <irrlicht.h>


using namespace irr;


using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;


class EventReceiver : public IEventReceiver
{
public:

	EventReceiver();
	// This is the one method that we have to implement
	virtual bool OnEvent(const SEvent& event);


	// This is used to check whether a key is being held down
	virtual bool IsKeyDown(EKEY_CODE keyCode) const;

	


private:
	// We use this array to store the current state of each key
	bool KeyIsDown[KEY_KEY_CODES_COUNT];
};

#endif
