// Copyright (C) 2002-2006 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "CImageLoaderJPG.h"
#include <string.h>
#include "CImage.h"
#include "os.h"

namespace irr
{
namespace video
{

//! constructor
CImageLoaderJPG::CImageLoaderJPG()
{
	#ifdef _DEBUG
	setDebugName("CImageLoaderJPG");
	#endif
}



//! destructor
CImageLoaderJPG::~CImageLoaderJPG()
{
}



//! returns true if the file maybe is able to be loaded by this class
//! based on the file extension (e.g. ".tga")
bool CImageLoaderJPG::isALoadableFileExtension(const c8* fileName)
{
	return strstr(fileName, ".jpg") != 0;
}


#ifdef _IRR_COMPILE_WITH_LIBJPEG_

void CImageLoaderJPG::init_source (j_decompress_ptr cinfo)
{
	// DO NOTHING
}



boolean CImageLoaderJPG::fill_input_buffer (j_decompress_ptr cinfo)
{
	// DO NOTHING
	return 1;
}



void CImageLoaderJPG::skip_input_data (j_decompress_ptr cinfo, long count)
{
	jpeg_source_mgr * src = cinfo->src;
	if(count > 0)
	{
		src->bytes_in_buffer -= count;
		src->next_input_byte += count;
	}
}



void CImageLoaderJPG::resync_to_restart (j_decompress_ptr cinfo, long desired)
{
	// DO NOTHING
}



void CImageLoaderJPG::term_source (j_decompress_ptr cinfo)
{
	// DO NOTHING
}
#endif


void CImageLoaderJPG::error_exit (j_common_ptr cinfo)
{
    // raise error
    throw 1;
}


//! returns true if the file maybe is able to be loaded by this class
bool CImageLoaderJPG::isALoadableFileFormat(irr::io::IReadFile* file)
{
    #ifndef _IRR_COMPILE_WITH_LIBJPEG_

    return false;

    #else

	if (!file)
		return false;

	s32 jfif = 0;
	file->seek(6);
	file->read(&jfif, sizeof(s32));
	return (jfif == 0x4a464946 || jfif == 0x4649464a);

	#endif
}

//! creates a surface from the file
IImage* CImageLoaderJPG::loadImage(irr::io::IReadFile* file)
{
    #ifndef _IRR_COMPILE_WITH_LIBJPEG_
    return 0;
    #else

	u8* input = new u8[file->getSize()];
	file->read(input, file->getSize());

	struct jpeg_decompress_struct cinfo;

	// allocate and initialize JPEG decompression object
	struct jpeg_error_mgr jerr;


    u16 rowspan;
    unsigned width, height, rowsRead;
    u8 *output, **rowPtr=0;

    try
    {

        //We have to set up the error handler first, in case the initialization
        //step fails.  (Unlikely, but it could happen if you are out of memory.)
        //This routine fills in the contents of struct jerr, and returns jerr's
        //address which we place into the link field in cinfo.

        cinfo.err = jpeg_std_error(&jerr);
        cinfo.err->error_exit = error_exit;
        // Now we can initialize the JPEG decompression object.
        jpeg_create_decompress(&cinfo);

        // specify data source
        jpeg_source_mgr jsrc;

        // Set up data pointer
        jsrc.bytes_in_buffer = file->getSize();
        jsrc.next_input_byte = (JOCTET*)input;
        cinfo.src = &jsrc;

        jsrc.init_source = init_source;
        jsrc.fill_input_buffer = fill_input_buffer;
        jsrc.skip_input_data = skip_input_data;
        jsrc.resync_to_restart = jpeg_resync_to_restart;
        jsrc.term_source = term_source;

        // Decodes JPG input from whatever source
        // Does everything AFTER jpeg_create_decompress
        // and BEFORE jpeg_destroy_decompress
        // Caller is responsible for arranging these + setting up cinfo

        // read file parameters with jpeg_read_header()
        (void) jpeg_read_header(&cinfo, TRUE);

        cinfo.out_color_space=JCS_RGB;
        cinfo.out_color_components=3;
        cinfo.do_fancy_upsampling=FALSE;

        // Start decompressor
        (void) jpeg_start_decompress(&cinfo);

        // Get image data
        rowspan = cinfo.image_width * cinfo.out_color_components;
        width = cinfo.image_width;
        height = cinfo.image_height;

        // Allocate memory for buffer
        output = new u8[rowspan * height];

        // Here we use the library's state variable cinfo.output_scanline as the
        // loop counter, so that we don't have to keep track ourselves.
        // Create array of row pointers for lib
        rowPtr = new u8 * [height];

        for( unsigned i = 0; i < height; i++ )
            rowPtr[i] = &output[ i * rowspan ];

        rowsRead = 0;

        while( cinfo.output_scanline < cinfo.output_height )
            rowsRead += jpeg_read_scanlines( &cinfo, &rowPtr[rowsRead], cinfo.output_height - rowsRead );

        delete [] rowPtr;
        // Finish decompression

        (void) jpeg_finish_decompress(&cinfo);


    }
    catch(int e)
    {
        // Release JPEG decompression object
        // This is an important step since it will release a good deal of memory.
        jpeg_destroy_decompress(&cinfo);

        // if the image data was created, delete it.
        if (rowPtr)
            delete [] rowPtr;

        // report the error

        c8 temp1[JMSG_LENGTH_MAX];
        c8 temp2[256];
        format_message ((jpeg_common_struct*)&cinfo, temp1);
        sprintf(temp2,"JPEG FATAL ERROR: %s",temp1);
	    os::Printer::log(temp2, ELL_ERROR);

        return 0;
    }

    // Release JPEG decompression object
    // This is an important step since it will release a good deal of memory.
    jpeg_destroy_decompress(&cinfo);

	// convert image
	IImage* image = new CImage(ECF_R8G8B8,
		core::dimension2d<s32>(width, height), output);

	delete [] input;

	return image;

	#endif
}

void CImageLoaderJPG::format_message (j_common_ptr cinfo, char * buffer)
{
  struct jpeg_error_mgr * err = cinfo->err;
  int msg_code = err->msg_code;
  const char * msgtext = NULL;
  const char * msgptr;
  char ch;
  boolean isstring;

  /* Look up message string in proper table */
  if (msg_code > 0 && msg_code <= err->last_jpeg_message) {
    msgtext = err->jpeg_message_table[msg_code];
  } else if (err->addon_message_table != NULL &&
	     msg_code >= err->first_addon_message &&
	     msg_code <= err->last_addon_message) {
    msgtext = err->addon_message_table[msg_code - err->first_addon_message];
  }

  /* Defend against bogus message number */
  if (msgtext == NULL) {
    err->msg_parm.i[0] = msg_code;
    msgtext = err->jpeg_message_table[0];
  }

  /* Check for string parameter, as indicated by %s in the message text */
  isstring = FALSE;
  msgptr = msgtext;
  while ((ch = *msgptr++) != '\0') {
    if (ch == '%') {
      if (*msgptr == 's') isstring = TRUE;
      break;
    }
  }

  /* Format the message into the passed buffer */
  if (isstring)
    sprintf(buffer, msgtext, err->msg_parm.s);
  else
    sprintf(buffer, msgtext,
	    err->msg_parm.i[0], err->msg_parm.i[1],
	    err->msg_parm.i[2], err->msg_parm.i[3],
	    err->msg_parm.i[4], err->msg_parm.i[5],
	    err->msg_parm.i[6], err->msg_parm.i[7]);
}



//! creates a loader which is able to load jpeg images
IImageLoader* createImageLoaderJPG()
{
	return new CImageLoaderJPG();
}

} // end namespace video
} // end namespace irr

