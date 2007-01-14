#include "CGUIEditFactory.h"
#include "IGUIEnvironment.h"
#include "irrString.h"

#include "CGUIEditWorkspace.h"
#include "CGUIEditWindow.h"
#include "CGUIAttributeEditor.h"

namespace irr
{
namespace gui
{

enum EGUIEDIT_ELEMENT_TYPES
{
	EGUIEDIT_GUIEDIT=0,
	EGUIEDIT_GUIEDITWINDOW,
	EGUIEDIT_ATTRIBUTEEDITOR,
	EGUIEDIT_COUNT
};

const c8* const GUIEditElementTypeNames[] =
{
	"GUIEditor",
	"GUIEditWindow",
	"attributeEditor",
	0
};



CGUIEditFactory::CGUIEditFactory(IGUIEnvironment* env)
: Environment(env)
{
	#ifdef _DEBUG
	setDebugName("CGUIEditFactory");
	#endif

	// don't grab the gui environment here to prevent cyclic references
}


CGUIEditFactory::~CGUIEditFactory()
{
}


//! adds an element to the environment based on its type name
IGUIElement* CGUIEditFactory::addGUIElement(const c8* typeName, IGUIElement* parent)
{
	/*
		here we create elements, add them to the manager, and then drop them
	*/

	core::stringc elementType(typeName);
	IGUIElement* ret=0;
	if (parent == 0)
	{
		parent = Environment->getRootGUIElement();
	}

	// editor workspace
	if (elementType == core::stringc(GUIEditElementTypeNames[EGUIEDIT_GUIEDIT]))
		ret = new CGUIEditWorkspace(Environment, -1, parent);
	// editor window
	else if (elementType == core::stringc(GUIEditElementTypeNames[EGUIEDIT_GUIEDITWINDOW]))
		ret = new CGUIEditWindow(Environment, core::rect<s32>(0,0,100,100), parent);
	// attribute editor
	else if (elementType == core::stringc(GUIEditElementTypeNames[EGUIEDIT_ATTRIBUTEEDITOR]))
		ret = new CGUIAttributeEditor(Environment, -1, parent);

	// the environment now has the reference, so we can drop the element
	if (ret)
		ret->drop();

	return ret;
}


//! returns amount of element types this factory is able to create
s32 CGUIEditFactory::getCreatableGUIElementTypeCount()
{
	return EGUIEDIT_COUNT;
}


//! returns type name of a createable element type 
const c8* CGUIEditFactory::getCreateableGUIElementTypeName(s32 idx)
{
	if (idx>=0 && idx<EGUIEDIT_COUNT)
		return GUIEditElementTypeNames[idx];

	return 0;
}



} // end namespace gui
} // end namespace irr

