// Copyright (C) 2002-2006 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __I_GUI_FONT_H_INCLUDED__
#define __I_GUI_FONT_H_INCLUDED__

#include "IUnknown.h"
#include "rect.h"
#include "irrTypes.h"
#include "SColor.h"
#include "irrArray.h"
#include <ITexture.h>

namespace irr
{
namespace gui
{

//! An enum for the different types of GUI font.
enum EGUI_FONT_TYPE
{
	//! A bitmap font created by the ASCII font tool, the character 
	//! rectangles are defined by red and yellow dots within the image.
	//! This for the fast CGUIFont type like the built-in font and does
	//! not support wide characters
	EGFT_BITMAP = 0,

	//! Bitmap fonts which support wide characters and can be made of 
	//! many images. These fonts are loaded from an XML file.
	//! Because they can use many textures, they are considerably slower
	//! than EGFT_BITMAP fonts
	EGFT_XML_BITMAP,

	//! Scalable vector fonts loaded from an XML file. 
	//! These fonts reside in system memory and use no video memory 
	//! until they are displayed. These are slower than bitmap fonts
	//! but can be easily scaled and rotated.
	EGFT_XML_VECTOR,

	//! A font which uses a the native API provided by the operating system.
	//! Currently not used.
	EGFT_OS,

	//! An external font type provided by the user.
	EGFT_CUSTOM
};

//! Font interface.
class IGUIFont : public virtual IUnknown
{
public:

	//! Destructor
	virtual ~IGUIFont() {};

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
		const core::rect<s32>* clip=0) = 0;

	//! Calculates the dimension of a text.
	/** \return Returns width and height of the area covered by the text if it would be
	  drawn. */
	virtual core::dimension2d<s32> getDimension(const wchar_t* text) = 0;

	//! Calculates the index of the character in the text which is on a specific position.
	/** \param text: Text string.
	\param pixel_x: X pixel position of which the index of the character will be returned.
	\return Returns zero based index of the character in the text, and -1 if no no character
	is on this position. (=the text is too short). */
	virtual s32 getCharacterFromPos(const wchar_t* text, s32 pixel_x) = 0;

	//! Returns the type of this font
	virtual EGUI_FONT_TYPE getType() { return EGFT_CUSTOM; }

	//! set an Pixel Offset on Drawing ( scale position on width )
	virtual void setKerning ( s32 kerning ) = 0;

	//! set an Pixel Offset on Drawing ( scale position on width )
	virtual s32 getKerning () = 0;

	/*
	//! get the Font Texture
	virtual video::ITexture* getTexture () = 0;
	//! returns the parsed Symbol Information
	virtual const core::array< core::rect<s32> > & getPositions () = 0;
	*/

};

} // end namespace gui
} // end namespace irr

#endif

