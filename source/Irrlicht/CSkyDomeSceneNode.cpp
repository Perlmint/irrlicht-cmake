// Copyright (C) 2002-2006 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h
// Code for this scene node has been contributed by Anders la Cour-Harbo (alc)

#include "CSkyDomeSceneNode.h"
#include "IVideoDriver.h"
#include "ISceneManager.h"
#include "ICameraSceneNode.h"
#include "os.h"

namespace irr
{
namespace scene
{

/* horiRes and vertRes:
	Controls the number of faces along the horizontal axis (30 is a good value)
	and the number of faces along the vertical axis (8 is a good value).

	texturePercentage:
	Only the top texturePercentage of the image is used, e.g. 0.8 uses the top 80% of the image,
	1.0 uses the entire image. This is useful as some landscape images have a small banner
	at the bottom that you don't want.

	spherePercentage:
	This controls how far around the sphere the sky dome goes. For value 1.0 you get exactly the upper
	hemisphere, for 1.1 you get slightly more, and for 2.0 you get a full sphere. It is sometimes useful
	to use a value slightly bigger than 1 to avoid a gap between some ground place and the sky. This
	parameters stretches the image to fit the chosen "sphere-size". */

CSkyDomeSceneNode::CSkyDomeSceneNode(video::ITexture* sky, u32 horiRes, u32 vertRes,
			f64 texturePercentage, f64 spherePercentage, ISceneNode* parent, ISceneManager* mgr, s32 id)
			: ISceneNode(parent, mgr, id)
{
	#ifdef _DEBUG
	setDebugName("CSkyDomeSceneNode");
	#endif

	f64 radius = 1000.0; /* Adjust this to get more or less perspective distorsion. */
	f64 azimuth, azimuth_step;
	f64 elevation, elevation_step;
	u32 k, c;

	video::S3DVertex vtx;

	AutomaticCullingEnabled = false;
	Material.Lighting = false;
	Material.ZBuffer = false;
	Material.ZWriteEnable = false;
	Material.Texture1 = sky;
	Box.MaxEdge.set(0,0,0);
	Box.MinEdge.set(0,0,0);

	azimuth_step = 2.*core::PI64/(f64)horiRes;
	if (spherePercentage<0.)
		spherePercentage=-spherePercentage;
	if (spherePercentage>2.)
		spherePercentage=2.;
	elevation_step = spherePercentage*core::PI64/2./(f64)vertRes;

	NumOfVertices = (horiRes+1)*(vertRes+1);
	NumOfFaces = (2*vertRes-1)*horiRes;

	Vertices = new video::S3DVertex[NumOfVertices];
	Indices = new u16[3*NumOfFaces];

	vtx.Color.set(255,255,255,255);
	vtx.Normal.set(0.0f,0.0f,0.0f);

	c = 0;
	for (k = 0, azimuth = 0; k <= horiRes; ++k)
	{
		elevation = core::PI64/2.;
		for (u32 j = 0; j <= vertRes; ++j)
		{
			vtx.Pos.set(radius*cos(elevation)*sin(azimuth),radius*sin(elevation)+50,radius*cos(elevation)*cos(azimuth));
			vtx.TCoords.set((f32)k/(f32)horiRes, (f32)j/(f32)vertRes*texturePercentage);
			Vertices[c++] = vtx;
			elevation -= elevation_step;
		}
		azimuth += azimuth_step;
	}

	c = 0;
	for (k = 0; k < horiRes; ++k)
	{
		Indices[c++] = vertRes+2+(vertRes+1)*k;
		Indices[c++] = 1+(vertRes+1)*k;
		Indices[c++] = 0+(vertRes+1)*k;

		for (u32 j = 1; j < vertRes; ++j)
		{
			Indices[c++] = vertRes+2+(vertRes+1)*k+j;
			Indices[c++] = 1+(vertRes+1)*k+j;
			Indices[c++] = 0+(vertRes+1)*k+j;

			Indices[c++] = vertRes+1+(vertRes+1)*k+j;
			Indices[c++] = vertRes+2+(vertRes+1)*k+j;
			Indices[c++] = 0+(vertRes+1)*k+j;
		}
	}
}


//! destructor
CSkyDomeSceneNode::~CSkyDomeSceneNode()
{
	if (Vertices)
		delete [] Vertices;
	if (Indices)
		delete [] Indices;
}


//! renders the node.
void CSkyDomeSceneNode::render()
{
	video::IVideoDriver* driver = SceneManager->getVideoDriver();
	scene::ICameraSceneNode* camera = SceneManager->getActiveCamera();

	if (!camera || !driver)
		return;

	if ( !camera->isOrthogonal() )
	{
		core::matrix4 mat;
		mat.setTranslation(camera->getAbsolutePosition());

		driver->setTransform(video::ETS_WORLD, mat);

		driver->setMaterial(Material);
		driver->drawIndexedTriangleList(Vertices, NumOfVertices, Indices, NumOfFaces);
	}
}

//! returns the axis aligned bounding box of this node
const core::aabbox3d<f32>& CSkyDomeSceneNode::getBoundingBox() const
{
	return Box;
}


void CSkyDomeSceneNode::OnPreRender()
{
	if (IsVisible)
	{
		SceneManager->registerNodeForRendering(this, ESNRP_SKY_BOX);
		ISceneNode::OnPreRender();
	}
}


//! returns the material based on the zero based index i. To get the amount
//! of materials used by this scene node, use getMaterialCount().
//! This function is needed for inserting the node into the scene hirachy on a
//! optimal position for minimizing renderstate changes, but can also be used
//! to directly modify the material of a scene node.
video::SMaterial& CSkyDomeSceneNode::getMaterial(s32 i)
{
	return Material;
}


//! returns amount of materials used by this scene node.
s32 CSkyDomeSceneNode::getMaterialCount()
{
	return 1;
}


}
}

