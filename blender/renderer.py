import bpy
import bgl

class TowerEngineRenderEngine(bpy.types.RenderEngine):
	bl_idname = "towerengine_renderer"
	bl_label = "Tower Engine Renderer"
	bl_use_preview = True
	bl_use_texture_preview = True

	def render(self, scene):
		print("RENDER")
		return

	def view_update(self, context):
		draw(self, context)

	def view_draw(self, context):
		draw(self, context)

def draw(self, context):
	print("DRAW")
	bgl.glClearColor(0.0, 1.0, 0.0, 1.0)
	bgl.glClear(bgl.GL_COLOR_BUFFER_BIT)
