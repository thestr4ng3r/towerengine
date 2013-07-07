
#ifndef _LIGHT_H
#define _LIGHT_H


class CPointLight
{
	private:
		CVector pos;
		CVector color;
		float distance;

	public:
		CPointLight(CVector pos, CVector color, float distance);

		void SetPos(CVector pos)		{ this->pos = pos; }
		void SetColor(CVector color)	{ this->color = color; }
		void SetDistance(float dist)	{ this->distance = dist; }

		CVector GetPos(void)	{ return pos; }
		CVector GetColor(void)	{ return color; }
		float GetDistance(void)	{ return distance; }
};

class CDirectionalLight
{
	private:
		CVector dir;
		CVector color;

	public:
		CDirectionalLight(CVector dir, CVector color);

		void SetDirection(CVector dir)	{ this->dir = dir; this->dir.Normalize(); }
		void SetColor(CVector color)	{ this->color = color; }

		CVector GetDirection(void)	{ return dir; }
		CVector GetColor(void)		{ return color; }
};


#endif
