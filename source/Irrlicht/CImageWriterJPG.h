#ifndef _C_IMAGE_WRITER_JPG_H_INCLUDED__
#define _C_IMAGE_WRITER_JPG_H_INCLUDED__

#include "IImageWriter.h"


#include "IrrCompileConfig.h"
#ifdef _IRR_COMPILE_WITH_LIBJPEG_
extern "C" {
	#ifndef _IRR_USE_NON_SYSTEM_JPEG_LIB_
	#include <jpeglib.h> // use system lib
	#else
	#include "jpeglib/jpeglib.h" // use irrlicht jpeglib
	#endif
	#include <setjmp.h>
}
#endif // _IRR_COMPILE_WITH_LIBJPEG_


namespace irr
{
namespace video
{

class CImageWriterJPG : public IImageWriter
{
public:
	//! constructor
	CImageWriterJPG();

	//! return true if this writer can write a file with the given extension
	virtual bool isAWriteableFileExtension(const c8* fileName);

	//! write image to file
	virtual bool writeImage(io::IWriteFile *file, IImage *image, u32 param);

private:
};

}; // namespace video
}; // namespace irr

#endif // _C_IMAGE_WRITER_JPG_H_INCLUDED__
