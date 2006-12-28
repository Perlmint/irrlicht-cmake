
#include "CGUIEditWindow.h"
#include "IGUISkin.h"
#include "IGUIEnvironment.h"
#include "IGUIElementFactory.h"

// bah
#include "IVideoDriver.h"
#include "d:/svn/irrlicht/source/irrlicht/CAttributes.h"

using namespace irr;
using namespace gui;

//! constructor
CGUIEditWindow::CGUIEditWindow(IGUIEnvironment* environment, core::rect<s32> rectangle, IGUIElement *parent)
		 : IGUIWindow(environment, parent, -1, rectangle),
		 Dragging(false), SelectedElement(0), Attribs(),
		
		TabControl(0), FileTab(0), EditTab(0), MiscTab(0),
		
		ClearGUI(0), LoadGUI(0), SaveGUI(0),

		AttribList(0), DeleteAttrib(0), AddAttrib(0),
		SaveAttribs(0), RefreshAttribs(0), SetParent(0), AttribEditor(0)


{
	// set window text
	setText(L"GUI Creator");

	// return if we have no skin.
	IGUISkin *skin = environment->getSkin();
	if (!skin)
		return;

	s32 th = skin->getSize(EGDS_WINDOW_BUTTON_WIDTH);

	setRelativePosition(core::rect<s32>(50,50,250,350));

	// add the tab control and tabs
	TabControl = environment->addTabControl( core::rect<s32>(1,th+5,249,350-th-5), this);
	FileTab	= TabControl->addTab(L"File");
	EditTab	= TabControl->addTab(L"Edit");
	MiscTab	= TabControl->addTab(L"Misc");
	TabControl->setSubElement(true);

	//
	// File tab
	//

	core::position2di nextButton = core::position2di(0,skin->getSize(EGDS_BUTTON_HEIGHT)+th);
	core::rect<s32> r = core::rect<s32>(th,th*2,th+skin->getSize(EGDS_BUTTON_WIDTH),th*2 +skin->getSize(EGDS_BUTTON_HEIGHT));

	ClearGUI	=	environment->addButton( r, FileTab, -1, L"Clear");
	r += nextButton;

	LoadGUI		=	environment->addButton( r, FileTab, -1, L"Load");
	r += nextButton;

	SaveGUI		=	environment->addButton( r, FileTab, -1, L"Save");

	//
	// New tab
	//
/*
	// add combo box for node types...
	TypeCombo = environment->addComboBox(core::rect<s32>(th,th*2,150,th*3),NewTab);

	core::stringw tmpStr(environment->getRegisteredGUIElementFactoryCount());
	tmpStr += L" factories loaded containing ";
	// count total elements
	s32 nCount=0;
	s32 i,j;
	
	for (i=0; i<environment->getRegisteredGUIElementFactoryCount(); ++i)
	{
		IGUIElementFactory *f = environment->getGUIElementFactory(i);

		nCount += f->getCreatableGUIElementTypeCount();
		for (j=0; j<f->getCreatableGUIElementTypeCount(); ++j)
			TypeCombo->addItem(core::stringw(f->getCreateableGUIElementTypeName(j)).c_str());
	}

	tmpStr += nCount;
	tmpStr += L" elements.";
	environment->addStaticText(tmpStr.c_str(), core::rect<s32>(10,10,200,th+10), false, false, NewTab );

	// 'New' button
	CreateNew = environment->addButton( core::rect<s32>(th,th*4,th+skin->getSize(EGDS_BUTTON_WIDTH),th*4 +skin->getSize(EGDS_BUTTON_HEIGHT)), NewTab, -1, L"New");
*/

	//
	// Edit tab
	//

	r = core::rect<s32>(150+th,th*4,150+th*2,th*5);
	AttribList		= environment->addListBox( core::rect<s32>(th,th*4,150,th*7), EditTab);
	AddAttrib		= environment->addButton( r, EditTab, -1, L"+");
	r += core::position2di(0,th*2);
	DeleteAttrib	= environment->addButton( r, EditTab, -1, L"-");
	
	r = core::rect<s32>(th,10, th*4, th*2-10);
	SaveAttribs		= environment->addButton( r, EditTab, -1, L"Save");
	r += core::position2di(th*4,0);
	RefreshAttribs	= environment->addButton( r, EditTab, -1, L"Refresh");
	r += core::position2di(th*4,0);
	SetParent		= environment->addButton( r, EditTab, -1, L"Set Parent");

	Attribs = new irr::io::CAttributes(environment->getVideoDriver());

}

//! destructor
CGUIEditWindow::~CGUIEditWindow()
{
	// drop everything

	// drop attributes
	Attribs->drop();
}



void CGUIEditWindow::setSelectedElement(IGUIElement *sel)
{
	// switch to editor tab
	TabControl->setActiveTab(EditTab);

	if (SelectedElement && sel != SelectedElement)
	{
		// serialize attributes
		SelectedElement->deserializeAttributes(Attribs);

	}
	// clear the attributes list
	Attribs->clear();
	SelectedElement = sel;

	// get the new attributes 
	if (SelectedElement)
	{
		SelectedElement->serializeAttributes(Attribs);
	}

	// update the attribute list
	updateAttributeList();

}

void CGUIEditWindow::updateAttributeList()
{
	AttribList->clear();
	s32 c = Attribs->getAttributeCount();
	s32 i;
	for (i=0; i<c; ++i)
		AttribList->addItem( core::stringw(Attribs->getAttributeName(i)).c_str());
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

		case EGET_LISTBOX_CHANGED:
			if (event.GUIEvent.Caller == AttribList)
			{
				//AttribEditor->saveAttribute(Attribs);
				//SelectedElement->deserializeAttributes(Attribs);

				if (AttribList->getSelected()>0)
				{/*
					core::stringc s = core::stringc(AttribList->getListItem(AttribList->getSelected()) );
					s32 i = Attribs->findAttribute(s.c_str());
					if (i>0)
					{
						// set type name
						AttribEditor->setAttribName(Attribs->getAttributeName());
						
						// set attribute by type
						switch (Attribs->getAttributeType(i))
						{
							case io::EAT_ARRAY:
								AttribEditor->setArray(Attribs->getAttributeAsArray(i));
								break;

							case io::EAT_BINARY;
								Attribs->getAttributeAsBinaryData(i,(void*)tmpData,1024);
								AttribEditor->setBinary(tmpData,1024);
								
								break;

							case io::EAT_BOOL;
								AttribEditor->setBool(Attribs->getAttributeAsBool(i));
								break;

							case io::EAT_COLOR;
								AttribEditor->setColor(Attribs->getAttributeAsColor(i));
								break;

							case io::EAT_COLORF;
								AttribEditor->setColorf(Attribs->getAttributeAsColorf(i));
								break;

							case io::EAT_ENUM;
								//AttribEditor->setEnum(Attribs->getAttributeAsEnumeration(i));
								AttribEditor->setUnsupported();
								break;

							case io::EAT_FLOAT;
								AttribEditor->setFloat(Attribs->getAttributeAsFloat(i));
								break;

							case io::EAT_FONT;
								//AttribEditor->setFont(Attribs->getAttributeAsFont(i));
								AttribEditor->setUnsupported();
								break;

							case io::EAT_INT;
								AttribEditor->setInt(Attribs->getAttributeAsInt(i));
								break;

							case io::EAT_POSITION2D;
								AttribEditor->setPosition(Attribs->getAttributeAsPosition2d(i));
								break;

							case io::EAT_RECT;
								AttribEditor->setRect(Attribs->getAttributeAsRect(i));
								break;

							case io::EAT_STRING;
								AttribEditor->setString(Attribs->getAttributeAsString(i));
								break;

							case io::EAT_TEXTURE;
								AttribEditor->setTexture(Attribs->getAttributeAsTexture(i));
								break;

							case io::EAT_UNKNOWN;
								AttribEditor->setUnsupported();
								break;
						}
					}
					else
					{
						AttribEditor->setAttribute(0);
					}*/

					DeleteAttrib->setEnabled(true);
				}
				else
				{
					// AttribEditor->setAttribute(0);
					// disable delete button
					DeleteAttrib->setEnabled(false);
				}
				return true;
			}
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
// but we don't need them so we'll just return null (0)
IGUIButton* CGUIEditWindow::getCloseButton()	   {return 0;}
IGUIButton* CGUIEditWindow::getMinimizeButton() {return 0;}
IGUIButton* CGUIEditWindow::getMaximizeButton() {return 0;}
