
import bpy
from bpy.props import BoolProperty
from bpy_extras.io_utils import ExportHelper
from .Mesh import ExportMesh


class TowerEngineMeshExporter(bpy.types.Operator, ExportHelper):
	bl_idname	   = "export_tem.tem"
	bl_label		= "Export"
	bl_options	  = {'PRESET'}
	
	filename_ext = ".tem"

	selection_prop = BoolProperty(name = "Selection Only", description = "Export selection only", default = True)
	include_tex_prop = BoolProperty(name = "Include Textures in File",
									description = "Include Texture Images directly into .tem file instead of copying and linking the URL",
									default = False)
	
	def execute(self, context):
		scene = bpy.data.scenes.values()[0]
		export_mesh = ExportMesh()
		
		for obj in bpy.data.objects:
			if self.selection_prop == True and obj.select == False:
				continue
			
			try:
				mesh = obj.to_mesh(scene, True, 'PREVIEW')
				mesh.transform(obj.matrix_world)
				mesh.calc_normals()
				export_mesh.add_mesh(mesh)
			except RuntimeError:
				continue
			
		export_mesh.save(self.filepath, self.include_tex_prop)
		
		return {'FINISHED'}