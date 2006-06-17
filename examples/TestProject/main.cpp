// this project is only for development and test purposes,
// it will not be included in the final sdk.

#include <irrlicht.h>
using namespace irr;

using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

#pragma comment(lib, "..\\..\\lib\\Win32-visualstudio\\Irrlicht.lib")

int main()
{
	IrrlichtDevice *device =
		createDevice( video::EDT_DIRECT3D9, dimension2d<s32>(640, 480), 16,
			false, false, false, 0);

	device->setWindowCaption(L"Hello World! - Irrlicht Engine Demo");

	IVideoDriver* driver = device->getVideoDriver();
	ISceneManager* smgr = device->getSceneManager();
	IGUIEnvironment* guienv = device->getGUIEnvironment();

    guienv->addStaticText(L"Hello World! This is the Irrlicht Apfelbaum Software renderer!",
		rect<int>(10,10,260,22), true);

	IAnimatedMesh* mesh = smgr->getMesh("../../media/sydney.md2");
	IAnimatedMeshSceneNode* node = smgr->addAnimatedMeshSceneNode( mesh );

	if (node)
	{
		node->setMaterialFlag(EMF_LIGHTING, false);
		node->setMD2Animation ( scene::EMAT_STAND );
		node->setMaterialTexture( 0, driver->getTexture("../../media/sydney.bmp") );
	}

	smgr->addCameraSceneNode(0, vector3df(0,30,-40), vector3df(0,5,0));

	device->run();

	while(device->run())
	{
		driver->beginScene(true, true, SColor(255,100,101,140));

		smgr->drawAll();
		guienv->drawAll();

		driver->endScene();
	}

	device->drop();

	return 0;
}

