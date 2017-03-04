
import bpy
from bgl import *
from mathutils import *

def render(render_engine, context):
	print("render")
	glClearColor(0.0, 0.0, 0.0, 1.0)
	glClear(GL_COLOR_BUFFER_BIT)

	region3d = context.space_data.region_3d

	mat = region3d.perspective_matrix

	glLineWidth(2)

	glMatrixMode(GL_PROJECTION)
	glLoadIdentity()

	glMatrixMode(GL_MODELVIEW)
	glLoadIdentity()

	for obj in context.scene.objects:
		if obj.type != "MESH":
			continue

		mesh = obj.to_mesh(context.scene, True, "PREVIEW")

		if obj in context.selected_objects:
			glColor4f(0.0, 1.0, 0.0, 1.0)
		else:
			glColor4f(0.0, 0.0, 1.0, 1.0)

		glBegin(GL_LINES)
		for edge in mesh.edges:
			for i in [0, 1]:
				vert = mesh.vertices[edge.vertices[i]]
				v = mat * obj.matrix_world * Vector((vert.co.x, vert.co.y, vert.co.z, 1.0))
				glVertex4f(*v)
		glEnd()
