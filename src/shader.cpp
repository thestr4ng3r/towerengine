
#include "towerengine.h"


#include "resources.h"

using namespace std;


const int tShader::vertex_attribute;

const int tShader::matrix_binding_point;
const int tShader::material_binding_point;
const int tShader::position_restore_data_binding_point;
const int tShader::point_light_binding_point;


void PrintGLInfoLog(const char *log_title, GLuint handle, const char *shader_name = 0);


void tShader::InitSource(void)
{
	const char **sources;
	const char **names;
	unsigned int source_count = resources_get_all(&names, &sources);

	for(unsigned int i=0; i<source_count; i++)
	{
		string name = string("/") + names[i];
		glNamedStringARB(GL_SHADER_INCLUDE_ARB, -1, name.c_str(), -1, sources[i]);
	}
}



GLuint tShader::CreateShaderFromSource(GLenum type, const char *src)
{
	GLuint shader = glCreateShader(type);

	GLint len = (GLint)strlen(src);
	glShaderSource(shader, 1, &src, &len);

	return shader;
}


GLuint tShader::CreateShaderFromNamedString(GLenum type, const char *name)
{
	int len;
	glGetNamedStringivARB(-1, name, GL_NAMED_STRING_LENGTH_ARB, &len);

	if(len <= 0)
		return 0;

	GLint stringlen;

	GLchar *source = new char[len];
	glGetNamedStringARB(-1, name, len, &stringlen, source);

	GLuint shader = CreateShaderFromSource(type, source);

	delete [] source;

	return shader;
}

void tShader::CompileAndAttachShader(GLuint shader)
{
	//glCompileShader(shader);

	const GLchar *search_path = "/";
	const GLint search_path_len[] = { -1 };
	glCompileShaderIncludeARB(shader, 1, &search_path, search_path_len);

	GLint compile_status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compile_status);
	if(compile_status != GL_TRUE)
		printf("failed to compile shader of %s\n", name);

	PrintGLInfoLog("Compile", shader, name);

	glAttachShader(program, shader);
}

void tShader::LinkProgram(void)
{
	status = false;
	glLinkProgram(program);

	GLint param;
	glGetProgramiv(program, GL_LINK_STATUS, &param);
	if(param != GL_TRUE)
		printf("failed to link %s\n", name);

	status = param == GL_TRUE;
}

void tShader::InitShader(const char *vert_src, const char *frag_src, const char *shader_name)
{
	status = false;
	name = shader_name;
	program = glCreateProgram();
#ifdef TOWERENGINE_SHADER_LABELS
	if(name)
		glObjectLabel(GL_PROGRAM, program, strlen(name), name);
#endif
	CompileAndAttachShader(CreateShaderFromSource(GL_VERTEX_SHADER, vert_src));
	CompileAndAttachShader(CreateShaderFromSource(GL_FRAGMENT_SHADER, frag_src));
}

GLint tShader::GetUniformLocation(const char *uniform)
{
	return glGetUniformLocation(program, uniform);
}

void tShader::Bind(void)
{
	glUseProgram(program);
}

void tShader::Unbind(void)
{
	glUseProgram(0);
}


// ------------------------



tUniformBuffer::tUniformBuffer(unsigned int size)
{
	this->size = size;
	data = new unsigned char[this->size];

	glGenBuffers(1, &buffer);
}

tUniformBuffer::~tUniformBuffer()
{
	glDeleteBuffers(1, &buffer);
	delete [] data;
}

void tUniformBuffer::UploadData(GLenum usage)
{
	glBindBuffer(GL_UNIFORM_BUFFER, buffer);
	glBufferData(GL_UNIFORM_BUFFER, size, data, usage);
}

void tUniformBuffer::Bind(GLuint binding_point)
{
	glBindBufferBase(GL_UNIFORM_BUFFER, binding_point, buffer);
}



// ------------------------



tMatrixBuffer::tMatrixBuffer(void)
		: tUniformBuffer(4*16)
{
}

tMatrixBuffer::~tMatrixBuffer(void)
{
}



void tMatrixBuffer::UpdateBuffer(const tMatrix4 &matrix)
{
	matrix.GetDataTranspose((float *)data);
	UploadData();
}

void tMatrixBuffer::Bind(void)
{
	tUniformBuffer::Bind(tShader::matrix_binding_point);
}




// ------------------------

void PrintGLInfoLog(const char *log_title, GLuint handle, const char *shader_name)
{
	GLchar *string;
	GLint size;

	size = 0;
	glGetShaderiv(handle, GL_INFO_LOG_LENGTH, &size);
	if(size > 1)
	{
		string = new GLchar [size];
		glGetShaderInfoLog(handle, size, &size, string);
		printf("%s info log", log_title);
		if(shader_name != 0)
			 printf(" for %s:\n", shader_name);
		printf(":\n");
		printf("-----------\n%s\n-----------\n", string);
		delete [] string;
	}
}



