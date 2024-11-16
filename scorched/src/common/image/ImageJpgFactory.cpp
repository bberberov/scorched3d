////////////////////////////////////////////////////////////////////////////////
//    Scorched3D (c) 2000-2011
//
//    This file is part of Scorched3D.
//
//    Scorched3D is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    Scorched3D is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License along
//    with this program; if not, write to the Free Software Foundation, Inc.,
//    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <math.h>

#ifdef __cplusplus
extern "C" { 
#endif /* __cplusplus */

#ifdef __DARWIN__
#include <UnixImageIO/jpeglib.h>
#else
#include <jpeglib.h>
#endif

#ifdef __cplusplus
}
#endif

#include <common/Defines.h>
#include <image/ImageJpgFactory.h>
#include <setjmp.h>

Image ImageJpgFactory::loadFromFile(const char * filename, bool readalpha)
{
	FILE *file = fopen(filename, "rb");
	if (!file) return Image();

	int read = 0;
	char buffer[256];
	NetBuffer netBuffer;
	while (read = (int) fread(buffer, 1, 256, file))
	{
		netBuffer.addDataToBuffer(buffer, read);
	}
	fclose(file);

	std::string errorMessage;
	Image result = loadFromBuffer(netBuffer, readalpha, errorMessage);
	if (!result.getBits())
	{
		S3D::dialogExit("Scorched3D", 
			S3D::formatStringBuffer("Failed to load jpg file %s, %s\n", 
			filename, errorMessage.c_str()));
	}
	return result;
}

METHODDEF(void)
init_source (j_decompress_ptr cinfo)
{
}

METHODDEF(boolean)
fill_input_buffer (j_decompress_ptr cinfo)
{
	struct jpeg_source_mgr * src = cinfo->src;
	static JOCTET FakeEOI[] = { 0xFF, JPEG_EOI };

	/* Insert a fake EOI marker */
	src->next_input_byte = FakeEOI;
	src->bytes_in_buffer = 2;

	return TRUE;
}

METHODDEF(void)
skip_input_data (j_decompress_ptr cinfo, long num_bytes)
{
	struct jpeg_source_mgr * src = cinfo->src;

	if(num_bytes >= (long)src->bytes_in_buffer)
	{
		fill_input_buffer(cinfo);
		return;
	}

	src->bytes_in_buffer -= num_bytes;
	src->next_input_byte += num_bytes;
}

METHODDEF(void)
term_source (j_decompress_ptr cinfo)
{
  /* no work necessary here */
}

struct my_error_mgr {
  struct jpeg_error_mgr pub;	/* "public" fields */

  jmp_buf setjmp_buffer;	/* for return to caller */
};

typedef struct my_error_mgr *my_error_ptr;

METHODDEF(void)
my_error_exit (j_common_ptr cinfo)
{
  /* cinfo->err really points to a my_error_mgr struct, so coerce pointer */
  my_error_ptr myerr = (my_error_ptr) cinfo->err;

  /* Always display the message. */
  /* We could postpone this until after returning, if we chose. */
  (*cinfo->err->output_message) (cinfo);

  /* Return control to the setjmp point */
  longjmp(myerr->setjmp_buffer, 1);
}

Image ImageJpgFactory::loadFromBuffer(NetBuffer &buffer, bool readalpha, std::string &errorMessage)
{
	struct jpeg_decompress_struct cinfo;
	struct my_error_mgr jerr;

	Image result;

	cinfo.err = jpeg_std_error(&jerr.pub);	
	jerr.pub.error_exit = my_error_exit;
	/* Establish the setjmp return context for my_error_exit to use. */
	if (setjmp(jerr.setjmp_buffer)) {
		/* If we get here, the JPEG code has signaled an error.
		* We need to clean up the JPEG object, close the input file, and return.
		*/
		errorMessage = "LibJPEG error";
		jpeg_destroy_decompress(&cinfo);
		return result;
	}

	jpeg_create_decompress(&cinfo);

	// Get the buffer reading entry points
	if (!cinfo.src)
	{
		cinfo.src = (struct jpeg_source_mgr *)
			(*cinfo.mem->alloc_small) ((j_common_ptr) &cinfo, JPOOL_PERMANENT,
				sizeof(struct jpeg_source_mgr));
	}
	struct jpeg_source_mgr *src = cinfo.src;

	// Set up buffer reading functions
	src->init_source = init_source;
	src->fill_input_buffer = fill_input_buffer;
	src->skip_input_data = skip_input_data;
	src->resync_to_restart = jpeg_resync_to_restart; /* use default method */
	src->term_source = term_source;

	// Set up buffer
	src->bytes_in_buffer = buffer.getBufferUsed();
	src->next_input_byte = (JOCTET *) buffer.getBuffer();

	jpeg_read_header(&cinfo, TRUE);
	jpeg_start_decompress(&cinfo);

	if ((cinfo.output_components == 3 && !readalpha) ||
		(cinfo.output_components == 4 && readalpha) ||
		(cinfo.output_components == 1 && !readalpha))
	{
		result = Image(cinfo.output_width, cinfo.output_height, cinfo.output_components, 255);
		result.setLossless(false);

		JSAMPARRAY buffer = (*cinfo.mem->alloc_sarray)
			((j_common_ptr) &cinfo, JPOOL_IMAGE, 
				cinfo.output_width * cinfo.output_components, 1);
		while (cinfo.output_scanline < cinfo.output_height)
		{
			int currentLine = cinfo.output_scanline;
			int lines = jpeg_read_scanlines(&cinfo, buffer, 1);
			if (lines > 0)
			{
				int destPos = cinfo.output_width * cinfo.output_components * 
					(cinfo.output_height - currentLine - 1);
				JSAMPLE *src = buffer[0];
				unsigned char *dest = &result.getBits()[destPos];

				for (unsigned int i = 0; i < cinfo.output_width; ++i, 
					dest+=cinfo.output_components, src+=cinfo.output_components)
				{
					dest[0] = src[0];
					if (cinfo.output_components > 1)
					{
						dest[1] = src[1];
						dest[2] = src[2];
						if (cinfo.output_components > 3)
						{
							dest[3] = src[3];
						}
					}
				}
			}
		}
	} 
	else 
	{
		errorMessage = "Image format not supported";
	}

	jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);

	return result;
}
