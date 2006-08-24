// Copyright (C) 2002-2006 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __C_OPEN_GL_TEXTURE_H_INCLUDED__
#define __C_OPEN_GL_TEXTURE_H_INCLUDED__

#include "ITexture.h"
#include "IImage.h"
#include "COpenGLDriver.h"

#include "IrrCompileConfig.h"
#ifdef _IRR_COMPILE_WITH_OPENGL_

#ifdef _IRR_WINDOWS_
	#define WIN32_LEAN_AND_MEAN
	#include <windows.h>
	#include <GL/gl.h>
	#include <GL/glu.h>
#else // _IRR_WINDOWS_
	#ifdef MACOSX
		#include <OpenGL/gl.h>
		#include <OpenGL/glext.h>
		#include <OpenGL/glu.h>
	#else
		#include <GL/gl.h>
		#include <GL/glx.h>
		#include <GL/glu.h>
	#endif // MACOSX
#endif // _IRR_WINDOWS_

namespace irr
{
namespace video
{

//! OpenGL texture.
class COpenGLTexture : public ITexture
{
public:

	//! constructor
	COpenGLTexture(IImage* surface, bool generateMipLevels, const char* name, COpenGLDriver* driver=0);

	//! destructor
	virtual ~COpenGLTexture();

	//! lock function
	virtual void* lock();

	//! unlock function
	virtual void unlock();

	//! Returns original size of the texture.
	virtual const core::dimension2d<s32>& getOriginalSize();

	//! Returns size of the texture.
	virtual const core::dimension2d<s32>& getSize();

	//! returns driver type of texture (=the driver, who created the texture)
	virtual E_DRIVER_TYPE getDriverType();

	//! returns color format of texture
	virtual ECOLOR_FORMAT getColorFormat();

	//! returns pitch of texture (in bytes)
	virtual s32 getPitch();

	//! return open gl texture name
	GLuint getOpenGLTextureName();

	//! return whether this texture has mipmaps
	virtual bool hasMipMaps();

	//! Regenerates the mip map levels of the texture. Useful after
	//! locking and modifying the texture
	virtual void regenerateMipMapLevels();

private:

	void getImageData(IImage* image);

	//! copies the the texture into an open gl texture.
	void copyTexture();

	//! returns the size of a texture which would be the optimize size for rendering it
	inline s32 getTextureSizeFromSurfaceSize(s32 size);

	//! test if an error occurred, prints the problem, and returns
	//! true if an error happened
	inline bool testError();

	core::dimension2d<s32> ImageSize;
	core::dimension2d<s32> OriginalSize;
	s32 Pitch;
	bool SurfaceHasSameSize; // true if Surface has the same dimension as texture.
	c8* ImageData;
	ECOLOR_FORMAT ColorFormat;
	COpenGLDriver* Driver;

	GLuint TextureName;
	GLint InternalFormat;
	GLenum PixelFormat;
	GLenum PixelType;
	bool HasMipMaps;
	bool AutomaticMipmapUpdate;
};


} // end namespace video
} // end namespace irr

#endif
#endif // _IRR_COMPILE_WITH_OPENGL_
