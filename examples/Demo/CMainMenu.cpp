// This is a Demo of the Irrlicht Engine (c) 2005 by N.Gebhardt.
// This file is not documentated.

#include "CMainMenu.h"

CMainMenu::CMainMenu()
: startButton(0), device(0), start(false), fullscreen(true), selected(2),
	music(false), shadows(false), additive(false), transparent(true), vsync(true)
{
}



bool CMainMenu::run(bool& outFullscreen, bool& outMusic, bool& outShadows,
					bool& outAdditive, bool &outVSync, video::E_DRIVER_TYPE& outDriver)
{
	device = createDevice(video::EDT_SOFTWARE2,
		core::dimension2d<s32>(512, 384), 16, false, false, false, this);

	device->getFileSystem()->addZipFileArchive("irrlicht.dat");
	device->getFileSystem()->addZipFileArchive("../../media/irrlicht.dat");

	video::IVideoDriver* driver = device->getVideoDriver();
	scene::ISceneManager* smgr = device->getSceneManager();
	gui::IGUIEnvironment* guienv = device->getGUIEnvironment();

	core::stringw str = "Irrlicht Engine Demo v";
	str += device->getVersion();
	device->setWindowCaption(str.c_str());

	// load font
	gui::IGUIFont* font = guienv->getFont("../../media/fontlucida.png");
	//gui::IGUIFont* font = guienv->getFont("../../media/fonthaettenschweiler.bmp");
	
	if (font)
		guienv->getSkin()->setFont(font);

	// add images

	//gui::IGUIImage* img = guienv->addImage(core::rect<int>(0,0,512,384));
	//img->setImage(driver->getTexture("../../media/demoback.bmp"));

	const s32 leftX = 260;

	// add tab control
	gui::IGUITabControl* tabctrl = guienv->addTabControl(core::rect<int>(leftX,10,512-10,384-10),
		0, true, true);
	gui::IGUITab* optTab = tabctrl->addTab(L"Demo");
	gui::IGUITab* aboutTab = tabctrl->addTab(L"About");

	// add list box

	gui::IGUIListBox* box = guienv->addListBox(core::rect<int>(10,10,230,120), optTab, 1);
	box->addItem(L"OpenGL 1.5");
	box->addItem(L"Direct3D 8.1");
	box->addItem(L"Direct3D 9.0c");
	box->addItem(L"Apfelsoft Renderer 0.2");
	box->addItem(L"Irrlicht Software Renderer 1.0");
	box->setSelected(selected);

	// add button

	startButton = guienv->addButton(core::rect<int>(30,295,200,324), optTab, 2, L"Start Demo");

	// add checkbox

	const s32 d = 50;

	guienv->addCheckBox(fullscreen, core::rect<int>(20,85+d,230,110+d),
		optTab, 3, L"Fullscreen");
	guienv->addCheckBox(music, core::rect<int>(20,110+d,230,135+d),
		optTab, 4, L"Music & Sfx");
	guienv->addCheckBox(shadows, core::rect<int>(20,135+d,230,160+d),
		optTab, 5, L"Realtime shadows");
	guienv->addCheckBox(additive, core::rect<int>(20,160+d,230,185+d),
		optTab, 6, L"Old HW compatible blending");
	guienv->addCheckBox(vsync, core::rect<int>(20,185+d,230,210+d),
		optTab, 7, L"Vertical synchronisation");

	// add text

	/*wchar_t* text = L"Welcome to the Irrlicht Engine. Please select "\
		L"the settings you prefer and press 'Start Demo'. "\
		L"Right click for changing menu style.";

	guienv->addStaticText(text, core::rect<int>(10, 220, 220, 280),
		true, true, optTab);*/

	// add about text

	wchar_t* text2 = L"This is the tech demo of the Irrlicht engine. To start, "\
		L"select a device which works best with your hardware and press 'start demo'. "\
		L"What you currently see is displayed using the Software Renderer. "\
		L"The Irrlicht Engine was written by me, Nikolaus Gebhardt. The models, "\
		L"maps and textures were placed at my disposal by B.Collins, M.Cook and J.Marton. The music was created by "\
		L"M.Rohde and is played back by Audiere.\n"\
		L"For more informations, please visit the homepage of the Irrlicht engine:\nhttp://www.irrlicht.sourceforge.net";

	guienv->addStaticText(text2, core::rect<int>(10, 10, 230, 320),
		true, true, aboutTab);


	// add md2 model

	scene::IAnimatedMesh* mesh = smgr->getMesh("../../media/faerie.md2");
	scene::IAnimatedMeshSceneNode* node = smgr->addAnimatedMeshSceneNode(mesh);
	if (node)
	{
		node->setMaterialTexture(0, driver->getTexture("../../media/faerie2.bmp"));
		node->setMaterialFlag(video::EMF_LIGHTING, true);
		node->setMD2Animation ( scene::EMAT_STAND );
	}

	// set ambient light
	driver->setAmbientLight ( video::SColorf ( 0x005F6613 ) );

	// add light 1 (nearly red)
	scene::ILightSceneNode* light1 =
		smgr->addLightSceneNode(0, core::vector3df(0,0,0),
		video::SColorf(0.8f, 0.0f, 0.f, 0.0f), 200.0f);

	light1->getLightData().Type = video::ELT_DIRECTIONAL;

	// add fly circle animator to light 1
	scene::ISceneNodeAnimator* anim =
		smgr->createFlyCircleAnimator (core::vector3df(0,18,0),14.0f, -0.003f);
	light1->addAnimator(anim);
	anim->drop();

	// attach billboard to the light
	scene::ISceneNode* bill =
		smgr->addBillboardSceneNode(light1, core::dimension2d<f32>(10, 10));

	bill->setMaterialFlag(video::EMF_LIGHTING, false);
	bill->setMaterialType(video::EMT_TRANSPARENT_ADD_COLOR);
	bill->setMaterialTexture(0, driver->getTexture("../../media/particlered.bmp"));

	// add light 2 (nearly green)
	scene::ILightSceneNode* light2 =
		smgr->addLightSceneNode(0, core::vector3df(0,0,0),
		video::SColorf(0.f, 0.f, 0.8f, 0.0f), 200.0f);

	light2->getLightData().Type = video::ELT_DIRECTIONAL;

	// add fly circle animator to light 2
	anim = smgr->createFlyCircleAnimator (core::vector3df(0,-10.f,0),10.0f, 0.003f);
	light2->addAnimator(anim);
	anim->drop();

	// attach billboard to the light
	bill = smgr->addBillboardSceneNode(light2, core::dimension2d<f32>(10, 10));

	bill->setMaterialFlag(video::EMF_LIGHTING, false);
	bill->setMaterialType(video::EMT_TRANSPARENT_ADD_COLOR);
	bill->setMaterialTexture(0, driver->getTexture("../../media/portal1.bmp"));

	smgr->addCameraSceneNode(0, core::vector3df(45,0,0), core::vector3df(0,0,10));

	// irrlicht logo
	// add irrlicht logo
	guienv->addImage(driver->getTexture("../../media/irrlichtlogoalpha2.tga"),
		core::position2d<s32>(5,5));

	video::ITexture* irrlichtBack = driver->getTexture("../../media/demoback.bmp");

	// set transparency
	setTransparency();

	// draw all

	while(device->run())
	{
		if (device->isWindowActive())
		{
			driver->beginScene(false, true, video::SColor(0,0,0,0));

			if ( irrlichtBack )
				driver->draw2DImage(irrlichtBack,
									core::position2d<int>(0,0)
									);

			smgr->drawAll();
			guienv->drawAll();
			driver->endScene();
		}
	}

	device->drop();

	outFullscreen = fullscreen;
	outMusic = music;
	outShadows = shadows;
	outAdditive = additive;
	outVSync = vsync;

	switch(selected)
	{
	case 0:	outDriver = video::EDT_OPENGL; break;
	case 1:	outDriver = video::EDT_DIRECT3D8; break;
	case 2:	outDriver = video::EDT_DIRECT3D9; break;
	case 3:	outDriver = video::EDT_SOFTWARE2; break;
	case 4:	outDriver = video::EDT_SOFTWARE; break;
	}

	return start;
}



bool CMainMenu::OnEvent(SEvent event)
{
	if (event.EventType == irr::EET_MOUSE_INPUT_EVENT &&
		event.MouseInput.Event == EMIE_RMOUSE_LEFT_UP )
	{
		core::rect<s32> r(event.MouseInput.X, event.MouseInput.Y, 0, 0);
		gui::IGUIContextMenu* menu = device->getGUIEnvironment()->addContextMenu(r, 0, 45);
		menu->addItem(L"transparent menus", 666, transparent == false);
		menu->addItem(L"solid menus", 666, transparent == true);
		menu->addSeparator();
		menu->addItem(L"Cancel");
	}
	else
	if (event.EventType == EET_GUI_EVENT)
	{
		s32 id = event.GUIEvent.Caller->getID();
		switch(id)
		{
		case 45: // context menu
			if (event.GUIEvent.EventType == gui::EGET_MENU_ITEM_SELECTED)
			{
				s32 s = ((gui::IGUIContextMenu*)event.GUIEvent.Caller)->getSelectedItem();
				if (s == 0 || s == 1)
				{
					transparent = !transparent;
					setTransparency();
				}
			}
			break;
		case 1:
			if (event.GUIEvent.EventType == gui::EGET_LISTBOX_CHANGED ||
				event.GUIEvent.EventType == gui::EGET_LISTBOX_SELECTED_AGAIN)
			{
				selected = ((gui::IGUIListBox*)event.GUIEvent.Caller)->getSelected();
				startButton->setEnabled(selected != 4);
			}
			break;
		case 2:
			if (event.GUIEvent.EventType == gui::EGET_BUTTON_CLICKED )
			{
				device->closeDevice();
				start = true;
			}
		case 3:
			if (event.GUIEvent.EventType == gui::EGET_CHECKBOX_CHANGED )
				fullscreen = ((gui::IGUICheckBox*)event.GUIEvent.Caller)->isChecked();
			break;
		case 4:
			if (event.GUIEvent.EventType == gui::EGET_CHECKBOX_CHANGED )
				music = ((gui::IGUICheckBox*)event.GUIEvent.Caller)->isChecked();
			break;
		case 5:
			if (event.GUIEvent.EventType == gui::EGET_CHECKBOX_CHANGED )
				shadows = ((gui::IGUICheckBox*)event.GUIEvent.Caller)->isChecked();
			break;
		case 6:
			if (event.GUIEvent.EventType == gui::EGET_CHECKBOX_CHANGED )
				additive = ((gui::IGUICheckBox*)event.GUIEvent.Caller)->isChecked();
			break;
		case 7:
			if (event.GUIEvent.EventType == gui::EGET_CHECKBOX_CHANGED )
				vsync = ((gui::IGUICheckBox*)event.GUIEvent.Caller)->isChecked();
			break;
		}
	}

	return false;
}


void CMainMenu::setTransparency()
{
	for (s32 i=0; i<gui::EGDC_COUNT ; ++i)
	{
		video::SColor col = device->getGUIEnvironment()->getSkin()->
			getColor((gui::EGUI_DEFAULT_COLOR)i);
		col.setAlpha(transparent ? 201 : 255);
		device->getGUIEnvironment()->getSkin()->setColor((gui::EGUI_DEFAULT_COLOR)i, col);
	}
}
