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
#include <common/Defines.h>
#include <common/Logger.h>
#include <image/ImagePngFactory.h>
#ifdef __DARWIN__
#include <UnixImageIO/png.h>
#else
#include <png.h>
#endif

Image ImagePngFactory::loadFromFile(const char *filename, bool readalpha)
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

	Image result = loadFromBuffer(netBuffer, readalpha);
	if (!result.getBits())
	{
		Logger::log(
			S3D::formatStringBuffer("Failed to load PNG file \"%s\"", filename));
	}
	return result;
}

struct user_read_struct
{
	user_read_struct(NetBuffer &b) : buffer(b), position(0) {}

	int position;
	NetBuffer &buffer;
};

static void user_png_error(png_structp png_ptr, png_const_charp msg) 
{
	longjmp(png_jmpbuf(png_ptr),1);
}

static void user_png_warning(png_structp png_ptr, png_const_charp msg) 
{
}

static void user_read_fn(png_structp png_ptr,
        png_bytep data, png_size_t length)
{
	user_read_struct *read_io_ptr = (user_read_struct *) png_get_io_ptr(png_ptr);

	int length_left = MIN(length, read_io_ptr->buffer.getBufferUsed() - read_io_ptr->position);
	memcpy(data, &read_io_ptr->buffer.getBuffer()[read_io_ptr->position], length_left);
	read_io_ptr->position += length_left;
}

// CODE TAKEN FROM PNG SUPPLIED example.c
Image ImagePngFactory::loadFromBuffer(NetBuffer &buffer, bool readalpha)
{
	png_structp png_ptr;
	png_infop info_ptr;

	/* Create and initialize the png_struct with the desired error handler
	* functions.  If you want to use the default stderr and longjump method,
	* you can supply NULL for the last three parameters.  We also supply the
	* the compiler header file version, so that we know if the application
	* was compiled with a compatible version of the library.  REQUIRED
	*/
	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);
	if (png_ptr == NULL)
	{
	   return Image();
	}

	/* Allocate/initialize the memory for image information.  REQUIRED. */
	info_ptr = png_create_info_struct(png_ptr);
	if (info_ptr == NULL)
	{
	  png_destroy_read_struct(&png_ptr, (png_infopp) 0, (png_infopp) 0);
	  return Image();
	}

	/* Set error handling if you are using the setjmp/longjmp method (this is
	* the normal method of doing things with libpng).  REQUIRED unless you
	* set up your own error handlers in the png_create_read_struct() earlier.
	*/
	if (setjmp(png_jmpbuf(png_ptr)))
	{
	  /* Free all of the memory associated with the png_ptr and info_ptr */
	  png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp) 0);
	  /* If we get here, we had a problem reading the file */
	  return Image();
	}
	png_set_error_fn(png_ptr, NULL, user_png_error, user_png_warning);

	/* If you are using replacement read functions, instead of calling
	* png_init_io() here you would call:
	*/
	user_read_struct read_struct(buffer);
	png_set_read_fn(png_ptr, (void *)&read_struct, user_read_fn);
	/* where user_io_ptr is a structure you want available to the callbacks */

	/*
	* If you have enough memory to read in the entire image at once,
	* and you need to specify only transforms that can be controlled
	* with one of the PNG_TRANSFORM_* bits (this presently excludes
	* dithering, filling, setting background, and doing gamma
	* adjustment), then you can read the entire image (including
	* pixels) into the info structure with this call:
	*/
	unsigned int settings = 
		PNG_TRANSFORM_PACKING | 
		PNG_TRANSFORM_STRIP_16 | 
		PNG_TRANSFORM_EXPAND;
	if (!readalpha) settings |= PNG_TRANSFORM_STRIP_ALPHA;

	png_read_png(png_ptr, info_ptr, settings, NULL);

	/* At this point you have read the entire image */

	// NEW CODE
	png_uint_32 width = png_get_image_width(png_ptr, info_ptr);
	png_uint_32 height = png_get_image_height(png_ptr, info_ptr);
	png_uint_32 bytes = png_get_rowbytes(png_ptr, info_ptr);
	png_uint_32 bitdepth = png_get_bit_depth(png_ptr, info_ptr);
	png_uint_32 coltype = png_get_color_type(png_ptr, info_ptr);
	png_byte channels = png_get_channels(png_ptr, info_ptr);

	Image result;
	if (coltype == (readalpha?PNG_COLOR_TYPE_RGB_ALPHA:PNG_COLOR_TYPE_RGB) &&
		bitdepth == 8 &&
		channels == (readalpha?4:3) &&
		(bytes / width) == (readalpha?4:3))
	{
		png_bytepp row_pointers = png_get_rows(png_ptr, info_ptr);

		result = Image(width, height, readalpha);

		for (unsigned int h=0; h<height; h++)
		{
			memcpy(result.getBits() + bytes * (height - 1 - h), row_pointers[h], bytes);
		}
	}
	else
	{
		png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp) 0);

		Logger::log(S3D::formatStringBuffer(
			"Invalid PNG format.\n"
			"width %d, hei %d, rowbytes %d, bitd %d, "
			"colt %d, channels %d\n",
            (int)width,(int)height,(int)bytes,(int)bitdepth,
            (int)coltype,(int)channels));
		return Image();
	}

	// END NEW CODE

	/* clean up after the read, and free any memory allocated - REQUIRED */
	png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp) 0);

	/* that's it */
	return result;
}

struct user_write_struct
{
	user_write_struct(NetBuffer &b) : buffer(b) {}

	NetBuffer &buffer;
};

static void user_write_fn(png_structp png_ptr,
        png_bytep data, png_size_t length)
{
	user_write_struct *write_io_ptr = (user_write_struct *) png_get_io_ptr(png_ptr);

	write_io_ptr->buffer.addDataToBuffer(data, length);
}

bool ImagePngFactory::writeToBuffer(Image image, NetBuffer &buffer)
{
	png_structp png_ptr;
	png_infop info_ptr;
	int y;

	// Create structs
	png_ptr = 
		png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (!png_ptr) return false;

	info_ptr = 
		png_create_info_struct(png_ptr);
	if (!info_ptr) return false;

	// Set Error Handling
	if (setjmp(png_jmpbuf(png_ptr)))
	{
		png_destroy_write_struct(&png_ptr, &info_ptr);
		return false;
	}
	png_set_error_fn(png_ptr, NULL, user_png_error, user_png_warning);

	// Set output
	user_read_struct read_struct(buffer);
	png_set_read_fn(png_ptr, (void *)&read_struct, user_read_fn);

	// png_init_io(png_ptr, fp);
	user_write_struct write_struct(buffer);
	png_set_write_fn(png_ptr, (void *)&write_struct, user_write_fn, NULL);

	// Write Header
	png_set_IHDR(png_ptr, info_ptr, 
		image.getWidth(), image.getHeight(),
		8, 
		PNG_COLOR_TYPE_RGB, 
		PNG_INTERLACE_NONE,
		PNG_COMPRESSION_TYPE_DEFAULT,
		PNG_FILTER_TYPE_DEFAULT);

	// Write File
	png_write_info(png_ptr, info_ptr);
	for (y=0; y<image.getHeight(); y++) 
	{
		png_write_row(png_ptr, &image.getBits()[(image.getHeight() - y - 1) * image.getWidth() * image.getComponents()]);
	}
	png_write_end(png_ptr, NULL);

	// Tidy
	png_destroy_write_struct(&png_ptr, &info_ptr);

	return true;
}
