
#include "CGUIEditWindow.h"
#include "IGUISkin.h"
#include "IGUIEnvironment.h"
#include "IGUIElementFactory.h"
#include "IAttributes.h"
#include "IGUIFont.h"

// bah
#include "IVideoDriver.h"
#include "../../source/irrlicht/CAttributes.h"

using namespace irr;
using namespace gui;

//! constructor
CGUIEditWindow::CGUIEditWindow(IGUIEnvironment* environment, core::rect<s32> rectangle, IGUIElement *parent)
		 : IGUIWindow(environment, parent, -1, rectangle),
		 Dragging(false), Resizing(false), SelectedElement(0), AttribEditor(0)

{

	#ifdef _DEBUG
	setDebugName("CGUIEditWindow");
	#endif

	// set window text
	setText(L"Attributes");

	// return if we have no skin.
	IGUISkin *skin = environment->getSkin();
	if (!skin)
		return;

	s32 th = skin->getSize(EGDS_WINDOW_BUTTON_WIDTH);

	setRelativePosition(core::rect<s32>(50,50,250,500));
	setMinSize( core::dimension2di(200,400));

	AttribEditor = (CGUIAttributeEditor*) environment->addGUIElement("attributeEditor",this);
	AttribEditor->grab();
	AttribEditor->setSubElement(true);
	AttribEditor->setRelativePosition(core::rect<s32>(1,th+5,200-th,450-th-5));
	AttribEditor->setAlignment(EGUIA_UPPERLEFT, EGUIA_LOWERRIGHT, EGUIA_UPPERLEFT, EGUIA_LOWERRIGHT);
	
	ResizeButton = environment->addStaticText(L"/",core::rect<s32>(199-th,449-th,199,449), true, false, this, true);
	ResizeButton->grab();
	ResizeButton->setSubElement(true);
	ResizeButton->setAlignment(EGUIA_LOWERRIGHT, EGUIA_LOWERRIGHT, EGUIA_LOWERRIGHT, EGUIA_LOWERRIGHT);

}

//! destructor
CGUIEditWindow::~CGUIEditWindow()
{
	// drop everything
	if (AttribEditor)
		AttribEditor->drop();
	if (ResizeButton)
		ResizeButton->drop();
}

void CGUIEditWindow::setSelectedElement(IGUIElement *sel)
{

	// save changes
	AttribEditor->updateAttribs();

	io::IAttributes* Attribs = AttribEditor->getAttribs();

	if (SelectedElement && sel != SelectedElement)
	{
		// deserialize attributes
		SelectedElement->deserializeAttributes(Attribs);
	}
	// clear the attributes list
	Attribs->clear();
	SelectedElement = sel;

	// get the new attributes
	if (SelectedElement)
		SelectedElement->serializeAttributes(Attribs);

	AttribEditor->refreshAttribs();

}

//! draws the element and its children.
//! same code as for a window
void CGUIEditWindow::draw()
{
	if (!IsVisible)
		return;

	IGUISkin* skin = Environment->getSkin();

	core::rect<s32> rect = AbsoluteRect;

	// draw body fast
	rect = skin->draw3DWindowBackground(this, true, skin->getColor(EGDC_ACTIVE_BORDER),
		AbsoluteRect, &AbsoluteClippingRect);

	if (Text.size())
	{
		rect.UpperLeftCorner.X += skin->getSize(EGDS_TEXT_DISTANCE_X);
		rect.UpperLeftCorner.Y += skin->getSize(EGDS_TEXT_DISTANCE_Y);
		rect.LowerRightCorner.X -= skin->getSize(EGDS_WINDOW_BUTTON_WIDTH) + 5;

		IGUIFont* font = skin->getFont();
		if (font)
			font->draw(Text.c_str(), rect, skin->getColor(EGDC_ACTIVE_CAPTION), false, true, &AbsoluteClippingRect);
	}

	IGUIElement::draw();
}


//! called if an event happened.
bool CGUIEditWindow::OnEvent(SEvent event)
{
	switch(event.EventType)
	{
	case EET_GUI_EVENT:
		switch(event.GUIEvent.EventType)
		{
		case EGET_ELEMENT_FOCUS_LOST:
			Dragging = false;
			Resizing = false;
			return true;

			break;

		case EGET_BUTTON_CLICKED:
			break;
		}

		break;
	case EET_MOUSE_INPUT_EVENT:
		switch(event.MouseInput.Event)
		{
		case EMIE_LMOUSE_PRESSED_DOWN:
		{
			
			DragStart.X = event.MouseInput.X;
			DragStart.Y = event.MouseInput.Y;

			IGUIElement* clickedElement = getElementFromPoint(DragStart);

			if (clickedElement == this)
			{
				Dragging = true;
				Environment->setFocus(this);
				if (Parent)
					Parent->bringToFront(this);
				return true;
			}
			else if (clickedElement == ResizeButton)
			{
				Resizing = true;
				Environment->setFocus(this);
				if (Parent)
					Parent->bringToFront(this);
				return true;
			}
			break;
		}
		case EMIE_LMOUSE_LEFT_UP:
			if (Dragging || Resizing)
			{
				Dragging = false;
				Environment->removeFocus(this);
				return true;
			}
			break;
		case EMIE_MOUSE_MOVED:
			if (Dragging || Resizing)
			{
				// gui window should not be dragged outside of its parent
				if (Parent)
					if (event.MouseInput.X < Parent->getAbsolutePosition().UpperLeftCorner.X +1 ||
						event.MouseInput.Y < Parent->getAbsolutePosition().UpperLeftCorner.Y +1 ||
						event.MouseInput.X > Parent->getAbsolutePosition().LowerRightCorner.X -1 ||
						event.MouseInput.Y > Parent->getAbsolutePosition().LowerRightCorner.Y -1)

						return true;
				core::position2di diff(event.MouseInput.X - DragStart.X, event.MouseInput.Y - DragStart.Y);
				if (Dragging)
				{
					move(diff);
					DragStart.X = event.MouseInput.X;
					DragStart.Y = event.MouseInput.Y;
				}
				else if (Resizing)
				{
					core::position2di dp = RelativeRect.LowerRightCorner + diff;
					setRelativePosition( core::rect<s32>(RelativeRect.UpperLeftCorner, dp));
					DragStart += dp - RelativeRect.LowerRightCorner + diff;
				}

				return true;
			}
			break;
		}
	}

	return Parent ? Parent->OnEvent(event) : false;
}






// we're supposed to supply these if we're creating an IGUIWindow
// but we don't need them so we'll just return null
IGUIButton* CGUIEditWindow::getCloseButton()	   {return 0;}
IGUIButton* CGUIEditWindow::getMinimizeButton() {return 0;}
IGUIButton* CGUIEditWindow::getMaximizeButton() {return 0;}
