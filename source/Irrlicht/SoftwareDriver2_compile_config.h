// Copyright (C) 2002-2006 Nikolaus Gebhardt/Alten Thomas
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __S_VIDEO_2_SOFTWARE_COMPILE_CONFIG_H_INCLUDED__
#define __S_VIDEO_2_SOFTWARE_COMPILE_CONFIG_H_INCLUDED__

#include "IrrCompileConfig.h"


// Generic Render Flags for apfelsoft rasterizer

#define APFELSOFT_RENDERER_BEAUTIFUL

#ifdef APFELSOFT_RENDERER_BEAUTIFUL
	#define SOFTWARE_DRIVER_2_PERSPECTIVE_CORRECT
	#define SOFTWARE_DRIVER_2_SUBTEXEL
	#define SOFTWARE_DRIVER_2_BILINEAR
	#define SOFTWARE_DRIVER_2_LIGHTING
	#define SOFTWARE_DRIVER_2_32BIT
	#define SOFTWARE_DRIVER_2_MIPMAPPING
#endif

//#define SOFTWARE_DRIVER_2_USE_X32_ASSEMBLER
#ifdef _MSC_VER
#define SOFTWARE_DRIVER_2_CHANGE_FPU_STATE
#endif

#endif
