#ifndef __C_GUI_EDITOR_H_INCLUDED__
#define __C_GUI_EDITOR_H_INCLUDED__


#include "IGUITabControl.h"
#include "IGUIWindow.h"
#include "IGUIComboBox.h"
#include "IGUIListBox.h"
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
		virtual bool CGUIEditWindow::OnEvent(SEvent event);

		//! change selection
		virtual void setSelectedElement(IGUIElement *sel);

		// not used
		virtual IGUIButton* getCloseButton();
		virtual IGUIButton* getMinimizeButton();
		virtual IGUIButton* getMaximizeButton();

		//! destructor
		~CGUIEditWindow();

	private:

		void updateAttributeList();

		// for dragging the window
		bool					Dragging;
		core::position2d<s32>	DragStart;

		IGUIElement				*SelectedElement; // current selected element
		io::IAttributes			*Attribs;		  // attributes of the selected element

		// child elements
		IGUITabControl			*TabControl;	// main tab control
		IGUITab					*FileTab,		// ... for load/save/new
								*NewTab,		// ... for creating a new element
								*EditTab,		// ... for editing elements
								*MiscTab;		// ... for setting options etc

		// file tab children
		IGUIButton				*ClearGUI,		// remove all elements from the gui
								*LoadGUI,		// load a new gui from disk
								*SaveGUI;		// save the gui to disk

		// edit tab children
		IGUIListBox				*AttribList;	// Lists all attributes
		IGUIButton				*DeleteAttrib,	// remove an attribute
								*AddAttrib,		// add a new attribute
								*SaveAttribs,	// deserialize attributes
								*RefreshAttribs,// serialize attributes
								*SetParent;		// sets the element's parent
		IGUIElement				*AttribEditor;	// edits the current attribute

	};

} // end namespace gui
} // end namespace irr

#endif // __C_GUI_EDITOR_H_INCLUDED__