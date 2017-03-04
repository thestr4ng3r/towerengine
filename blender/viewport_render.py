
import bpy
from bgl import *
from mathutils import *
from _towerengine_python import *
#import OpenGL.GL


class TowerEngineContext:
	def __init__(self, width, height):
		if tEngine_Init():
			print("Tower Engine initialized.")
		else:
			print("Failed to initialize Tower Engine.")
			return

		self.world = new_tWorld()
		self.renderer = new_tDefaultRenderer(width, height, self.world)
		self.camera = tDefaultRenderer_GetCamera(self.renderer)

		tCamera_SetPosition(self.camera, Vec(3.0, 3.0, 3.0))
		tCamera_SetDirection(self.camera, Vec(-1.0, -1.0, -1.0))

		self.coordinate_system_object = new_tCoordinateSystemObject()
		tWorld_AddObject(self.world, self.coordinate_system_object)




te_context = None

def update(render_engine, context):
	region = context.region
	region3d = context.region_data
	space = context.space_data

	global te_context
	if not te_context:
		te_context = TowerEngineContext(region.width, region.height)



def vec_te(blender_vec):
	return Vec(blender_vec.x, blender_vec.z, -blender_vec.y)

def vec_bl(te_vec):
	return Vector((te_vec.x, -te_vec.z, te_vec.y))


def render(render_engine, context):
	global te_context

	region = context.region
	region3d = context.region_data
	space = context.space_data

	width = region.width
	height = region.height

	print("width: " + str(width) + ", height: " + str(height))

	tDefaultRenderer_ChangeSize(te_context.renderer, width, height)

	angle = space.lens
	aspect = float(width) / float(height)
	tCamera_SetFOVVerticalAngle(te_context.camera, angle, aspect)

	view_matrix = region3d.view_matrix
	view_matrix_inv = view_matrix.inverted()

	cam_pos = view_matrix_inv * Vector((0.0, 0.0, 0.0))
	cam_dir = region3d.view_location - cam_pos

	cam_pos_te = vec_te(cam_pos)
	tCamera_SetPosition(te_context.camera, cam_pos_te)

	cam_dir_te = vec_te(cam_dir)
	tCamera_SetDirection(te_context.camera, cam_dir_te)




	glClearColor(0.0, 0.0, 0.0, 1.0)
	glClear(GL_COLOR_BUFFER_BIT)

	mat = region3d.perspective_matrix

	glLineWidth(2)

	#glMatrixMode(GL_PROJECTION)
	#glLoadIdentity()

	#glMatrixMode(GL_MODELVIEW)
	#glLoadIdentity()


	tDefaultRenderer_Render(te_context.renderer, 0, int(region.x), int(region.y), int(width), int(height))


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
