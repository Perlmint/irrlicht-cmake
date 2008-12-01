// This test validates the last frame of a non-looped MD2 animation

#include "irrlicht.h"
#include "testUtils.h"
#include <assert.h>

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

/** Tests MD2 animations.  At the moment, this just verifies that the last frame of the
	animation produces the expected bitmap. */
bool md2Animation(void)
{
	// Make it small since we're using .png here and don't want a large image
	IrrlichtDevice *device = createDevice( EDT_BURNINGSVIDEO, dimension2d<s32>(640, 480));
	assert(device);
	if (!device)
		return false;
	
	IVideoDriver* driver = device->getVideoDriver();
	ISceneManager * smgr = device->getSceneManager();

	IAnimatedMesh* mesh = smgr->getMesh("../media/sydney.md2");
	IAnimatedMeshSceneNode* node;
	assert(mesh);

	if(mesh)
	{
		node = smgr->addAnimatedMeshSceneNode(mesh);
		assert(node);

		if(node)
		{
			node->setPosition(vector3df(20, 0, 50));
			node->setMaterialFlag(EMF_LIGHTING, false);
			node->setMaterialTexture(0, driver->getTexture("../media/sydney.bmp"));
			node->setLoopMode(false);

			(void)smgr->addCameraSceneNode();

			// Jump to the last frame to ensure that we get a consistent state.
			node->setMD2Animation(EMAT_DEATH_FALLBACK);
			node->setCurrentFrame((f32)(node->getEndFrame()));
			device->run();
			driver->beginScene(true, true, SColor(255, 255, 255, 0));
			smgr->drawAll();
			driver->endScene();
		}
	}

	bool result = takeScreenshotAndCompareAgainstReference(driver, "-md2Animation.png");
	device->drop();

	return result;
}
