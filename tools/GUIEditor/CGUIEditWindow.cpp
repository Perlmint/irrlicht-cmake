
#include "CGUIEditWindow.h"
#include "IGUISkin.h"
#include "IGUIEnvironment.h"
#include "IGUIElementFactory.h"
#include "IAttributes.h"

using namespace irr;
using namespace gui;

//! constructor
CGUIEditWindow::CGUIEditWindow(IGUIEnvironment* environment, core::rect<s32> rectangle, IGUIElement *parent)
		 : IGUIWindow(environment, parent, -1, rectangle),
		 Dragging(false), SelectedElement(0), AttribEditor()

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

	AttribEditor = (CGUIAttributeEditor*) environment->addGUIElement("attributeEditor",this);
	AttribEditor->grab();
	AttribEditor->setRelativePosition(core::rect<s32>(1,th+5,200,450-th-5));
	//AttribEditor->setSubElement(true);
}

//! destructor
CGUIEditWindow::~CGUIEditWindow()
{
	// drop everything
	AttribEditor->drop();
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
	core::rect<s32> *cl = &AbsoluteClippingRect;

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
			font->draw(Text.c_str(), rect, skin->getColor(EGDC_ACTIVE_CAPTION), false, true, cl);
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
			DragStart.X = event.MouseInput.X;
			DragStart.Y = event.MouseInput.Y;
			if (!Environment->hasFocus(this))
			{
				Dragging = true;
				Environment->setFocus(this);
				if (Parent)
					Parent->bringToFront(this);
			}
			return true;
		case EMIE_LMOUSE_LEFT_UP:
			Dragging = false;
			Environment->removeFocus(this);
			return true;
		case EMIE_MOUSE_MOVED:
			if (Dragging)
			{
				move(core::position2d<s32>(event.MouseInput.X - DragStart.X, event.MouseInput.Y - DragStart.Y));
				DragStart.X = event.MouseInput.X;
				DragStart.Y = event.MouseInput.Y;
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
