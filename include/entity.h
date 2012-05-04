
#ifndef _ENTITY_H
#define _ENTITY_H

struct CEntityAttribute;

struct CEntity
{
public:
	string name;
	string group;

	map<string, CEntityAttribute *> attributes;
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
