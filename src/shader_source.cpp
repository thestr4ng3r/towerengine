
#include "shader_source.h"

#include <stdio.h>
#include <stdlib.h>

#include "utils.h"

using namespace std;

tShaderSourceVariable::tShaderSourceVariable(int v)
{
	int_v = v;
	type = INT;
}

tShaderSourceVariable::tShaderSourceVariable(float v)
{
	float_v = v;
	type = FLOAT;
}

tShaderSourceVariable::tShaderSourceVariable(string v)
{
	string_v = v;
	type = STRING;
}

string tShaderSourceVariable::GetString(void)
{
	ostringstream ss;

	switch(type)
	{
		case INT:
			ss << int_v;
			break;
		case FLOAT:
			ss << float_v;
			break;
		case STRING:
			ss << string_v;
			break;
	}

	return ss.str();
}



tShaderSource::tShaderSource(string source)
{
	this->source = source;
}

tShaderSource::~tShaderSource(void)
{
	map<string, tShaderSourceVariable *>::iterator vi;

	for(vi = params.begin(); vi != params.end(); vi++)
		delete vi->second;
}

void tShaderSource::SetParameter(string name, tShaderSourceVariable *v)
{
	map<string, tShaderSourceVariable *>::iterator vi = params.find(name);

	if(vi != params.end())
		delete vi->second;

	params.insert(pair<string, tShaderSourceVariable *>(name, v));
}


vector<string> ParseSourceCommand(const string &source, size_t pos, size_t &end)
{
	vector<string> r;

	if(source.at(pos) != '$')
		return r;

	if(source.length() < pos + 3)
		return r;

	if(source.at(pos+1) != '(')
		return r;

	end = source.find(')', pos+2);
	if(end == string::npos)
		return r;

    stringstream ss(source.substr(pos+2, end-(pos+2)));
    string item;
    while(getline(ss, item, ' '))
    {
    	if(!item.empty())
    		r.push_back(item);
    }

    return r;
}

vector<string> ParseParameterList(const string &source, unsigned long pos, unsigned long *end_pos)
{
	vector<string> r;

	if(source.at(pos) != '(')
		return r;

	unsigned long length = source.length();
	*end_pos = pos;

	string param = "";

	while(true)
	{
		(*end_pos)++;

		if(*end_pos >= length)
			return vector<string>();

		char c = source.at(*end_pos);

		if(c == '\n')
			return vector<string>();

		if(c == ')')
		{
			(*end_pos)++;
			if(!param.empty())
				r.push_back(param);
			break;
		}
		else if(c == ' ')
		{
			if(!param.empty())
			{
				r.push_back(param);
				param = "";
			}
		}
		else
		{
			param.push_back(c);
		}
	}

	return r;
}



class tShaderSourceBlock
{
	protected:
		size_t begin;
		size_t end;

	public:
		tShaderSourceBlock(size_t begin, size_t end)	{ this->begin = begin; this->end = end; }
		virtual ~tShaderSourceBlock(void)	{}

		virtual void Resolve(string &source, int &offset) =0;
};

class tShaderSourceForLoop : public tShaderSourceBlock
{
	private:
		string var_name;
		int from, to;

	public:
		tShaderSourceForLoop(size_t begin, size_t end, string var_name, int from, int to) : tShaderSourceBlock(begin, end)
		{
			this->var_name = var_name;
			this->from = from; this->to = to;
		}

		void Resolve(string &source, int &offset)
		{
			string inside = source.substr(begin, end-begin+1);
			source.erase(begin, end-begin+1);

			int d = to < from ? -1 : 1;
			int loop_offset = 0;
			for(int i=from; i<=to; i+=d)
			{
				string var_set = var_name + " = " + itos(i) + ";\n";
				source.insert(begin + loop_offset, var_set);
				loop_offset += var_set.length();

				source.insert(begin + loop_offset, inside);
				loop_offset += inside.length();
			}

			offset += loop_offset - (end - begin);
		}
};

struct tSourceBlockCommand
{
	size_t cmd_pos;
	size_t cmd_end;
	bool open;
	vector<string> cmd;
};

#define NEXT_PARAM(i,v) (i)++; if((i) == (v).end()) { printf("more parameters expected\n"); continue; }

string tShaderSource::BuildSource(void)
{
	string final = source;

	vector<string> cmd_params;
	unsigned long cmd_end;

	unsigned long vi = 0;
	while((vi = final.find("#pragma define_param(", vi)) != string::npos)
	{
		cmd_params = ParseParameterList(final, vi+20, &cmd_end);

		if(cmd_params.size() == 2)
		{
			map<string, tShaderSourceVariable *>::iterator pi = params.find(cmd_params.at(1));

			if(pi == params.end())
			{
				printf("unknown parameter %s\n", cmd_params.at(1).c_str());
				vi++;
				continue;
			}

			final.replace(vi, cmd_end-vi, "#define " + cmd_params.at(0) + " " + pi->second->GetString());
		}
		else
		{
			printf("wrong parameter count for define_param.\n");
		}

		vi++;
	}

	vector<tSourceBlockCommand> block_cmds;

	vi = 0;
	while((vi = final.find("#pragma ", vi)) != string::npos)
	{
		if(final.compare(vi+8, 4, "for(") == 0)
		{
			tSourceBlockCommand bc;
			bc.cmd = ParseParameterList(final, vi+8+3, &cmd_end);
			bc.cmd_pos = vi;
			bc.cmd_end = cmd_end;
			bc.open = true;
			block_cmds.push_back(bc);
		}
		else if(final.compare(vi+8, 6, "endfor") == 0)
		{
			cmd_end = vi + 8 + 6;
			tSourceBlockCommand bc;
			bc.cmd = vector<string>();
			bc.cmd_pos = vi;
			bc.cmd_end = cmd_end;
			bc.open = false;
			block_cmds.push_back(bc);
		}

		vi++;
	}

	if((block_cmds.size() / 2) * 2 != block_cmds.size())
	{
		printf("number of block commands is not a multiple of 2\n");
		return final;
	}

	bool open = true;
	for(vector<tSourceBlockCommand>::iterator bi=block_cmds.begin(); bi!=block_cmds.end(); bi++)
	{
		if((*bi).open != open)
		{
			printf("block command order is wrong (nested blocks not supported yet)\n");
			return final;
		}
		open = !open;
	}

	int after_offset;

	for(vector<tSourceBlockCommand>::iterator bi=block_cmds.begin(); bi!=block_cmds.end(); bi++)
	{
		tSourceBlockCommand &start_bc = *bi;
		bi++;
		tSourceBlockCommand &end_bc = *bi;

		string var_name;
		int from, to;

		vector<string>::iterator ci=start_bc.cmd.begin();

		//NEXT_PARAM(ci, start_bc.cmd)
		var_name = *ci;

		NEXT_PARAM(ci, start_bc.cmd)
		if((*ci).compare("from") != 0)
		{
			printf("expected from in for command, got %s\n", (*ci).c_str());
			continue;
		}

		NEXT_PARAM(ci, start_bc.cmd)
		if((*ci).compare("param") == 0)
		{
			NEXT_PARAM(ci, start_bc.cmd)
			map<string, tShaderSourceVariable *>::iterator pi = params.find(*ci);
			if(pi == params.end())
			{
				printf("param %s not found\n", (*ci).c_str());
				continue;
			}
			if(pi->second->type != tShaderSourceVariable::INT)
			{
				printf("can only loop over int param\n");
				continue;
			}
			from = pi->second->int_v;
		}
		else
			from = atoi((*ci).c_str());


		NEXT_PARAM(ci, start_bc.cmd)

		bool exclude;
		if((*ci).compare("to") == 0)
			exclude = false;
		else if((*ci).compare("ex") == 0)
			exclude = true;
		else
		{
			printf("expected to or ex in for command\n");
			continue;
		}

		NEXT_PARAM(ci, start_bc.cmd)
		if((*ci).compare("param") == 0)
		{
			NEXT_PARAM(ci, start_bc.cmd)
			map<string, tShaderSourceVariable *>::iterator pi = params.find(*ci);
			if(pi == params.end())
			{
				printf("param %s not found\n", (*ci).c_str());
				continue;
			}
			if(pi->second->type != tShaderSourceVariable::INT)
			{
				printf("can only loop over int param\n");
				continue;
			}
			to = pi->second->int_v;
			if(exclude)
				to--;
		}
		else
			to = atoi((*ci).c_str());

		after_offset = 0;
		after_offset -= (int)(start_bc.cmd_end - start_bc.cmd_pos) + 1;
		after_offset -= (int)(end_bc.cmd_end - end_bc.cmd_pos) + 1;

		final.erase(end_bc.cmd_pos, end_bc.cmd_end - end_bc.cmd_pos + 1);
		final.erase(start_bc.cmd_pos, start_bc.cmd_end - start_bc.cmd_pos + 1);

		tShaderSourceForLoop *loop = new tShaderSourceForLoop(start_bc.cmd_pos, end_bc.cmd_pos - (start_bc.cmd_end - start_bc.cmd_pos + 1), var_name, from, to);
		loop->Resolve(final, after_offset);
		delete loop;

		for(vector<tSourceBlockCommand>::iterator obi=bi+1; obi!=block_cmds.end(); obi++)
		{
			(*obi).cmd_pos += after_offset;
			(*obi).cmd_end += after_offset;
		}
	}

	return final;
}

