// Copyright (C) 2002-2006 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __I_GUI_FONT_ASCII_H_INCLUDED__
#define __I_GUI_FONT_ASCII_H_INCLUDED__

#include "IGUIFont.h"

namespace irr
{
namespace gui
{

//! Font interface.
class IGUIFontASCII : public IGUIFont
{
public:

	//! Destructor
	virtual ~IGUIFontASCII() {};

	//! Returns the type of this font
	virtual EGUI_FONT_TYPE getType() { return EGFT_BITMAP; }

	//! get the Font Texture
	virtual video::ITexture* getTexture () = 0;

	//! returns the parsed Symbol Information
	virtual const core::array< core::rect<s32> > & getPositions () = 0;

};

} // end namespace gui
} // end namespace irr

#endif

