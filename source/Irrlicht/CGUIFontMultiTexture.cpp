// Copyright (C) 2002-2006 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "CGUIFontMultiTexture.h"
#include "os.h"
#include "CImage.h"

namespace irr
{
namespace gui
{

//! constructor
CGUIFontMultiTexture::CGUIFontMultiTexture(video::IVideoDriver* driver)
: Driver(driver), Textures(0), WrongCharacter(0), Areas()
{
	#ifdef _DEBUG
	setDebugName("CGUIFontMultiTexture");
	#endif

	if (Driver)
		Driver->grab();
}



//! destructor
CGUIFontMultiTexture::~CGUIFontMultiTexture()
{
	if (Driver)
		Driver->drop();

	for (u32 i=0; i<Textures.size(); ++i)
		if (Textures[i])
			Textures[i]->drop();
}

//! loads a font file from xml
bool CGUIFontMultiTexture::load(io::IXMLReader* xml)
{

	while (xml->read())
	{
		if (io::EXN_ELEMENT == xml->getNodeType())
		{
			if (core::stringw(L"Texture") == xml->getNodeName())
			{
				// add a texture
				core::stringc fn = xml->getAttributeValue(L"filename");
				u32 i = (u32)xml->getAttributeValueAsInt(L"index");
				core::stringw alpha = xml->getAttributeValue(L"hasAlpha");

				while (i+1 > Textures.size())
					Textures.push_back(0);

				// disable mipmaps+filtering
				bool mipmap = Driver->getTextureCreationFlag(video::ETCF_CREATE_MIP_MAPS);
				Driver->setTextureCreationFlag(video::ETCF_CREATE_MIP_MAPS, false);

				// load texture
				Textures[i] = Driver->getTexture(fn.c_str());

				// set previous mip-map+filter state
				Driver->setTextureCreationFlag(video::ETCF_CREATE_MIP_MAPS, mipmap);

				// couldn't load texture, abort.
				if (!Textures[i])
				{
					os::Printer::log("Unable to load all textures in the font, aborting", ELL_ERROR);
					_IRR_IMPLEMENT_MANAGED_MARSHALLING_BUGFIX;
					return false;
				}
				else
				{
					// grab the texture
					Textures[i]->grab();

					// colorkey texture rather than alpha channel?
					if (alpha == core::stringw("false"))
						Driver->makeColorKeyTexture(Textures[i], core::position2di(0,0));
				}
			} 
			else if (core::stringw(L"c") == xml->getNodeName())
			{
				// adding a character to this font
				SFontArea a;
				a.underhang		= xml->getAttributeValueAsInt(L"u");
				a.overhang		= xml->getAttributeValueAsInt(L"o");
				a.sourceimage	= xml->getAttributeValueAsInt(L"i");
				// parse rectangle
				core::stringc rectstr	= xml->getAttributeValue(L"r");
				wchar_t ch				= xml->getAttributeValue(L"c")[0];

				const c8 *c = rectstr.c_str();
				s32 val;
				val = 0;
				while (*c >= '0' && *c <= '9') 
				{ 
					val *= 10; 
					val += *c - '0'; 
					c++;
				}
				a.rectangle.UpperLeftCorner.X = val;
				while (*c == L' ' || *c == L',') c++;

				val = 0;
				while (*c >= '0' && *c <= '9') 
				{ 
					val *= 10; 
					val += *c - '0'; 
					c++;
				}
				a.rectangle.UpperLeftCorner.Y = val;
				while (*c == L' ' || *c == L',') c++;

				val = 0;
				while (*c >= '0' && *c <= '9') 
				{ 
					val *= 10; 
					val += *c - '0'; 
					c++;
				}
				a.rectangle.LowerRightCorner.X = val;
				while (*c == L' ' || *c == L',') c++;

				val = 0;
				while (*c >= '0' && *c <= '9') 
				{ 
					val *= 10; 
					val += *c - '0'; 
					c++;
				}
				a.rectangle.LowerRightCorner.Y = val;

				CharacterMap.insert(ch,Areas.size());
				Areas.push_back(a);

			}
		}

	}

	// make sure no textures were missed 
	for (u32 i=0; i<Textures.size(); ++i)
		if (Textures[i] == 0)
		{
			os::Printer::log("Texture missing from font, aborting", ELL_ERROR);
			_IRR_IMPLEMENT_MANAGED_MARSHALLING_BUGFIX;
			return false;
		}

	// set bad character
	WrongCharacter = getAreaFromCharacter(L' ');

	_IRR_IMPLEMENT_MANAGED_MARSHALLING_BUGFIX;
	return true;
}

//! set an Pixel Offset on Drawing ( scale position on width )
void CGUIFontMultiTexture::setKerning ( s32 kerning )
{
	return;
}

//! set an Pixel Offset on Drawing ( scale position on width )
s32 CGUIFontMultiTexture::getKerning ()
{
	return 0;
}

//! returns the dimension of a text
core::dimension2d<s32> CGUIFontMultiTexture::getDimension(const wchar_t* text)
{
	core::dimension2d<s32> dim(0, 0);

	s32 h;
	for(const wchar_t* p = text; *p; ++p)
	{
		SFontArea &area = Areas[getAreaFromCharacter(*p)];

		dim.Width += area.underhang;
		dim.Width += area.rectangle.getWidth() + area.overhang;
		h = area.rectangle.getHeight();
		if (h>dim.Height)
			dim.Height = h;
	}

	return dim;
}


s32 CGUIFontMultiTexture::getWidthFromCharacter(const wchar_t c)
{
	SFontArea &a = Areas[getAreaFromCharacter(c)];
	return a.rectangle.getWidth() + a.overhang;
}

s32 CGUIFontMultiTexture::getAreaFromCharacter(const wchar_t c)
{
	core::map<wchar_t, s32>::Node* n = CharacterMap.find(c);
	if (n)
		return n->getValue();
	else
		return WrongCharacter;
}


//! draws an text and clips it to the specified rectangle if wanted
void CGUIFontMultiTexture::draw(const wchar_t* text, const core::rect<s32>& position, video::SColor color, bool hcenter, bool vcenter, const core::rect<s32>* clip)
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

	while(*text)
	{
		SFontArea& area = Areas[getAreaFromCharacter(*text)];

		offset.X += area.underhang;

		Driver->draw2DImage(Textures[area.sourceimage], 
							offset, area.rectangle, clip, 
							color, 
							true);

		offset.X += area.rectangle.getWidth() + area.overhang;

		++text;
	}
}


//! Calculates the index of the character in the text which is on a specific position.
s32 CGUIFontMultiTexture::getCharacterFromPos(const wchar_t* text, s32 pixel_x)
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
