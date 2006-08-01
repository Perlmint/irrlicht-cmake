
#include "CImageWriterPNG.h"
#include "CImageLoaderPNG.h"
#include "CColorConverter.h"
#include "IWriteFile.h"
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

CImageWriterPNG::CImageWriterPNG()
{
#ifdef _DEBUG
	setDebugName("CImageWriterPNG");
#endif
}

bool CImageWriterPNG::isAWriteableFileExtension(const c8* fileName)
{
	return strstr(fileName, ".png") != 0;
}

bool CImageWriterPNG::writeImage(io::IWriteFile* file, IImage* image)
{
#ifndef _IRR_COMPILE_WITH_LIBPNG_
	return false;
#else
	os::Printer::log("PNG writer not yet implemented. Image not written.", ELL_WARNING);
	return false;
#endif
}

}; // namespace video
}; // namespace irr
