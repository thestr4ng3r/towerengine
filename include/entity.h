
#ifndef _ENTITY_H
#define _ENTITY_H

class tEntityAttribute;

class tEntity
{
	private:
		std::string name;
		std::string group;

		std::map<std::string, tEntityAttribute> attributes;

	public:
		tEntity(std::string name, std::string group = "");
		~tEntity();

		std::string GetName()						{ return name; }
		std::string GetGroup()						{ return group; }

		void SetAttribute(std::string name, tEntityAttribute attr);
		tEntityAttribute *GetAttribute(std::string name);
		void RemoveAttribute(std::string name)		{ attributes.erase(name); }
};


struct tEntityAttribute
{
	enum Type { VECTOR, VECTOR2, FLOAT, INT, STRING } type;

	tVector vec_v;
	tVector2 vec2_v;
	float float_v;
	int int_v;
	std::string string_v;

	~tEntityAttribute() {}
};

#endif
