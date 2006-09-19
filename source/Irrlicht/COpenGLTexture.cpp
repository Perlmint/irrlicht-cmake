// Copyright (C) 2002-2006 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "irrTypes.h"
#include "COpenGLTexture.h"
#include "COpenGLDriver.h"
#include "os.h"
#include "CColorConverter.h"

#include "IrrCompileConfig.h"
#ifdef _IRR_COMPILE_WITH_OPENGL_

#include <cstring>

namespace irr
{
namespace video
{

//! constructor
COpenGLTexture::COpenGLTexture(IImage* image, bool generateMipLevels, const char* name, COpenGLDriver* driver)
 : ITexture(name), Pitch(0), ImageSize(0,0), HasMipMaps(generateMipLevels),
  Driver(driver), ImageData(0), ColorFormat(ECF_A8R8G8B8), TextureName(0),
  InternalFormat(GL_RGBA), PixelFormat(GL_BGRA_EXT), PixelType(GL_UNSIGNED_BYTE)
{
	#ifdef _DEBUG
	setDebugName("COpenGLTexture");
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
	if (Driver && Driver->queryFeature(EVDF_TEXTURE_NPOT))
		nImageSize=ImageSize;
	else
	{
		nImageSize.Width = getTextureSizeFromSurfaceSize(ImageSize.Width);
		nImageSize.Height = getTextureSizeFromSurfaceSize(ImageSize.Height);
	}
	SurfaceHasSameSize=ImageSize==nImageSize;

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
	ImageData = new u8[Pitch * nImageSize.Height];

	if (nImageSize == ImageSize)
	{
		void* source = image->lock();
		if (image->getColorFormat()==ECF_R8G8B8)
			CColorConverter::convert_R8G8B8toA8R8G8B8(source,ImageSize.Width*ImageSize.Height,ImageData);
		else
			memcpy(ImageData,source,Pitch*nImageSize.Height);
	}
	else
	{
		u8* source = (u8*)image->lock();
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
				{
					i*=3;
					((s32*)ImageData)[y*nImageSize.Width + x]=SColor(255,source[i],source[i+1],source[i+2]).color;
				}
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



//! copies the the texture into an open gl texture.
void COpenGLTexture::copyTexture()
{
	glBindTexture(GL_TEXTURE_2D, TextureName);
	if (Driver->testGLError())
		os::Printer::log("Could not bind Texture", ELL_ERROR);

	switch (ColorFormat)
	{
		case ECF_A1R5G5B5:
			InternalFormat=GL_RGBA;
			PixelFormat=GL_BGRA_EXT;
			PixelType=GL_UNSIGNED_SHORT_1_5_5_5_REV;
			break;
		case ECF_R5G6B5:
			InternalFormat=GL_RGB;
			PixelFormat=GL_RGB;
			PixelType=GL_UNSIGNED_SHORT_5_6_5;
			break;
		case ECF_R8G8B8:
			InternalFormat=GL_RGB8;
			PixelFormat=GL_RGB;
			PixelType=GL_UNSIGNED_BYTE;
			break;
		case ECF_A8R8G8B8:
			InternalFormat=GL_RGBA;
			PixelFormat=GL_BGRA_EXT;
			PixelType=GL_UNSIGNED_INT_8_8_8_8_REV;
			break;
		default:
			os::Printer::log("Unsupported texture format", ELL_ERROR);
			break;
	}

	#ifndef DISABLE_MIPMAPPING
	if (HasMipMaps && Driver && Driver->queryFeature(EVDF_MIP_MAP_AUTO_UPDATE))
	{
		// automatically generate and update mipmaps
		glTexParameteri( GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE );
		AutomaticMipmapUpdate=true;
	}
	else
	{
		AutomaticMipmapUpdate=false;
		regenerateMipMapLevels();
	}
	if (HasMipMaps) // might have changed in regenerateMipMapLevels
	{
		// enable bilinear mipmap filter
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST );
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	else
	#else
		HasMipMaps=false;
		os::Printer::log("Did not create OpenGL texture mip maps.", ELL_ERROR);
	#endif
	{
		// enable bilinear filter without mipmaps
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	glTexImage2D(GL_TEXTURE_2D, 0, InternalFormat, ImageSize.Width,
		ImageSize.Height, 0, PixelFormat, PixelType, ImageData);

	if (Driver->testGLError())
		os::Printer::log("Could not glTexImage2D", ELL_ERROR);
}



//! returns the size of a texture which would be the optimal size for rendering it
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
const core::dimension2d<s32>& COpenGLTexture::getOriginalSize()
{
	return OriginalSize;
}



//! Returns (=size) of the texture.
const core::dimension2d<s32>& COpenGLTexture::getSize()
{
	return ImageSize;
}



//! returns driver type of texture (=the driver, who created the texture)
E_DRIVER_TYPE COpenGLTexture::getDriverType()
{
	return EDT_OPENGL;
}



//! returns color format of texture
ECOLOR_FORMAT COpenGLTexture::getColorFormat() const
{
	return ColorFormat;
}



//! returns pitch of texture (in bytes)
s32 COpenGLTexture::getPitch()
{
	return Pitch;
}



//! return open gl texture name
GLuint COpenGLTexture::getOpenGLTextureName()
{
	return TextureName;
}



//! Returns whether this texture has mipmaps
//! return true if texture has mipmaps
bool COpenGLTexture::hasMipMaps()
{
	return HasMipMaps;
}



//! Regenerates the mip map levels of the texture. Useful after locking and
//! modifying the texture
//! MipMap updates are automatically performed by OpenGL.
void COpenGLTexture::regenerateMipMapLevels()
{
	if (AutomaticMipmapUpdate || !HasMipMaps)
		return;
		HasMipMaps=false;
	return;
	if (gluBuild2DMipmaps(GL_TEXTURE_2D, InternalFormat,
			ImageSize.Width, ImageSize.Height,
			PixelFormat, PixelType, ImageData))
		return;
	else
		HasMipMaps=false;
	return;

	// This code is wrong as it does not take into account the image scaling
	// Therefore it is currently disabled
	u32 width=ImageSize.Width>>1;
	u32 height=ImageSize.Height>>1;
	u32 i=1;
	while (width>1 || height>1)
	{
		//TODO: Add image scaling
		glTexImage2D(GL_TEXTURE_2D, i, InternalFormat, ImageSize.Width,
			ImageSize.Height, 0, PixelFormat, PixelType, ImageData);
		if (width>1)
			width>>=1;
		if (height>1)
			height>>=1;
		++i;
	}
}


} // end namespace video
} // end namespace irr

#endif // _IRR_COMPILE_WITH_OPENGL_
