
#include "towerengine.h"
#include "resources.h"

void tScreenShader::InitScreenShader(const char *frag_src, const char *shader_name)
{
	InitShader(resources_get("screen_shader.vert"), frag_src, shader_name);
	glBindAttribLocation(program, vertex_attribute, "vertex_attr");
	glBindAttribLocation(program, uv_coord_attribute, "uv_coord_attr_attr");
	LinkProgram();
}


// -------------------------------

tPositionRestoreDataBuffer::tPositionRestoreDataBuffer(void)
	: tUniformBuffer(20*4)
{
}

tPositionRestoreDataBuffer::~tPositionRestoreDataBuffer(void)
{
}



void tPositionRestoreDataBuffer::UpdateBuffer(tCamera *camera, float uv_factor_x, float uv_offset_x)
{
	tMatrix4 modelview_projection_matrix_inv = camera->GetModelViewProjectionMatrix().GetInverse();
	modelview_projection_matrix_inv.GetDataTranspose((float *)data);

	float *projection_matrix = camera->GetProjectionMatrix().GetData();
	((float *)data)[16] = projection_matrix[11];
	((float *)data)[17] = projection_matrix[10] / projection_matrix[14];

	((float *)data)[18] = uv_factor_x;
	((float *)data)[19] = uv_offset_x;

	UploadData();
}

void tPositionRestoreDataBuffer::Bind(void)
{
	tUniformBuffer::Bind(tShader::position_restore_data_binding_point);
}
