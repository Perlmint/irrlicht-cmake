// Copyright (C) 2002-2006 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "CFPSCounter.h"
#include "irrMath.h"

namespace irr
{
namespace video  
{


CFPSCounter::CFPSCounter()
:	fps(60), startTime(0), framesCounted(0),
	primitive(0), primitivesCounted ( 0 )
{

}



//! returns current fps
s32 CFPSCounter::getFPS()
{
	return fps;
}

//! returns current primitive count
u32 CFPSCounter::getPrimitve()
{
	return primitive;
}



//! to be called every frame
void CFPSCounter::registerFrame(u32 now, u32 primitivesDrawn )
{
	++framesCounted;
	primitive = primitivesDrawn;
	primitivesCounted += primitivesDrawn;

	u32 milliseconds = now - startTime;

	if (milliseconds >= 1500 )
	{
		f32 invMilli = core::reciprocal ( (f32) milliseconds );
		
		fps = core::ceil32 ( ( 1000 * framesCounted ) * invMilli );

		framesCounted = 0;
		primitivesCounted = 0;
		startTime = now;
	}
}


} // end namespace video
} // end namespace irr

