
#ifndef _MESHOBJECT_H
#define _MESHOBJECT_H

class tCubeMapReflection;

class tMeshObject : public tTransformObject
{
	private:
		tMesh *mesh;

		tCubeMapReflection *cube_map_reflection;

		bool visible;

		std::string pose;

		tVector color;
		float alpha;

		float transform_matrix[16];

		btCollisionShape *collision_shape;
		btRigidBody *rigid_body;
		btMotionState *motion_state;

		std::map<tMaterial *, tMaterial *> *replace_materials;

		void CreateRigidBody(btScalar &mass, btVector3 &inertia);

	protected:
		void TransformChanged(void);

		void AddedToWorld(tWorld *world);
		void RemovedFromWorld(tWorld *world);

	public:
		tMeshObject(tMesh *mesh);
		~tMeshObject(void);

		void SetMesh(tMesh *mesh)						{ this->mesh = mesh; }

		void SetColor(tVector c)						{ color = c; }
		void SetAlpha(float a)							{ alpha = a; }
		void SetColor(tVector c, float a)				{ SetColor(c); SetAlpha(a); }
		void SetVisible(bool visible)					{ this->visible = visible; }
		void SetCubeMapReflection(tCubeMapReflection *reflection)	{ this->cube_map_reflection = reflection; }

		void ReplaceMaterial(std::string name, tMaterial *material);
		void RestoreReplaceMaterial(std::string name);

		void InitMeshRigidBody(float mass, bool convex, bool hull = true);
		void InitBoxRigidBody(tVector half_extents, float mass);
		void UpdateRigidBodyTransformation(void);

		void DepthPrePass(tRenderer *renderer);
		void GeometryPass(tRenderer *renderer, bool cube_map_reflection_enabled);
		void ForwardPass(tRenderer *renderer);
		void RefractionPass(tRenderer *renderer);
		tBoundingBox GetBoundingBox(void);

		tCubeMapReflection *GetCubeMapReflection(void)	{ return cube_map_reflection; }
		bool GetCubeMapReflectionEnabled(void)			{ return mesh->GetCubeMapReflectionEnabled(); }

		btRigidBody *GetRigidBody(void)		{ return rigid_body; }
		tMesh *GetMesh(void)				{ return mesh; }

		void SetTransformWithoutRigidBody(tTransform transform);
};

class tMeshObjectMotionState : public btMotionState
{
	private:
		tMeshObject *object;

	public:
		tMeshObjectMotionState(tMeshObject *object);

		virtual void getWorldTransform(btTransform &trans) const;
		virtual void setWorldTransform(const btTransform &trans);
};

#endif
