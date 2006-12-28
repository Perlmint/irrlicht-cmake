// Copyright (C) 2002-2006 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "CAttributes.h"
#include "fast_atof.h"
#include "ITexture.h"
#include "IVideoDriver.h"

namespace irr
{
namespace io
{

/*
	basic types
*/

// Attribute implemented for boolean values
class CBoolAttribute : public IAttribute
{
public:

	CBoolAttribute(const char* name, bool value)
	{
		Name = name;
		setBool(value);
	}

	bool BoolValue;

	virtual s32 getInt()
	{
		return BoolValue ? 1 : 0;
	}

	virtual f32 getFloat()
	{
		return BoolValue ? 1.0f : 0.0f;
	}

	virtual bool getBool()
	{
		return BoolValue;
	}

	virtual void getString(char* target)
	{
		strcpy(target, BoolValue ? "true" : "false");
	}

	virtual void setInt(s32 intValue)
	{
		BoolValue = (intValue != 0);
	}

	virtual void setFloat(f32 floatValue)
	{
		BoolValue = (floatValue != 0);
	}

	virtual void setBool(bool boolValue)
	{
		BoolValue = boolValue;
	}

	virtual void setString(const char* string)
	{
		BoolValue = strcmp(string, "true") == 0;
	}

	virtual E_ATTRIBUTE_TYPE getType()
	{
		return EAT_BOOL;
	}

	virtual const wchar_t* getTypeString()
	{
		return L"bool";
	}
};

// Attribute implemented for integers
class CIntAttribute : public IAttribute
{
public:

	CIntAttribute(const char* name, s32 value)
	{
		Name = name;
		setInt(value);
	}

	virtual s32 getInt()
	{
		return Value;
	}

	virtual f32 getFloat()
	{
		return (f32)Value;
	}

	virtual bool getBool()
	{
		return (Value != 0);
	}

	virtual void getString(char* target)
	{
		sprintf(target, "%d", Value);
	}

	virtual void setInt(s32 intValue)
	{
		Value = intValue;
	}

	virtual void setFloat(f32 floatValue)
	{
		Value = (s32)floatValue;
	};

	virtual void setString(const char* text)
	{
		Value = atoi(text);
	}

	virtual E_ATTRIBUTE_TYPE getType()
	{
		return EAT_INT;
	}


	virtual const wchar_t* getTypeString()
	{
		return L"int";
	}

	s32 Value;
};

// Attribute implemented for floats
class CFloatAttribute : public IAttribute
{
public:

	CFloatAttribute(const char* name, f32 value)
	{
		Name = name;
		setFloat(value);
	}

	virtual s32 getInt()
	{
		return (s32)Value;
	}

	virtual f32 getFloat()
	{
		return Value;
	}

	virtual bool getBool()
	{
		return (Value != 0);
	}

	virtual void getString(char* target)
	{
		sprintf(target, "%f", Value);
	}

	virtual void setInt(s32 intValue)
	{
		Value = (f32)intValue;
	}

	virtual void setFloat(f32 floatValue)
	{
		Value = floatValue;
	};

	virtual void setString(const char* text)
	{
		Value = core::fast_atof(text);
	}

	virtual E_ATTRIBUTE_TYPE getType()
	{
		return EAT_FLOAT;
	}


	virtual const wchar_t* getTypeString()
	{
		return L"float";
	}

	f32 Value;
};





/*
	Types which can be represented as a list of numbers
*/

// Base class for all attributes which are a list of numbers-
// vectors, colors, positions, triangles, etc
class CNumbersAttribute : public IAttribute
{
public:

	CNumbersAttribute(const char* name, video::SColorf value) : 
		IsFloat(true), Count(4), ValueI(), ValueF()
	{
		Name = name;
		ValueF.push_back(value.r);
		ValueF.push_back(value.g);
		ValueF.push_back(value.b);
		ValueF.push_back(value.a);
	}

	CNumbersAttribute(const char* name, video::SColor value) :
		IsFloat(false), Count(4), ValueI(), ValueF()
	{
		Name = name;
		ValueI.push_back(value.getRed());
		ValueI.push_back(value.getGreen());
		ValueI.push_back(value.getBlue());
		ValueI.push_back(value.getAlpha());
	}


	CNumbersAttribute(const char* name, core::vector3df value) : 
		IsFloat(true), Count(3), ValueI(), ValueF()
	{
		Name = name;
		ValueF.push_back(value.X);
		ValueF.push_back(value.Y);
		ValueF.push_back(value.Z);
	}

	CNumbersAttribute(const char* name, core::position2df value) : 
		IsFloat(true), Count(2), ValueI(), ValueF()
	{
		Name = name;
		ValueF.push_back(value.X);
		ValueF.push_back(value.Y);
	}

	CNumbersAttribute(const char* name, core::position2di value) : 
		IsFloat(false), Count(2), ValueI(), ValueF()
	{
		Name = name;
		ValueI.push_back(value.X);
		ValueI.push_back(value.Y);
	}

	CNumbersAttribute(const char* name, core::rect<s32> value) : 
		IsFloat(false), Count(4), ValueI(), ValueF()
	{
		Name = name;
		ValueI.push_back(value.UpperLeftCorner.X);
		ValueI.push_back(value.UpperLeftCorner.Y);
		ValueI.push_back(value.LowerRightCorner.X);
		ValueI.push_back(value.LowerRightCorner.Y);
	}

	CNumbersAttribute(const char* name, core::rect<f32> value) : 
		IsFloat(true), Count(4), ValueI(), ValueF()
	{
		Name = name;
		ValueF.push_back(value.UpperLeftCorner.X);
		ValueF.push_back(value.UpperLeftCorner.Y);
		ValueF.push_back(value.LowerRightCorner.X);
		ValueF.push_back(value.LowerRightCorner.Y);
	}

	// todo: matrix4, quaternion, aabbox3d, plane, triangle3d, vector2df, 
	//		vector2di, line2di, line2df, line3df, dimension2di, dimension2df
	
	// getting values
	virtual s32 getInt()
	{
		if (Count==0)
			return 0;

		if (IsFloat)
			return (s32)ValueF[0];
		else
			return ValueI[0];
	}

	virtual f32 getFloat()
	{
		if (Count==0) 
			return 0.0f;

		if (IsFloat)
			return ValueF[0];
		else
			return (f32)ValueI[0];	
	}

	virtual bool getBool()
	{
		// return true if any number is nonzero
		// keeps 
		bool ret=false;
		
		for (u32 i=0; i < Count; ++i)
			if ( IsFloat ? (ValueF[i] != 0) : (ValueI[i] != 0) )
			{
				ret=true;
				break;
			}

		return ret;
		
	}

	virtual void getString(char* target)
	{
		char outstr[50];
		target[0] = '\0';
		for (u32 i=0; i <Count; ++i)
		{
			if (IsFloat)
				sprintf(outstr, "%f%s", ValueF[i], i==Count-1 ? "" : ", " );
			else
				sprintf(outstr, "%d%s", ValueI[i], i==Count-1 ? "" : ", " );

			strcat(target,outstr);
		}
	}

	virtual core::position2di getPosition()
	{
		core::position2di p;

		if (IsFloat)
		{
			p.X = (s32)(Count > 0 ? ValueF[0] : 0);
			p.Y = (s32)(Count > 1 ? ValueF[1] : 0);
		}
		else
		{
			p.X = Count > 0 ? ValueI[0] : 0;
			p.Y = Count > 1 ? ValueI[1] : 0;
		}

		return p;
	}

	virtual core::vector3df getVector()
	{
		core::vector3df v;

		if (IsFloat)
		{
			v.X = Count > 0 ? ValueF[0] : 0;
			v.Y = Count > 1 ? ValueF[1] : 0;
			v.Z = Count > 2 ? ValueF[2] : 0;
		}
		else
		{
			v.X = (f32)(Count > 0 ? ValueI[0] : 0);
			v.Y = (f32)(Count > 1 ? ValueI[1] : 0);
			v.Z = (f32)(Count > 2 ? ValueI[2] : 0);
		}

		return v;
	}

	virtual video::SColorf getColorf()
	{
		video::SColorf c;
		if (IsFloat)
		{
			c.setColorComponentValue(0, Count > 0 ? ValueF[0] : 0);
			c.setColorComponentValue(1, Count > 1 ? ValueF[1] : 0);
			c.setColorComponentValue(2, Count > 2 ? ValueF[2] : 0);
			c.setColorComponentValue(3, Count > 3 ? ValueF[3] : 0);
		}
		else
		{
			c.setColorComponentValue(0, Count > 0 ? (f32)(ValueI[0]) / 255.0f : 0);
			c.setColorComponentValue(1, Count > 1 ? (f32)(ValueI[1]) / 255.0f : 0);
			c.setColorComponentValue(2, Count > 2 ? (f32)(ValueI[2]) / 255.0f : 0);
			c.setColorComponentValue(3, Count > 3 ? (f32)(ValueI[3]) / 255.0f : 0);
		}

		return c;
	}

	virtual video::SColor getColor()
	{
		return getColorf().toSColor();
	}


	virtual core::rect<s32> getRect()
	{
		core::rect<s32> r;

		if (IsFloat)
		{
			r.UpperLeftCorner.X  = (s32)(Count > 0 ? ValueF[0] : 0);
			r.UpperLeftCorner.Y  = (s32)(Count > 1 ? ValueF[1] : 0);
			r.LowerRightCorner.X = (s32)(Count > 2 ? ValueF[2] : r.UpperLeftCorner.X);
			r.LowerRightCorner.Y = (s32)(Count > 2 ? ValueF[2] : r.UpperLeftCorner.Y);
		}
		else
		{
			r.UpperLeftCorner.X  = Count > 0 ? ValueI[0] : 0;
			r.UpperLeftCorner.Y  = Count > 1 ? ValueI[1] : 0;
			r.LowerRightCorner.X = Count > 2 ? ValueI[2] : r.UpperLeftCorner.X;
			r.LowerRightCorner.Y = Count > 2 ? ValueI[2] : r.UpperLeftCorner.Y;
		}
		return r;
	}

	// setting values
	virtual void setInt(s32 intValue)
	{
		// set all values
		for (u32 i=0; i < Count; ++i)
			if (IsFloat)
				ValueF[i] = (f32)intValue;
			else
				ValueI[i] = intValue;
	}

	virtual void setFloat(f32 floatValue)
	{
		// set all values
		for (u32 i=0; i < Count; ++i)
			if (IsFloat)
				ValueF[i] = floatValue;
			else
				ValueI[i] = (s32)floatValue;
	}

	virtual void setBool(bool boolValue)
	{
		setInt( boolValue ? 1 : 0);
	}
	virtual void setString(const char* text)
	{
		// parse text

		const char* P = (const char*)text;

		reset();

		u32 i=0;

		for ( i=0; i<Count && *P; ++i )
		{
			while(*P && P[0]!='-' && ( P[0]==' ' || (P[0] < '0' || P[0] > '9') ) )
				++P;

			// set value
			if ( *P)
			{
				if (IsFloat)
				{
					f32 c = 0;
					P = core::fast_atof_move(P, c);
					ValueF[i] = c;
				}
				else
				{
					// read int
					
				}
			}
		}
		// warning message
		if (i < Count-1)
		{
			
		}
	}

	virtual void setPosition(core::position2di v)
	{
		reset();
		if (IsFloat)
		{
			if (Count > 0) ValueF[0] = (f32)v.X;
			if (Count > 1) ValueF[1] = (f32)v.Y;
		}
		else
		{
			if (Count > 0) ValueI[0] = v.X;
			if (Count > 1) ValueI[1] = v.Y;
		}
	}
	virtual void setVector(core::vector3df v)
	{
		reset();
		if (IsFloat)
		{
			if (Count > 0) ValueF[0] = v.X;
			if (Count > 1) ValueF[1] = v.Y;
			if (Count > 2) ValueF[2] = v.Z;
		}
		else
		{
			if (Count > 0) ValueI[0] = (s32)v.X;
			if (Count > 1) ValueI[1] = (s32)v.Y;
			if (Count > 2) ValueI[2] = (s32)v.Z;
		}
	}

	virtual void setColor(video::SColorf color)
	{
		reset();
		if (IsFloat)
		{
			if (Count > 0) ValueF[0] = color.r;
			if (Count > 1) ValueF[1] = color.g;
			if (Count > 2) ValueF[2] = color.b;
			if (Count > 3) ValueF[3] = color.a;
		}
		else
		{
			if (Count > 0) ValueI[0] = (s32)(color.r * 255);
			if (Count > 1) ValueI[1] = (s32)(color.g * 255);
			if (Count > 2) ValueI[2] = (s32)(color.b * 255);
			if (Count > 3) ValueI[3] = (s32)(color.a * 255);
		}

	}

	virtual void setColor(video::SColor color)
	{
		reset();
		if (IsFloat)
		{
			if (Count > 0) ValueF[0] = (f32)color.getRed() / 255.0f;
			if (Count > 1) ValueF[1] = (f32)color.getGreen() / 255.0f;
			if (Count > 2) ValueF[2] = (f32)color.getBlue() / 255.0f;
			if (Count > 3) ValueF[3] = (f32)color.getAlpha() / 255.0f;
		}
		else
		{
			if (Count > 0) ValueI[0] = color.getRed();
			if (Count > 1) ValueI[1] = color.getGreen();
			if (Count > 2) ValueI[2] = color.getBlue();
			if (Count > 3) ValueI[3] = color.getAlpha();
		}
	}
	virtual void setRect(core::rect<s32> value)
	{
		reset();
		if (IsFloat)
		{
			if (Count > 0) ValueF[0] = (f32)value.UpperLeftCorner.X;
			if (Count > 1) ValueF[1] = (f32)value.UpperLeftCorner.Y;
			if (Count > 2) ValueF[2] = (f32)value.LowerRightCorner.X;
			if (Count > 3) ValueF[3] = (f32)value.LowerRightCorner.Y;
		}
		else
		{
			if (Count > 0) ValueI[0] = value.UpperLeftCorner.X;
			if (Count > 1) ValueI[1] = value.UpperLeftCorner.Y;
			if (Count > 2) ValueI[2] = value.LowerRightCorner.X;
			if (Count > 3) ValueI[3] = value.LowerRightCorner.Y;
		}
	}

	//! clear all values
	void reset()
	{
		
		for (u32 i=0; i < Count ; ++i)
			if (IsFloat)
				ValueF[i]=0.0f;
			else
				ValueI[i]=0;
	}


	core::array<f32> ValueF;
	core::array<s32> ValueI;
	u32 Count;
	bool IsFloat;
};


// Attribute implemented for floating point colors
class CColorfAttribute : public CNumbersAttribute
{
public:

	CColorfAttribute(const char* name, video::SColorf value) : CNumbersAttribute(name, value) {}

	virtual s32 getInt()
	{
		return getColor().color;
	}

	virtual f32 getFloat()
	{
		return (f32)getColor().color;
	}

	virtual void setInt(s32 intValue)
	{
		video::SColorf c = video::SColor(intValue);
		ValueF[0] = c.r;
		ValueF[1] = c.g;
		ValueF[2] = c.b;
		ValueF[3] = c.a;
	}

	virtual void setFloat(f32 floatValue)
	{
		setInt((s32)floatValue);
	}

	virtual E_ATTRIBUTE_TYPE getType()
	{
		return EAT_COLORF;
	}


	virtual const wchar_t* getTypeString()
	{
		return L"colorf";
	}
};



// Attribute implemented for colors
class CColorAttribute : public CNumbersAttribute
{
public:

	CColorAttribute(const char* name, video::SColorf value) : CNumbersAttribute(name, value) {}

	virtual s32 getInt()
	{
		return getColor().color;
	}

	virtual f32 getFloat()
	{
		return (f32)getColor().color;
	}

	virtual void setInt(s32 intValue)
	{
		video::SColorf c = video::SColor(intValue);
		ValueF[0] = c.r;
		ValueF[1] = c.g;
		ValueF[2] = c.b;
		ValueF[3] = c.a;
	}

	virtual void setFloat(f32 floatValue)
	{
		setInt((s32)floatValue);
	}

	virtual void getString(char* target)
	{
		video::SColor c = getColor();
		sprintf(target, "%08x", c.color);
	}

	virtual void setString(const char* text)
	{
		video::SColor c;
		sscanf(text, "%08x", &c.color);
		setColor(c);
	}

	virtual E_ATTRIBUTE_TYPE getType()
	{
		return EAT_COLOR;
	}


	virtual const wchar_t* getTypeString()
	{
		return L"color";
	}

};



// Attribute implemented for 3d vectors
class CVector3DAttribute : public CNumbersAttribute
{
public:

	CVector3DAttribute(const char* name, core::vector3df value) : CNumbersAttribute(name, value) {}

	virtual E_ATTRIBUTE_TYPE getType()
	{
		return EAT_VECTOR3D;
	}

	virtual const wchar_t* getTypeString()
	{
		return L"vector3d";
	}
};

// Attribute implemented for 2d vectors
class CPosition2DAttribute : public CNumbersAttribute
{
public:

	CPosition2DAttribute(const char* name, core::position2di value) : CNumbersAttribute(name, value) {}

	virtual E_ATTRIBUTE_TYPE getType()
	{
		return EAT_POSITION2D;
	}

	virtual const wchar_t* getTypeString()
	{
		return L"position";
	}
};



// Attribute implemented for rectangles
class CRectAttribute : public CNumbersAttribute
{
public:

	CRectAttribute(const char* name, core::rect<s32> value) : CNumbersAttribute(name, value) { }

	virtual E_ATTRIBUTE_TYPE getType()
	{
		return EAT_RECT;
	}

	virtual const wchar_t* getTypeString()
	{
		return L"rect";
	}

	core::rect<s32> Value;
};





// quaternion
// matrix4
// triangle3d
// vector2d
// line2d
// line3d
// dimension2d
// aabbox3d
// plane

/* 
	Special attributes
*/

// Attribute implemented for enumeration literals
class CEnumAttribute : public IAttribute
{
public:

	CEnumAttribute(const char* name, const char* value, const char* const* literals)
	{
		Name = name;
		setEnum(value, literals);
	}

	virtual void setEnum(const char* enumValue, const char* const* enumerationLiterals)
	{
		int literalCount = 0;

		if (enumerationLiterals)
		{
			s32 i;
			for (i=0; enumerationLiterals[i]; ++i)
				++literalCount;

			EnumLiterals.reallocate(literalCount);
			for (i=0; enumerationLiterals[i]; ++i)
				EnumLiterals.push_back(enumerationLiterals[i]);
		}

		setString(enumValue);
	}

	virtual s32 getInt()
	{
		for (s32 i=0; EnumLiterals.size(); ++i)
			if (Value.equals_ignore_case(EnumLiterals[i]))
			{
				return i;
			}

		return -1;
	}

	virtual f32 getFloat()
	{
		return (f32)getInt();
	}

	virtual bool getBool()
	{
		return (getInt() != 0); // does not make a lot of sense, I know
	}

	virtual void getString(char* target)
	{
		strcpy(target, Value.c_str());
	}

	virtual void setInt(s32 intValue)
	{
		if (intValue>=0 && intValue<(s32)EnumLiterals.size())
			Value = EnumLiterals[intValue];
		else
			Value = "";
	}

	virtual void setFloat(f32 floatValue)
	{
		setInt((s32)floatValue);
	};

	virtual void setString(const char* text)
	{
		Value = text;
	}

	virtual const char* getEnum()
	{
		return Value.c_str();
	}

	virtual E_ATTRIBUTE_TYPE getType()
	{
		return EAT_ENUM;
	}


	virtual const wchar_t* getTypeString()
	{
		return L"enum";
	}

	core::stringc Value;
	core::array<core::stringc> EnumLiterals;
};





// Attribute implemented for strings
class CStringAttribute : public IAttribute
{
public:

	CStringAttribute(const char* name, const char* value)
	{
		IsStringW=false;
		Name = name;
		setString(value);
	}
	
	CStringAttribute(const char* name, const wchar_t* value)
	{
		IsStringW = true;
		Name = name;
		setString(value);
	}

	CStringAttribute(const char* name, void* binaryData, s32 lenghtInBytes)
	{
		IsStringW=false;
		Name = name;
		setBinary(binaryData, lenghtInBytes);
	}

	virtual s32 getInt()
	{
		if (IsStringW)
			return atoi(core::stringc(ValueW.c_str()).c_str());
		else 
			return atoi(Value.c_str());
	}

	virtual f32 getFloat()
	{
		if (IsStringW)
			return core::fast_atof(core::stringc(ValueW.c_str()).c_str());
		else
			return core::fast_atof(Value.c_str());
	}

	virtual bool getBool()
	{
		if (IsStringW)
			return Value.equals_ignore_case(L"true");
		else
			return Value.equals_ignore_case("true");
	}

	virtual void getString(char* target)
	{
		if (IsStringW)
			strcpy(target, core::stringc(ValueW.c_str()).c_str() );
		else
			strcpy(target, Value.c_str());
	}
	virtual void getString(wchar_t* target)
	{
		if (IsStringW)
			swprintf(target, L"%s", ValueW.c_str() );
		else
			swprintf(target, L"%s", core::stringw(Value.c_str()).c_str());
	}

	virtual void setInt(s32 intValue)
	{
		if (IsStringW)
			ValueW = core::stringw(intValue);
		else
			Value = core::stringc(intValue);
	}

	virtual void setFloat(f32 floatValue)
	{
		if (IsStringW)
		{
			wchar_t tmp[32];
			swprintf(tmp, L"%f", floatValue);
			ValueW = tmp;
		}
		else
		{
			char tmp[32];
			sprintf(tmp, "%f", floatValue);
			Value = tmp;
		}
	};

	virtual void setString(const char* text)
	{
		if (IsStringW)
			ValueW = core::stringw(text);
		else
			Value = text;
	}

	virtual void setString(const wchar_t* text)
	{
		if (IsStringW)
			ValueW = text;
		else
			Value = core::stringc(text);
	}

	virtual E_ATTRIBUTE_TYPE getType()
	{
		return EAT_STRING;
	}


	virtual const wchar_t* getTypeString()
	{
		return L"string";
	}

	virtual void getBinary(void* outdata, s32 maxLenght)
	{
		s32 dataSize = maxLenght;
		c8* datac8 = (c8*)(outdata);
		s32 p = 0;
		const c8* dataString = Value.c_str();

		for (s32 i=0; i<dataSize; ++i)
			datac8[i] = 0;

		while(dataString[p] && p<dataSize)
		{
			s32 v = getByteFromHex((c8)dataString[p*2]) * 16;

			if (dataString[(p*2)+1])
				v += getByteFromHex((c8)dataString[(p*2)+1]);

			datac8[p] = v;
			++p;
		}
	};

	virtual void setBinary(void* data, s32 maxLenght)
	{
		s32 dataSize = maxLenght;
		c8* datac8 = (c8*)(data);
		char tmp[3];
		tmp[2] = 0;
		Value = "";

		for (s32 b=0; b<dataSize; ++b)
		{
			getHexStrFromByte(datac8[b], tmp);
			Value.append(tmp);
		}
	};

	bool IsStringW;
	core::stringc Value;
	core::stringw ValueW;

protected:

	static inline s32 getByteFromHex(c8 h)
	{
		if (h >= '0' && h <='9')
			return h-'0';

		if (h >= 'a' && h <='f')
			return h-'a' + 10;

		return 0;
	}

	static inline void getHexStrFromByte(c8 byte, c8* out)
	{
		s32 b = (byte & 0xf0) >> 4;

		for (s32 i=0; i<2; ++i)
		{
			if (b >=0 && b <= 9)
				out[i] = b+'0';
			if (b >=10 && b <= 15)
				out[i] = (b-10)+'a';

			b = byte & 0x0f;
		}
	}
};

// Attribute implemented for binary data
class CBinaryAttribute : public CStringAttribute
{
public:

	CBinaryAttribute(const char* name, void* binaryData, s32 lenghtInBytes)
		: CStringAttribute(name, binaryData, lenghtInBytes)
	{

	}

	virtual E_ATTRIBUTE_TYPE getType()
	{
		return EAT_BINARY;
	}


	virtual const wchar_t* getTypeString()
	{
		return L"binary";
	}
};



// Attribute implemented for texture references
class CTextureAttribute : public IAttribute
{
public:

	CTextureAttribute(const char* name, video::ITexture* value, video::IVideoDriver* driver)
		: Value(0), Driver(driver)
	{
		if (Driver)
			Driver->grab();

		Name = name;
		setTexture(value);
	}

	~CTextureAttribute()
	{
		if (Driver)
			Driver->drop();

		if (Value)
			Value->drop();
	}

	virtual video::ITexture* getTexture()
	{
		return Value;
	}

	virtual bool getBool()
	{
		return (Value != 0);
	}

	virtual void getString(char* target)
	{
		if (Value)
			strcpy(target, Value->getName().c_str());
		else
			target[0] = 0x0;
	}

	virtual void setString(const char* text)
	{
		if (Driver)
		{
			if (text && *text)
				setTexture(Driver->getTexture(text));
			else
				setTexture(0);
		}
	}

	virtual void setTexture(video::ITexture* value)
	{
		if (Value)
			Value->drop();

		Value = value;

		if (Value)
			Value->grab();
	}

	virtual E_ATTRIBUTE_TYPE getType()
	{
		return EAT_TEXTURE;
	}


	virtual const wchar_t* getTypeString()
	{
		return L"texture";
	}

	video::ITexture* Value;
	video::IVideoDriver* Driver;
};



// Attribute implemented for array of stringw
class CStringWArrayAttribute : public IAttribute
{
public:

	CStringWArrayAttribute(const char* name, core::array<core::stringw> value)
	{
		Name = name;
		setArray(value);
	}

	virtual core::array<core::stringw> getArray()
	{
		return Value;
	}

	virtual void setArray(core::array<core::stringw> value)
	{
		Value = value;
	}

	virtual E_ATTRIBUTE_TYPE getType()
	{
		return EAT_ARRAY;
	}

	virtual const wchar_t* getTypeString()
	{
		return L"array";
	}

	core::array<core::stringw> Value;
};




// todo: CGUIFontAttribute

} // end namespace io
} // end namespace irr
