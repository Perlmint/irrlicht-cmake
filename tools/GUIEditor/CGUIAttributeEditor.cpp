
#include "CGUIAttributeEditor.h"
#include "IGUIEnvironment.h"

#include "IVideoDriver.h"
#include "IAttributes.h"
#include "IGUIFont.h"
#include "CGUIEditWorkspace.h"

using namespace irr;
using namespace core;
using namespace gui;
using namespace io;

CGUIAttributeEditor::CGUIAttributeEditor(IGUIEnvironment* environment, s32 id, IGUIElement *parent) :
	IGUIElement(EGUIET_ELEMENT, environment, parent, id, rect<s32>(0,0,100,100)),
		Attribs(0), LastOffset(0)
{
	#ifdef _DEBUG
	setDebugName("CGUIAttributeEditor");
	#endif

	// create attributes
	Attribs = environment->getFileSystem()->createEmptyAttributes(Environment->getVideoDriver());
	// add scrollbar
	ScrollBar = environment->addScrollBar(false, rect<s32>(84, 5, 99, 85), this);
	ScrollBar->setAlignment(EGUIA_LOWERRIGHT, EGUIA_LOWERRIGHT, EGUIA_UPPERLEFT, EGUIA_LOWERRIGHT);
	ScrollBar->grab();
	ScrollBar->setSubElement(true);

	refreshAttribs();
}

CGUIAttributeEditor::~CGUIAttributeEditor()
{
	u32 i;
	for (i=0; i<AttribList.size(); ++i)
	{
		AttribList[i]->remove();
		AttribList[i]->drop();
	}
	AttribList.clear();

	Attribs->drop();
	ScrollBar->drop();
}

bool CGUIAttributeEditor::OnEvent(SEvent e)
{

	switch (e.EventType)
	{

	case EET_GUI_EVENT:
		switch (e.GUIEvent.EventType)
		{
		case EGET_SCROLL_BAR_CHANGED:
			{
				// set the offset of every attribute
				s32 diff = LastOffset - ScrollBar->getPos();
				for (u32 i=0; i<AttribList.size(); ++i)
					AttribList[i]->setRelativePosition(AttribList[i]->getRelativePosition() + position2di(0,diff));

				LastOffset = ScrollBar->getPos();
				return true;
			}
		}
		break;
	case EET_MOUSE_INPUT_EVENT:
		switch (e.MouseInput.Event)
		{
		case EMIE_MOUSE_WHEEL:
			{
				ScrollBar->setPos(ScrollBar->getPos() - (s32)(e.MouseInput.Wheel)*20);
				
				s32 diff = LastOffset - ScrollBar->getPos();
				for (u32 i=0; i<AttribList.size(); ++i)
					AttribList[i]->setRelativePosition(AttribList[i]->getRelativePosition() + position2di(0,diff));

				LastOffset = ScrollBar->getPos();
				return true;
			}
		}
		break;
	}
	return Parent->OnEvent(e);
}

IAttributes* CGUIAttributeEditor::getAttribs()
{
	return Attribs;
}

void CGUIAttributeEditor::refreshAttribs()
{
	// clear the attribute list
	u32 i;
	for (i=0; i<AttribList.size(); ++i)
	{
		AttribList[i]->remove();
		AttribList[i]->drop();
	}
	AttribList.clear();
	position2di top(10,5);
	rect<s32> r(top.X,
				top.Y,
				AbsoluteRect.getWidth() - Environment->getSkin()->getSize(EGDS_WINDOW_BUTTON_WIDTH) * 2,
				5 + Environment->getSkin()->getFont()->getDimension(L"A").Height);

	// add attribute elements
	u32 c = Attribs->getAttributeCount();
	for (i=0; i<c; ++i)
	{
		AttribList.push_back(new CGUIAttribute(Environment, this, Attribs, i, r));
		// dont grab it because we created it with new
		AttribList[i]->setSubElement(true);
		AttribList[i]->setRelativePosition(r);
		AttribList[i]->setAlignment(EGUIA_UPPERLEFT, EGUIA_LOWERRIGHT, EGUIA_UPPERLEFT, EGUIA_UPPERLEFT);
		r += position2di(0, AttribList[i]->getRelativePosition().getHeight() + 5);
	}

	if (r.UpperLeftCorner.Y > RelativeRect.getHeight())
	{
		ScrollBar->setVisible(true);
		ScrollBar->setMax(r.UpperLeftCorner.Y - RelativeRect.getHeight());
		LastOffset = ScrollBar->getPos();
	}
	else
	{
		ScrollBar->setVisible(false);
		ScrollBar->setPos(0);
		LastOffset = 0;
	}

}
void CGUIAttributeEditor::updateAttribs()
{
	for (u32 i=0; i<AttribList.size(); ++i)
		AttribList[i]->updateAttrib(false);
}

void CGUIAttributeEditor::updateAbsolutePosition()
{
	// get real position from desired position
	IGUIElement::updateAbsolutePosition();

	s32 p=0;
	// get lowest position
	if (AttribList.size())
		p = AttribList[AttribList.size() - 1]->getRelativePosition().LowerRightCorner.Y + ScrollBar->getPos();

	p -= RelativeRect.getHeight();

	if (p > 1)
	{
		ScrollBar->setMax(p);
		ScrollBar->setVisible(true);
	}
	else
	{
		ScrollBar->setMax(0);
		ScrollBar->setVisible(false);
	}
}


//
// Attribute
//


CGUIAttribute::CGUIAttribute(IGUIEnvironment* environment, IGUIElement *parent,
							io::IAttributes *attribs, u32 attribIndex, rect<s32> r) :
	IGUIElement(EGUIET_ELEMENT, environment, parent, -1, r),
		Attribs(attribs), Index(attribIndex),
		AttribName(0), AttribEditBox(0), AttribCheckBox(0), AttribComboBox(0)
{
	#ifdef _DEBUG
	setDebugName("CGUIAttribute");
	#endif

	attribs->grab();

	stringw name(attribs->getAttributeName(attribIndex));
	name += L" (";
	name += stringw(attribs->getAttributeTypeString(attribIndex));
	name += L")";

	AttribName = environment->addStaticText(
			name.c_str(),
			rect<s32>(0, 0, r.getWidth(), Environment->getSkin()->getFont()->getDimension(L"A").Height),
			false, false, this, -1, false);
	AttribName->grab();
	AttribName->setAlignment(EGUIA_UPPERLEFT, EGUIA_LOWERRIGHT, EGUIA_UPPERLEFT, EGUIA_UPPERLEFT);

	rect<s32> r2(0, Environment->getSkin()->getFont()->getDimension(L"A").Height + 5, 
		r.getWidth(), Environment->getSkin()->getFont()->getDimension(L"A").Height*2 + 10 );

	io::E_ATTRIBUTE_TYPE aType = attribs->getAttributeType(attribIndex);

	switch(aType)
	{
	case io::EAT_BOOL:
		{
			AttribCheckBox = environment->addCheckBox(
					attribs->getAttributeAsBool(attribIndex),
					r2, this);
			AttribCheckBox->grab();
		}
		break;
	case io::EAT_ENUM:
		{
			core::array<core::stringc> outLiterals;
			attribs->getAttributeEnumerationLiteralsOfEnumeration(attribIndex, outLiterals);

			if (outLiterals.size() > 0)
			{
				AttribComboBox = environment->addComboBox(r2, this, -1);
				for (u32 i=0; i<outLiterals.size(); ++i)
					AttribComboBox->addItem( core::stringw(outLiterals[i].c_str()).c_str());

				AttribComboBox->setSelected( attribs->getAttributeAsInt(attribIndex) );

				AttribComboBox->grab();
				AttribComboBox->setAlignment(EGUIA_UPPERLEFT, EGUIA_LOWERRIGHT, EGUIA_UPPERLEFT, EGUIA_UPPERLEFT);
			}
			else
			{

				AttribEditBox = environment->addEditBox(
						attribs->getAttributeAsStringW(attribIndex).c_str(),
						r2, true, this, -1);
				AttribEditBox->grab();
				AttribEditBox->setAlignment(EGUIA_UPPERLEFT, EGUIA_LOWERRIGHT, EGUIA_UPPERLEFT, EGUIA_UPPERLEFT);
			}
		}
		break;
	case io::EAT_COLOR:
	case io::EAT_COLORF:
		{
			video::SColor col = attribs->getAttributeAsColor(attribIndex);

			core::rect<s32> r3 = r2;
			s32 h=5;
			r2 += position2di(0, h*4 + Environment->getSkin()->getSize(EGDS_WINDOW_BUTTON_WIDTH)*4);
			r3.LowerRightCorner.Y = r3.UpperLeftCorner.Y + Environment->getSkin()->getSize(EGDS_WINDOW_BUTTON_WIDTH);

			AttribSliderA = environment->addScrollBar(true, r3, this, -1);
			AttribSliderA->setMax(255);
			AttribSliderA->setPos(col.getAlpha());
			AttribSliderA->setAlignment(EGUIA_UPPERLEFT, EGUIA_LOWERRIGHT, EGUIA_UPPERLEFT, EGUIA_UPPERLEFT);
			r3 += position2di(0, r3.getHeight()+h);
			AttribSliderR = environment->addScrollBar(true, r3, this, -1);
			AttribSliderR->setMax(255);
			AttribSliderR->setPos(col.getRed());
			AttribSliderR->setAlignment(EGUIA_UPPERLEFT, EGUIA_LOWERRIGHT, EGUIA_UPPERLEFT, EGUIA_UPPERLEFT);
			r3 += position2di(0, r3.getHeight()+h);
			AttribSliderG = environment->addScrollBar(true, r3, this, -1);
			AttribSliderG->setMax(255);
			AttribSliderG->setPos(col.getGreen());
			AttribSliderG->setAlignment(EGUIA_UPPERLEFT, EGUIA_LOWERRIGHT, EGUIA_UPPERLEFT, EGUIA_UPPERLEFT);
			r3 += position2di(0, r3.getHeight()+h);
			AttribSliderB = environment->addScrollBar(true, r3, this, -1);
			AttribSliderB->setMax(255);
			AttribSliderB->setPos(col.getBlue());
			AttribSliderB->setAlignment(EGUIA_UPPERLEFT, EGUIA_LOWERRIGHT, EGUIA_UPPERLEFT, EGUIA_UPPERLEFT);

			// add editbox
			AttribEditBox = environment->addEditBox(
					attribs->getAttributeAsStringW(attribIndex).c_str(),
					r2,
					true, this, -1);
			AttribEditBox->grab();
			AttribEditBox->setAlignment(EGUIA_UPPERLEFT, EGUIA_LOWERRIGHT, EGUIA_UPPERLEFT, EGUIA_UPPERLEFT);
		}
		break;
	default:
		{
			AttribEditBox = environment->addEditBox(
					attribs->getAttributeAsStringW(attribIndex).c_str(),
					r2,
					true, this, -1);
			AttribEditBox->grab();
			AttribEditBox->setAlignment(EGUIA_UPPERLEFT, EGUIA_LOWERRIGHT, EGUIA_UPPERLEFT, EGUIA_UPPERLEFT);
		}
		break;
	}

	// get minimum height
	s32 y=0;
	core::list<IGUIElement*>::Iterator it = Children.begin();
	for (; it != Children.end(); ++it)
	{
		if (y < (*it)->getRelativePosition().LowerRightCorner.Y)
			y = (*it)->getRelativePosition().LowerRightCorner.Y;
	}
	setMinSize( core::dimension2di(50, y+5));	

}

CGUIAttribute::~CGUIAttribute()
{
	Attribs->drop();
	AttribName->drop();
	if (AttribEditBox)
		AttribEditBox->drop();
	if (AttribCheckBox)
		AttribCheckBox->drop();
	if (AttribComboBox)
		AttribComboBox->drop();
}


bool CGUIAttribute::OnEvent(SEvent e)
{

	switch (e.EventType)
	{

	case EET_GUI_EVENT:
		switch (e.GUIEvent.EventType)
		{
		case EGET_EDITBOX_ENTER:
		case EGET_CHECKBOX_CHANGED:
		case EGET_COMBO_BOX_CHANGED:
		case EGET_ELEMENT_FOCUS_LOST:
			if ((Attribs->getAttributeType(Index) == io::EAT_COLOR || 
				Attribs->getAttributeType(Index) == io::EAT_COLORF) &&
				e.GUIEvent.Caller == AttribEditBox)
			{
				// update scrollbars from textbox
				Attribs->setAttribute(Index, AttribEditBox->getText());
				video::SColor col = Attribs->getAttributeAsColor(Index);
				AttribSliderA->setPos(col.getAlpha());
				AttribSliderR->setPos(col.getRed()); 
				AttribSliderG->setPos(col.getGreen());
				AttribSliderB->setPos(col.getBlue());
			}
			return updateAttrib();
		case EGET_SCROLL_BAR_CHANGED:
			if (Attribs->getAttributeType(Index) == io::EAT_COLOR || 
				Attribs->getAttributeType(Index) == io::EAT_COLORF)
			{
				video::SColor col(
					AttribSliderA->getPos(), 
					AttribSliderR->getPos(), 
					AttribSliderG->getPos(),
					AttribSliderB->getPos());

				Attribs->setAttribute(Index, col);
				AttribEditBox->setText( Attribs->getAttributeAsStringW(Index).c_str());
			}
			return updateAttrib();
		case EGET_ELEMENT_FOCUSED:
			if (Parent)
				Parent->bringToFront(this);
			break;
		}
		break;
	case EET_KEY_INPUT_EVENT:
		return true;
	}
	return Parent->OnEvent(e);
}

bool CGUIAttribute::updateAttrib(bool sendEvent)
{
	io::E_ATTRIBUTE_TYPE eType = Attribs->getAttributeType(Index);
	switch(eType)
	{
		case io::EAT_BOOL:
			Attribs->setAttribute(Index, AttribCheckBox->isChecked());
			break;
	
		case io::EAT_ENUM:
			Attribs->setAttribute(Index, AttribComboBox->getText());
			break;
		case io::EAT_COLOR:
		case io::EAT_COLORF:
		default:
			Attribs->setAttribute(Index, AttribEditBox->getText());
			AttribEditBox->setText(Attribs->getAttributeAsStringW(Index).c_str());
	}
	if (sendEvent)
	{
		// build event and pass to parent
		SEvent event;
		event.EventType = EET_USER_EVENT;
		event.UserEvent.UserData1 = EGUIEDCE_ATTRIBUTE_CHANGED;
		event.UserEvent.UserData2 = Parent->getID();
		event.UserEvent.UserData3 = (f32)Index;
		return Parent->OnEvent(event);
	}

	return true;
}
