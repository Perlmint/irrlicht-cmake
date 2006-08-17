// Copyright (C) 2002-2006 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "CSceneNodeAnimatorFlyCircle.h"

namespace irr
{
namespace scene
{

//! constructor
CSceneNodeAnimatorFlyCircle::CSceneNodeAnimatorFlyCircle(u32 time, const core::vector3df& center, f32 radius, f32 speed)
: Radius(radius), Center(center), Speed(speed), StartTime(time)
{
	#ifdef _DEBUG
	setDebugName("CSceneNodeAnimatorFlyCircle");
	#endif
}



//! destructor
CSceneNodeAnimatorFlyCircle::~CSceneNodeAnimatorFlyCircle()
{
}



//! animates a scene node
void CSceneNodeAnimatorFlyCircle::animateNode(ISceneNode* node, u32 timeMs)
{
	if (node)
	{
		f32 t = (timeMs-StartTime) * Speed;
		core::vector3df circle(Radius * sinf(t), 0, Radius * cosf(t));
		node->setPosition(Center + circle);
	}
}


//! Writes attributes of the scene node animator.
void CSceneNodeAnimatorFlyCircle::serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options)
{
	out->addVector3d("Center", Center);
	out->addFloat("Radius", Radius);
	out->addFloat("Speed", Speed);
}


//! Reads attributes of the scene node animator.
void CSceneNodeAnimatorFlyCircle::deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options)
{
	Center = in->getAttributeAsVector3d("Center");
	Radius = in->getAttributeAsFloat("Radius");
	Speed = in->getAttributeAsFloat("Speed");
}



} // end namespace scene
} // end namespace irr

