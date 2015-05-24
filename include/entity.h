
#ifndef _ENTITY_H
#define _ENTITY_H

// TODO: CEntity and CEntityAttribute as class with private attributes instead of struct

struct tEntityAttribute;

struct tEntity
{
	std::string name;
	std::string group;

	std::map<std::string, tEntityAttribute *> attributes;

	void RemoveAttribute(std::string name) { attributes.erase(name); }
};


struct tEntityAttribute
{
	enum Type { VECTOR, VECTOR2, FLOAT, INT, STRING } type;

	union
	{
		tVector vec_v;
		tVector2 vec2_v;
		float float_v;
		int int_v;
	};
	std::string string_v;
};

#endif
