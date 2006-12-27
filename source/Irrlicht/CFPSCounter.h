// Copyright (C) 2002-2006 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __C_FPSCOUNTER_H_INCLUDED__
#define __C_FPSCOUNTER_H_INCLUDED__

#include "irrTypes.h"

namespace irr
{
namespace video  
{


class CFPSCounter  
{
public:
	CFPSCounter();

	//! returns current fps
	s32 getFPS();
	//! returns primitive count
	u32 getPrimitve();

	//! to be called every frame
	void registerFrame(u32 now, u32 primitive);

private:

	s32 fps;
	u32 primitive;
	u32 startTime;

	u32 framesCounted;
	u32 primitivesCounted;
};


} // end namespace video
} // end namespace irr


#endif 

