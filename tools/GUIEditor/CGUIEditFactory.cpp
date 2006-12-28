#include "CGUIEditFactory.h"
#include "IGUIEnvironment.h"
#include "irrString.h"

#include "CGUIEditWorkspace.h"

namespace irr
{
namespace gui
{

enum EGUIEdit_ELEMENT_TYPES
{
	EGUIEditT_GUIEdit=0,
	EGUIEditT_COUNT
};

const c8* const GUIEditElementTypeNames[] =
{
	"GUIEditor",
	0
};



CGUIEditFactory::CGUIEditFactory(IGUIEnvironment* env)
: Environment(env)
{
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
	IGUIElement *ret=0;
	if (parent == 0)
	{
		parent = Environment->getRootGUIElement();
	}

	if (elementType == core::stringc("GUIEditor"))
	{
		// create it
		ret = new CGUIEditWorkspace(Environment, -1, parent);
		// drop it
		ret->drop();
	}

	return ret;
}


//! returns amount of element types this factory is able to create
s32 CGUIEditFactory::getCreatableGUIElementTypeCount()
{
	return EGUIEditT_COUNT;
}


//! returns type name of a createable element type 
const c8* CGUIEditFactory::getCreateableGUIElementTypeName(s32 idx)
{
	if (idx>=0 && idx<EGUIEditT_COUNT)
		return GUIEditElementTypeNames[idx];

	return 0;
}



} // end namespace gui
} // end namespace irr

