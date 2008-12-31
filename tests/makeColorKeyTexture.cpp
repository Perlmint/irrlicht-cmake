// Copyright (C) 2008 Colin MacDonald
// No rights reserved: this software is in the public domain.

#include "irrlicht.h"
#include "testUtils.h"

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;

bool makeColorKeyTexture(void)
{
	IrrlichtDevice *device = createDevice( EDT_BURNINGSVIDEO,
											dimension2d<s32>(160, 120), 32);
	if (!device)
		return false;

	IVideoDriver* driver = device->getVideoDriver();
	ISceneManager * smgr = device->getSceneManager();

	// Draw a cube background so that we can check that the keying is working.
	ISceneNode * cube = smgr->addCubeSceneNode(50.f, 0, -1, vector3df(0, 0, 60));
	cube->setMaterialTexture(0, driver->getTexture("../media/wall.bmp"));
	cube->setMaterialFlag(video::EMF_LIGHTING, false);

	ITexture * Texture = device->getVideoDriver()->getTexture("../media/portal2.bmp");

	// This should result in only the centre of the texture being transparent,
	// not the black around the edges as well.
	device->getVideoDriver()->makeColorKeyTexture(Texture,position2d<s32>(64,64));
	device->getVideoDriver()->makeColorKeyTexture(Texture,position2d<s32>(64,64));

	(void)smgr->addCameraSceneNode();

	driver->beginScene(true, true, SColor(255,100,101,140));
	smgr->drawAll();

	driver->draw2DImage(Texture,
						position2di(40, 40),
						rect<s32>(0, 0, Texture->getSize().Width, Texture->getSize().Height),
						0,
						SColor(255,255,255,255),
						true);
	driver->endScene();

	bool result = takeScreenshotAndCompareAgainstReference(driver, "-makeColorKeyTexture.png");

	device->drop();

	return result;
}
