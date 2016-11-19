
#ifdef BUILD_LIBPNG

#include "towerengine.h"

#include <png.h>

GLuint LoadGLTexture_PNG(const char *filename, int channels, int *w, int *h, bool *transparent, int alpha_channel)
{
	FILE *fp = fopen(filename, "rb");
	if(!fp)
	{
		fprintf(stderr, "Failed to open file \"%s\".\n", filename);
		return 0;
	}

	png_byte header[8];
	memset(header, 0, sizeof(header));

	if(fread(header, 1, sizeof(header), fp) < sizeof(header))
	{
		fclose(fp);
		return 0;
	}

	if(png_sig_cmp(header, 0, 8))
	{
		fclose(fp);
		return 0;
	}


	png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);
	if(!png_ptr)
	{
		fclose(fp);
		return 0;
	}

	png_infop info_ptr = png_create_info_struct(png_ptr);
	if(!info_ptr)
	{
		png_destroy_read_struct(&png_ptr, 0, 0);
		fclose(fp);
		return 0;
	}


	if(setjmp(png_jmpbuf(png_ptr))) // libpng error
	{
		png_destroy_read_struct(&png_ptr, &info_ptr, 0);
		fclose(fp);
		return 0;
	}

	png_init_io(png_ptr, fp);
	png_set_sig_bytes(png_ptr, 8);


	png_read_info(png_ptr, info_ptr);

	png_uint_32 width = png_get_image_width(png_ptr, info_ptr);
	png_uint_32 height = png_get_image_height(png_ptr, info_ptr);
	png_byte color_type = png_get_color_type(png_ptr, info_ptr);
	png_byte bit_depth = png_get_bit_depth(png_ptr, info_ptr);
	png_size_t row_bytes = png_get_rowbytes(png_ptr, info_ptr);


	GLenum gl_format;
	switch(color_type)
	{
		case PNG_COLOR_TYPE_RGB:
			gl_format = GL_RGB;
			break;
		case PNG_COLOR_TYPE_RGBA:
			gl_format = GL_RGBA;
			break;
		case PNG_COLOR_TYPE_GRAY:
			gl_format = GL_LUMINANCE;
			break;
		default:
			printf("File \"%s\" has unsupported color format.\n", filename);
			png_destroy_read_struct(&png_ptr, &info_ptr, 0);
			fclose(fp);
			return 0;
	}


	png_bytep data = (png_bytep)png_malloc(png_ptr, row_bytes * height);

	png_bytepp rows = (png_bytepp)png_malloc(png_ptr, sizeof(png_bytep) * height);
	for(int i=0; i<height; i++)
		rows[i] = &data[row_bytes * i];

	png_read_image(png_ptr, rows);
	png_read_end(png_ptr, info_ptr);



	GLuint tex;
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, gl_format, width, height, 0, gl_format, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);



	png_free(png_ptr, data);
	png_free(png_ptr, rows);
	png_destroy_read_struct(&png_ptr, &info_ptr, 0);
	fclose(fp);

	return tex;
}

#endif
