// Copyright (C) 2002-2006 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "CTextSceneNode.h"
#include "ISceneManager.h"
#include "IVideoDriver.h"
#include "ICameraSceneNode.h"


namespace irr
{
namespace scene
{

//! constructor
CTextSceneNode::CTextSceneNode(ISceneNode* parent, ISceneManager* mgr, s32 id,
							   gui::IGUIFont* font, scene::ISceneCollisionManager* coll,
	const core::vector3df& position, const wchar_t* text,
	video::SColor color)
	: ITextSceneNode(parent, mgr, id, position), Font(font), Coll(coll), Color(color)
{
	#ifdef _DEBUG
	setDebugName("CTextSceneNode");
	#endif

	Text = text;
	setAutomaticCulling(scene::EAC_OFF);

	if (Font)
		Font->grab();
}

//! destructor
CTextSceneNode::~CTextSceneNode()
{
	if (Font)
		Font->drop();
}

void CTextSceneNode::OnPreRender()
{
	if (IsVisible)
	{
		SceneManager->registerNodeForRendering(this, ESNRP_SHADOW);
		ISceneNode::OnPreRender();
	}
}

//! renders the node.
void CTextSceneNode::render()
{
	if (!Font || !Coll)
		return;

	core::position2d<s32> pos = Coll->getScreenCoordinatesFrom3DPosition(getAbsolutePosition(), 
		SceneManager->getActiveCamera());

	core::rect<s32> r(pos, core::dimension2d<s32>(1,1));
	Font->draw(Text.c_str(), r, Color, true, true);
}


//! returns the axis aligned bounding box of this node
const core::aabbox3d<f32>& CTextSceneNode::getBoundingBox() const
{
	return Box;
}

//! returns amount of materials used by this scene node.
u32 CTextSceneNode::getMaterialCount()
{
	return 0;
}

//! sets the text string
void CTextSceneNode::setText(const wchar_t* text)
{
	Text = text;
}


//! sets the color of the text
void CTextSceneNode::setTextColor(video::SColor color)
{
	Color = color;
}


//!--------------------------------- CTextSceneNode2 ----------------------------------------------

//! constructor
CTextSceneNode2::CTextSceneNode2(ISceneNode* parent, ISceneManager* mgr, s32 id,	
	gui::IGUIFontASCII* font,const wchar_t* text,
	const core::vector3df& position, const core::dimension2d<f32>& size,f32 kerning,
	video::SColor shade_top,video::SColor shade_down )
: ITextSceneNode(parent, mgr, id, position),
	Font(0),Shade_top ( shade_top ), Shade_down ( shade_down ),	Kerning ( kerning )
{
	#ifdef _DEBUG
	setDebugName("CTextSceneNode2");
	#endif

	setText ( text );

	setSize(size);

	if (font)
	{
		// doesn't support other font types
		if (Font->getType() == gui::EGFT_BITMAP)
		{
			Font = font;
			Font->grab();
			Material.Texture1 = Font->getTexture ();
		}
	}

	Material.MaterialType = video::EMT_TRANSPARENT_ALPHA_CHANNEL_REF;
	Material.MaterialTypeParam = 0.5f;
	Material.BackfaceCulling = false;
	Material.Lighting = false;
	Material.ZBuffer = true;
	Material.ZWriteEnable = true;

	setAutomaticCulling ( scene::EAC_BOX );

}



CTextSceneNode2::~CTextSceneNode2()
{
	if (Font)
		Font->drop();

}


//! sets the text string
void CTextSceneNode2::setText(const wchar_t* text)
{
	Text = text;

	Symbol.clear ();

	if ( 0 == Font )
		return;

	SSymbolInfo info ( Shade_top, Shade_down );


	const core::array< core::rect<s32> > &sourceRects = Font->getPositions();

	f32 dim[2];
	f32 tex[4];

	dim[0] = core::reciprocal ( (f32) Font->getTexture ()->getOriginalSize().Width );
	dim[1] = core::reciprocal ( (f32) Font->getTexture ()->getOriginalSize().Height );

	u32 i;
	for ( i = 0; i != Text.size (); ++i )
	{
		s32 symbol = Text[i] - 32;
		if ( symbol < 0 )
			continue;

		const core::rect<s32>& s = sourceRects [ symbol ];

		tex[0] = s.LowerRightCorner.X * dim[0];
		tex[1] = s.LowerRightCorner.Y * dim[1];
		tex[2] = s.UpperLeftCorner.Y * dim[1];
		tex[3] = s.UpperLeftCorner.X * dim[0];
		
		info.vertices[0].TCoords.set(tex[0], tex[1]);
		info.vertices[1].TCoords.set(tex[0], tex[2]);
		info.vertices[2].TCoords.set(tex[3], tex[2]);
		info.vertices[3].TCoords.set(tex[3], tex[1]);

		info.Width = tex[3] - tex[0];

		Symbol.push_back ( info );
	}
}


//! pre render event
void CTextSceneNode2::OnPreRender()
{
	if (!IsVisible)
		return;

	ICameraSceneNode* camera = SceneManager->getActiveCamera();
	if ( 0 == camera )
		return;


	const core::matrix4 &m = camera->getViewFrustum()->Matrices[ video::ETS_VIEW ];
	// make billboard look to camera

	core::vector3df pos = getAbsolutePosition();

	core::vector3df campos = camera->getAbsolutePosition();
	core::vector3df target = camera->getTarget();
	core::vector3df up = camera->getUpVector();
	core::vector3df view = target - campos;
	view.normalize();

	core::vector3df horizontal = up.crossProduct(view);
	if ( horizontal.getLength() == 0 )
	{
		horizontal.set(up.Y,up.X,up.Z);
	}

	
	horizontal.normalize();
	core::vector3df space = horizontal;

	horizontal *= 0.5f * Size.Width;

	core::vector3df vertical = horizontal.crossProduct(view);
	vertical.normalize();
	vertical *= 0.5f * Size.Height;

	view *= -1.0f;


	// center text

	f32 textLength = 0.f;
	u32 i;
	for ( i = 0; i!= Symbol.size(); ++i )
	{
		SSymbolInfo &info = Symbol[i];
		textLength += Kerning + ( info.Width * 0.5f * Size.Width );
	}

	pos += space * ( textLength * -0.5f );

	for ( i = 0; i!= Symbol.size(); ++i )
	{
		SSymbolInfo &info = Symbol[i];

		info.vertices[0].Normal = view;
		info.vertices[1].Normal = view;
		info.vertices[2].Normal = view;
		info.vertices[3].Normal = view;

		info.vertices[0].Pos = pos + horizontal + vertical;
		info.vertices[1].Pos = pos + horizontal - vertical;
		info.vertices[2].Pos = pos - horizontal - vertical;
		info.vertices[3].Pos = pos - horizontal + vertical;

		pos += space * ( Kerning + ( info.Width * 0.5f * Size.Width ) );
	}

	// reset bounding box
	pos = getAbsolutePosition ();
	BBox.reset ( Symbol[0].vertices[0].Pos - pos );
	
	for ( i = 0; i!= Symbol.size(); ++i )
	{
		SSymbolInfo &info = Symbol[i];

		for ( u32 v = 0; v!= 4; ++v )
		{
			BBox.addInternalPoint ( info.vertices[v].Pos - pos );
		}
	}

	SceneManager->registerNodeForRendering(this);
	ISceneNode::OnPreRender();
}


//! render
void CTextSceneNode2::render()
{
	video::IVideoDriver* driver = SceneManager->getVideoDriver();

	// draw
	core::matrix4 mat;
	driver->setTransform(video::ETS_WORLD, mat);

	driver->setMaterial(Material);

	u32 i;
	for ( i = 0; i!= Symbol.size(); ++i )
	{
		SSymbolInfo &info = Symbol[i];

		driver->drawIndexedTriangleList( info.vertices, 4, info.indices, 2);
	}

	if (DebugDataVisible)
	{
		driver->setTransform(video::ETS_WORLD, AbsoluteTransformation);
		video::SMaterial m;
		m.Lighting = false;
		driver->setMaterial(m);
		driver->draw3DBox(BBox, video::SColor(0,208,195,152));
	}

}

//! returns the axis aligned bounding box of this node
const core::aabbox3d<f32>& CTextSceneNode2::getBoundingBox() const
{
	return BBox;
}


//! sets the size of the billboard
void CTextSceneNode2::setSize(const core::dimension2d<f32>& size)
{
	Size = size;

	if (Size.Width == 0.0f)
		Size.Width = 1.0f;

	if (Size.Height == 0.0f )
		Size.Height = 1.0f;

	//f32 avg = (size.Width + size.Height)/6;
	//BBox.MinEdge.set(-avg,-avg,-avg);
	//BBox.MaxEdge.set(avg,avg,avg);
}


video::SMaterial& CTextSceneNode2::getMaterial(u32 i)
{
	return Material;
}


//! returns amount of materials used by this scene node.
u32 CTextSceneNode2::getMaterialCount()
{
	return 1;
}


//! gets the size of the billboard
const core::dimension2d<f32>& CTextSceneNode2::getSize()
{
	return Size;
}




//! sets the color of the text
void CTextSceneNode2::setTextColor(video::SColor color)
{
	Color = color;
}


} // end namespace scene
} // end namespace irr

