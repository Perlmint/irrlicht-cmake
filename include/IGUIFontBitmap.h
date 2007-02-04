// Copyright (C) 2002-2006 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __I_GUI_FONT_BITMAP_H_INCLUDED__
#define __I_GUI_FONT_BITMAP_H_INCLUDED__

#include "IGUIFont.h"
#include "IGUISpriteBank.h"

namespace irr
{
namespace gui
{

//! Font interface.
class IGUIFontBitmap : public IGUIFont
{
public:

	//! Destructor
	virtual ~IGUIFontBitmap() {};

	//! Returns the type of this font
	virtual EGUI_FONT_TYPE getType() { return EGFT_BITMAP; }

	//! returns the parsed Symbol Information
	virtual IGUISpriteBank* getSpriteBank() = 0;

};

} // end namespace gui
} // end namespace irr

#endif

