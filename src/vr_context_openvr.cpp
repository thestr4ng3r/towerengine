
#include "towerengine.h"
#include "vr_context_openvr.h"


using namespace std;

tMesh *tMeshFromOpenVRRenderModel(vr::RenderModel_t *render_model, vr::RenderModel_TextureMap_t *texture)
{
	tMesh *mesh = new tMesh();

	tStandardMaterial *material = new tStandardMaterial();

	GLuint tex;
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex),
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, texture->unWidth, texture->unHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture->rubTextureMapData);
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);

	material->SetTexture(tStandardMaterial::BASE_COLOR, tex);
	material->SetMetallic(0.05);
	material->SetRoughness(0.95);

	material->UpdateUniformBuffer();

	mesh->AddMaterial("render_model", material);

	for(unsigned int i=0; i<render_model->unVertexCount; i++)
	{
		tVertex v;

		v.pos = tVec(render_model->rVertexData[i].vPosition.v[0],
					render_model->rVertexData[i].vPosition.v[1],
					render_model->rVertexData[i].vPosition.v[2]);

		v.normal = tVec(render_model->rVertexData[i].vNormal.v[0],
					   render_model->rVertexData[i].vNormal.v[1],
					   render_model->rVertexData[i].vNormal.v[2]);

		v.uv = tVec(render_model->rVertexData[i].rfTextureCoord[0],
					render_model->rVertexData[i].rfTextureCoord[1]);

		mesh->AddVertex(v);
	}

	for(unsigned int i=0; i<render_model->unTriangleCount; i++)
	{
		tTriangle t;
		t.v[0] = render_model->rIndexData[i * 3 + 0];
		t.v[1] = render_model->rIndexData[i * 3 + 1];
		t.v[2] = render_model->rIndexData[i * 3 + 2];
		t.mat = material;
		mesh->AddTriangle(t);
	}

	mesh->GenerateAABB();
	mesh->GeneratePhysicsMesh();

	return mesh;
}



tVRContextOpenVR::tVRContextOpenVR(float resolution_scale)
{
	this->resolution_scale = resolution_scale;

	vr::EVRInitError error = vr::VRInitError_None;

	system = vr::VR_Init(&error, vr::VRApplication_Scene);
	if(error != vr::VRInitError_None)
	{
		fprintf(stderr, "Failed to initialize OpenVR: %s\n", vr::VR_GetVRInitErrorAsEnglishDescription(error));
		throw std::exception(); //string("Failed to initialize OpenVR: ") + string(vr::VR_GetVRInitErrorAsEnglishDescription(error)));
	}
	else
		printf("OpenVR initialized!\n");

	if(!vr::VRCompositor())
		throw std::exception(); //string("Failed to initialize OpenVR Compositor."));

	render_models = (vr::IVRRenderModels *)vr::VR_GetGenericInterface(vr::IVRRenderModels_Version, &error);
	if(!render_models)
		throw std::exception(); //"Failed to get OpenVR IVRRenderModels interface."); // TODO: display error

	system->GetRecommendedRenderTargetSize(&eye_render_width, &eye_render_height);

	printf("Render Target Size: %d, %d\n", (int)eye_render_width, (int)eye_render_height);

	eye_render_width = (unsigned int)((float)eye_render_width * this->resolution_scale);
	eye_render_height = (unsigned int)((float)eye_render_height * this->resolution_scale);
	printf("Rendering at: %d, %d\n", (int)eye_render_width, (int)eye_render_height);

	system->GetProjectionRaw(vr::Eye_Left, &fov[0].left, &fov[0].right, &fov[0].top, &fov[0].bottom);
	system->GetProjectionRaw(vr::Eye_Right, &fov[1].left, &fov[1].right, &fov[1].top, &fov[1].bottom);
		
	eye_matrix[0] = tMatrix4FromOpenVR(system->GetEyeToHeadTransform(vr::Eye_Left));
	eye_matrix[1] = tMatrix4FromOpenVR(system->GetEyeToHeadTransform(vr::Eye_Right));

	glGenTextures(1, &render_tex);
	glBindTexture(GL_TEXTURE_2D, render_tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, eye_render_width * 2, eye_render_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, render_tex, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	hidden_area_mesh[0] = system->GetHiddenAreaMesh(vr::Eye_Left);
	hidden_area_mesh[1] = system->GetHiddenAreaMesh(vr::Eye_Right);

	RenderHiddenAreaStencil();
}

tVRContextOpenVR::~tVRContextOpenVR(void)
{
	vr::VR_Shutdown();
}



void tVRContextOpenVR::RenderHiddenAreaStencil(void)
{
	if(hidden_area_mesh[0].unTriangleCount == 0 && hidden_area_mesh[1].unTriangleCount == 0)
	{
		hidden_area_stencil_rbo = 0;
		return;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	GLuint rbo;
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_STENCIL_INDEX8, eye_render_width * 2, eye_render_height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	glDepthMask(GL_FALSE);
	glStencilMask(GL_TRUE);

	glDisable(GL_DEPTH_TEST);
	glEnable(GL_STENCIL_TEST);

	GLint clear_value = 0;
	glClearBufferiv(GL_STENCIL, 0, &clear_value);


	glStencilFunc(GL_ALWAYS, 1, 0xff);
	glStencilOp(GL_KEEP, GL_ZERO, GL_REPLACE);


	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	unsigned int triangle_count = hidden_area_mesh[0].unTriangleCount + hidden_area_mesh[1].unTriangleCount;
	unsigned int vertex_data_count = triangle_count * 3 * 2;
	GLfloat *vertex_data = new GLfloat[vertex_data_count];
	for(int eye = 0; eye < 2; eye++)
	{
		int offset = (eye == 1 ? hidden_area_mesh[0].unTriangleCount * 3 * 2 : 0);
		for(unsigned int v = 0; v < hidden_area_mesh[eye].unTriangleCount * 3; v++)
		{
			vertex_data[offset + v * 2 + 0] = hidden_area_mesh[eye].pVertexData[v].v[0] + (eye == 0 ? -1.0f : 0.0f);
			vertex_data[offset + v * 2 + 1] = hidden_area_mesh[eye].pVertexData[v].v[1] * 2.0f - 1.0f;
		}
	}
	glBufferData(GL_ARRAY_BUFFER, vertex_data_count * sizeof(GLfloat), vertex_data, GL_STATIC_DRAW);
	glEnableVertexAttribArray(tShader::vertex_attribute);
	glVertexAttribPointer(tShader::vertex_attribute, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glViewport(0, 0, eye_render_width * 2, eye_render_height);

	tEngine::GetNoOpShader()->Bind();

	glDisable(GL_CULL_FACE);

	glDrawArrays(GL_TRIANGLES, 0, triangle_count * 3);
	glBindVertexArray(0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glDepthMask(GL_TRUE);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);

	delete[] vertex_data;

	hidden_area_stencil_rbo = rbo;



	glEnable(GL_STENCIL_TEST);

	glStencilFunc(GL_EQUAL, 0, 0xff);
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
}



unsigned int tVRContextOpenVR::GetOpenVRControllerStates(tVector src_pos,
												 unsigned int max_controller_count,
												 vr::TrackedDeviceIndex_t *device_indexes,
												 vr::TrackedDevicePose_t *poses,
												 vr::VRControllerState_t *controller_states,
												 tMesh **_render_model_meshes)
{
	unsigned int controller_index = 0;
	for(vr::TrackedDeviceIndex_t i=vr::k_unTrackedDeviceIndex_Hmd+1; i<vr::k_unMaxTrackedDeviceCount; i++)
	{
		if(!tracked_device_pose[i].bDeviceIsConnected)
			continue;

		if(system->GetTrackedDeviceClass(i) != vr::TrackedDeviceClass_Controller)
			continue;

		if(controller_index >= max_controller_count)
			break;

		device_indexes[controller_index] = i;

		poses[controller_index] = tracked_device_pose[i];
		system->GetControllerState(i, &controller_states[controller_index], sizeof(controller_states[controller_index]));

		_render_model_meshes[controller_index] = 0;
		char buffer[vr::k_unMaxPropertyStringSize];
		if(system->GetStringTrackedDeviceProperty(i, vr::Prop_RenderModelName_String, buffer, vr::k_unMaxPropertyStringSize) > 0)
		{
			auto it = render_model_meshes.find(string(buffer));
			if(it != render_model_meshes.end())
			{
				_render_model_meshes[controller_index] = it->second;
			}
		}

		controller_index++;
	}

	for(unsigned int i=controller_index; i<2; i++)
	{
		memset(&poses[i], 0, sizeof(vr::TrackedDevicePose_t));
		memset(&controller_states[i], 0, sizeof(vr::VRControllerState_t));
		_render_model_meshes[i] = 0;
	}

	return controller_index;
}


void tVRContextOpenVR::StartFrame(tVector2 cam_rot, tVector src_pos, tVector &center_pos, tVector &center_dir, tCamera *camera[2])
{
	vr::VRCompositor()->WaitGetPoses(tracked_device_pose, vr::k_unMaxTrackedDeviceCount, NULL, 0 );

	vr::TrackedDevicePose_t hmd_pose = tracked_device_pose[vr::k_unTrackedDeviceIndex_Hmd];
	if(hmd_pose.bPoseIsValid)
	{
		hmd_matrix = tMatrix4FromOpenVR(hmd_pose.mDeviceToAbsoluteTracking);
	}

	float *data = hmd_matrix.GetData();
	center_pos = src_pos + tVec(data[3], data[7], data[11]);
	center_dir = -tVec(data[2], data[6], data[10]);


	for(int i=0; i<2; i++)
	{
		tMatrix4 eye_hmd_matrix = hmd_matrix * eye_matrix[i];
		data = eye_hmd_matrix.GetData();

		tVector pos = src_pos + tVec(data[3], data[7], data[11]);
		tVector dir = -tVec(data[2], data[6], data[10]);
		tVector up = tVec(data[1], data[5], data[9]);
		
		camera[i]->SetFOV(fov[i].left, fov[i].right, fov[i].top, fov[i].bottom);
		camera[i]->SetPosition(pos);
		camera[i]->SetDirection(dir);
		camera[i]->SetUp(up);
	}


	for(vr::TrackedDeviceIndex_t i = 0; i<vr::k_unMaxTrackedDeviceCount; i++)
	{
		if(!tracked_device_pose[i].bDeviceIsConnected)
			continue;

		//if(system->GetTrackedDeviceClass(i) != vr::TrackedDeviceClass_Controller)
		//	continue;

		char buffer[vr::k_unMaxPropertyStringSize];
		if(system->GetStringTrackedDeviceProperty(i, vr::Prop_RenderModelName_String, buffer, vr::k_unMaxPropertyStringSize) <= 0)
			continue;

		map<string, tMesh *>::iterator it = render_model_meshes.find(string(buffer));

		if(it != render_model_meshes.end())
			continue;


		vr::RenderModel_t *render_model;
		while(1)
		{
			if(render_models->LoadRenderModel_Async(buffer, &render_model) != vr::VRRenderModelError_Loading)
				break;
			//Sleep(1);
			// TODO: asynchronous render model loading
		}

		vr::RenderModel_TextureMap_t *render_model_tex;
		while(1)
		{
			if(render_models->LoadTexture_Async(render_model->diffuseTextureId, &render_model_tex) != vr::VRRenderModelError_Loading)
				break;
			//Sleep(1);
		}

		tMesh *mesh = tMeshFromOpenVRRenderModel(render_model, render_model_tex);
		render_models->FreeRenderModel(render_model);
		render_models->FreeTexture(render_model_tex);
		render_model_meshes.insert(pair<string, tMesh *>(string(buffer), mesh));
	}
}

void tVRContextOpenVR::FinishFrame(void)
{
	vr::Texture_t left_tex;
	left_tex.eType = vr::TextureType_OpenGL;
	left_tex.eColorSpace = vr::ColorSpace_Gamma;
	left_tex.handle = (void *)render_tex;

	vr::VRTextureBounds_t left_bounds;
	left_bounds.uMin = 0.0f;
	left_bounds.uMax = 0.5f;
	left_bounds.vMin = 0.0f;
	left_bounds.vMax = 1.0f;
	vr::VRCompositor()->Submit(vr::Eye_Left, &left_tex, &left_bounds);

	vr::Texture_t right_tex;
	right_tex.eType = vr::TextureType_OpenGL;
	right_tex.eColorSpace = vr::ColorSpace_Gamma;
	right_tex.handle = (void *)render_tex;

	vr::VRTextureBounds_t right_bounds;
	right_bounds.uMin = 0.5f;
	right_bounds.uMax = 1.0f;
	right_bounds.vMin = 0.0f;
	right_bounds.vMax = 1.0f;
	vr::VRCompositor()->Submit(vr::Eye_Right, &right_tex, &right_bounds);

	glFlush();
	glFinish();
}

void tVRContextOpenVR::BlitMirrorFrame(GLint dst_x0, GLint dst_y0, GLint dst_x1, GLint dst_y1)
{
	glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);
	glBlitFramebuffer(0, 0, eye_render_width * 2, eye_render_height, dst_x0, dst_y0, dst_x1, dst_y1, GL_COLOR_BUFFER_BIT, GL_NEAREST);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
}
