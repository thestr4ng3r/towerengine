
#ifndef _SHADER_SOURCE_H
#define _SHADER_SOURCE_H

#include <string>
#include <map>
#include <sstream>
#include <vector>

struct tShaderSourceVariable
{
	union
	{
		int int_v;
		float float_v;
	};
	std::string string_v;

	enum Type { INT, FLOAT, STRING } type;

	tShaderSourceVariable(int v);
	tShaderSourceVariable(float v);
	tShaderSourceVariable(std::string v);

	std::string GetString(void);
};

class tShaderSource
{
	private:
		std::string source;

		std::map<std::string, tShaderSourceVariable *> params;

	public:
		tShaderSource(std::string source);
		~tShaderSource(void);

		void SetParameter(std::string name, tShaderSourceVariable *v);

		std::string BuildSource(void);
};



#endif
