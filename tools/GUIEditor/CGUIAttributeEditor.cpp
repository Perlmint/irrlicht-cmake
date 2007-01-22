
#include "CGUIAttributeEditor.h"
#include "IGUIEnvironment.h"

#include "IVideoDriver.h"
#include "IAttributes.h"
#include "IGUIFont.h"

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
	ScrollBar = environment->addScrollBar(false, rect<s32>(0, 0,100,100), this);
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

void CGUIAttributeEditor::setRelativePosition(const rect<s32>& r)
{
	IGUIElement::setRelativePosition(r);
	s32 w = Environment->getSkin()->getSize(EGDS_WINDOW_BUTTON_WIDTH);
	ScrollBar->setRelativePosition(rect<s32>( r.getWidth() - (s32)(w * 1.5f), 10, r.getWidth() - (s32)(w * 0.5f), r.getHeight() - 1));
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
				5 + Environment->getSkin()->getFont()->getDimension(L"A").Height * 2);

	// add attribute elements
	u32 c = Attribs->getAttributeCount();
	for (i=0; i<c; ++i)
	{
		AttribList.push_back(new CGUIAttribute(Environment, this, Attribs, i, r));
		// dont grab it because we created it with new
		AttribList[i]->setSubElement(true);
		AttribList[i]->setRelativePosition(r);
		AttribList[i]->setAlignment(EGUIA_UPPERLEFT, EGUIA_LOWERRIGHT, EGUIA_UPPERLEFT, EGUIA_UPPERLEFT);
		r += position2di(0, r.getHeight() + 2);
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
		AttribList[i]->updateAttrib();
}

void CGUIAttributeEditor::updateAbsolutePosition()
{
	// get real pos from desired pos
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
		AttribName(0), AttribEditBox(0), AttribCheckBox(0)
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
			rect<s32>(0, 0, r.getWidth(), r.getHeight()/2),
			false, false, this, -1, false);
	AttribName->grab();
	AttribName->setAlignment(EGUIA_UPPERLEFT, EGUIA_LOWERRIGHT, EGUIA_UPPERLEFT, EGUIA_UPPERLEFT);

	rect<s32> r2(0, r.getHeight()/2, r.getWidth(), r.getHeight());

	if (attribs->getAttributeType(attribIndex) == io::EAT_BOOL)
	{
		AttribCheckBox = environment->addCheckBox(
				attribs->getAttributeAsBool(attribIndex),
				r2, this);
		AttribCheckBox->grab();
	}
	else
	{
		AttribEditBox = environment->addEditBox(
				attribs->getAttributeAsStringW(attribIndex).c_str(),
				r2,
				true, this, -1);
		AttribEditBox->grab();
		AttribEditBox->setAlignment(EGUIA_UPPERLEFT, EGUIA_LOWERRIGHT, EGUIA_UPPERLEFT, EGUIA_UPPERLEFT);
	}
}

CGUIAttribute::~CGUIAttribute()
{
	Attribs->drop();
	AttribName->drop();
	if (AttribEditBox)
		AttribEditBox->drop();
	if (AttribCheckBox)
		AttribCheckBox->drop();
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
		case EGET_ELEMENT_FOCUS_LOST:
			updateAttrib();
			return true;
		}
		break;
	}
	return Parent->OnEvent(e);
}

void CGUIAttribute::updateAttrib()
{
	if (Attribs->getAttributeType(Index) == io::EAT_BOOL)
	{
		Attribs->setAttribute(Index, AttribCheckBox->isChecked());
	}
	else
	{
		Attribs->setAttribute(Index, AttribEditBox->getText());
		AttribEditBox->setText(Attribs->getAttributeAsStringW(Index).c_str());
	}
}
