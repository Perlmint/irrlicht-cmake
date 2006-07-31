// Copyright (C) 2002-2006 Nikolaus Gebhardt/Alten Thomas
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __C_VIDEO_2_SOFTWARE_H_INCLUDED__
#define __C_VIDEO_2_SOFTWARE_H_INCLUDED__

#include "SoftwareDriver2_compile_config.h"
#include "ITriangleRenderer2.h"
#include "CNullDriver.h"
#include "CImage.h"
#include "os.h"
#include <memory.h>

namespace irr
{
namespace video
{
	class CSoftwareDriver2 : public CNullDriver
	{
	public:

		//! constructor
		CSoftwareDriver2(const core::dimension2d<s32>& windowSize, bool fullscreen, io::IFileSystem* io, video::IImagePresenter* presenter);

		//! destructor
		virtual ~CSoftwareDriver2();

		//! presents the rendered scene on the screen, returns false if failed
		virtual bool endScene( s32 windowId = 0, core::rect<s32>* sourceRect=0 );

		//! queries the features of the driver, returns true if feature is available
		virtual bool queryFeature(E_VIDEO_DRIVER_FEATURE feature);

		//! sets transformation
		virtual void setTransform(E_TRANSFORMATION_STATE state, const core::matrix4& mat);

		//! sets a material
		virtual void setMaterial(const SMaterial& material);

		virtual bool setRenderTarget(video::ITexture* texture, bool clearBackBuffer, 
								 bool clearZBuffer, SColor color);

		//! sets a viewport
		virtual void setViewPort(const core::rect<s32>& area);

		//! clears the zbuffer
		virtual bool beginScene(bool backBuffer, bool zBuffer, SColor color);

		//! deletes all dynamic lights there are
		virtual void deleteAllDynamicLights();

		//! adds a dynamic light
		virtual void addDynamicLight(const SLight& light);

		//! returns the maximal amount of dynamic lights the device can handle
		virtual s32 getMaximalDynamicLightAmount();

		//! Sets the dynamic ambient light color. The default color is
		//! (0,0,0,0) which means it is dark.
		//! \param color: New color of the ambient light.
		virtual void setAmbientLight(const SColorf& color);

		//! draws an indexed triangle list
		virtual void drawIndexedTriangleList(const S3DVertex* vertices, s32 vertexCount,
			const u16* indexList, s32 triangleCount);

		//! draws an indexed triangle list
		virtual void drawIndexedTriangleList(const S3DVertex2TCoords* vertices,
			s32 vertexCount, const u16* indexList, s32 triangleCount);

		//! Draws an indexed triangle list.
		virtual void drawIndexedTriangleList(const S3DVertexTangents* vertices,
			s32 vertexCount, const u16* indexList, s32 triangleCount);

		//! Draws an indexed triangle fan.
		virtual void drawIndexedTriangleFan(const S3DVertex* vertices,
			s32 vertexCount, const u16* indexList, s32 triangleCount);

		//! Draws an indexed triangle fan.
		virtual void drawIndexedTriangleFan(const S3DVertex2TCoords* vertices,
			s32 vertexCount, const u16* indexList, s32 triangleCount);

		//! draws an 2d image
		virtual void draw2DImage(video::ITexture* texture, const core::position2d<s32>& destPos);

		//! draws an 2d image, using a color (if color is other then Color(255,255,255,255)) and the alpha channel of the texture if wanted.
		virtual void draw2DImage(video::ITexture* texture, const core::position2d<s32>& destPos,
			const core::rect<s32>& sourceRect, const core::rect<s32>* clipRect = 0, 
			SColor color=SColor(255,255,255,255), bool useAlphaChannelOfTexture=false);

		//! Draws a 3d line.
		virtual void draw3DLine(const core::vector3df& start,
			const core::vector3df& end, SColor color = SColor(255,255,255,255));

		//! draw an 2d rectangle
		virtual void draw2DRectangle(SColor color, const core::rect<s32>& pos, 
			const core::rect<s32>* clip = 0);

		//!Draws an 2d rectangle with a gradient.
		virtual void draw2DRectangle(const core::rect<s32>& pos,
			SColor colorLeftUp, SColor colorRightUp, SColor colorLeftDown, SColor colorRightDown,
			const core::rect<s32>* clip = 0);

		//! Draws a 2d line. 
		virtual void draw2DLine(const core::position2d<s32>& start,
								const core::position2d<s32>& end, 
								SColor color=SColor(255,255,255,255));

		//! \return Returns the name of the video driver. Example: In case of the DirectX8
		//! driver, it would return "Direct3D8.1".
		virtual const wchar_t* getName();

		//! Returns type of video driver
		virtual E_DRIVER_TYPE getDriverType();

		//! Returns the transformation set by setTransform
		virtual const core::matrix4& getTransform(E_TRANSFORMATION_STATE state);

		//! Creates a render target texture.
		virtual ITexture* createRenderTargetTexture(core::dimension2d<s32> size);
	
		//! Clears the ZBuffer. 
		virtual void clearZBuffer();

	protected:

		//! sets a render target
		void setRenderTarget(video::CImage* image);

		//! sets the current Texture
		void setTexture(u32 stage, video::ITexture* texture);

		//! returns a device dependent texture from a software surface (IImage)
		//! THIS METHOD HAS TO BE OVERRIDDEN BY DERIVED DRIVERS WITH OWN TEXTURES
		virtual video::ITexture* createDeviceDependentTexture(IImage* surface, const char* name);

		video::CImage* BackBuffer;
		video::IImagePresenter* Presenter;


		//! void selects the right triangle renderer based on the render states.
		void selectRightTriangleRenderer();


		video::ITexture* RenderTargetTexture;	
		video::IImage* RenderTargetSurface;	
		core::dimension2d<s32> RenderTargetSize;

		ITriangleRenderer2* CurrentTriangleRenderer;
		ITriangleRenderer2* TriangleRenderer[ETR2_COUNT];
		ETriangleRenderer2 CurrentRenderer;

		IZBuffer2* ZBuffer;

		video::ITexture* Texture[2];
		sInternalTexture Texmap[2];


		/*
			extend Matrix Stack
			-> combined CameraProjection
			-> combined CameraProjectionWorld
			-> ClipScale from NDC to DC Space
		*/
		enum E_TRANSFORMATION_STATE_2
		{
			ETS_VIEW_PROJECTION = ETS_COUNT,
			ETS_CURRENT,
			ETS_CLIPSCALE,

			ETS2_COUNT
		};

		core::matrix4 TransformationMatrix[ETS2_COUNT];

		// holds transformed, clipped vertices
		s4DVertex CurrentOut[10];
		s4DVertex Temp[10];

		u32 clipToFrustrum_NoStat ( s4DVertex *source, s4DVertex * temp, u32 vIn );
		u32 clipToFrustrum_Stat ( s4DVertex *source, s4DVertex * temp, u32 vIn );
		void ndc_2_dc_and_project ( s4DVertex *source, u32 vIn ) const;
		f32 backface ( s4DVertex *v0 ) const;


		void transform_and_lighting ( s4DVertex *dest, const S3DVertex **face );

		void apply_tex_coords ( s4DVertex *dest, const S3DVertex **face );
		void apply_tex_coords ( s4DVertex *dest, const S3DVertex2TCoords ** face );

		sVec4 Global_AmbientLight;

		struct SInternalLight
		{
			SLight org;

			sVec4 AmbientColor;
			sVec4 DiffuseColor;
		};
		core::array<SInternalLight> Light;

		struct SInternalMaterial
		{
			SMaterial org;

			sVec4 AmbientColor;
			sVec4 DiffuseColor;
			sVec4 EmissiveColor;
			sVec4 SpecularColor;
		};

		SInternalMaterial Material;
	
		


#ifdef SOFTWARE_DRIVER_2_STATISTIC
		struct SClipStat
		{
			u32 In;
			u32 Out;
			u32 Part;
		};
		struct SStat
		{
			void clear ()
			{
				Primitive = 0;
				Backface = 0;
				DrawTriangle = 0;
				for ( int i = 0; i!= 7; ++i )
				{
					Clip[i].In = 0;
					Clip[i].Out = 0;
					Clip[i].Part = 0;
				}

				Start = os::Timer::getRealTime();
			}

			void clip ( u32 vOut, int plane )
			{
				if ( vOut < 3 )
				{
					Clip[plane].Out += 1;
				}
				else if ( vOut == 3 )
				{
					Clip[plane].In += 1;
				}
				else
				{
					Clip[plane].Part += 1;
				}
			}

			void stop ()
			{
				Stop = os::Timer::getRealTime();
			}

			void dump ()
			{
				char buf[255];
				sprintf ( buf, "Stat: %d ms Prim: %d Back: %d Clip_In: %d Clip_Out: %d Clip_Part %d Tri: %d",
								Stop - Start,
								Primitive,
								Backface,
								Clip[6].In,
								Clip[6].Out,
								Clip[6].Part,
								DrawTriangle
							);
				os::Printer::print(buf);
				for ( int i = 0; i!= 6; ++i )
				{
					sprintf ( buf," Plane:%d Out: %d In: %d Part:%d",
								i,
								Clip[i].Out,
								Clip[i].In,
								Clip[i].Part
							);
					os::Printer::print(buf);
				}
			}

			u32 Start;
			u32 Stop;

			u32 Primitive;
			u32 Backface;
			u32 DrawTriangle;

			SClipStat Clip[6 + 1];
			
		};
		SStat Stat;
#endif
	};

} // end namespace video
} // end namespace irr


#endif

