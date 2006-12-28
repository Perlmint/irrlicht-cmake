// Copyright (C) 2002-2006 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "CGUIEditWorkspace.h"
#include "IGUIEnvironment.h"
#include "IVideoDriver.h"
#include "IGUISkin.h"
#include "IGUIElementFactory.h"
#include "CGUIEditWindow.h"
#include "IGUIContextMenu.h"
#include "IGUIFileOpenDialog.h"

namespace irr
{
namespace gui
{

//! constructor
CGUIEditWorkspace::CGUIEditWorkspace(IGUIEnvironment* environment, s32 id, IGUIElement *parent)
: IGUIElement(EGUIET_ELEMENT, environment, parent ? parent : environment->getRootGUIElement(), id, environment->getRootGUIElement()->getAbsolutePosition()),
	DrawGrid(false), UseGrid(true), GridSize(10,10), MenuCommandStart(0x3D17),
	CurrentMode(EGUIEDM_SELECT), MouseOverMode(EGUIEDM_SELECT),
	MouseOverElement(0), SelectedElement(0), EditorWindow(0)
{
	#ifdef _DEBUG
	setDebugName("CGUIEditWorkspace");
	#endif

	EditorWindow = new CGUIEditWindow(environment, core::rect<s32>(0,0,100,100), this);
	EditorWindow->setSubElement(true);

	environment->setFocus(EditorWindow);
	
}


//! destructor
CGUIEditWorkspace::~CGUIEditWorkspace()
{
	EditorWindow->drop();
}


void CGUIEditWorkspace::setMenuCommandIDStart(s32 id)
{
	MenuCommandStart = id;
}

CGUIEditWorkspace::EGUIEDIT_MODE CGUIEditWorkspace::getModeFromPos(core::position2di p)
{
	s32 accuracy = 4; // TODO: accuracy

	if (SelectedElement)
	{
		core::rect<s32>		r = SelectedElement->getAbsolutePosition();

		if		( p == r.UpperLeftCorner)
			return EGUIEDM_RESIZE_TL;
		else if ( p == core::position2di(r.UpperLeftCorner.Y, r.LowerRightCorner.X) )
			return EGUIEDM_RESIZE_TR;
		else if ( p == core::position2di(r.LowerRightCorner.Y, r.UpperLeftCorner.X) )
			return EGUIEDM_RESIZE_BL;
		else if ( p == r.LowerRightCorner )
			return EGUIEDM_RESIZE_BR;
		else if ( p.Y == r.UpperLeftCorner.Y )
			return EGUIEDM_RESIZE_T;
		else if ( p.Y == r.LowerRightCorner.Y )
			return EGUIEDM_RESIZE_B;
		else if ( p.X == r.UpperLeftCorner.X )
			return EGUIEDM_RESIZE_L;
		else if ( p.X == r.LowerRightCorner.X )
			return EGUIEDM_RESIZE_R;
		else
			return EGUIEDM_MOVE;
	}
	
	return EGUIEDM_SELECT;

}

IGUIElement* CGUIEditWorkspace::getEditableElementFromPoint(IGUIElement *start, core::position2di &point, s32 index )
{
	IGUIElement* target = 0;

	// we have to search from back to front.

	core::list<IGUIElement*>::Iterator it = start->getChildren().getLast();
	s32 count=0;
	if (start->isVisible() && !start->isSubElement())
		while(it != start->getChildren().end())
		{
			target = getEditableElementFromPoint((*it),point);
			if (target)
			{
				if (!target->isSubElement() && target->isVisible() && !isMyChild(target) && target != this)
				{
					if (index == count)
						return target;
					else
						count++;
				}
				else
					target = 0;
			}

			--it;
		}

	if (start->getAbsolutePosition().isPointInside(point))
		target = start;
	
	return target;

}

bool CGUIEditWorkspace::isMyChild(IGUIElement* target)
{
	if (!target)
		return false;

	IGUIElement *current = target;
	while(current->getParent())
	{
		current = current->getParent();
		if (current == this)
			return true;
	}

	return false;
}

void CGUIEditWorkspace::setSelectedElement(IGUIElement *sel)
{
	IGUIElement* focus = Environment->getFocus();
	// we only give focus back to children
	if (!isMyChild(focus))
		focus = 0;

	if (SelectedElement != Parent)
	{
		if (SelectedElement != sel)
		{
			EditorWindow->setSelectedElement(sel);
			SelectedElement = sel;
		}
	}
	else
		SelectedElement = 0;

	Environment->setFocus(focus);
}

IGUIElement* CGUIEditWorkspace::getSelectedElement()
{
	return SelectedElement;
}


//! called if an event happened.
bool CGUIEditWorkspace::OnEvent(SEvent event)
{
	IGUIFileOpenDialog* dialog=0;
    switch(event.EventType)
	{
	case EET_MOUSE_INPUT_EVENT:
		switch(event.MouseInput.Event)
		{
		case EMIE_MOUSE_WHEEL:
			{
				f32 wheel = event.MouseInput.Wheel;
				if (wheel > 0)
				{
					// select next element
				}
				else
				{
					// select previous element
				}
			}
			break;
		case EMIE_LMOUSE_PRESSED_DOWN:
			// hide the gui editor
			EditorWindow->setVisible(false);

			if (CurrentMode== EGUIEDM_SELECT)
			{
				// selecting an element...
				MouseOverElement = getEditableElementFromPoint(Parent, core::position2di(event.MouseInput.X,event.MouseInput.Y));

				if (MouseOverElement == Parent)
						MouseOverElement = 0;

				if (MouseOverElement && SelectedElement == MouseOverElement)
				{
					// start moving or dragging

					core::position2di p = core::position2di(event.MouseInput.X,event.MouseInput.Y);

					CurrentMode = getModeFromPos(p);

					if (CurrentMode == EGUIEDM_MOVE)
						StartMovePos = SelectedElement->getAbsolutePosition().UpperLeftCorner;

					DragStart	 = p;
					SelectedArea = SelectedElement->getAbsolutePosition();
				}

				setSelectedElement(MouseOverElement);
			}

			break;
		case EMIE_RMOUSE_PRESSED_DOWN:
			if (CurrentMode >= EGUIEDM_MOVE)
			{
				// cancel dragging
				CurrentMode = EGUIEDM_SELECT;
			}
			else
			{
				DragStart = core::position2di(event.MouseInput.X,event.MouseInput.Y);
				// root menu
				IGUIContextMenu* mnu = Environment->addContextMenu(core::rect<s32>(event.MouseInput.X, event.MouseInput.Y, event.MouseInput.Y+100, event.MouseInput.Y+100),this);
				mnu->addItem(L"File",-1,true,true);
				mnu->addItem(L"Edit",-1,true,true);
				mnu->addItem(L"View",-1,true,true);
				mnu->addItem(SelectedElement ? L"Add child" : L"Add" ,-1,true,true);

				// file menu
				IGUIContextMenu* sub = mnu->getSubMenu(0);
				
				sub->addItem(L"New",	MenuCommandStart + EGUIEDMC_FILE_NEW );
				sub->addItem(L"Load...",MenuCommandStart + EGUIEDMC_FILE_LOAD);
				sub->addItem(L"Save...",MenuCommandStart + EGUIEDMC_FILE_SAVE);

				// edit menu
				sub = mnu->getSubMenu(1);
				sub->addItem(L"Cut (ctrl+x)",   MenuCommandStart + EGUIEDMC_CUT_ELEMENT,	(SelectedElement != 0));
				sub->addItem(L"Copy (ctrl+x)",  MenuCommandStart + EGUIEDMC_COPY_ELEMENT,	(SelectedElement != 0));
				sub->addItem(L"Paste (ctrl+x)", MenuCommandStart + EGUIEDMC_PASTE_ELEMENT,	(CopyBuffer != ""));
				sub->addItem(L"Delete (del)",   MenuCommandStart + EGUIEDMC_DELETE_ELEMENT, (SelectedElement != 0));
				sub->addSeparator();
				sub->addItem(L"Set parent",		MenuCommandStart + EGUIEDMC_SET_PARENT,		(SelectedElement != 0));
				sub->addItem(L"Bring to front", MenuCommandStart + EGUIEDMC_BRING_TO_FRONT, (SelectedElement != 0));
				sub->addSeparator();
				sub->addItem(L"Save to XML...", MenuCommandStart + EGUIEDMC_SAVE_ELEMENT,	(SelectedElement != 0));

				// view menu

				sub = mnu->getSubMenu(2);
				sub->addItem(L"Grid",-1,true,true);
					IGUIContextMenu* sub2 = sub->getSubMenu(0);
					sub2->addItem( DrawGrid ? L"Hide grid" : L"Draw grid",	MenuCommandStart + EGUIEDMC_TOGGLE_GRID);
					sub2->addItem( UseGrid  ? L"Don't snap" : L"Snap",		MenuCommandStart + EGUIEDMC_TOGGLE_SNAP_GRID);
					sub2->addItem(L"Set size",								MenuCommandStart + EGUIEDMC_SET_GRID_SIZE);

				sub->addItem(EditorWindow->isVisible() ? L"Hide property editor" : L"Show property editor", MenuCommandStart + EGUIEDMC_TOGGLE_EDITOR);
				

				sub = mnu->getSubMenu(3);

				s32 i,j,c=0;
				sub->addItem(L"Default factory",-1,true, true);
				
				// add elements from each factory
				for (i=0; i < Environment->getRegisteredGUIElementFactoryCount(); ++i)
				{
					sub2 = sub->getSubMenu(i);

					IGUIElementFactory *f = Environment->getGUIElementFactory(i);

					for (j=0; j< f->getCreatableGUIElementTypeCount(); ++j)
					{
						sub2->addItem(core::stringw(f->getCreateableGUIElementTypeName(j)).c_str(), MenuCommandStart + EGUIEDMC_COUNT + c);
						c++;
					}

					if (i+1 < Environment->getRegisteredGUIElementFactoryCount())
					{
						core::stringw strFact;
						strFact = L"Factory ";
						strFact += i+1;
						sub->addItem(strFact.c_str(),-1, true, true);
					}
				}
				sub->addSeparator();
				sub->addItem(L"From XML...", MenuCommandStart + EGUIEDMC_INSERT_XML);

				// add menu

				Environment->setFocus(mnu);

				// create menu
			}
			break;
		case EMIE_LMOUSE_LEFT_UP:

			// make window visible again
			EditorWindow->setVisible(true);

			if (CurrentMode >= EGUIEDM_MOVE)
			{
				IGUIElement *sel = SelectedElement;
				// unselect
				setSelectedElement(0);
				
				// move
				core::position2d<s32> p = sel->getParent()->getAbsolutePosition().UpperLeftCorner;
				sel->setRelativePosition(SelectedArea - p);

				// select
				setSelectedElement(sel);

				// reset selection mode...
				CurrentMode = EGUIEDM_SELECT;
			}
			break;
		case EMIE_MOUSE_MOVED:
			// always on top
			Parent->bringToFront(this);

			// if selecting
			if (CurrentMode == EGUIEDM_SELECT)
			{
				// highlight the element that the mouse is over
				MouseOverElement = getEditableElementFromPoint(Parent, core::position2di(event.MouseInput.X,event.MouseInput.Y));
				if (MouseOverElement == Parent)
						MouseOverElement = 0;

				core::position2di p = core::position2di(event.MouseInput.X,event.MouseInput.Y);
				MouseOverMode = getModeFromPos(p);

			}
			else if (CurrentMode == EGUIEDM_MOVE)
			{
				// get difference
				core::position2di p = core::position2di(event.MouseInput.X,event.MouseInput.Y);
				p -= DragStart;
				
				// apply to top corner
				p = StartMovePos + p;
				if (UseGrid)
				{
					p.X = (p.X/GridSize.Width)*GridSize.Width;
					p.Y = (p.Y/GridSize.Height)*GridSize.Height;
				}

				SelectedArea += p - SelectedArea.UpperLeftCorner;
				
			}
			else if (CurrentMode > EGUIEDM_MOVE)
			{
				// get difference from start position
				core::position2di p = core::position2di(event.MouseInput.X,event.MouseInput.Y);
				if (UseGrid)
				{
					p.X = (p.X/GridSize.Width)*GridSize.Width;
					p.Y = (p.Y/GridSize.Height)*GridSize.Height;
				}

				switch(CurrentMode)
				{
					case EGUIEDM_RESIZE_T:
						SelectedArea.UpperLeftCorner.Y = p.Y;
						break;
					case EGUIEDM_RESIZE_B:
						SelectedArea.LowerRightCorner.Y = p.Y;
						break;
					case EGUIEDM_RESIZE_L:
						SelectedArea.UpperLeftCorner.X = p.X;
						break;
					case EGUIEDM_RESIZE_R:
						SelectedArea.LowerRightCorner.X = p.X;
						break;
					case EGUIEDM_RESIZE_TL:
						SelectedArea.UpperLeftCorner = p;
						break;
					case EGUIEDM_RESIZE_TR:
						SelectedArea.UpperLeftCorner.Y = p.Y;
						SelectedArea.LowerRightCorner.X = p.X;
						break;
					case EGUIEDM_RESIZE_BL:
						SelectedArea.UpperLeftCorner.X = p.X;
						SelectedArea.LowerRightCorner.Y = p.Y;
						break;
					case EGUIEDM_RESIZE_BR:
						SelectedArea.LowerRightCorner = p;
						break;
				}
			}

			break;
		}
		break;
		
	case EET_GUI_EVENT:
		switch(event.GUIEvent.EventType)
		{
		// load a gui file
		case EGET_FILE_SELECTED:
			dialog = (IGUIFileOpenDialog*)event.GUIEvent.Caller;
			Environment->loadGUI(core::stringc(dialog->getFilename()).c_str());
			break;

		case EGET_MENU_ITEM_SELECTED:

			IGUIContextMenu *menu = (IGUIContextMenu*)event.GUIEvent.Caller;
			s32 cmdID = menu->getItemCommandId(menu->getSelectedItem()) - MenuCommandStart;

			IGUIElement* el;

			switch(cmdID)
			{

				//! file commands
				case EGUIEDMC_FILE_NEW:
					// clear all elements belonging to our parent
					setSelectedElement(0);
					MouseOverElement = 0;
					el = Parent;
					grab();
					// remove all children
					while(el->getChildren().begin() != Children.end())
						el->removeChild(*(el->getChildren().begin()));
					// attach to parent again
					el->addChild(this);
					drop();

					break;
				case EGUIEDMC_FILE_LOAD:
					Environment->addFileOpenDialog(L"Please select a GUI file to open", false, this);
					break;
				case EGUIEDMC_FILE_SAVE:
					break;

				//! edit menu
				case EGUIEDMC_CUT_ELEMENT:
					// TODO
					break;
				case EGUIEDMC_COPY_ELEMENT:
					// TODO
					break;
				case EGUIEDMC_PASTE_ELEMENT:
					// TODO
					break;
				case EGUIEDMC_DELETE_ELEMENT:
					el = SelectedElement;
					setSelectedElement(0);
					MouseOverElement = 0;
					el->remove();
					break;
				case EGUIEDMC_SET_PARENT:
					// TODO
					CurrentMode = EGUIEDM_SELECT_NEW_PARENT;
					break;
				case EGUIEDMC_BRING_TO_FRONT:
					SelectedElement->getParent()->bringToFront(SelectedElement);
					break;
				case EGUIEDMC_SAVE_ELEMENT:
					break;

				//! grid
				case EGUIEDMC_TOGGLE_GRID:
					DrawGrid = !DrawGrid;
					break;
				case EGUIEDMC_TOGGLE_SNAP_GRID:
					UseGrid = !UseGrid;
					break;
				case EGUIEDMC_SET_GRID_SIZE:
					break;
				case EGUIEDMC_TOGGLE_EDITOR:
					break;
				// 
				case EGUIEDMC_INSERT_XML:
					// TODO
					//Environment->loadGUI("d:\\GUITest.xml", SelectedElement );
					break;

				default:
					// create element from factory?
					if (cmdID >= EGUIEDMC_COUNT)
					{

						s32 num = cmdID - EGUIEDMC_COUNT; // get index
						// loop through all factories
						s32 i, c=Environment->getRegisteredGUIElementFactoryCount();
						for (i=0; i<c && num > Environment->getGUIElementFactory(i)->getCreatableGUIElementTypeCount(); ++i)
						{
							num -= Environment->getGUIElementFactory(i)->getCreatableGUIElementTypeCount();
						}
						if (num < Environment->getGUIElementFactory(i)->getCreatableGUIElementTypeCount() )
						{
							core::stringc name = Environment->getGUIElementFactory(i)->getCreateableGUIElementTypeName(num);
							IGUIElement *parentElement = SelectedElement ? SelectedElement : Environment->getRootGUIElement();
							// add it
							IGUIElement *newElement = Environment->getGUIElementFactory(i)->addGUIElement(name.c_str(),parentElement);
							if (newElement)
							{
								core::position2di p = DragStart - parentElement->getAbsolutePosition().UpperLeftCorner;
								newElement->setRelativePosition(core::rect<s32>(p,p+core::position2di(100,100)));
								//Environment->removeFocus(newElement);
							}
						}
					}
					break;
			}
			return true;
		
		}	
		break;
		
	}

	return true;
	//return IGUIElement::OnEvent(event);	
}


//! draws the element and its children
void CGUIEditWorkspace::draw()
{
	video::IVideoDriver *driver = Environment->getVideoDriver();

	if (DrawGrid)
	{
		// draw the grid

		core::rect<s32> r = getAbsolutePosition();

		s32 cy = r.UpperLeftCorner.Y;
		while (cy < r.LowerRightCorner.Y)
		{
			s32 cx = r.UpperLeftCorner.X;
			while (cx < r.LowerRightCorner.X)
			{
				driver->draw2DRectangle(video::SColor(40,0,0,90),core::rect<s32>(cx+1,cy+1,GridSize.Width+cx,GridSize.Height+cy));
				cx += GridSize.Width;
			}
			cy += GridSize.Height;
		}
	}
	if (MouseOverElement && 
		MouseOverElement != SelectedElement && 
		MouseOverElement != Parent)
	{
		core::rect<s32> r = MouseOverElement->getAbsolutePosition();
		r.clipAgainst(MouseOverElement->getParent()->getAbsolutePosition());
		driver->draw2DRectangle(video::SColor(100,0,0,255), r);
	}
	if (SelectedElement && CurrentMode == EGUIEDM_SELECT)
	{
		driver->draw2DRectangle(video::SColor(100,0,255,0),SelectedElement->getAbsolutePosition());
	}
	if (CurrentMode >= EGUIEDM_MOVE)
	{
		driver->draw2DRectangle(video::SColor(100,255,0,0),SelectedArea);
	}

	if ( (SelectedElement  && CurrentMode   >= EGUIEDM_MOVE) ||
		 (SelectedElement  && MouseOverElement == SelectedElement && MouseOverMode >= EGUIEDM_MOVE) )
	{
		// draw handles for moving
		EGUIEDIT_MODE m = CurrentMode;
		core::rect<s32>   r = SelectedArea;
		if (m < EGUIEDM_MOVE)
		{
			m = MouseOverMode;
			r = SelectedElement->getAbsolutePosition();
		}

		core::position2di d = core::position2di(4,4);
		
		// top left
		if (m == EGUIEDM_RESIZE_T || m == EGUIEDM_RESIZE_L || m == EGUIEDM_RESIZE_TL || m == EGUIEDM_MOVE )
			driver->draw2DRectangle(video::SColor(100,255,255,255), 
									core::rect<s32>(r.UpperLeftCorner, r.UpperLeftCorner + d ) );

		if (m == EGUIEDM_RESIZE_T || m == EGUIEDM_RESIZE_R || m == EGUIEDM_RESIZE_TR || m == EGUIEDM_MOVE )
			driver->draw2DRectangle(video::SColor(100,255,255,255), 
							core::rect<s32>(r.LowerRightCorner.X-4, r.UpperLeftCorner.Y, r.LowerRightCorner.X, r.UpperLeftCorner.Y+4) );

		if (m == EGUIEDM_RESIZE_T || m == EGUIEDM_MOVE )
			driver->draw2DRectangle(video::SColor(100,255,255,255), 
				core::rect<s32>(r.getCenter().X-2, r.UpperLeftCorner.Y,r.getCenter().X+2, r.UpperLeftCorner.Y+4 ) );

		if (m == EGUIEDM_RESIZE_L || m == EGUIEDM_RESIZE_BL || m == EGUIEDM_RESIZE_B || m == EGUIEDM_MOVE )
			driver->draw2DRectangle(video::SColor(100,255,255,255), 
									core::rect<s32>(r.UpperLeftCorner.X, r.LowerRightCorner.Y-4, r.UpperLeftCorner.X+4, r.LowerRightCorner.Y) );

		if (m == EGUIEDM_RESIZE_L || m == EGUIEDM_MOVE )
			driver->draw2DRectangle(video::SColor(100,255,255,255), 
				core::rect<s32>(r.UpperLeftCorner.X,r.getCenter().Y-2, r.UpperLeftCorner.X+4, r.getCenter().Y+2 ) );

		if (m == EGUIEDM_RESIZE_R || m == EGUIEDM_MOVE )
			driver->draw2DRectangle(video::SColor(100,255,255,255), 
				core::rect<s32>(r.LowerRightCorner.X-4,r.getCenter().Y-2, r.LowerRightCorner.X, r.getCenter().Y+2 ) );

		if (m == EGUIEDM_RESIZE_R || m == EGUIEDM_RESIZE_BR || m == EGUIEDM_RESIZE_B || m == EGUIEDM_MOVE )
			driver->draw2DRectangle(video::SColor(100,255,255,255), 
							core::rect<s32>(r.LowerRightCorner-d, r.LowerRightCorner) );

		if (m == EGUIEDM_RESIZE_B || m == EGUIEDM_MOVE )
			driver->draw2DRectangle(video::SColor(100,255,255,255), 
				core::rect<s32>(r.getCenter().X-2, r.LowerRightCorner.Y-4,r.getCenter().X+2, r.LowerRightCorner.Y ) );


	}

	IGUIElement::draw();
}


void CGUIEditWorkspace::setDrawGrid(bool drawGrid)
{
	DrawGrid = drawGrid;
}

void CGUIEditWorkspace::setGridSize(core::dimension2di	&gridSize)
{
	GridSize = gridSize;
	if (GridSize.Width < 2)
		GridSize.Width = 2;
	if (GridSize.Height < 2)
		GridSize.Height = 2;
}

void CGUIEditWorkspace::setUseGrid(bool useGrid)
{

}


//! Removes a child.
void CGUIEditWorkspace::removeChild(IGUIElement* child)
{
	IGUIElement::removeChild(child);

	if (Children.empty())
		remove();
}


void CGUIEditWorkspace::updateAbsolutePosition()
{
	core::rect<s32> parentRect(0,0,0,0);

	if (Parent)
	{
		parentRect = Parent->getAbsolutePosition();
		RelativeRect.UpperLeftCorner.X = 0;
		RelativeRect.UpperLeftCorner.Y = 0;
		RelativeRect.LowerRightCorner.X = parentRect.getWidth();
		RelativeRect.LowerRightCorner.Y = parentRect.getHeight();
	}

	IGUIElement::updateAbsolutePosition();
}


} // end namespace gui
} // end namespace irr

