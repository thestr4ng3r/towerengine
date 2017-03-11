
import bpy
from bgl import *
from mathutils import *
from .towerengine import *
from .mesh_converter import MeshConverter
from math import atan, degrees



def vec_te(blender_vec):
	return Vec(blender_vec.x, blender_vec.z, -blender_vec.y)

def vec2_te(blender_vec2):
	return Vec(blender_vec2.x, blender_vec2.y)

def mat3_te(blender_mat):
	x = Vec(blender_mat[0][0], blender_mat[0][2], -blender_mat[0][1])
	y = Vec(blender_mat[2][0], blender_mat[2][2], -blender_mat[2][1])
	z = Vec(-blender_mat[1][0], -blender_mat[1][2], blender_mat[1][1])
	return tMatrix3(x, y, z)

def vec_bl(te_vec):
	return Vector((te_vec.x, -te_vec.z, te_vec.y))


class TowerEngineContext:
	def __init__(self, context):
		if tEngine_Init():
			print("Tower Engine initialized.")
		else:
			print("Failed to initialize Tower Engine.")
			return

		self.world = tWorld()
		self.renderer = tDefaultRenderer(context.region.width, context.region.height, self.world)
		self.camera = self.renderer.GetCamera()

		self.camera.SetPosition(Vec(3.0, 3.0, 3.0))
		self.camera.SetDirection(Vec(-1.0, -1.0, -1.0))

		self.coordinate_system_object = tCoordinateSystemObject()
		self.world.AddObject(self.coordinate_system_object)

		self.skybox = tSkyBox()
		self.world.SetSkyBox(self.skybox)

		self.objects = {}
		self.meshes = {}
		self.materials = {}

		self.objects_updated = {}
		self.meshes_updated = {}
		self.materials_updated = {}


	def update_material(self, m):
		if m in self.materials_updated:
			return materials_updated[m]

		if m in self.materials:
			tmaterial = self.materials[m]
		else:
			tmaterial = tDefaultMaterial()

		return tmaterial


	def __update_mesh(self, context, o):
		if len(o.modifiers) > 0:
			mesh_id = (o.data, o)
		else:
			mesh_id = (o.data, None)

		if mesh_id in self.meshes_updated:
			return self.meshes_updated[mesh_id]

		if mesh_id in self.meshes:
			tmesh = self.meshes[mesh_id]
			tmesh.ClearTriangles()
			tmesh.ClearVertices()
		else:
			tmesh = None

		mesh = o.to_mesh(context.scene, True, "PREVIEW")
		converter = TowerEngineMeshConverter(self, tmesh)
		converter.add_mesh(mesh)
		converter.execute()
		tmesh = converter.tmesh

		self.meshes_updated[mesh_id] = tmesh

		tmesh.CalculateAllTangents()
		tmesh.GenerateBoundingBox()
		return tmesh


	def __update_mesh_object(self, context, o):
		tmesh = self.__update_mesh(context, o)

		if o in self.objects:
			tobject = self.objects[o]
			tobject.SetMesh(tmesh)
		else:
			tobject = tMeshObject(tmesh)
			tobject.thisown = 0
			self.world.AddObject(tobject)

		ttransform = tTransform()
		ttransform.SetPosition(vec_te(o.location))
		ttransform.SetBasis(mat3_te(o.matrix_world))
		tobject.SetTransform(ttransform)

		self.objects_updated[o] = tobject


	def __update_cleanup(self):
		for o in self.objects:
			if o not in self.objects_updated:
				tobject = self.objects[o]
				self.world.RemoveObject(tobject)
				del tobject

		for m in self.meshes:
			if m not in self.meshes_updated:
				del self.meshes[m]

		for m in self.materials:
			if m not in self.materials_updated:
				del self.materials[m]


	def __update_skybox_test(self, context):
		cubemap_tex = None
		for texture_slot in context.scene.world.texture_slots.values():
			if texture_slot is None:
				continue

			tex = texture_slot.texture

			if tex.type != "ENVIRONMENT_MAP":
				continue

			if tex.image is None:
				continue

			if texture_slot.use_map_horizon:
				cubemap_tex = tex

		if cubemap_tex is not None and self.skybox.GetCubeMap() == 0:
			cubemap_tex.image.gl_load()
			cubemap_gl = LoadGLCubeMap(cubemap_tex.image.filepath_from_user())
			self.skybox.SetCubeMap(cubemap_gl)
		# else:
		#	self.skybox.SetCubeMap(0)


	def update(self, context):
		self.world.ClearPointLights()

		self.objects_updated = {}
		self.meshes_updated = {}
		self.materials_updated = {}

		for o in context.scene.objects:
			if o.type == "MESH":
				self.__update_mesh_object(context, o)
			elif o.type == "LAMP":
				tlight = tPointLight(vec_te(o.location), Vec(1.0, 1.0, 1.0), 100.0)
				tlight.thisown = 0
				self.world.AddPointLight(tlight)

		self.__update_skybox_test(context)

		self.__update_cleanup()

		self.objects = self.objects_updated
		self.meshes = self.meshes_updated
		self.materials = self.materials_updated



	def render(self, context):
		region = context.region
		region3d = context.region_data
		space = context.space_data

		viewport = [region.x, region.y, region.width, region.height]

		self.renderer.ChangeSize(viewport[2], viewport[3])

		sensor_width = 32
		lens = space.lens
		angle = degrees(2 * atan(sensor_width / (2 * lens)))

		aspect = float(viewport[2]) / float(viewport[3])
		self.camera.SetFOVVerticalAngle(angle, aspect)

		view_matrix = region3d.view_matrix
		view_matrix_inv = view_matrix.inverted()

		cam_pos = view_matrix_inv * Vector((0.0, 0.0, 0.0))
		cam_dir = region3d.view_location - cam_pos

		cam_pos_te = vec_te(cam_pos)
		self.camera.SetPosition(cam_pos_te)

		cam_dir_te = vec_te(cam_dir)
		cam_dir_te.Normalize()
		self.camera.SetDirection(cam_dir_te)

		glClearColor(0.0, 1.0, 0.0, 1.0)
		glClear(GL_COLOR_BUFFER_BIT)

		glDisable(GL_SCISSOR_TEST)
		self.renderer.Render(0, viewport[0], viewport[1], viewport[2], viewport[3])
		glEnable(GL_SCISSOR_TEST)



class TowerEngineMeshConverter(MeshConverter):
	def __init__(self, te_context, mesh=None):
		super().__init__()

		self.te_context = te_context

		self.tmesh = mesh

		if not self.tmesh:
			self.tmesh = tMesh()
			self.tmesh.thisown = 0

	def _add_material(self, material):
		return

	def _add_vertex(self, vertex, uv, normal):
		tvertex = tVertex()
		tvertex.pos = vec_te(vertex.co)
		tvertex.normal = vec_te(vertex.co)
		tvertex.uv = Vec(0.0, 0.0)#vec2_te(v.uv))
		self.tmesh.AddVertex(tvertex)

	def _add_triangle(self, vertices, material):
		ttriangle = tTriangle()
		ttriangle.mat = self.tmesh.GetIdleMaterial() #self.te_context.update_material(material)
		ttriangle.SetVertices(vertices[0], vertices[1], vertices[2])
		self.tmesh.AddTriangle(ttriangle)



te_context = None

def update(render_engine, context):
	global te_context
	if not te_context:
		te_context = TowerEngineContext(context)
	te_context.update(context)

def render(render_engine, context):
	global te_context
	te_context.render(context)