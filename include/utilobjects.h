

#ifndef _UTILOBJECTS_H
#define _UTILOBJECTS_H



class tLineObject : public tObject
{
	private:
		tVector positions[2];
		tVector colors[2];

		float line_width;

		tVAO *vao;
		tVBO<float> *vertex_vbo;
		tVBO<float> *color_vbo;

		void UpdateVertexVBO();
		void UpdateColorVBO();

	public:
		tLineObject(tVector position_a, tVector position_b);
		virtual ~tLineObject();

		void SetPositions(tVector position_a, tVector position_b);
		void SetColor(tVector color);
		void SetColors(tVector color_a, tVector color_b);
		void SetLineWidth(float width)								{ this->line_width = width; }

		void ForwardPass(tRenderer *renderer);

		tAABB GetAABB(void);
};


class tCoordinateSystemObject : public tObject
{
	private:
		tVector pos;
		tVector size;
		bool depth_test;

		tVAO *vao;
		tVBO<float> *vertex_vbo;
		tVBO<float> *color_vbo;

	public:
		tCoordinateSystemObject(bool depth_test = false);
		~tCoordinateSystemObject(void);

		void SetPosition(tVector pos)	{ this->pos = pos; }
		void SetSize(tVector size)		{ this->size = size; }

		void ForwardPass(tRenderer *renderer);

		tAABB GetAABB(void);
};


#endif
