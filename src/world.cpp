
#include "towerengine.h"

CWorld::CWorld(void)
{
	light_pos = Vec(0.0, 0.0, 0.0);
	light_color = Vec(1.0, 1.0, 1.0);
	ambient_color = Vec(0.1, 0.1, 0.1);
	world_object = 0;
	shadow.enabled = false;
}

CWorld::~CWorld(void)
{
	if(world_object)
		delete world_object;
}

void CWorld::AddObject(CObject *o)
{
	vector<CObject *>::iterator i;

	for(i=objects.begin(); i!=objects.end(); i++)
		if(*i == o)
			return;

	objects.push_back(o);
}

void CWorld::RemoveObject(CObject *o)
{
	vector<CObject *>::iterator i;

	if(o == world_object)
	{
		world_object = 0;
		light_pos = Vec(0.0, 0.0, 0.0);
		light_color = Vec(1.0, 1.0, 1.0);
		ambient_color = Vec(0.1, 0.1, 0.1);
	}

	for(i=objects.begin(); i!=objects.end(); i++)
		if(*i == o)
		{
			objects.erase(i);
			return;
		}
}

void CWorld::SetWorldMesh(CMesh *m, bool load_light)
{
	map<string, CEntity *> light_entities;
	map<string, CEntity *>::iterator ei;
	map<string, CEntityAttribute *>::iterator ai;
	CEntity *light_entity;

	RemoveWorldMesh();
	AddObject(world_object = new CMeshObject(m));

	if(load_light)
	{
		light_pos = Vec(0.0, 0.0, 0.0);
		light_color = Vec(1.0, 1.0, 1.0);
		ambient_color = Vec(0.1, 0.1, 0.1);

		light_entities = m->GetEntitiesInGroup("lights");
		if((ei = light_entities.find(string("light"))) != light_entities.end())
		{
			light_entity = ei->second;

			if((ai = light_entity->attributes.find(string("pos"))) != light_entity->attributes.end())
				if(ai->second->type == CEntityAttribute::VECTOR)
					light_pos = ai->second->vec_v;

			if((ai = light_entity->attributes.find(string("color"))) != light_entity->attributes.end())
				if(ai->second->type == CEntityAttribute::VECTOR)
					light_color = ai->second->vec_v;

			if((ai = light_entity->attributes.find(string("ambient"))) != light_entity->attributes.end())
				if(ai->second->type == CEntityAttribute::VECTOR)
					ambient_color = ai->second->vec_v;
		}
	}
}

void CWorld::RemoveWorldMesh(void)
{
	if(!world_object)
		return;

	CMeshObject *m = world_object;
	RemoveObject(world_object);

	delete m;
}

void CWorld::Clear(void)
{
	RemoveWorldMesh();
	objects.clear();
}

void CWorld::InitShadow(int width, int height)
{
	shadow.width = width;
	shadow.height = height;

	glGenTextures(1, &shadow.tex);
	glBindTexture(GL_TEXTURE_2D, shadow.tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
	glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_INTENSITY);
	glTexImage2D( GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, shadow.width, shadow.height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	glGenFramebuffersEXT(1, &shadow.fbo);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, shadow.fbo);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT,GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_2D, shadow.tex, 0);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

	shadow.enabled = true;
}

void CWorld::RenderShadow(CVector cam_pos)
{
	shader_enabled = 0;
	UseNoShader();
	glEnable(GL_DEPTH_TEST);

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, shadow.fbo);
	glViewport(0, 0, shadow.width, shadow.height);
	glClear(GL_DEPTH_BUFFER_BIT);
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, (double)shadow.width / (double)shadow.height, 1.0, 100.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(light_pos.x, light_pos.y, light_pos.z, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
	SetShadowTextureMatrix();
	Render(cam_pos);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

	shader_enabled = 1;

	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
}

void CWorld::SetShadowTextureMatrix(void)
{
	static double modelView[16];
	static double projection[16];

	// This is matrix transform every coordinate x,y,z
	// x = x* 0.5 + 0.5
	// y = y* 0.5 + 0.5
	// z = z* 0.5 + 0.5
	// Moving from unit cube [-1,1] to [0,1]
	const GLdouble bias[16] = {
		0.5, 0.0, 0.0, 0.0,
		0.0, 0.5, 0.0, 0.0,
		0.0, 0.0, 0.5, 0.0,
		0.5, 0.5, 0.5, 1.0};

	// Grab modelview and transformation matrices
	glGetDoublev(GL_MODELVIEW_MATRIX, modelView);
	glGetDoublev(GL_PROJECTION_MATRIX, projection);

	glMatrixMode(GL_TEXTURE);
	glActiveTextureARB(GL_TEXTURE5);

	glLoadIdentity();
	glLoadMatrixd(bias);

	// concatating all matrices into one.
	glMultMatrixd (projection);
	glMultMatrixd (modelView);

	// Go back to normal matrix mode
	glMatrixMode(GL_MODELVIEW);
}

void CWorld::Render(CVector cam_pos)
{
	CEngine::BindFaceShader();
	CEngine::GetFaceShader()->SetLight(light_pos, light_color);
	CEngine::GetFaceShader()->SetLightAmbientColor(ambient_color);
	CEngine::GetFaceShader()->SetTwoSide(0);
	CEngine::GetFaceShader()->SetBorder(0, Vec(0.0, 0.0), Vec(0.0, 0.0));
	CEngine::GetFaceShader()->SetShadow(shadow.enabled ? 1 : 0, shadow.tex, Vec(1.0 / (float)shadow.width, 1.0 / (float)shadow.height));
	//CEngine::GetFaceShader()->SetClip(clip.v, clip.d);

	PutToGL(cam_pos);
}

void CWorld::PutToGL(CVector cam_pos)
{
	vector<CObject *>::iterator i;

	for(i=objects.begin(); i!=objects.end(); i++) // TODO: Sort objects of CWorld by z-distance before painting
	{
		(*i)->PutToGL(cam_pos);
	}
}
