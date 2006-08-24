
#include "CImageWriterPNG.h"
#include "CImageLoaderPNG.h"
#include "CColorConverter.h"
#include "IWriteFile.h"
#include "irrString.h"
#include "os.h" // for logging
#include <string.h>

#include "IrrCompileConfig.h"
#ifdef _IRR_COMPILE_WITH_LIBPNG_
#ifndef _IRR_USE_NON_SYSTEM_LIB_PNG_
	#include <png.h> // use system lib png
#else // _IRR_USE_NON_SYSTEM_LIB_PNG_
	#include "libpng/png.h" // use irrlicht included lib png
#endif // _IRR_USE_NON_SYSTEM_LIB_PNG_
#endif // _IRR_COMPILE_WITH_LIBPNG_

namespace irr
{
namespace video
{

IImageWriter* createImageWriterPNG()
{
	return new CImageWriterPNG;
}

#ifdef _IRR_COMPILE_WITH_LIBPNG_
// PNG function for error handling
static void png_cpexcept_error(png_structp png_ptr, png_const_charp msg)
{
	os::Printer::log("PNG FATAL ERROR", msg, ELL_ERROR);
	longjmp(png_ptr->jmpbuf, 1);
}

// PNG function for file writing
void user_write_data_fcn(png_structp png_ptr, png_bytep data, png_size_t length)
{
	png_size_t check;

	irr::io::IWriteFile* file=(irr::io::IWriteFile*)png_ptr->io_ptr;
	check=(png_size_t) file->write((void*)data,length);

	if (check != length)
		png_error(png_ptr, "Write Error");
}
#endif // _IRR_COMPILE_WITH_LIBPNG_

CImageWriterPNG::CImageWriterPNG()
{
#ifdef _DEBUG
	setDebugName("CImageWriterPNG");
#endif
}

bool CImageWriterPNG::isAWriteableFileExtension(const c8* fileName)
{
#ifdef _IRR_COMPILE_WITH_LIBPNG_
	return strstr(fileName, ".png") != 0;
#else
	return false;
#endif
}

bool CImageWriterPNG::writeImage(io::IWriteFile* file, IImage* image)
{
#ifdef _IRR_COMPILE_WITH_LIBPNG_
	if (!file || !image)
		return false;

	// Allocate the png write struct
	png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING,
		NULL, (png_error_ptr)png_cpexcept_error, NULL);
	if (!png_ptr)
	{
		os::Printer::log("LOAD PNG: Internal PNG create write struct failure\n", file->getFileName(), ELL_ERROR);
		return false;
	}

	// Allocate the png info struct
	png_infop info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr)
	{
		os::Printer::log("LOAD PNG: Internal PNG create info struct failure\n", file->getFileName(), ELL_ERROR);
		png_destroy_write_struct(&png_ptr, NULL);
		return false;
	}

	// for proper error handling
	if (setjmp(png_jmpbuf(png_ptr)))
	{
		png_destroy_write_struct(&png_ptr, &info_ptr);
		return false;
	}

	png_set_write_fn(png_ptr, file, user_write_data_fcn, NULL);

os::Printer::log(core::stringc(image->getColorFormat()).c_str());
	// Set info
	png_set_IHDR(png_ptr, info_ptr,
		image->getDimension().Width, image->getDimension().Height,
		8, image->getColorFormat()==ECF_A8R8G8B8?PNG_COLOR_TYPE_RGB_ALPHA:PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

	// Create array of pointers to rows in image data
	RowPointers = new png_bytep[image->getDimension().Height];
	if (!RowPointers)
	{
		os::Printer::log("LOAD PNG: Internal PNG create row pointers failure\n", file->getFileName(), ELL_ERROR);
		png_destroy_write_struct(&png_ptr, &info_ptr);
		return false;
	}

	// Fill array of pointers to rows in image data
	unsigned char* data = (unsigned char*)image->lock();
	for (u32 i=0; i<image->getDimension().Height; ++i)
	{
		RowPointers[i]=data;
		data += image->getPitch();
	}
	// for proper error handling
	if (setjmp(png_jmpbuf(png_ptr)))
	{
		png_destroy_write_struct(&png_ptr, &info_ptr);
		delete [] RowPointers;
		image->unlock();
		return false;
	}

	png_set_rows(png_ptr, info_ptr, RowPointers);

	png_write_png(png_ptr, info_ptr, PNG_TRANSFORM_BGR, NULL);

	delete [] RowPointers;
	image->unlock();
	png_destroy_write_struct(&png_ptr, &info_ptr);
	return true;
#else
	return false;
#endif
}

}; // namespace video
}; // namespace irr
