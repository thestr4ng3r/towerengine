#include "towerengine.h"

tTerrain::tTerrain(tHeightMap *height_map, float size, float height, tTerrainMaterial *material)
{
	float *vertex_data, *normal_data, *tang_data, *bitang_data, *uvcoord_data;
	tVector ***face_normals;
	tVector a, b, c;
	tVector n;
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

	vao = new tVAO();
	vertex_vbo = new tVBO<float>(3, vao, map_size * map_size);
	normal_vbo = new tVBO<float>(3, vao, map_size * map_size);
	tang_vbo = new tVBO<float>(3, vao, map_size * map_size);
	bitang_vbo = new tVBO<float>(3, vao, map_size * map_size);
	uvcoord_vbo = new tVBO<float>(2, vao, map_size * map_size);

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
			uvcoord_data[d + 0] = ((float)x / (float)map_size) * 50.0;
			uvcoord_data[d + 1] = ((float)y / (float)map_size) * 50.0;
		}
	}

	vertex_vbo->AssignData();
	tang_vbo->AssignData();
	bitang_vbo->AssignData();
	uvcoord_vbo->AssignData();

	ibo = new tIBO(vao, (map_size - 1) * (map_size - 1) * 6);

	face_normals = new tVector **[map_size-1];

	ibo_data = ibo->GetData();
	d = 0;
	for(x=0; x<map_size-1; x++)
	{
		face_normals[x] = new tVector *[map_size-1];
		for(y=0; y<map_size-1; y++)
		{
			face_normals[x][y] = new tVector[2];

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


	physics_vertices = new float[map_size * map_size * 3];
	memcpy(physics_vertices, vertex_data, map_size * map_size * 3 * sizeof(float));
	physics_triangles = new int[(map_size - 1) * (map_size - 1) * 6];

	for(int i=0; i<(map_size - 1) * (map_size - 1) * 6; i++)
		physics_triangles[i] = (int)ibo_data[i];

	btTriangleIndexVertexArray *va = new btTriangleIndexVertexArray((map_size - 1) * (map_size - 1) * 2, physics_triangles, sizeof(int) * 3,
			map_size * map_size, physics_vertices, sizeof(float) * 3);

	collision_shape = new btBvhTriangleMeshShape(va, false);


}

void tTerrain::Paint(tRenderer *renderer)
{
	vertex_vbo->SetAttribute(tFaceShader::vertex_attribute, GL_FLOAT);
	normal_vbo->SetAttribute(tFaceShader::normal_attribute, GL_FLOAT);
	tang_vbo->SetAttribute(tFaceShader::tang_attribute, GL_FLOAT);
	bitang_vbo->SetAttribute(tFaceShader::bitang_attribute, GL_FLOAT);
	uvcoord_vbo->SetAttribute(tFaceShader::uvcoord_attribute, GL_FLOAT);
	renderer->GetCurrentFaceShader()->SetVertexMix(0.0);

	renderer->GetCurrentFaceShader()->SetTransformation(tEngine::identity_matrix4);

	renderer->GetCurrentFaceShader()->SetDiffuseColor2(Vec(1.0, 1.0, 1.0), 1.0);

	if(material)
		material->InitGeometryPass(renderer);
	else
	{
		renderer->GetCurrentFaceShader()->SetDiffuseTexture(true, height_map->GetHeightTex());
		renderer->GetCurrentFaceShader()->SetSpecularTexture(false);
		renderer->GetCurrentFaceShader()->SetNormalTexture(false);
		//CEngine::GetCurrentFaceShader()->SetHeightTexture(false);
		renderer->GetCurrentFaceShader()->SetDiffuseColor(Vec(1.0, 1.0, 1.0));
		renderer->GetCurrentFaceShader()->SetSpecularColor(Vec(0.0, 0.0, 0.0));
		renderer->GetCurrentFaceShader()->SetSpecular(0.0);
		//CEngine::GetCurrentFaceShader()->SetHeightFactor(Vec(0.0, 0.0, 0.0));
	}

	ibo->Draw(GL_TRIANGLES);
}

tBoundingBox tTerrain::GetBoundingBox(void)
{
	return tBoundingBox(	Vec(-size / 2.0, height, -size / 2.0),
							Vec(size / 2.0, 0.0, size / 2.0));
}




