// Copyright (C) 2008-2009 Colin MacDonald
// No rights reserved: this software is in the public domain.

#include "testUtils.h"

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

/** This tests verifies that textures opened from different places in the
	filesystem don't create duplicated textures. */
bool loadFromFileFolder(void)
{
	IrrlichtDevice *device =
		createDevice( video::EDT_NULL, dimension2d<s32>(640, 480));

	if (!device)
	{
		logTestString("Unable to create EDT_NULL device\n");
		return false;
	}

	IVideoDriver * driver = device->getVideoDriver();

	ITexture * tex1 = driver->getTexture("../media/tools.png");
	assert(tex1);
	if(!tex1)
		logTestString("Unable to open ../media/tools.png\n");

	IReadFile * readFile = device->getFileSystem()->createAndOpenFile("../media/tools.png");
	assert(readFile);
	if(!readFile)
		logTestString("Unable to open ../media/tools.png\n");

	ITexture * tex2 = driver->getTexture(readFile);
	assert(tex2);
	if(!readFile)
		logTestString("Unable to create texture from ../media/tools.png\n");

	readFile->drop();

	// adding  a folder archive
	device->getFileSystem()->addFolderFileArchive( "../media/" );
	ITexture * tex3 = driver->getTexture("tools.png");
	assert(tex3);
	if(!tex3)
		logTestString("Unable to open tools.png\n");

	device->drop();
	return (tex1 == tex2 && tex1 == tex3);
}

bool loadTextures()
{
	bool result = true;
	result |= loadFromFileFolder();
	return result;
}

