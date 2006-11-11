// This is a Demo of the Irrlicht Engine (c) 2006 by N.Gebhardt.
// This file is not documented.

#ifndef __C_DEMO_H_INCLUDED__
#define __C_DEMO_H_INCLUDED__

//#define USE_AUDIERE
//#define USE_SDL_MIXER

#include <irrlicht.h>

#ifdef _IRR_WINDOWS_
#include <windows.h>
#endif

#ifdef USE_AUDIERE
#include <audiere.h> // your compiler throws an error here? get audiere from 
                     // http://audiere.sourceforge.net/ or comment 
                     // the 'define USE_AUDIERE' above.
using namespace audiere;
#ifdef _IRR_WINDOWS_
#pragma comment (lib, "audiere.lib")
#endif
#endif
#ifdef USE_SDL_MIXER
# include <SDL/SDL.h>
# include <SDL/SDL_mixer.h>
#endif

using namespace irr;
const int CAMERA_COUNT = 7;

class CDemo : public IEventReceiver
{
public:

	CDemo(bool fullscreen, bool music, bool shadows, bool additive, bool vsync, video::E_DRIVER_TYPE driver);

	~CDemo();

	void run();

	virtual bool OnEvent(SEvent event);

private:

	void createLoadingScreen();
	void loadSceneData();
	void switchToNextScene();
	void shoot();
	void createParticleImpacts();

	bool fullscreen;
	bool music;
	bool shadows;
	bool additive;
	bool vsync;
	video::E_DRIVER_TYPE driverType;
	IrrlichtDevice *device;

#ifdef USE_AUDIERE
	void startAudiere();
	AudioDevicePtr audiereDevice;
	OutputStreamPtr stream;
	OutputStreamPtr ballSound;
	OutputStreamPtr impactSound;
#endif
#ifdef USE_SDL_MIXER
	void startSound();
	void playSound(Mix_Chunk *);
	void pollSound();
	Mix_Music *stream;
	Mix_Chunk *ballSound;
	Mix_Chunk *impactSound;
#endif

	struct SParticleImpact
	{
		u32 when;
		core::vector3df pos;
		core::vector3df outVector;
	};

	int currentScene;
	video::SColor backColor;

	gui::IGUIStaticText* statusText;
	gui::IGUIInOutFader* inOutFader;

	scene::IAnimatedMesh* quakeLevelMesh;
	scene::ISceneNode* quakeLevelNode;
	scene::ISceneNode* skyboxNode;
	scene::IAnimatedMeshSceneNode* model1;
	scene::IAnimatedMeshSceneNode* model2;
	scene::IParticleSystemSceneNode* campFire;

	scene::IMetaTriangleSelector* metaSelector;
	scene::ITriangleSelector* mapSelector;

	u32 sceneStartTime;
	u32 timeForThisScene;

	core::array<SParticleImpact> Impacts;
};

#endif

