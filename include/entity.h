
#ifndef _ENTITY_H
#define _ENTITY_H

// TODO: CEntity and CEntityAttribute as class with private attributes instead of struct

struct CEntityAttribute;

struct CEntity
{
	string name;
	string group;

	map<string, CEntityAttribute *> attributes;

	void RemoveAttribute(string name) { attributes.erase(name); }
};


struct CEntityAttribute
{
	enum Type { VECTOR, VECTOR2, FLOAT, INT, STRING } type;

	union
	{
		CVector vec_v;
		CVector2 vec2_v;
		float float_v;
		int int_v;
	};
	string string_v;
};

#endif
