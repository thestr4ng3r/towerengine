
#ifdef BUILD_LIBPNG

#include "towerengine.h"

#include <png.h>



#ifdef _WIN32
FILE *fp;

void PNGCBAPI
user_png_read_data(png_structp png_ptr, png_bytep data, png_size_t length)
{
	png_size_t check;

	if(png_ptr == NULL)
		return;

	/* fread() returns 0 on error, so it is OK to store this in a png_size_t
	* instead of an int, which is what fread() actually returns.
	*/
	check = fread(data, 1, length, fp);

	if(check != length)
		png_error(png_ptr, "Read Error");
}
#endif


struct tPNGImage
{
#ifndef _WIN32
	FILE *fp;
#endif
	png_structp png_ptr;
	png_infop info_ptr;

	png_uint_32 width;
	png_uint_32 height;
	png_byte color_type;
	png_byte bit_depth;
	png_size_t row_bytes;

	png_bytep data;
	png_bytepp rows;

	GLenum gl_format;

	bool Load(const char *filename, bool mirror_y = true)
	{
		fp = fopen(filename, "rb");
		if(!fp)
		{
			fprintf(stderr, "Failed to open file \"%s\".\n", filename);
			return false;
		}

		png_byte header[8];
		memset(header, 0, sizeof(header));

		if(fread(header, 1, sizeof(header), fp) < sizeof(header))
		{
			fclose(fp);
			return false;
		}

		if(png_sig_cmp(header, 0, 8))
		{
			fclose(fp);
			return false;
		}


		png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);
		if(!png_ptr)
		{
			fclose(fp);
			return false;
		}

		info_ptr = png_create_info_struct(png_ptr);
		if(!info_ptr)
		{
			png_destroy_read_struct(&png_ptr, 0, 0);
			fclose(fp);
			return false;
		}


		if(setjmp(png_jmpbuf(png_ptr))) // libpng error
		{
			png_destroy_read_struct(&png_ptr, &info_ptr, 0);
			fclose(fp);
			return false;
		}

		png_init_io(png_ptr, fp);
		png_set_sig_bytes(png_ptr, 8);


		png_read_info(png_ptr, info_ptr);

		width = png_get_image_width(png_ptr, info_ptr);
		height = png_get_image_height(png_ptr, info_ptr);
		color_type = png_get_color_type(png_ptr, info_ptr);
		bit_depth = png_get_bit_depth(png_ptr, info_ptr);
		row_bytes = png_get_rowbytes(png_ptr, info_ptr);

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
				fprintf(stderr, "File \"%s\" has unsupported color format.\n", filename);
				png_destroy_read_struct(&png_ptr, &info_ptr, 0);
				fclose(fp);
				return 0;
		}


		data = (png_bytep)png_malloc(png_ptr, row_bytes * height);
		
		rows = (png_bytepp)png_malloc(png_ptr, sizeof(png_bytep) * height);
		for(int i=0; i<height; i++)
			rows[i] = &(data[row_bytes * (mirror_y ? (height - i - 1) : i)]);

#ifdef _WIN32
		png_set_read_fn(png_ptr, fp, user_png_read_data);
#endif
		png_read_image(png_ptr, rows);
		png_read_end(png_ptr, info_ptr);

		return true;
	}

	void Destroy(void)
	{
		png_free(png_ptr, data);
		png_free(png_ptr, rows);
		png_destroy_read_struct(&png_ptr, &info_ptr, 0);
		fclose(fp);
	}

	bool IsTransparent(void)
	{
		int stride;
		int offset;

		switch(gl_format)
		{
			case GL_RGBA:
				stride = 4;
				offset = 3;
				break;

			case GL_RGB:
			case GL_LUMINANCE:
			default:
				return false;
		}

		for(int i=0; i<sizeof(data); i+=stride)
		{
			if(data[i + offset] < 255)
				return true;
		}

		return false;
	}

	int GetBytesPerPixel(void)
	{
		switch(gl_format)
		{
			case GL_RGBA:
				return 4;
			case GL_RGB:
				return 3;
			case GL_LUMINANCE:
			default:
				return 1;
		}
	}
};

GLuint LoadGLTexture_PNG(const char *filename, int channels, int *w, int *h, bool *transparent)
{
	tPNGImage image;

	if(!image.Load(filename))
		return 0;

	GLuint tex;
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, image.gl_format, image.width, image.height, 0, image.gl_format, GL_UNSIGNED_BYTE, image.data);
	glGenerateMipmap(GL_TEXTURE_2D);

	if(w)
		*w = image.width;
	if(h)
		*h = image.height;
	if(transparent)
		*transparent = image.IsTransparent();

	image.Destroy();

	return tex;
}




GLuint LoadGLTextureArray_PNG(const char **filenames, const int count, int *w, int *h)
{
	tPNGImage *images = new tPNGImage[count];

	int i;
	for(i=0; i<count; i++)
	{
		if(!images[i].Load(filenames[i]))
			break;

		if(i > 0)
		{
			if(images[i].width != images[0].width
					|| images[i].height != images[0].height
					|| images[i].gl_format != images[0].gl_format)
			{
				fprintf(stderr, "File \"%s\" of array has different dimensions or format.\n", filenames[i]);
				images[i].Destroy();
				break;
			}
		}
	}

	if(i < count) // for loop exited early
	{
		for(i--; i >= 0; i--)
		{
			images[i].Destroy();
		}
		return 0;
	}


	GLuint tex;
	glGenTextures(1, &tex);

	glBindTexture(GL_TEXTURE_2D_ARRAY, tex);
	glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, images[0].gl_format, images[0].width, images[0].height, count, 0, images[0].gl_format, GL_UNSIGNED_BYTE, 0);

	for(i=0; i<count; i++)
	{
		glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, i, images[i].width, images[i].height, 1, GL_RGBA, GL_UNSIGNED_BYTE, images[i].data);
		images[i].Destroy();
	}

	if(w != 0)
		*w = images[0].width;
	if(h != 0)
		*h = images[0].height;

	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glGenerateMipmap(GL_TEXTURE_2D_ARRAY);

	delete[] images;

	return tex;
}



GLuint LoadGLCubeMap_PNG(const char *filename[6])
{
	tPNGImage images[6];

	int i;
	for(i=0; i<6; i++)
	{
		if(!images[i].Load(filename[i]))
			break;
	}

	if(i<6) // for loop exited early
	{
		for(i--; i>=0; i--)
			images[i].Destroy();
		return 0;
	}

	GLuint tex;
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_CUBE_MAP, tex);

	for(i=0; i<6; i++)
	{
		glTexImage2D(CubeTex(i), 0, images[i].gl_format, images[i].width, images[i].height, 0, images[i].gl_format, GL_UNSIGNED_BYTE, images[i].data);
		images[i].Destroy();
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

	return tex;
}


GLuint LoadGLCubeMap_PNG(const char *filename)
{
	tPNGImage image;
	if(!image.Load(filename, false))
		return 0;


	GLuint tex;
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_CUBE_MAP, tex);

	/*
	 * ----------------
	 * | -x | -z | +x |
	 * ----------------
	 * | -y | +y | +z |
	 * ----------------
	 */

	int width = image.width;
	int height = image.height;
	int side_width = image.width / 3;
	int side_height = image.height / 2;
	GLenum format = image.gl_format;
	GLint bpp = image.GetBytesPerPixel();
	GLubyte *data = image.data;

	GLubyte *side_data = new GLubyte[side_width * side_height * bpp];


	GetSubImage(side_data, data, width, height, bpp, 0, 0, side_width, side_height);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, format, side_width, side_height, 0, format, GL_UNSIGNED_BYTE, side_data);

	GetSubImage(side_data, data, width, height, bpp, side_width, 0, side_width, side_height);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, format, side_width, side_height, 0, format, GL_UNSIGNED_BYTE, side_data);

	GetSubImage(side_data, data, width, height, bpp, side_width * 2, 0, side_width, side_height);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, format, side_width, side_height, 0, format, GL_UNSIGNED_BYTE, side_data);


	GetSubImage(side_data, data, width, height, bpp, 0, side_height, side_width, side_height);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, format, side_width, side_height, 0, format, GL_UNSIGNED_BYTE, side_data);

	GetSubImage(side_data, data, width, height, bpp, side_width, side_height, side_width, side_height);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, format, side_width, side_height, 0, format, GL_UNSIGNED_BYTE, side_data);

	GetSubImage(side_data, data, width, height, bpp, side_width * 2, side_height, side_width, side_height);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, format, side_width, side_height, 0, format, GL_UNSIGNED_BYTE, side_data);

	delete [] side_data;

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

	image.Destroy();

	return tex;
}


#endif
