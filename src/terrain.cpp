#include "towerengine.h"

CTerrain::CTerrain(CHeightMap *height_map, float size, float height, CTerrainMaterial *material)
{
	float *vertex_data, *normal_data, *tang_data, *bitang_data, *uvcoord_data;
	CVector ***face_normals;
	CVector a, b, c;
	CVector n;
	int n_count;
	GLuint *ibo_data;
	int x, y, d;
	float tile_size;

	this->height_map = height_map;
	map_size = height_map->GetSize();
	this->size = size;
	this->height = height;
	tile_size = size / (map_size - 1);

	this->material = material;

	vao = new VAO();
	vertex_vbo = new VBO<float>(3, vao, map_size * map_size);
	normal_vbo = new VBO<float>(3, vao, map_size * map_size);
	tang_vbo = new VBO<float>(3, vao, map_size * map_size);
	bitang_vbo = new VBO<float>(3, vao, map_size * map_size);
	uvcoord_vbo = new VBO<float>(2, vao, map_size * map_size);

	vertex_data = vertex_vbo->GetData();
	normal_data = normal_vbo->GetData();
	tang_data = tang_vbo->GetData();
	bitang_data = bitang_vbo->GetData();
	uvcoord_data = uvcoord_vbo->GetData();
	for(x=0; x<map_size; x++)
	{
		for(y=0; y<map_size; y++)
		{
			d = y * map_size * 3 + x * 3;

			vertex_data[d + 0] = -(size / 2) + tile_size * x;
			vertex_data[d + 1] = height_map->GetValue(x, y) * height;
			vertex_data[d + 2] = -(size / 2) + tile_size * y;

			tang_data[d + 0] = 1.0;
			tang_data[d + 1] = 0.0;
			tang_data[d + 2] = 0.0;

			bitang_data[d + 0] = 0.0;
			bitang_data[d + 1] = 0.0;
			bitang_data[d + 2] = 1.0;

			d = y * map_size * 2 + x * 2;
			uvcoord_data[d + 0] = ((float)x / (float)map_size) * 20.0;
			uvcoord_data[d + 1] = ((float)y / (float)map_size) * 20.0;
		}
	}

	vertex_vbo->AssignData();
	tang_vbo->AssignData();
	bitang_vbo->AssignData();
	uvcoord_vbo->AssignData();

	ibo = new IBO(vao, (map_size - 1) * (map_size - 1) * 6);

	face_normals = new CVector **[map_size-1];

	ibo_data = ibo->GetData();
	d = 0;
	for(x=0; x<map_size-1; x++)
	{
		face_normals[x] = new CVector *[map_size-1];
		for(y=0; y<map_size-1; y++)
		{
			face_normals[x][y] = new CVector[2];

			ibo_data[d + 0] = y * map_size + x;
			ibo_data[d + 1] = ibo_data[d + 0] + map_size;
			ibo_data[d + 2] = ibo_data[d + 0] + 1;

			a = Vec(vertex_data[ibo_data[d + 0] * 3 + 0], vertex_data[ibo_data[d + 0] * 3 + 1], vertex_data[ibo_data[d + 0] * 3 + 2]);
			b = Vec(vertex_data[ibo_data[d + 1] * 3 + 0], vertex_data[ibo_data[d + 1] * 3 + 1], vertex_data[ibo_data[d + 1] * 3 + 2]);
			c = Vec(vertex_data[ibo_data[d + 2] * 3 + 0], vertex_data[ibo_data[d + 2] * 3 + 1], vertex_data[ibo_data[d + 2] * 3 + 2]);

			face_normals[x][y][0] = Cross(b-a, c-a);


			ibo_data[d + 3] = ibo_data[d + 0] + 1;
			ibo_data[d + 4] = ibo_data[d + 0] + map_size;
			ibo_data[d + 5] = ibo_data[d + 0] + map_size + 1;

			a = Vec(vertex_data[ibo_data[d + 3] * 3 + 0], vertex_data[ibo_data[d + 3] * 3 + 1], vertex_data[ibo_data[d + 3] * 3 + 2]);
			b = Vec(vertex_data[ibo_data[d + 4] * 3 + 0], vertex_data[ibo_data[d + 4] * 3 + 1], vertex_data[ibo_data[d + 4] * 3 + 2]);
			c = Vec(vertex_data[ibo_data[d + 5] * 3 + 0], vertex_data[ibo_data[d + 5] * 3 + 1], vertex_data[ibo_data[d + 5] * 3 + 2]);

			face_normals[x][y][1] = Cross(b-a, c-a);

			d += 6;
		}
	}

	for(x=0; x<map_size; x++)
	{
		for(y=0; y<map_size; y++)
		{
			n_count = 0;

			if(x < map_size - 1)
			{
				if(y < map_size -1)
				{
					n += face_normals[x][y][0] + face_normals[x][y][1];
					n_count += 2;
				}
				if(y > 0)
				{
					n += face_normals[x][y-1][0] + face_normals[x][y-1][1];
					n_count += 2;
				}
			}

			if(x > 0)
			{
				if(y < map_size -1)
				{
					n += face_normals[x-1][y][0] + face_normals[x-1][y][1];
					n_count += 2;
				}
				if(y > 0)
				{
					n += face_normals[x-1][y-1][0] + face_normals[x-1][y-1][1];
					n_count += 2;
				}
			}

			n *= 1.0 / (float)n_count;

			memcpy(normal_data + y * map_size * 3 + x * 3, n.v, sizeof(float) * 3);
		}
	}


	normal_vbo->AssignData();

	ibo->AssignData();
}

void CTerrain::Paint(void)
{
	vertex_vbo->SetAttribute(CFaceShader::vertex_attribute, GL_FLOAT);
	normal_vbo->SetAttribute(CFaceShader::normal_attribute, GL_FLOAT);
	tang_vbo->SetAttribute(CFaceShader::tang_attribute, GL_FLOAT);
	bitang_vbo->SetAttribute(CFaceShader::bitang_attribute, GL_FLOAT);
	uvcoord_vbo->SetAttribute(CFaceShader::uvcoord_attribute, GL_FLOAT);
	CEngine::GetCurrentFaceShader()->SetVertexMix(0.0);

	CEngine::GetCurrentFaceShader()->SetTwoSide(0);
	CEngine::GetCurrentFaceShader()->SetTransformation(CEngine::identity_matrix4);
	//CEngine::GetPointShadowShader()->SetTransformation(CEngine::identity_matrix);

	CEngine::GetCurrentFaceShader()->SetDiffuseColor2(Vec(1.0, 1.0, 1.0), 1.0);

	if(material)
		material->PutToGL();
	else
	{
		CEngine::GetCurrentFaceShader()->SetDiffuseTexture(true, height_map->GetHeightTex());
		CEngine::GetCurrentFaceShader()->SetSpecularTexture(false);
		CEngine::GetCurrentFaceShader()->SetNormalTexture(false);
		//CEngine::GetCurrentFaceShader()->SetHeightTexture(false);
		CEngine::GetCurrentFaceShader()->SetDiffuseColor(Vec(1.0, 1.0, 1.0));
		CEngine::GetCurrentFaceShader()->SetSpecularColor(Vec(0.0, 0.0, 0.0));
		CEngine::GetCurrentFaceShader()->SetSpecular(0.0);
		//CEngine::GetCurrentFaceShader()->SetHeightFactor(Vec(0.0, 0.0, 0.0));
		CEngine::GetCurrentFaceShader()->SetAmbient(0.3);
	}

	ibo->Draw(GL_TRIANGLES);
}







CTerrainMaterial::CTerrainMaterial()
{
	diffuse.enabled = false;
	diffuse.filename = string();
	diffuse.color.Set(1.0, 1.0, 1.0);
	diffuse.ambient = 1.0;
    diffuse_tex = 0;

	specular.enabled = false;
	specular.filename = string();
	specular.color.Set(1.0, 1.0, 1.0);
	specular.exponent = 64.0;
	specular_tex = 0;

	normal.enabled = false;
	normal.filename = string();
	normal_tex = 0;

	height.enabled = false;
	height.filename = string();
	height.factor = 0.0;
	height_tex = 0;
}

CTerrainMaterial::~CTerrainMaterial(void)
{
	if(diffuse_tex != 0)
		glDeleteTextures(1, &diffuse_tex);
	if(specular_tex != 0)
		glDeleteTextures(1, &specular_tex);
	if(normal_tex != 0)
		glDeleteTextures(1, &normal_tex);
	if(height_tex != 0)
		glDeleteTextures(1, &height_tex);
}

void CTerrainMaterial::SetDiffuse(bool enabled, string filename, CVector color, float ambient)
{
	diffuse.enabled = enabled;
	diffuse.filename = filename;
	diffuse.color = color;
	diffuse.ambient = ambient;
}

void CTerrainMaterial::SetSpecular(bool enabled, string filename, CVector color, float exponent)
{
	specular.enabled = enabled;
	specular.filename = filename;
	specular.color = color;
	specular.exponent = exponent;
}

void CTerrainMaterial::SetNormal(bool enabled, string filename)
{
	normal.enabled = enabled;
	normal.filename = filename;
}

void CTerrainMaterial::SetHeight(bool enabled, string filename, float factor)
{
	height.enabled = enabled;
	height.filename = filename;
	height.factor = factor;
}

void CTerrainMaterial::Load(string path)
{
	string file;
	struct stat s;

	if(diffuse.enabled)
	{
		file = path + diffuse.filename;
		if(stat(file.c_str(), &s) == 0)
			diffuse_tex = LoadGLTexture(file.c_str());
	}

	if(specular.enabled)
	{
		file = path + specular.filename;
		if(stat(file.c_str(), &s) == 0)
			specular_tex = LoadGLTexture(file.c_str());
	}

	if(normal.enabled)
	{
		file = path + normal.filename;
		if(stat(file.c_str(), &s) == 0)
			normal_tex = LoadGLTexture(file.c_str());
	}

	if(height.enabled)
	{
		file = path + height.filename;
		if(stat(file.c_str(), &s) == 0)
			height_tex = LoadGLTexture(file.c_str());
	}
}

void CTerrainMaterial::PutToGL(void)
{
	CEngine::GetCurrentFaceShader()->SetDiffuseTexture(diffuse.enabled, diffuse_tex);
	CEngine::GetCurrentFaceShader()->SetSpecularTexture(specular.enabled, specular_tex);
	CEngine::GetCurrentFaceShader()->SetNormalTexture(normal.enabled, normal_tex);
	//CEngine::GetCurrentFaceShader()->SetHeightTexture(height.enabled, height_tex);
	CEngine::GetCurrentFaceShader()->SetDiffuseColor(diffuse.color);
	CEngine::GetCurrentFaceShader()->SetSpecularColor(specular.color);
	CEngine::GetCurrentFaceShader()->SetSpecular(specular.exponent);
	//CEngine::GetCurrentFaceShader()->SetHeightFactor(Vec(1.0, 1.0, 1.0) * height.factor);
	CEngine::GetCurrentFaceShader()->SetAmbient(diffuse.ambient);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

