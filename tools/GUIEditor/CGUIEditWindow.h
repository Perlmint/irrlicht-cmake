#ifndef __C_GUI_EDITOR_H_INCLUDED__
#define __C_GUI_EDITOR_H_INCLUDED__


#include "IGUIWindow.h"
#include "CGUIAttributeEditor.h"
#include "irrArray.h"
#include "IAttributes.h"

namespace irr
{
namespace gui
{
	class CGUIEditWindow : public IGUIWindow
	{
	public:

		//! constructor
		CGUIEditWindow(IGUIEnvironment* environment, core::rect<s32> rectangle, IGUIElement *parent);

		//! this part draws the window
		virtual void draw();
		//! handles events
		virtual bool OnEvent(SEvent event);

		//! change selection
		virtual void setSelectedElement(IGUIElement *sel);

		// not used
		virtual IGUIButton* getCloseButton();
		virtual IGUIButton* getMinimizeButton();
		virtual IGUIButton* getMaximizeButton();

		//! destructor
		~CGUIEditWindow();

	private:


		// for dragging the window
		bool					Dragging;
		core::position2d<s32>	DragStart;

		IGUIElement				*SelectedElement; // current selected element

		CGUIAttributeEditor		*AttribEditor;	// edits the current attribute

	};

} // end namespace gui
} // end namespace irr

#endif // __C_GUI_EDITOR_H_INCLUDED__

