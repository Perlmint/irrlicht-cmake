#ifndef __C_GUI_ATTRIBUTE_EDITOR_H_INCLUDED__
#define __C_GUI_ATTRIBUTE_EDITOR_H_INCLUDED__

#include "IGUIElement.h"
#include "IGUIStaticText.h"
#include "IGUIEditBox.h"
#include "IGUICheckBox.h"
#include "IGUIComboBox.h"
#include "IGUIScrollBar.h"
#include "irrArray.h"
#include "IAttributes.h"

namespace irr
{
namespace gui
{

	class CGUIAttribute : public IGUIElement
	{
	public:
		//
		CGUIAttribute(IGUIEnvironment* environment, IGUIElement *parent,
				io::IAttributes *attribs, u32 attribIndex, core::rect<s32> r);
		//
		~CGUIAttribute();

		virtual bool OnEvent(SEvent event);

		// save the attribute and possibly post the event to its parent
		bool updateAttrib(bool sendEvent=true);

		//! this shoudln't be serialized, but this is included as it's an example
		virtual const c8* getTypeName() { return "attribute"; }

	private:
		io::IAttributes*	Attribs;
		u32					Index;
		IGUIStaticText*		AttribName;
		IGUIEditBox*		AttribEditBox;
		IGUICheckBox*		AttribCheckBox;
		IGUIComboBox*		AttribComboBox;
		IGUIScrollBar*		AttribSliderA;
		IGUIScrollBar*		AttribSliderR;
		IGUIScrollBar*		AttribSliderG;
		IGUIScrollBar*		AttribSliderB;
	};

	class CGUIAttributeEditor : public IGUIElement
	{
	public:

		//! constructor
		CGUIAttributeEditor(IGUIEnvironment* environment, s32 id, IGUIElement *parent=0);

		//! destructor
		~CGUIAttributeEditor();

		//! handles events
		virtual bool OnEvent(SEvent event);

		// gets the current attributes list
		virtual io::IAttributes* getAttribs();

		// update the attribute list after making a change
		void refreshAttribs();

		// save the attributes
		void updateAttribs();

		//! update scrollbar and stuff
		virtual void updateAbsolutePosition();

		//! this shoudln't be serialized, but this is included as it's an example
		virtual const c8* getTypeName() { return "attributeEditor"; }

	private:

		core::array<CGUIAttribute*>	AttribList;	// attributes editing controls
		io::IAttributes*			Attribs;	// current attributes
		IGUIScrollBar*				ScrollBar;
		s32							LastOffset; // distance to move children when scrollbar is changed

	};

} // end namespace gui
} // end namespace irr

#endif // __C_GUI_ATTRIBUTE_EDITOR_H_INCLUDED__

