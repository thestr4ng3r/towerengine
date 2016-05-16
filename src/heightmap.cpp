#include "towerengine.h"

#include <IL/il.h>
#include <IL/ilu.h>

tHeightMap::tHeightMap(int size)
{
	this->size = size;
	height_tex = 0;
	data = new float [size*size];
}

bool tHeightMap::Load(const char *file)
{
	ILuint image;
	ILenum error;
	ILubyte *image_data;
	int x, y;

	ilGenImages(1, &image);
	ilBindImage(image);
	if(!ilLoadImage(file))
	{
		error = ilGetError();
		printf("Failed to load image \"%s\": %s\n", file, iluErrorString(error));
		return false;
	}

	ILinfo ImageInfo;
	iluGetImageInfo(&ImageInfo);
	if(ImageInfo.Origin == IL_ORIGIN_UPPER_LEFT)
		iluFlipImage();

	ilConvertImage(IL_RGB, IL_UNSIGNED_BYTE);
	iluImageParameter(ILU_FILTER, ILU_BILINEAR);
	iluScale(size, size, ilGetInteger(IL_IMAGE_DEPTH));

	image_data = ilGetData();

	for(x=0; x<size; x++)
		for(y=0; y<size; y++)
			data[y*size + x] = (float)image_data[y*size*3 + x*3] / 255.0f;


	glGenTextures(1, &height_tex);

	glBindTexture(GL_TEXTURE_2D, height_tex);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);

	glGenerateMipmap(GL_TEXTURE_2D);


	glTexImage2D(GL_TEXTURE_2D, 				// Type of texture
				 0,								// Pyramid level (for mip-mapping) - 0 is the top level
				 ilGetInteger(IL_IMAGE_BPP),	// Image colour depth
				 size,							// Image width
				 size,							// Image height
				 0,								// Border width in pixels (can either be 1 or 0)
				 ilGetInteger(IL_IMAGE_FORMAT),	// Image format (i.e. RGB, RGBA, BGR etc.)
				 GL_UNSIGNED_BYTE,				// Image data type
				 image_data);							// The actual image data itself


	ilDeleteImages(1, &image);

	return true;
}
