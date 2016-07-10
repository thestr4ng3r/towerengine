bl_info = {
	"name":		 "TowerEngine Support for Blender",
	"author":	   "Florian Märkl",
	"blender":	  (2,7,0),
	"version":	  (0,0,4),
	"location":	 "File > Import-Export",
	"description":  "Support for exporting TowerEngine Meshes and Scenes",
	"category":	 "Import-Export"
}


import bpy
import bpy.utils
import bpy.types
from .TowerEngineMeshExporter import TowerEngineMeshExporter
from .TowerEngineSceneExporter import TowerEngineSceneExporter
from .TowerEngineProperties import *

def menu_func(self, context):
	self.layout.operator(TowerEngineMeshExporter.bl_idname, text="TowerEngine Mesh (.tem)")
	self.layout.operator(TowerEngineSceneExporter.bl_idname, text="TowerEngine Scene (.tes)")
	
	
	
	

def register():
	bpy.utils.register_module(__name__)
	bpy.types.INFO_MT_file_export.append(menu_func)
	
	bpy.types.Object.towerengine = bpy.props.PointerProperty(type=TowerEngineObjectPropertyGroup)
	bpy.types.Material.towerengine = bpy.props.PointerProperty(type=TowerEngineMaterialPropertyGroup)
	bpy.types.Mesh.towerengine = bpy.props.PointerProperty(type=TowerEngineMeshPropertyGroup)


def unregister():	
	bpy.utils.unregister_module(__name__)
	bpy.types.INFO_MT_file_export.remove(menu_func)
	
if __name__ == "__main__":
	register()
	