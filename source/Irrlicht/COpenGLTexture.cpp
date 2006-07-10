// Copyright (C) 2002-2006 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "irrTypes.h"
#include "COpenGLTexture.h"
#include "os.h"
#include "CColorConverter.h"

#include "IrrCompileConfig.h"
#ifdef _IRR_COMPILE_WITH_OPENGL_

#include "glext.h"
#include <cstring>

namespace irr
{
namespace video  
{

//! constructor
COpenGLTexture::COpenGLTexture(IImage* image, bool generateMipLevels, const char* name)
: ITexture(name), Pitch(0), ImageSize(0,0), hasMipMaps(generateMipLevels),
ImageData(0), ColorFormat(ECF_A8R8G8B8), TextureName(0)
{
	#ifdef _DEBUG
	setDebugName("COpenGLTexture");
	#endif

	#ifdef DISABLE_MIPMAPPING
	hasMipMaps = false;
	#endif

	getImageData(image);

	if (ImageData)
	{
		glGenTextures(1, &TextureName);
		copyTexture();
	}		
}


//! destructor
COpenGLTexture::~COpenGLTexture()
{
	if (ImageData)
	{
		glDeleteTextures(1, &TextureName);
		delete [] ImageData;
		ImageData=0;
	}
}


void COpenGLTexture::getImageData(IImage* image)
{
	if (!image)
	{
		os::Printer::log("No image for OpenGL texture.", ELL_ERROR);
		return;
	}

	ImageSize = image->getDimension();
	OriginalSize = ImageSize;

	if ( !ImageSize.Width || !ImageSize.Height)
	{
		os::Printer::log("Invalid size of image for OpenGL Texture.", ELL_ERROR);
		return;
	}

	core::dimension2d<s32> nImageSize;
	nImageSize.Width = getTextureSizeFromSurfaceSize(ImageSize.Width);
	nImageSize.Height = getTextureSizeFromSurfaceSize(ImageSize.Height);

	s32 bpp=0;
	if (image->getColorFormat()==ECF_R8G8B8)
	{
		bpp=4;
		ColorFormat = ECF_A8R8G8B8;
	}
	else
	{
		bpp=image->getBytesPerPixel();
		ColorFormat = image->getColorFormat();
	}

	Pitch = nImageSize.Width*bpp;
	ImageData = new c8[Pitch * nImageSize.Height];

	c8* source = (c8*)image->lock();
	if (nImageSize == ImageSize)
	{
		if (image->getColorFormat()==ECF_R8G8B8)
		{
			u32* dest = (u32*)ImageData;
			for (s32 i=0; i<ImageSize.Width*ImageSize.Height; ++i)
			{
				*dest++=SColor(255,source[3*i],source[3*i+1],source[3*i+2]).color;
			}
		}
		else
			memcpy(ImageData,source,Pitch * nImageSize.Height);
	}
	else
	{
		// scale texture

		f32 sourceXStep = (f32)ImageSize.Width / (f32)nImageSize.Width;
		f32 sourceYStep = (f32)ImageSize.Height / (f32)nImageSize.Height;
		f32 sx,sy;

		// copy texture scaling
		sy = 0.0f;
		for (s32 y=0; y<nImageSize.Height; ++y)
		{
			sx = 0.0f;
			for (s32 x=0; x<nImageSize.Width; ++x)
			{
				s32 i=((s32)(((s32)sy)*ImageSize.Width + sx));
				if (image->getColorFormat()==ECF_R8G8B8)
					((s32*)ImageData)[y*nImageSize.Width + x]=SColor(255,source[3*i],source[3*i+1],source[3*i+2]).color;
				else
					memcpy(&ImageData[(y*nImageSize.Width + x)*bpp],&source[i*bpp],bpp);
				sx+=sourceXStep;
			}
			sy+=sourceYStep;
		}
	}
	image->unlock();
	ImageSize = nImageSize;
}


//! test if an error occurred, prints the problem, and returns 
//! true if an error happened
inline bool COpenGLTexture::testError()
{
	#ifdef _DEBUG
	GLenum g = glGetError();
	switch(g)
	{
	case GL_NO_ERROR:
		return false;
	case GL_INVALID_ENUM:
		os::Printer::log("GL_INVALID_ENUM", ELL_ERROR); break;
	case GL_INVALID_VALUE:
		os::Printer::log("GL_INVALID_VALUE", ELL_ERROR); break;
	case GL_INVALID_OPERATION:
		os::Printer::log("GL_INVALID_OPERATION", ELL_ERROR); break;
	case GL_STACK_OVERFLOW:
		os::Printer::log("GL_STACK_OVERFLOW", ELL_ERROR); break;
	case GL_STACK_UNDERFLOW:
		os::Printer::log("GL_STACK_UNDERFLOW", ELL_ERROR); break;
	case GL_OUT_OF_MEMORY:
		os::Printer::log("GL_OUT_OF_MEMORY", ELL_ERROR); break;
	};

	return true;
#endif
	return false;	
}


//! copies the the texture into an open gl texture.
void COpenGLTexture::copyTexture()
{
	glBindTexture(GL_TEXTURE_2D, TextureName);
	if (testError())
		os::Printer::log("Could not bind Texture", ELL_ERROR);

	GLint internalFormat=GL_RGBA;
	GLenum format=GL_BGRA;
	GLenum type;
	#ifdef __BIG_ENDIAN__
	switch (ColorFormat)
	{
		case ECF_A1R5G5B5:
			type=GL_UNSIGNED_SHORT_1_5_5_5;
			break;
		case ECF_R5G6B5:
			internalFormat=GL_RGB;
			format=GL_BGR;
			type=GL_UNSIGNED_SHORT_5_6_5;
			break;
		case ECF_R8G8B8:
			internalFormat=GL_RGB8;
			format=GL_RGB;
			type=GL_UNSIGNED_BYTE;
			break;
		case ECF_A8R8G8B8:
			type=GL_UNSIGNED_INT_8_8_8_8;
			break;
		default:
			os::Printer::log("Unsupported texture format", ELL_ERROR);
			break;
	}
	#else
	switch (ColorFormat)
	{
		case ECF_A1R5G5B5:
			type=GL_UNSIGNED_SHORT_1_5_5_5_REV;
			break;
		case ECF_R5G6B5:
			internalFormat=GL_RGB;
			format=GL_BGR;
			type=GL_UNSIGNED_SHORT_5_6_5_REV;
			break;
		case ECF_R8G8B8:
			internalFormat=GL_RGB8;
			format=GL_BGR;
			type=GL_UNSIGNED_BYTE;
			break;
		case ECF_A8R8G8B8:
			type=GL_UNSIGNED_INT_8_8_8_8_REV;
			break;
		default:
			os::Printer::log("Unsupported texture format", ELL_ERROR);
			break;
	}
	#endif

	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, ImageSize.Width, 
		ImageSize.Height, 0, format, type, ImageData);

	if (testError())
		os::Printer::log("Could not glTexImage2D", ELL_ERROR);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	if (hasMipMaps)
	{
		s32 ret = 0;
		
#ifndef DISABLE_MIPMAPPING
		ret = gluBuild2DMipmaps(GL_TEXTURE_2D, internalFormat, ImageSize.Width, ImageSize.Height,
 					format, type, ImageData);
#endif

		if (ret)
		{
			#ifndef DISABLE_MIPMAPPING
			os::Printer::log("Could not create OpenGL texture mip maps.",
				(c8*)gluErrorString(ret), ELL_ERROR);
			#else
			os::Printer::log("Did not create OpenGL texture mip maps.", ELL_ERROR);
			#endif
		}
		else
		{
			glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST );
			glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
		}
	}	
}


//! returns the size of a texture which would be the optimize size for rendering it
inline s32 COpenGLTexture::getTextureSizeFromSurfaceSize(s32 size)
{
	s32 ts = 0x01;
	while(ts < size)
		ts <<= 1;

	return ts;
}


//! lock function
void* COpenGLTexture::lock()
{
	return ImageData;
}



//! unlock function
void COpenGLTexture::unlock()
{
	copyTexture();
}


//! Returns original size of the texture.
const core::dimension2d<s32> COpenGLTexture::getOriginalSize()
{
	return OriginalSize;
}


//! Returns (=size) of the texture.
const core::dimension2d<s32> COpenGLTexture::getSize()
{
	return ImageSize;
}


//! returns driver type of texture (=the driver, who created the texture)
E_DRIVER_TYPE COpenGLTexture::getDriverType()
{
	return EDT_OPENGL;
}



//! returns color format of texture
ECOLOR_FORMAT COpenGLTexture::getColorFormat()
{
	return ColorFormat;
}



//! returns pitch of texture (in bytes)
s32 COpenGLTexture::getPitch()
{
	return ImageSize.Width * 4;
}



//! return open gl texture name
GLuint COpenGLTexture::getOpenGLTextureName()
{
	return TextureName;
}


//! Regenerates the mip map levels of the texture. Useful after locking and 
//! modifying the texture
void COpenGLTexture::regenerateMipMapLevels()
{
	if (hasMipMaps)
		copyTexture();
}


} // end namespace video
} // end namespace irr

#endif // _IRR_COMPILE_WITH_OPENGL_
