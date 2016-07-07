
#include "towerengine.h"
#include "resources.h"
#include "shader_source.h"

using namespace std;

tPointLightingShader::tPointLightingShader(int lights_count)
{
	this->lights_count = lights_count;
}

tPointLightingShader::~tPointLightingShader(void)
{
	delete [] point_light_shadow_tex_unit;
}

void tPointLightingShader::Init(tGBuffer *gbuffer)
{
	tShaderSource *src = new tShaderSource(string(get_resource("point_lighting_shader.frag")));
	src->SetParameter("LIGHTS_COUNT", new tShaderSourceVariable(lights_count));
	InitScreenShader(src->BuildSource().c_str(), "Point Lighting Shader");
	delete src;

	depth_tex_uniform = GetUniformLocation("depth_tex_uni");
	diffuse_tex_uniform = GetUniformLocation("diffuse_tex_uni");
	normal_tex_uniform = GetUniformLocation("normal_tex_uni");
	specular_tex_uniform = GetUniformLocation("specular_tex_uni");

	cam_pos_uniform = GetUniformLocation("cam_pos_uni");

	point_light_pos_uniform = GetUniformLocation("point_light_pos_uni");
	point_light_color_uniform = GetUniformLocation("point_light_color_uni");
	point_light_distance_uniform = GetUniformLocation("point_light_distance_uni");
	point_light_shadow_map_uniform = GetUniformLocation("point_light_shadow_map_uni");
	point_light_shadow_enabled_uniform = GetUniformLocation("point_light_shadow_enabled_uni");


	GLuint position_restore_data_block_index = glGetUniformBlockIndex(program, "PositionRestoreDataBlock");
	glUniformBlockBinding(program, position_restore_data_block_index, position_restore_data_binding_point);

	Bind();
	glUniform1i(depth_tex_uniform, gbuffer->GetTextureUnit(tGBuffer::DEPTH_TEX));
	glUniform1i(diffuse_tex_uniform, gbuffer->GetTextureUnit(tGBuffer::DIFFUSE_TEX));
	glUniform1i(normal_tex_uniform, gbuffer->GetTextureUnit(tGBuffer::NORMAL_TEX));
	glUniform1i(specular_tex_uniform, gbuffer->GetTextureUnit(tGBuffer::SPECULAR_TEX));

	point_light_shadow_tex_unit = new int[lights_count];
	for(int i=0; i<lights_count; i++)
		point_light_shadow_tex_unit[i] = gbuffer->GetLastTextureUnit() + 1 + i;
	glUniform1iv(point_light_shadow_map_uniform, lights_count, point_light_shadow_tex_unit);
}

void tPointLightingShader::SetCameraPosition(tVector pos)
{
	glUniform3f(cam_pos_uniform, pos.x, pos.y, pos.z);
}

void tPointLightingShader::SetPointLights(float *pos, float *color, float *dist, int *shadow_enabled, GLuint *shadow_map)
{
	glUniform3fv(point_light_pos_uniform, lights_count, pos);
	glUniform3fv(point_light_color_uniform, lights_count, color);
	glUniform1fv(point_light_distance_uniform, lights_count, dist);

	glUniform1iv(point_light_shadow_enabled_uniform, lights_count, shadow_enabled);

	for(int i=0; i<lights_count; i++)
	{
		glActiveTexture(GL_TEXTURE0 + point_light_shadow_tex_unit[i]);
		glBindTexture(GL_TEXTURE_CUBE_MAP, shadow_map[i]);
	}

}
