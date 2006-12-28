// Copyright (C) 2002-2006 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __C_TEXT_SCENE_NODE_H_INCLUDED__
#define __C_TEXT_SCENE_NODE_H_INCLUDED__

#include "ITextSceneNode.h"
#include "IGUIFont.h"
#include "IGUIFontASCII.h"
#include "ISceneCollisionManager.h"
#include "S3DVertex.h"

namespace irr
{
namespace scene
{

	class CTextSceneNode : public ITextSceneNode
	{
	public:

		//! constructor
		CTextSceneNode(ISceneNode* parent, ISceneManager* mgr, s32 id,
			gui::IGUIFont* font, scene::ISceneCollisionManager* coll,
			const core::vector3df& position = core::vector3df(0,0,0), const wchar_t* text=0,
			video::SColor color=video::SColor(100,0,0,0));

		//! destructor
		virtual ~CTextSceneNode();

		virtual void OnPreRender();

		//! renders the node.
		virtual void render();

		//! returns the axis aligned bounding box of this node
		virtual const core::aabbox3d<f32>& getBoundingBox() const;

		//! returns amount of materials used by this scene node.
		virtual u32 getMaterialCount();

		//! sets the text string
		virtual void setText(const wchar_t* text);

		//! sets the color of the text
		virtual void setTextColor(video::SColor color);
		
		//! Returns type of the scene node
		virtual ESCENE_NODE_TYPE getType() { return ESNT_TEXT; }

	private:

		core::aabbox3d<f32> Box;
		core::stringw Text;
		video::SColor Color;
		gui::IGUIFont* Font;
		scene::ISceneCollisionManager* Coll;
	};


	class CTextSceneNode2 : public ITextSceneNode
	{
	public:

		CTextSceneNode2(ISceneNode* parent, ISceneManager* mgr, s32 id,	
			gui::IGUIFontASCII* font,const wchar_t* text,
			const core::vector3df& position, const core::dimension2d<f32>& size,
			f32 kerning,
			video::SColor shade_top,video::SColor shade_down );

		//! destructor
		virtual ~CTextSceneNode2();

		virtual void OnPreRender();

		//! renders the node.
		virtual void render();

		//! returns the axis aligned bounding box of this node
		virtual const core::aabbox3d<f32>& getBoundingBox() const;

		//! sets the text string
		virtual void setText(const wchar_t* text);

		//! sets the color of the text
		virtual void setTextColor(video::SColor color);
		
		//! sets the size of the billboard
		virtual void setSize(const core::dimension2d<f32>& size);

		//! gets the size of the billboard
		virtual const core::dimension2d<f32>& getSize();

		virtual video::SMaterial& getMaterial(u32 i);
		
		//! returns amount of materials used by this scene node.
		virtual u32 getMaterialCount();

		//! Returns type of the scene node
		virtual ESCENE_NODE_TYPE getType() { return ESNT_TEXT; }


	private:

		core::stringw Text;
		video::SColor Color;
		gui::IGUIFontASCII* Font;

		core::dimension2d<f32> Size;
		core::aabbox3d<f32> BBox;
		video::SMaterial Material;

		video::SColor Shade_top;
		video::SColor Shade_down;
		struct SSymbolInfo
		{
			SSymbolInfo ( video::SColor shade_down, video::SColor shade_up )
			{
				indices[0] = 0;
				indices[1] = 2;
				indices[2] = 1;
				indices[3] = 0;
				indices[4] = 3;
				indices[5] = 2;
				vertices[0].Color = shade_down;
				vertices[3].Color = shade_down;
				vertices[1].Color = shade_up;
				vertices[2].Color = shade_up;
			}
			video::S3DVertex vertices[4];
			u16 indices[6];
			f32 Width;
		};

		core::array < SSymbolInfo > Symbol;
		f32 Kerning;

	};

} // end namespace scene
} // end namespace irr

#endif

