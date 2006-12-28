// Copyright (C) 2002-2006 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __C_GUI_FONT_H_INCLUDED__
#define __C_GUI_FONT_H_INCLUDED__

#include "IGUIFontASCII.h"
#include "irrString.h"
#include "IVideoDriver.h"

namespace irr
{
namespace gui
{

class CGUIFont : public IGUIFontASCII
{
public:

	//! constructor
	CGUIFont(video::IVideoDriver* Driver);

	//! destructor
	virtual ~CGUIFont();

	//! loads a font file
	bool load(const c8* filename);

	//! loads a font file
	bool load(io::IReadFile* file);

	//! draws an text and clips it to the specified rectangle if wanted
	virtual void draw(const wchar_t* text, const core::rect<s32>& position, video::SColor color, bool hcenter=false, bool vcenter=false, const core::rect<s32>* clip=0);

	//! returns the dimension of a text
	virtual core::dimension2d<s32> getDimension(const wchar_t* text);

	//! Calculates the index of the character in the text which is on a specific position.
	virtual s32 getCharacterFromPos(const wchar_t* text, s32 pixel_x);

	//! Returns the type of this font
	virtual EGUI_FONT_TYPE getType() { return EGFT_BITMAP; }

	//! set an Pixel Offset on Drawing ( scale position on width )
	virtual void setKerning ( s32 kerning );

	//! set an Pixel Offset on Drawing ( scale position on width )
	virtual s32 getKerning ();

	//! get the Font Texture
	virtual video::ITexture* getTexture ();
	//! returns the parsed Symbol Information
	virtual const core::array< core::rect<s32> >& getPositions ();

private:

	//! load & prepare font from ITexture
	bool loadTexture(video::IImage * image, const c8* name);

	void readPositions16bit(video::IImage* texture, s32& lowerRightPositions);
	void readPositions32bit(video::IImage* texture, s32& lowerRightPositions);

	inline s32 getWidthFromCharacter(wchar_t c);

	video::IVideoDriver* Driver;
	core::array< core::rect<s32> > Positions;
	video::ITexture* Texture;
	s32 WrongCharacter;

	s32 GlobalKerningWidth;
};

} // end namespace gui
} // end namespace irr

#endif

