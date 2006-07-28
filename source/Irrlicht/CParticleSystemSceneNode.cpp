// Copyright (C) 2002-2006 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "CParticleSystemSceneNode.h"
#include "os.h"
#include "ISceneManager.h"
#include "ICameraSceneNode.h"
#include "IVideoDriver.h"
#include <string.h>

#include "CParticlePointEmitter.h"
#include "CParticleBoxEmitter.h"
#include "CParticleFadeOutAffector.h"
#include "CParticleGravityAffector.h"

namespace irr
{
namespace scene
{

//! constructor
CParticleSystemSceneNode::CParticleSystemSceneNode(bool createDefaultEmitter,
	ISceneNode* parent, ISceneManager* mgr, s32 id, 
	const core::vector3df& position, const core::vector3df& rotation,
	const core::vector3df& scale)
	: IParticleSystemSceneNode(parent, mgr, id, position, rotation, scale),
	Emitter(0), ParticlesAreGlobal(true)
{
	#ifdef _DEBUG
	setDebugName("CParticleSystemSceneNode");
	#endif

	if (createDefaultEmitter)
	{
		IParticleEmitter* e = createBoxEmitter();
		setEmitter(e);
		e->drop();
	}

	setParticleSize();
}



//! destructor
CParticleSystemSceneNode::~CParticleSystemSceneNode()
{
	if (Emitter)
		Emitter->drop();

	removeAllAffectors();
}



//! Sets the particle emitter, which creates the particles.
void CParticleSystemSceneNode::setEmitter(IParticleEmitter* emitter)
{
	lastEmitTime = os::Timer::getTime();

	if (Emitter)
		Emitter->drop();

	Emitter = emitter;

	if (Emitter)
		Emitter->grab();
}



//! Adds new particle effector to the particle system.
void CParticleSystemSceneNode::addAffector(IParticleAffector* affector)
{
	affector->grab();
	AffectorList.push_back(affector);
}



//! Removes all particle affectors in the particle system.
void CParticleSystemSceneNode::removeAllAffectors()
{
	core::list<IParticleAffector*>::Iterator it = AffectorList.begin();
	while (it != AffectorList.end())
	{
		(*it)->drop();
		it = AffectorList.erase(it);
	}
}


//! Returns the material based on the zero based index i.
video::SMaterial& CParticleSystemSceneNode::getMaterial(s32 i)
{
	return Material;
}



//! Returns amount of materials used by this scene node.
s32 CParticleSystemSceneNode::getMaterialCount()
{
	return 1;
}



//! Creates a point particle emitter.
IParticleEmitter* CParticleSystemSceneNode::createPointEmitter(
	core::vector3df direction, u32 minParticlesPerSecond,
	u32 maxParticlePerSecond, video::SColor minStartColor,
	video::SColor maxStartColor, u32 lifeTimeMin, u32 lifeTimeMax,
	s32 maxAngleDegrees)
{
	return new CParticlePointEmitter(direction, minParticlesPerSecond,
		maxParticlePerSecond, minStartColor, maxStartColor,
		lifeTimeMin, lifeTimeMax, maxAngleDegrees);
}


//! Creates a box particle emitter.
IParticleEmitter* CParticleSystemSceneNode::createBoxEmitter(
	core::aabbox3d<f32> box,	core::vector3df direction, 
	u32 minParticlesPerSecond,	u32 maxParticlePerSecond,
	video::SColor minStartColor,	video::SColor maxStartColor,
	u32 lifeTimeMin, u32 lifeTimeMax,
	s32 maxAngleDegrees)
{
	return new CParticleBoxEmitter(box, direction, minParticlesPerSecond,
		maxParticlePerSecond, minStartColor, maxStartColor, 
		lifeTimeMin, lifeTimeMax, maxAngleDegrees);
}




//! Creates a fade out particle affector.
IParticleAffector* CParticleSystemSceneNode::createFadeOutParticleAffector(
		video::SColor targetColor,	u32 timeNeededToFadeOut)
{
	return new CParticleFadeOutAffector(targetColor, timeNeededToFadeOut);
}


//! Creates a gravity affector.
IParticleAffector* CParticleSystemSceneNode::createGravityAffector(
		core::vector3df gravity, u32 timeForceLost)
{
	return new CParticleGravityAffector(gravity, timeForceLost);
}


//! pre render event
void CParticleSystemSceneNode::OnPreRender()
{
	if (IsVisible)
	{
		doParticleSystem(os::Timer::getTime());

		if (Particles.size() != 0)
		{
			SceneManager->registerNodeForRendering(this);
			ISceneNode::OnPreRender();
		}
	}
}



//! render
void CParticleSystemSceneNode::render()
{
	video::IVideoDriver* driver = SceneManager->getVideoDriver();
	ICameraSceneNode* camera = SceneManager->getActiveCamera();

	if (!camera || !driver)
		return;

	// calculate vectors for letting particles look to camera
	core::vector3df campos = camera->getAbsolutePosition();
	core::vector3df target = camera->getTarget();
	core::vector3df up = camera->getUpVector();
	core::vector3df view = target - campos;
	view.normalize();

	core::vector3df horizontal = up.crossProduct(view);
	horizontal.normalize();

	core::vector3df vertical = horizontal.crossProduct(view);
	vertical.normalize();

	horizontal *= 0.5f * ParticleSize.Width;
	vertical *= 0.5f * ParticleSize.Height;	

	view *= -1.0f;

	// reallocate arrays, if they are too small
	reallocateBuffers();

	// create particle vertex data
	for (u32 i=0; i<Particles.size(); ++i)
	{
		const SParticle& particle = Particles[i];

		s32 idx = i*4;

		Vertices[0+idx].Pos = particle.pos + horizontal + vertical;
		Vertices[0+idx].Color = particle.color;
		Vertices[0+idx].Normal = view;

		Vertices[1+idx].Pos = particle.pos + horizontal - vertical;
		Vertices[1+idx].Color = particle.color;
		Vertices[1+idx].Normal = view;

		Vertices[2+idx].Pos = particle.pos - horizontal - vertical;
		Vertices[2+idx].Color = particle.color;
		Vertices[2+idx].Normal = view;

		Vertices[3+idx].Pos = particle.pos - horizontal + vertical;
		Vertices[3+idx].Color = particle.color;
		Vertices[3+idx].Normal = view;
	}

	// render all 
	core::matrix4 mat;
	if (!ParticlesAreGlobal)
		mat.setTranslation(AbsoluteTransformation.getTranslation());
	driver->setTransform(video::ETS_WORLD, mat);
		

	driver->setMaterial(Material);

	driver->drawIndexedTriangleList(Vertices.pointer(), Particles.size()*4,
		Indices.pointer(), Particles.size()*2);

	// for debug purposes only:
	if (DebugDataVisible)
	{
		driver->setTransform(video::ETS_WORLD, AbsoluteTransformation);
		video::SMaterial m;
		m.Lighting = false;
		driver->setMaterial(m);
		driver->draw3DBox(Box, video::SColor(0,255,255,255));
	}
}



//! returns the axis aligned bounding box of this node
const core::aabbox3d<f32>& CParticleSystemSceneNode::getBoundingBox() const
{
	return Box;
}



void CParticleSystemSceneNode::doParticleSystem(u32 time)
{
	u32 now = os::Timer::getTime();
	u32 timediff = now - lastEmitTime;
	lastEmitTime = now;

	// run emitter
	
	if (Emitter)	
	{
		SParticle* array = 0;
		s32 newParticles = Emitter->emitt(now, timediff, array);

		if (newParticles && array)
		{
			s32 j=Particles.size();
			if (newParticles > 16250-j)
				newParticles=16250-j;
			Particles.set_sorted(false);
			Particles.set_used(j+newParticles);
			for (s32 i=0; i<newParticles; ++i)
			{
				AbsoluteTransformation.rotateVect(array[i].startVector); 

				if (ParticlesAreGlobal)
					AbsoluteTransformation.transformVect(array[i].pos);

				Particles[j]=array[i];
			}
		}
	}

	// run affectors
	core::list<IParticleAffector*>::Iterator ait;
	ait = AffectorList.begin();
		while (ait != AffectorList.end())
		{
			(*ait)->affect(now, Particles.pointer(), Particles.size());
			++ait;
		}


	if (ParticlesAreGlobal)
		Box.reset(AbsoluteTransformation.getTranslation());
	else
		Box.reset(core::vector3df(0,0,0));

	// animate all particles
	f32 scale = (f32)timediff;

	for (s32 i=0; i<(s32)Particles.size();)
	{
		if (now > Particles[i].endTime)
			Particles.erase(i);
		else
		{
			Particles[i].pos += (Particles[i].vector * scale);
			Box.addInternalPoint(Particles[i].pos);
			++i;
		}
	}

	f32 m = ParticleSize.Width > ParticleSize.Height ? ParticleSize.Width : ParticleSize.Height;
	m *= 0.5f;
	Box.MaxEdge.X += m;
	Box.MaxEdge.Y += m;
	Box.MaxEdge.Z += m;

	Box.MinEdge.X -= m;
	Box.MinEdge.Y -= m;
	Box.MinEdge.Z -= m;

	if (ParticlesAreGlobal)
	{
		core::matrix4 absinv = AbsoluteTransformation;
		absinv.makeInverse();
		absinv.transformBox(Box);
	}
}


//! Sets if the particles should be global. If it is, the particles are affected by 
//! the movement of the particle system scene node too, otherwise they completely 
//! ignore it. Default is true.
void CParticleSystemSceneNode::setParticlesAreGlobal(bool global)
{
	ParticlesAreGlobal = global;
}



//! Sets the size of all particles.
void CParticleSystemSceneNode::setParticleSize(const core::dimension2d<f32> &size)
{
	ParticleSize = size;
}


void CParticleSystemSceneNode::reallocateBuffers()
{
	if (Particles.size() * 4 > Vertices.size() ||
        Particles.size() * 6 > Indices.size())
	{
		s32 oldSize = Vertices.size();
		Vertices.set_used(Particles.size() * 4);

		u32 i;

		// fill remaining vertices
		for (i=oldSize; i<Vertices.size(); i+=4)
		{
			Vertices[0+i].TCoords.set(0.0f, 0.0f);
			Vertices[1+i].TCoords.set(0.0f, 1.0f);
			Vertices[2+i].TCoords.set(1.0f, 1.0f);
			Vertices[3+i].TCoords.set(1.0f, 0.0f);
		}

		// fill remaining indices
		s32 oldIdxSize = Indices.size();
		s32 oldvertices = oldSize;
		Indices.set_used(Particles.size() * 6);
		
		for (i=oldIdxSize; i<Indices.size(); i+=6)
		{
			Indices[0+i] = 0+oldvertices;
			Indices[1+i] = 2+oldvertices;
			Indices[2+i] = 1+oldvertices;
			Indices[3+i] = 0+oldvertices;
			Indices[4+i] = 3+oldvertices;
			Indices[5+i] = 2+oldvertices;
			oldvertices += 4;
		}
	}
}


//! Writes attributes of the scene node.
void CParticleSystemSceneNode::serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options)
{
	IParticleSystemSceneNode::serializeAttributes(out, options);

	out->addBool("GlobalParticles", ParticlesAreGlobal);
	out->addFloat("ParticleWidth", ParticleSize.Width);
	out->addFloat("ParticleHeight", ParticleSize.Height);

	// write emitter

	E_PARTICLE_EMITTER_TYPE type = EPET_COUNT;
	if (Emitter)
		type = Emitter->getType();

	out->addEnum("Emitter", (s32)type, ParticleEmitterTypeNames);

	if (Emitter)
		Emitter->serializeAttributes(out, options);	

	// write affectors

	E_PARTICLE_AFFECTOR_TYPE atype = EPAT_NONE;

	for (core::list<IParticleAffector*>::Iterator it = AffectorList.begin();
		 it != AffectorList.end(); ++it)
	{
		atype = (*it)->getType();

		out->addEnum("Affector", (s32)atype, ParticleAffectorTypeNames);

		(*it)->serializeAttributes(out);
	}

	// add empty affector to make it possible to add further affectors

	if (options && options->Flags & io::EARWF_FOR_EDITOR)
		out->addEnum("Affector", EPAT_NONE, ParticleAffectorTypeNames);
}


//! Reads attributes of the scene node.
void CParticleSystemSceneNode::deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options)
{
	IParticleSystemSceneNode::deserializeAttributes(in, options);

	ParticlesAreGlobal = in->getAttributeAsBool("GlobalParticles");
	ParticleSize.Width = in->getAttributeAsFloat("ParticleWidth");
	ParticleSize.Height = in->getAttributeAsFloat("ParticleHeight");

	// read emitter

	int emitterIdx = in->findAttribute("Emitter");
	if (emitterIdx == -1)
		return;

	if (Emitter)
		Emitter->drop();
	Emitter = 0;

	E_PARTICLE_EMITTER_TYPE type = (E_PARTICLE_EMITTER_TYPE)
		in->getAttributeAsEnumeration("Emitter", ParticleEmitterTypeNames);

	switch(type)
	{
	case EPET_POINT:
		Emitter = createPointEmitter();
		break;
	case EPET_BOX:
		Emitter = createBoxEmitter();
		break;
	}

	s32 idx = 0;

	if (Emitter)
		idx = Emitter->deserializeAttributes(idx, in);

	++idx;

	// read affectors

	removeAllAffectors();
	s32 cnt = in->getAttributeCount();

	while(idx < cnt)
	{
		const char* name = in->getAttributeName(idx);

		if (!name || strcmp("Affector", name))
			return;

		E_PARTICLE_AFFECTOR_TYPE atype = 
			(E_PARTICLE_AFFECTOR_TYPE)in->getAttributeAsEnumeration(idx, ParticleAffectorTypeNames);

		IParticleAffector* aff = 0;
		
		switch(atype)
		{
		case EPAT_FADE_OUT:
			aff = createFadeOutParticleAffector();			
			break;
		case EPAT_GRAVITY:
			aff = createGravityAffector();
			break;
		}

		++idx;

		if (aff)
		{
			idx = aff->deserializeAttributes(idx, in, options);
			++idx;
			addAffector(aff);
			aff->drop();
		}
	}
}


} // end namespace scene
} // end namespace irr

