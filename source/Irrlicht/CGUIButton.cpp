// Copyright (C) 2002-2006 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "CGUIButton.h"
#include "IGUISkin.h"
#include "IGUIEnvironment.h"
#include "IVideoDriver.h"
#include "IGUIFont.h"

namespace irr
{
namespace gui
{

//! constructor
CGUIButton::CGUIButton(IGUIEnvironment* environment, IGUIElement* parent,
					   s32 id, core::rect<s32> rectangle, bool noclip)
: IGUIButton(environment, parent, id, rectangle), Pressed(false), 
	OverrideFont(0), Image(0), PressedImage(0),
	IsPushButton(false), UseAlphaChannel(false)
{
	#ifdef _DEBUG
	setDebugName("CGUIButton");
	#endif
	setNotClipped(noclip);
}



//! destructor
CGUIButton::~CGUIButton()
{
	if (OverrideFont)
		OverrideFont->drop();

	if (Image)
		Image->drop();

	if (PressedImage)
		PressedImage->drop();
}



//! called if an event happened.
bool CGUIButton::OnEvent(SEvent event)
{
	if (!IsEnabled)
		return Parent ? Parent->OnEvent(event) : false;

	switch(event.EventType)
	{
	case EET_KEY_INPUT_EVENT:
		if (event.KeyInput.PressedDown &&
			(event.KeyInput.Key == KEY_RETURN || 
			 event.KeyInput.Key == KEY_SPACE))
		{
			if (!IsPushButton)
				Pressed = true;
			else
				Pressed = !Pressed;

			return true;
		}
		else
		if (!event.KeyInput.PressedDown && Pressed &&
			(event.KeyInput.Key == KEY_RETURN || 
			 event.KeyInput.Key == KEY_SPACE))
		{
			Environment->removeFocus(this);

			if (!IsPushButton)
				Pressed = false;
			
			if (Parent)
			{
				SEvent newEvent;
				newEvent.EventType = EET_GUI_EVENT;
				newEvent.GUIEvent.Caller = this;
				newEvent.GUIEvent.EventType = EGET_BUTTON_CLICKED;
				Parent->OnEvent(newEvent);
			}
			return true;
		}
		break;
	case EET_GUI_EVENT:
		if (event.GUIEvent.EventType == EGET_ELEMENT_FOCUS_LOST)
		{
			if (event.GUIEvent.Caller == (IGUIElement*) this && !IsPushButton)
				Pressed = false;
		}
		break;
	case EET_MOUSE_INPUT_EVENT:
		if (event.MouseInput.Event == EMIE_LMOUSE_PRESSED_DOWN)
		{
			if (Environment->hasFocus(this) &&
			    !AbsoluteClippingRect.isPointInside(core::position2d<s32>(event.MouseInput.X, event.MouseInput.Y)))
			{
					Environment->removeFocus(this);
					return false;
			}

			if (!IsPushButton)
				Pressed = true;
			
			Environment->setFocus(this);
			return true;
		}
		else
		if (event.MouseInput.Event == EMIE_LMOUSE_LEFT_UP)
		{
			bool wasPressed = Pressed;
			Environment->removeFocus(this);

			if ( !AbsoluteClippingRect.isPointInside( core::position2d<s32>(event.MouseInput.X, event.MouseInput.Y ) ) )
			{
				if (!IsPushButton)
					Pressed = false;
				return true;
			}

			if (!IsPushButton)
				Pressed = false;
			else
			{
				Pressed = !Pressed;
			}
			
			if ((!IsPushButton && wasPressed && Parent) ||
				(IsPushButton && wasPressed != Pressed))
			{
				SEvent newEvent;
				newEvent.EventType = EET_GUI_EVENT;
				newEvent.GUIEvent.Caller = this;
				newEvent.GUIEvent.EventType = EGET_BUTTON_CLICKED;
				Parent->OnEvent(newEvent);
			}

			return true;
		}
		break;
	}

	return Parent ? Parent->OnEvent(event) : false;
}



//! draws the element and its children
void CGUIButton::draw()
{
	if (!IsVisible)
		return;

	IGUISkin* skin = Environment->getSkin();
	irr::video::IVideoDriver* driver = Environment->getVideoDriver();

	IGUIFont* font = OverrideFont;
	if (!OverrideFont)
		font = skin->getFont();

	core::rect<s32> rect = AbsoluteRect;

	if (!Pressed)
	{
		skin->draw3DButtonPaneStandard(this, rect, &AbsoluteClippingRect);

		if (Image)
		{
			core::position2d<s32> pos = AbsoluteRect.getCenter();
			pos.X -= ImageRect.getWidth() / 2;
			pos.Y -= ImageRect.getHeight() / 2;

			driver->draw2DImage(Image, pos, ImageRect, &AbsoluteClippingRect, 
				video::SColor(255,255,255,255), UseAlphaChannel);
		}
	}
	else
	{
		skin->draw3DButtonPanePressed(this, rect, &AbsoluteClippingRect);

		if (PressedImage)
		{
			core::position2d<s32> pos = AbsoluteRect.getCenter();
			pos.X -= PressedImageRect.getWidth() / 2;
			pos.Y -= PressedImageRect.getHeight() / 2;
			// patch by Alan Tyndall/Jonas Petersen
			if (Image == PressedImage)
			{
				pos.X += 1;
				pos.Y += 1;
			}
			driver->draw2DImage(PressedImage, pos, PressedImageRect, &AbsoluteClippingRect,
				video::SColor(255,255,255,255), UseAlphaChannel);
		}
	}

	if (Text.size())
	{
		rect = AbsoluteRect;
        if (Pressed)
			rect.UpperLeftCorner.Y += 2;

		if (font)
			font->draw(Text.c_str(), rect,
				skin->getColor(IsEnabled ? EGDC_BUTTON_TEXT : EGDC_GRAY_TEXT), true, true, 
					&AbsoluteClippingRect);
	}

	IGUIElement::draw();
}



//! sets another skin independent font. if this is set to zero, the button uses the font of the skin.
void CGUIButton::setOverrideFont(IGUIFont* font)
{
	if (OverrideFont)
		OverrideFont->drop();

	OverrideFont = font;

	if (OverrideFont)
		OverrideFont->grab();
}


//! Sets an image which should be displayed on the button when it is in normal state. 
void CGUIButton::setImage(video::ITexture* image)
{
	if (Image)
		Image->drop();

	Image = image;
	if (image)
		ImageRect = core::rect<s32>(core::position2d<s32>(0,0), image->getSize());

	if (Image)
		Image->grab();

	if (!PressedImage)
		setPressedImage(Image);
}

//! Sets an image which should be displayed on the button when it is in normal state. 
void CGUIButton::setImage(video::ITexture* image, const core::rect<s32>& pos)
{
	if (Image)
		Image->drop();

	Image = image;
	ImageRect = pos;

	if (Image)
		Image->grab();

	if (!PressedImage)
		setPressedImage(Image, pos);
}

//! Sets an image which should be displayed on the button when it is in pressed state. 
void CGUIButton::setPressedImage(video::ITexture* image)
{
	if (PressedImage)
		PressedImage->drop();

	PressedImage = image;
	if (image)
		PressedImageRect = core::rect<s32>(core::position2d<s32>(0,0), image->getSize());

	if (PressedImage)
		PressedImage->grab();
}

//! Sets an image which should be displayed on the button when it is in pressed state. 
void CGUIButton::setPressedImage(video::ITexture* image, const core::rect<s32>& pos)
{
	if (PressedImage)
		PressedImage->drop();

	PressedImage = image;
	PressedImageRect = pos;

	if (PressedImage)
		PressedImage->grab();
}


//! Sets if the button should behave like a push button. Which means it
//! can be in two states: Normal or Pressed. With a click on the button,
//! the user can change the state of the button.
void CGUIButton::setIsPushButton(bool isPushButton)
{
	IsPushButton = isPushButton;
}


//! Returns if the button is currently pressed
bool CGUIButton::isPressed()
{
	_IRR_IMPLEMENT_MANAGED_MARSHALLING_BUGFIX;
	return Pressed;
}

//! Sets the pressed state of the button if this is a pushbutton
void CGUIButton::setPressed(bool pressed)
{
	Pressed = pressed;
}

//! Sets if the alpha channel should be used for drawing images on the button (default is false)
void CGUIButton::setUseAlphaChannel(bool useAlphaChannel)
{
	UseAlphaChannel = useAlphaChannel;
}

//! Returns if the alpha channel should be used for drawing images on the button
bool CGUIButton::getUseAlphaChannel()
{
	return UseAlphaChannel;
}

//! Writes attributes of the element.
void CGUIButton::serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options=0)
{

	IGUIButton::serializeAttributes(out,options);

	out->addBool		("PushButton",		IsPushButton );
	if (IsPushButton)
		out->addBool	("Pressed",			Pressed);

	out->addTexture ("Image",			Image);
	out->addRect	("ImageRect",		ImageRect);
	out->addTexture	("PressedImage",	PressedImage);
	out->addRect	("PressedImageRect",PressedImageRect);

	out->addBool		("UseAlphaChannel",	UseAlphaChannel);

	//   out->addString  ("OverrideFont",	OverrideFont);
}

//! Reads attributes of the element
void CGUIButton::deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options=0)
{
	video::IVideoDriver *drv = Environment->getVideoDriver();

	IGUIButton::deserializeAttributes(in,options);

	IsPushButton	= in->getAttributeAsBool("PushButton");
	Pressed		= IsPushButton ? in->getAttributeAsBool("Pressed") : false;

	core::rect<s32> rec = in->getAttributeAsRect("ImageRect");
	if (rec.isValid())
		setImage( in->getAttributeAsTexture("Image"), rec);
	else
		setImage( in->getAttributeAsTexture("Image") );

	rec = in->getAttributeAsRect("PressedImageRect");
	if (rec.isValid())
		setPressedImage( in->getAttributeAsTexture("PressedImage"), rec);
	else
		setPressedImage( in->getAttributeAsTexture("PressedImage") );

	UseAlphaChannel = in->getAttributeAsBool("UseAlphaChannel");

	//   setOverrideFont(in->getAttributeAsString("OverrideFont"));

	updateAbsolutePosition();
}

} // end namespace gui
} // end namespace irr
