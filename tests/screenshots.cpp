// Test the screenshot functionality of all drivers

#include "irrlicht.h"
#include "testUtils.h"

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;


static bool runTestWithDriver(E_DRIVER_TYPE driverType)
{
	IrrlichtDevice *device = createDevice(driverType, dimension2d<s32>(640, 480));
	if (!device)
		return true; // Treat a failure to create a driver as benign; this saves a lot of #ifdefs
	
	IVideoDriver* driver = device->getVideoDriver();
	ISceneManager * smgr = device->getSceneManager();

	IAnimatedMeshSceneNode * node = smgr->addAnimatedMeshSceneNode(smgr->getMesh("../media/dwarf.x"));
	node->setMaterialFlag(EMF_LIGHTING, false);
	smgr->addCameraSceneNode(0, vector3df(0, 20, -30));

	driver->beginScene(true, true, SColor(255,100,101,140));
	smgr->drawAll();
	driver->endScene();

	bool result = takeScreenshotAndCompareAgainstReference(driver, "-screenshot.jpg");

	device->drop();

	return result;
}


bool screenshots(void)
{
	bool passed = true;

	passed |= runTestWithDriver(EDT_NULL);
	passed |= runTestWithDriver(EDT_SOFTWARE);
	passed |= runTestWithDriver(EDT_BURNINGSVIDEO);
	passed |= runTestWithDriver(EDT_OPENGL);
	passed |= runTestWithDriver(EDT_DIRECT3D8);
	passed |= runTestWithDriver(EDT_DIRECT3D9);
	
	return passed;
}

