
#include "towerengine.h"
#include "tresources.h"


using namespace std;


void tLightingShader::Init(tGBuffer *gbuffer)
{
	InitScreenShader(lighting_shader_frag, "Lighting Shader");

	position_tex_uniform = GetUniformLocation("position_tex_uni");
	diffuse_tex_uniform = GetUniformLocation("diffuse_tex_uni");
	normal_tex_uniform = GetUniformLocation("normal_tex_uni");
	specular_tex_uniform = GetUniformLocation("specular_tex_uni");
	self_illumination_tex_uniform = GetUniformLocation("self_illumination_tex_uni");

	ssao_enabled_uniform = GetUniformLocation("ssao_enabled_uni");
	ssao_tex_uniform = GetUniformLocation("ssao_tex_uni");

	light_ambient_color_uniform = GetUniformLocation("light_ambient_color_uni");

	cam_pos_uniform = GetUniformLocation("cam_pos_uni");

	diffuse_tex_uniform = GetUniformLocation("diffuse_tex_uni");


	Bind();
	glUniform1i(position_tex_uniform, gbuffer->GetTextureUnit(tGBuffer::POSITION_TEX));
	glUniform1i(diffuse_tex_uniform, gbuffer->GetTextureUnit(tGBuffer::DIFFUSE_TEX));
	glUniform1i(normal_tex_uniform, gbuffer->GetTextureUnit(tGBuffer::NORMAL_TEX));
	glUniform1i(specular_tex_uniform, gbuffer->GetTextureUnit(tGBuffer::SPECULAR_TEX));
	glUniform1i(self_illumination_tex_uniform, gbuffer->GetTextureUnit(tGBuffer::SELF_ILLUMINATION_TEX));
}

void tLightingShader::SetAmbientLight(tVector color)
{
	glUniform3f(light_ambient_color_uniform, color.x, color.y, color.z);
}


void tLightingShader::SetCameraPosition(tVector pos)
{
	glUniform3f(cam_pos_uniform, pos.x, pos.y, pos.z);
}


void tLightingShader::SetSSAO(bool enabled, GLuint64 tex_handle)
{
	glUniform1i(ssao_enabled_uniform, enabled ? 1 : 0);
	if(enabled)
		glUniformHandleui64ARB(ssao_tex_uniform, tex_handle);
}



// -------------------------------

tLightingShaderPointLightsBuffer::tLightingShaderPointLightsBuffer(void)
{
	size = 16+4*16*tLightingShader::max_point_lights_count;
	data = new unsigned char[size];

	glGenBuffers(1, &buffer);
}

tLightingShaderPointLightsBuffer::~tLightingShaderPointLightsBuffer(void)
{
	glDeleteBuffers(1, &buffer);
	delete [] data;
}



void tLightingShaderPointLightsBuffer::UpdateBuffer(std::vector<tPointLight *> lights)
{
	int lights_count = lights.size();
	if(lights_count > tLightingShader::max_point_lights_count)
		lights_count = tLightingShader::max_point_lights_count;

	((int *)(&data[0*16]))[0] = lights_count;

	vector<tPointLight *>::iterator point_light_it;
	int point_light_i = 0;

	for(point_light_it=lights.begin(); point_light_it!=lights.end() && point_light_i<lights_count; point_light_it++, point_light_i++)
	{
		tPointLight *light = *point_light_it;

		unsigned int buffer_pos = 16 + point_light_i*4*16;

		((float *)(&data[buffer_pos]))[0] = light->GetDistance();
		((int *)(&data[buffer_pos]))[1] = light->GetShadowEnabled() ? 1 : 0;
		((float *)(&data[buffer_pos+16]))[0] = light->GetPosition().x;
		((float *)(&data[buffer_pos+16]))[1] = light->GetPosition().y;
		((float *)(&data[buffer_pos+16]))[2] = light->GetPosition().z;
		((float *)(&data[buffer_pos+2*16]))[0] = light->GetColor().x;
		((float *)(&data[buffer_pos+2*16]))[1] = light->GetColor().y;
		((float *)(&data[buffer_pos+2*16]))[2] = light->GetColor().z;

		GLuint64 shadow_handle = 0;
		if(light->GetShadow())
		{
			light->GetShadow()->MakeTextureHandleResident(true);
			shadow_handle = light->GetShadow()->GetTextureHandle();
		}
		((GLuint64 *)(&data[buffer_pos+3*16]))[0] = shadow_handle;
	}

	glBindBuffer(GL_UNIFORM_BUFFER, buffer);
	glBufferData(GL_UNIFORM_BUFFER, size, data, GL_DYNAMIC_DRAW);
}

void tLightingShaderPointLightsBuffer::Bind(void)
{
	glBindBufferBase(GL_UNIFORM_BUFFER, tLightingShader::point_light_binding_point, buffer);
}








