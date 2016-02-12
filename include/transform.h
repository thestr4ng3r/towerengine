
#ifndef _TRANSFORM_H
#define _TRANSFORM_H


class tTransform
{
	private:
		tMatrix3 basis;
		tVector position;

	public:
		tTransform(tMatrix3 basis, tVector position);
		tTransform(void);

		tMatrix3 GetBasis(void)				{ return basis; }
		tVector GetPosition(void)			{ return position; }

		float *GetMatrix(float *matrix);

		void Set(tMatrix3 basis, tVector position);

		void SetBasis(tMatrix3 basis)		{ this->basis = basis; }
		void SetPosition(tVector position)	{ this->position = position; }

		void TranslateLocal(tVector v)		{ position += v * basis; }
		void TranslateWorld(tVector v)		{ position += v; }

		tVector ApplyToVector(tVector v);

		void SetIdentity(void);

		btTransform ToBtTransform(void)
		{
			return btTransform(basis.ToBtMatrix(), BtVec(position));
		}

		btTransform &ToBtTransform(btTransform &bt_trans)
		{
			bt_trans.getOrigin().setValue(position.x, position.y, position.z);
			basis.ToBtMatrix(bt_trans.getBasis());
			return bt_trans;
		}

		void SetFromBtTransform(const btTransform &bt_trans)
		{
			basis = tMatrix3::FromBtMatrix(bt_trans.getBasis());
			position = Vec(bt_trans.getOrigin());
		}

		static tTransform GetIdentity(void)	{ return tTransform(); }
		static tTransform FromBtTransform(const btTransform &trans);
};

inline tVector operator*(tVector v, tTransform t)		{ return t.ApplyToVector(v); }
inline tVector operator*(tTransform t, tVector v)		{ return t.ApplyToVector(v); }


#endif
