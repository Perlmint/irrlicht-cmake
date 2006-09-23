// Copyright (C) 2002-2006 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "SoftwareDriver2_helper.h"

namespace irr
{
namespace video
{

	void SColorHSL::setfromRGB ( const SColor &color )
	{
	}

	void SColorHSL::settoRGB ( SColor &color )
	{
		if ( Saturation == 0.0) // grey
		{
			u8 c = (u8) ( Luminance * 255.0 );
			color.setRed ( c );
			color.setGreen ( c );
			color.setBlue ( c );
			return;
		}

		f32 rm1, rm2;
			
		if ( Luminance <= 0.5f )
		{
			rm2 = Luminance + Luminance * Saturation;  
		}
		else
		{
			rm2 = Luminance + Saturation - Luminance * Saturation;
		}

		rm1 = 2.0f * Luminance - rm2;   

		color.setRed ( toRGB1(rm1, rm2, Hue + (120.0f * core::DEGTORAD )) );
		color.setGreen ( toRGB1(rm1, rm2, Hue) );
		color.setBlue ( toRGB1(rm1, rm2, Hue - (120.0f * core::DEGTORAD) ) );
	}


	u32 SColorHSL::toRGB1(f32 rm1, f32 rm2, f32 rh) const
	{
		while ( rh > 2.f * core::PI )
			rh -= 2.f * core::PI;

		while ( rh < 0.f )
			rh += 2.f * core::PI;

		if      (rh <  60.0f * core::DEGTORAD ) rm1 = rm1 + (rm2 - rm1) * rh / (60.0f * core::DEGTORAD);
		else if (rh < 180.0f * core::DEGTORAD ) rm1 = rm2;
		else if (rh < 240.0f * core::DEGTORAD ) rm1 = rm1 + (rm2 - rm1) * ( ( 240.0f * core::DEGTORAD ) - rh) / (60.0f * core::DEGTORAD);
		                
		return (u32) (rm1 * 255.f);
	}

} // end namespace video
} // end namespace irr


