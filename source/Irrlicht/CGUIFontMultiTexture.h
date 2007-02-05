// Copyright (C) 2002-2006 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __C_GUI_FONT_MULTITEX_H_INCLUDED__
#define __C_GUI_FONT_MULTITEX_H_INCLUDED__

#include "IGUIFont.h"
#include "irrString.h"
#include "IVideoDriver.h"
#include "irrArray.h"
#include "irrMap.h"
#include "IXMLReader.h"

namespace irr
{
namespace gui
{

//! Font interface.
class CGUIFontMultiTexture : public virtual IGUIFont
{
public:

	//! Constructor
	CGUIFontMultiTexture(video::IVideoDriver *driver);

	//! Destructor
	virtual ~CGUIFontMultiTexture();

	//! Draws an text and clips it to the specified rectangle if wanted.
	/** \param text: Text to draw
	 \param position: Rectangle specifying position where to draw the text.
	 \param color: Color of the text
	 \param hcenter: Specifiies if the text should be centered horizontally into the rectangle.
	 \param vcenter: Specifiies if the text should be centered vertically into the rectangle.
	 \param clip: Optional pointer to a rectangle against which the text will be clipped.
	 If the pointer is null, no clipping will be done. */
	virtual void draw(const wchar_t* text, const core::rect<s32>& position, 
		video::SColor color, bool hcenter=false, bool vcenter=false,
		const core::rect<s32>* clip=0);

	//! Calculates the dimension of a text.
	/** \return Returns width and height of the area covered by the text if it would be
	  drawn. */
	virtual core::dimension2d<s32> getDimension(const wchar_t* text);

	//! Calculates the index of the character in the text which is on a specific position.
	/** \param text: Text string.
	\param pixel_x: X pixel position of which the index of the character will be returned.
	\return Returns zero based index of the character in the text, and -1 if no no character
	is on this position. (=the text is too short). */
	virtual s32 getCharacterFromPos(const wchar_t* text, s32 pixel_x);

	//! set an Pixel Offset on Drawing ( scale position on width )
	virtual void setKerningWidth (s32 kerning);
	virtual void setKerningHeight (s32 kerning);

	//! set an Pixel Offset on Drawing ( scale position on width )
	virtual s32 getKerningWidth();
	virtual s32 getKerningHeight();

	//! Returns the type of this font
	virtual EGUI_FONT_TYPE getType() { return EGFT_XML_BITMAP; }

	//! loads a font from an XML file
	bool load(io::IXMLReader* xml);

private:

	struct SFontArea
	{
		SFontArea() : rectangle(), underhang(0), overhang(0), sourceimage(0) {}
		core::rect<s32> rectangle;
		s32				underhang;
		s32				overhang;
		u32				sourceimage;
	};

	s32 getAreaFromCharacter (const wchar_t c);
	s32	getWidthFromCharacter(const wchar_t c);

	video::IVideoDriver*			Driver;
	core::array<video::ITexture*>	Textures;
	core::array<SFontArea>			Areas;
	core::map<wchar_t, s32>			CharacterMap;
	u32								WrongCharacter;
	s32								Kerning;
};

} // end namespace gui
} // end namespace irr

#endif

