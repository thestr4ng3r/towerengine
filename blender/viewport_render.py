
import bpy
from bgl import *
from mathutils import *
from .towerengine import *
from .mesh_converter import MeshConverter



def vec_te(blender_vec):
	return Vec(blender_vec.x, blender_vec.z, -blender_vec.y)

def vec2_te(blender_vec2):
	return Vec(blender_vec2.x, blender_vec2.y)

def vec_bl(te_vec):
	return Vector((te_vec.x, -te_vec.z, te_vec.y))

def te_vec2(x, y):
	v = tVector2()
	v.x = x
	v.y = y
	return v


class TowerEngineContext:
	def __init__(self, width, height):
		if tEngine_Init():
			print("Tower Engine initialized.")
		else:
			print("Failed to initialize Tower Engine.")
			return

		self.world = tWorld()
		self.renderer = tDefaultRenderer(width, height, self.world)
		self.camera = self.renderer.GetCamera()

		#tWorld_SetAmbientColor(self.world, Vec(1.0, 1.0, 1.0))

		self.camera.SetPosition(Vec(3.0, 3.0, 3.0))
		self.camera.SetDirection(Vec(-1.0, -1.0, -1.0))

		self.coordinate_system_object = tCoordinateSystemObject()
		self.world.AddObject(self.coordinate_system_object)

		self.skybox = tSkyBox()
		self.world.SetSkyBox(self.skybox)

		self.objects = {}
		self.meshes = {}


	def convert_mesh(self, mesh):
		converter = TowerEngineMeshConverter()
		converter.add_mesh(mesh)
		converter.execute()
		tmesh = converter.tmesh
		tmesh.CalculateAllTangents()
		tmesh.GenerateBoundingBox()
		return tmesh


class TowerEngineMeshConverter(MeshConverter):
	def __init__(self):
		super().__init__()

		self.tmesh = tMesh()
		self.tmesh.thisown = 0

	def _add_material(self, material):
		return

	def _add_vertex(self, vertex, uv, normal):
		tvertex = tVertex()
		tvertex.pos = vec_te(vertex.co)
		tvertex.normal = vec_te(vertex.co)
		tvertex.uv = te_vec2(0.0, 0.0)#vec2_te(v.uv))
		self.tmesh.AddVertex(tvertex)

	def _add_triangle(self, vertices, material):
		ttriangle = tTriangle()
		# TODO: get material
		ttriangle.mat = self.tmesh.GetIdleMaterial()
		ttriangle.SetVertices(vertices[0], vertices[1], vertices[2])
		self.tmesh.AddTriangle(ttriangle)


te_context = None

def update(render_engine, context):
	region = context.region
	region3d = context.region_data
	space = context.space_data

	global te_context
	if not te_context:
		te_context = TowerEngineContext(region.width, region.height)

	te_context.world.ClearObjects()
	te_context.world.ClearPointLights()

	te_context.world.AddObject(te_context.coordinate_system_object)

	for o in context.scene.objects:
		if o.type == "MESH":
			tmesh = te_context.convert_mesh(o.to_mesh(context.scene, True, "PREVIEW"))
			tmeshobject = tMeshObject(tmesh)
			tmeshobject.thisown = 0
			te_context.world.AddObject(tmeshobject)
		elif o.type == "LAMP":
			tlight = tPointLight(vec_te(o.location), Vec(1.0, 1.0, 1.0), 100.0)
			tlight.thisown = 0
			te_context.world.AddPointLight(tlight)







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

	if cubemap_tex is not None and te_context.skybox.GetCubeMap() == 0:
		cubemap_tex.image.gl_load()
		cubemap_gl = LoadGLCubeMap(cubemap_tex.image.filepath_from_user())
		te_context.skybox.SetCubeMap(cubemap_gl)
	#else:
	#	te_context.skybox.SetCubeMap(0)




def render(render_engine, context):
	global te_context

	region = context.region
	region3d = context.region_data
	space = context.space_data

	width = region.width
	height = region.height

	#print("width: " + str(width) + ", height: " + str(height))

	te_context.renderer.ChangeSize(width, height)

	angle = space.lens
	aspect = float(width) / float(height)
	angle = 49.5
	te_context.camera.SetFOVVerticalAngle(angle, aspect)

	view_matrix = region3d.view_matrix
	view_matrix_inv = view_matrix.inverted()

	cam_pos = view_matrix_inv * Vector((0.0, 0.0, 0.0))
	cam_dir = region3d.view_location - cam_pos

	cam_pos_te = vec_te(cam_pos)
	te_context.camera.SetPosition(cam_pos_te)

	cam_dir_te = vec_te(cam_dir)
	te_context.camera.SetDirection(cam_dir_te)




	glClearColor(0.0, 0.0, 0.0, 1.0)
	glClear(GL_COLOR_BUFFER_BIT)

	mat = region3d.perspective_matrix

	glLineWidth(2)

	#glMatrixMode(GL_PROJECTION)
	#glLoadIdentity()

	#glMatrixMode(GL_MODELVIEW)
	#glLoadIdentity()


	te_context.renderer.Render(0, int(region.x), int(region.y), int(width), int(height))


	#OpenGL.GL.glBindFramebuffer(OpenGL.GL.GL_FRAMEBUFFER, 0)


	# for obj in context.scene.objects:
	# 	if obj.type != "MESH":
	# 		continue
	#
	# 	mesh = obj.to_mesh(context.scene, True, "PREVIEW")
	#
	# 	if obj in context.selected_objects:
	# 		glColor4f(0.0, 1.0, 0.0, 1.0)
	# 	else:
	# 		glColor4f(0.0, 0.0, 1.0, 1.0)
	#
	# 	glBegin(GL_LINES)
	# 	for edge in mesh.edges:
	# 		for i in [0, 1]:
	# 			vert = mesh.vertices[edge.vertices[i]]
	# 			v = mat * obj.matrix_world * Vector((vert.co.x, vert.co.y, vert.co.z, 1.0))
	# 			glVertex4f(*v)
	# 	glEnd()
