// Copyright (C) 2002-2006 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "CGUIFont.h"
#include "os.h"

namespace irr
{
namespace gui
{

//! constructor
CGUIFont::CGUIFont(video::IVideoDriver* driver)
: Driver(driver), Positions(382), Texture(0), WrongCharacter(0)
{
	#ifdef _DEBUG
	setDebugName("CGUIFont");
	#endif

	if (Driver)
		Driver->grab();
}



//! destructor
CGUIFont::~CGUIFont()
{
	if (Driver)
		Driver->drop();

	if (Texture)
		Texture->drop();
}



//! loads a font file
bool CGUIFont::load(io::IReadFile* file)
{
	if (!Driver)
		return false;

	return loadTexture(Driver->getTexture(file));
}


//! loads a font file
bool CGUIFont::load(const c8* filename)
{
	if (!Driver)
		return false;

	// turn mip-maps off
	bool mipmap = Driver->getTextureCreationFlag(video::ETCF_CREATE_MIP_MAPS);
	Driver->setTextureCreationFlag(video::ETCF_CREATE_MIP_MAPS, false);

	// get a pointer to the texture
	video::ITexture* tex = Driver->getTexture(filename);

	// set previous mip-map state
	Driver->setTextureCreationFlag(video::ETCF_CREATE_MIP_MAPS, mipmap);

	// load the texture
	return loadTexture(tex);
}



//! load & prepare font from ITexture
bool CGUIFont::loadTexture(video::ITexture* texture)
{
	if (!texture)
		return false;

	Texture = texture;
	Texture->grab();

	s32 lowerRightPositions = 0;

	switch(texture->getColorFormat())
	{
	case video::ECF_A1R5G5B5:
		readPositions16bit(texture, lowerRightPositions);
		break;
	case video::ECF_A8R8G8B8:
		readPositions32bit(texture, lowerRightPositions);
		break;
	default:
		os::Printer::log("Unsupported font texture color format.", ELL_ERROR);
		return false;
	}

	if (Positions.size() > 127)
		WrongCharacter = 127;

	return (!Positions.empty() && lowerRightPositions);
}



void CGUIFont::readPositions32bit(video::ITexture* texture, s32& lowerRightPositions)
{
	const core::dimension2d<s32>& size = texture->getOriginalSize();

	s32* p = (s32*)texture->lock();
	if (!p)
	{
		os::Printer::log("Could not lock texture while preparing texture for a font.", ELL_ERROR);
		return;
	}

	s32 colorTopLeft = *p;
	s32 colorLowerRight = *(p+1);
	s32 colorBackGround = *(p+2);
	s32 colorBackGroundWithAlphaFalse = 0x00FFFFFF & colorBackGround;
	s32 colorFont = 0xFFFFFFFF;

	*(p+1) = colorBackGround;

	// start parsing

	core::position2d<s32> pos(0,0);
	for (pos.Y=0; pos.Y<size.Height; ++pos.Y)
	{
		for (pos.X=0; pos.X<size.Width; ++pos.X)
		{
			if (*p == colorTopLeft)
			{
				*p = colorBackGroundWithAlphaFalse;
				Positions.push_back(core::rect<s32>(pos, pos));
			}
			else
			if (*p == colorLowerRight)
			{
				if (Positions.size()<=(u32)lowerRightPositions)
				{
					texture->unlock();
					lowerRightPositions = 0;
					return;
				}

				*p = colorBackGroundWithAlphaFalse;
				Positions[lowerRightPositions].LowerRightCorner = pos;
				++lowerRightPositions;
			}
			else
			if (*p == colorBackGround)
				*p = colorBackGroundWithAlphaFalse;
			else
				*p = colorFont;
			++p;
		}
	}

	// Positions parsed.

	texture->unlock();

	// output warnings
	if (!lowerRightPositions || !Positions.size())
		os::Printer::log("The amount of upper corner pixels or lower corner pixels is == 0, font file may be corrupted.", ELL_ERROR);
	else
	if (lowerRightPositions != (s32)Positions.size())
		os::Printer::log("The amount of upper corner pixels and the lower corner pixels is not equal, font file may be corrupted.", ELL_ERROR);
}



void CGUIFont::readPositions16bit(video::ITexture* texture, s32& lowerRightPositions)
{
	core::dimension2d<s32> size = texture->getOriginalSize();

	s16* p = (s16*)texture->lock();
	if (!p)
	{
		os::Printer::log("Could not lock texture while preparing texture for a font.", ELL_ERROR);
		return;
	}

	s16 colorTopLeft = *p;
	s16 colorLowerRight = *(p+1);
	s16 colorBackGround = *(p+2);
	s16 colorBackGroundWithAlphaFalse = 0x7FFF & colorBackGround;
	s16 colorFont = 0xFFFF;

	*(p+1) = colorBackGround;

	// start parsing

	core::position2d<s32> pos(0,0);
	for (pos.Y=0; pos.Y<size.Height; ++pos.Y)
	{
		for (pos.X=0; pos.X<size.Width; ++pos.X)
		{
			if (*p == colorTopLeft)
			{
				*p = colorBackGroundWithAlphaFalse;
				Positions.push_back(core::rect<s32>(pos, pos));
			}
			else
			if (*p == colorLowerRight)
			{
				if (Positions.size()<=(u32)lowerRightPositions)
				{
					texture->unlock();
					lowerRightPositions = 0;
					return;
				}

				*p = colorBackGroundWithAlphaFalse;
				Positions[lowerRightPositions].LowerRightCorner = pos;
				++lowerRightPositions;
			}
			else
			if (*p == colorBackGround)
				*p = colorBackGroundWithAlphaFalse;
			else
				*p = colorFont;
			++p;
		}
	}

	// Positions parsed.

	texture->unlock();

	// output warnings
	if (!lowerRightPositions || !Positions.size())
		os::Printer::log("The amount of upper corner pixels or lower corner pixels is == 0, font file may be corrupted.", ELL_ERROR);
	else
	if (lowerRightPositions != (s32)Positions.size())
		os::Printer::log("The amount of upper corner pixels and the lower corner pixels is not equal, font file may be corrupted.", ELL_ERROR);
}



//! returns the dimension of a text
core::dimension2d<s32> CGUIFont::getDimension(const wchar_t* text)
{
	core::dimension2d<s32> dim(0, Positions[0].getHeight());

	u32 n;

	for(const wchar_t* p = text; *p; ++p)
	{
		n = (*p) - 32;
		if (n > Positions.size())
			n = WrongCharacter;

		dim.Width += Positions[n].getWidth();
	}

	return dim;
}



inline s32 CGUIFont::getWidthFromCharacter(wchar_t c)
{
	u32 n = c - 32;
	if (n > Positions.size())
		n = WrongCharacter;

	return Positions[n].getWidth();
}


//! draws an text and clips it to the specified rectangle if wanted
void CGUIFont::draw(const wchar_t* text, const core::rect<s32>& position, video::SColor color, bool hcenter, bool vcenter, const core::rect<s32>* clip)
{
	if (!Driver)
		return;

	core::dimension2d<s32> textDimension;
	core::position2d<s32> offset = position.UpperLeftCorner;

	if (hcenter || vcenter)
	{
		textDimension = getDimension(text);

		if (hcenter)
			offset.X = ((position.getWidth() - textDimension.Width)>>1) + offset.X;

		if (vcenter)
			offset.Y = ((position.getHeight() - textDimension.Height)>>1) + offset.Y;
	}

	u32 n;

	while(*text)
	{
		n = (*text) - 32;
		if ( n > Positions.size())
			n = WrongCharacter;

		Driver->draw2DImage(Texture, offset, Positions[n], clip, color, true);

		offset.X += Positions[n].getWidth();

		++text;
	}
}


//! Calculates the index of the character in the text which is on a specific position.
s32 CGUIFont::getCharacterFromPos(const wchar_t* text, s32 pixel_x)
{
	s32 x = 0;
	s32 idx = 0;

	while (text[idx])
	{
		x += getWidthFromCharacter(text[idx]);

		if (x >= pixel_x)
			return idx;

		++idx;
	}

	return -1;
}


} // end namespace gui
} // end namespace irr
