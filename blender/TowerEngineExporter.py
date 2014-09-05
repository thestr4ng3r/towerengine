bl_info = {
	"name":         "TowerEngine Mesh Exporter",
	"author":       "Florian Märkl",
	"blender":      (2,6,2),
	"version":      (0,0,4),
	"location":     "File > Import-Export",
	"description":  "An exporter for TowerEngine Meshes (.tem)",
	"category":     "Import-Export"
}

import bpy
import os
import shutil
import base64
from bpy.props import BoolProperty
from bpy_extras.io_utils import ExportHelper
from xml.dom.minidom import Document

class TowerEngineExporter(bpy.types.Operator, ExportHelper):
	bl_idname		= "export_tem.tem"
	bl_label		= "Export"
	bl_options 		= {'PRESET'}
	
	filename_ext = ".tem"

	selection_prop = BoolProperty(name = "Selection Only", description = "Export selection only", default = True)
	include_tex_prop = BoolProperty(name = "Include Textures in File", description = "Include Texture Images directly into .tem file instead of copying and linking the URL", default = True)
	
	def execute(self, context):
		doc = Document()
		root = doc.createElement("tmesh")
		root.setAttribute("version", "0.3")
		doc.appendChild(root)
		
		path = os.path.dirname(self.filepath)

		current_vertex_id = 0
		meshes = dict()
		materials = dict()

		scene = bpy.data.scenes.values()[0];
		
		for obj in bpy.data.objects:
			if self.selection_prop == True and obj.select == False:
				continue
			
			try:
				mesh = obj.to_mesh(scene, True, 'PREVIEW')
				mesh.transform(obj.matrix_world)
				mesh.calc_normals();
				meshes[mesh.name] = mesh
				for material in mesh.materials:
					materials[material.name] = material
			except RuntimeError:
				continue
				
		for material in materials.values():
			diffuse_tex = specular_tex = normal_tex = None
			
			for tex_slot in material.texture_slots:
				if tex_slot == None:
					continue
				texture = tex_slot.texture
				if texture.type != 'IMAGE':
					continue
				
				if tex_slot.use_map_color_diffuse:
					diffuse_tex = texture
				if tex_slot.use_map_color_spec or tex_slot.use_map_specular:
					specular_tex = texture
				if tex_slot.use_map_normal:
					normal_tex = texture
					
			if self.include_tex_prop == False and (diffuse_tex != None or specular_tex != None or normal_tex != None):
				tex_path = os.path.join(path, os.path.basename(self.filepath) + "_tex", material.name)
				try:
					os.makedirs(tex_path)
				except OSError as e:
					print(str(e))
					
			
			node = doc.createElement("material")
			node.setAttribute("name", material.name)
			
			if material.diffuse_intensity > 0.0:
				node2 = doc.createElement("diffuse")
				if diffuse_tex != None:
					file_extension = diffuse_tex.image.filepath.split(".")[-1]
					if self.include_tex_prop == False:
						tex_file = os.path.join(tex_path, "diffuse." + file_extension)
						try:
							shutil.copy(bpy.path.abspath(diffuse_tex.image.filepath), tex_file)
						except OSError as e:
							print(str(e))
						except IOError as e:
							print(str(e))
						node2.setAttribute("file", os.path.relpath(tex_file, path))
					else:
						with open(bpy.path.abspath(diffuse_tex.image.filepath), "rb") as image_file:
							image_data = image_file.read()
							image_base64 = base64.b64encode(image_data)
							image_text = doc.createTextNode(image_base64.decode("utf-8"))
							node2.appendChild(image_text)
						node2.setAttribute("image-extension", file_extension)

				node2.setAttribute("r", str(material.diffuse_color.r * material.diffuse_intensity))
				node2.setAttribute("g", str(material.diffuse_color.g * material.diffuse_intensity))
				node2.setAttribute("b", str(material.diffuse_color.b * material.diffuse_intensity))
				node2.setAttribute("ambient", str(material.ambient))
				node.appendChild(node2);
	
			if material.specular_intensity > 0.0:
				node2 = doc.createElement("specular")
				if specular_tex != None:
					file_extension = specular_tex.image.filepath.split(".")[-1]
					if self.include_tex_prop == False:
						tex_file = os.path.join(tex_path, "specular." + specular_tex.image.filepath.split(".")[-1])
						try:
							shutil.copy(bpy.path.abspath(specular_tex.image.filepath), tex_file)
						except OSError as e:
							print(str(e))
						except IOError as e:
							print(str(e))
						node2.setAttribute("file", os.path.relpath(tex_file, path))
					else:
						with open(bpy.path.abspath(specular_tex.image.filepath), "rb") as image_file:
							image_data = image_file.read()
							image_base64 = base64.b64encode(image_data)
							image_text = doc.createTextNode(image_base64.decode("utf-8"))
							node2.appendChild(image_text)
						node2.setAttribute("image-extension", file_extension)

				node2.setAttribute("r", str(material.specular_color.r * material.specular_intensity))
				node2.setAttribute("g", str(material.specular_color.g * material.specular_intensity))
				node2.setAttribute("b", str(material.specular_color.b * material.specular_intensity))
				node2.setAttribute("exponent", str(material.specular_hardness))
				node.appendChild(node2)

			if normal_tex != None:
				file_extension = normal_tex.image.filepath.split(".")[-1]
				node2 = doc.createElement("normal")
				if self.include_tex_prop == False:
					tex_file = os.path.join(tex_path, "normal." + normal_tex.image.filepath.split(".")[-1])
					try:
						shutil.copy(bpy.path.abspath(normal_tex.image.filepath), tex_file)
					except OSError as e:
						print(str(e))
					except IOError as e:
						print(str(e))
					node2.setAttribute("file", os.path.relpath(tex_file, path))
				else:
					with open(bpy.path.abspath(normal_tex.image.filepath), "rb") as image_file:
						image_data = image_file.read()
						image_base64 = base64.b64encode(image_data)
						image_text = doc.createTextNode(image_base64.decode("utf-8"))
						node2.appendChild(image_text)
					node2.setAttribute("image-extension", file_extension)


				node.appendChild(node2)

			root.appendChild(node)

		for mesh in meshes.values():
			face_vertex_id = dict()
			vertex_id = dict()
			vertex_nodes = dict()
			vertex_uv = dict()
			vertex_normal = dict()
			mesh.update(False, True)
			
			if mesh.tessface_uv_textures.active != None:
				uv_layer = mesh.tessface_uv_textures.active.data
			else:
				uv_layer = None
			
			for face in mesh.tessfaces: # Create Vertex Nodes from Face Vertices
					face_vertex_id[face] = dict()
					
					for i in range(len(face.vertices)): # All Vertices of Face
						vertex = face.vertices[i]
						vertex_found = False
						
						if vertex not in vertex_id:
							vertex_id[vertex] = list()
							vertex_nodes[vertex] = list()
							vertex_uv[vertex] = list()
							vertex_normal[vertex] = list()
							
						if face.use_smooth:
							normal = mesh.vertices[vertex].normal
						else:
							normal = face.normal
						
						if uv_layer != None:
							uv = uv_layer[face.index].uv[i]
						else:
							uv = [0.0, 0.0]
						
						for j in range(len(vertex_id[vertex])):
							if vertex_uv[vertex][j][0] == uv[0] and vertex_uv[vertex][j][1] == uv[1] and vertex_normal[vertex][j] == normal:
								face_vertex_id[face][i] = vertex_id[vertex][j]
								vertex_found = True
								break
						if vertex_found:
							continue
						
						node = doc.createElement("vertex")
						node.setAttribute("id", str(current_vertex_id))
						vertex_id[vertex].append(current_vertex_id)
						vertex_nodes[vertex].append(node)
						vertex_uv[vertex].append(uv)
						vertex_normal[vertex].append(normal)
						face_vertex_id[face][i] = current_vertex_id
						current_vertex_id = current_vertex_id + 1
						vertex = mesh.vertices[vertex]
						node.setAttribute("x", str(vertex.co.x))
						node.setAttribute("y", str(vertex.co.z))
						node.setAttribute("z", str(-vertex.co.y))
						node.setAttribute("u", str(uv[0]))
						node.setAttribute("v", str(uv[1]))
						node.setAttribute("nx", str(normal.x))
						node.setAttribute("ny", str(normal.z))
						node.setAttribute("nz", str(-normal.y))
						root.appendChild(node)
						
					
				
			for face in mesh.tessfaces:
				if len(face.vertices) >= 3:
					node = doc.createElement("triangle")
					node.setAttribute("mat", "$NONE")
					
					if len(mesh.materials) != 0:
						if mesh.materials[face.material_index] != None:
							node.setAttribute("mat", mesh.materials[face.material_index].name)
					
					for i in range(3):
						node2 = doc.createElement("vertex");	node2.setAttribute("id", str(face_vertex_id[face][i]));	node.appendChild(node2)
					
					root.appendChild(node)

				if len(face.vertices) >= 4:
					node = doc.createElement("triangle")
					node.setAttribute("mat", "$NONE")
					
					if len(mesh.materials) != 0:
						if mesh.materials[face.material_index] != None:
							node.setAttribute("mat", mesh.materials[face.material_index].name)
							
					for i in [2, 3, 0]:
						node2 = doc.createElement("vertex");	node2.setAttribute("id", str(face_vertex_id[face][i]));	node.appendChild(node2)
						
					root.appendChild(node)

		
			for armature in bpy.data.armatures:
				node = doc.createElement("armature")
				node.setAttribute("name", armature.name)

				for bone in armature.bones:
					node2 = doc.createElement("bone")
					node2.setAttribute("name", bone.name)
					if bone.parent != None:
						node2.setAttribute("parent", bone.parent.name)
					node2.setAttribute("hx", str(bone.head.x))
					node2.setAttribute("hy", str(bone.head.y))
					node2.setAttribute("hz", str(bone.head.z))
					node2.setAttribute("hr", str(bone.head_radius))
					node2.setAttribute("tx", str(bone.tail.x))
					node2.setAttribute("ty", str(bone.tail.y))
					node2.setAttribute("tz", str(bone.tail.z))
					node2.setAttribute("tr", str(bone.tail_radius))
					
					node2.setAttribute("envdist", str(bone.envelope_distance))
					node2.setAttribute("envweight", str(bone.envelope_weight))
					node.appendChild(node2)

				root.appendChild(node)

		f = open(self.filepath, 'w')
		f.write(doc.toprettyxml(indent="  "))
		f.close()
		return {'FINISHED'}

def menu_func(self, context):
	self.layout.operator(TowerEngineExporter.bl_idname, text="TowerEngine Mesh (.tem)")

def register():
	bpy.utils.register_module(__name__)
	bpy.types.INFO_MT_file_export.append(menu_func)

def unregister():
	bpy.utils.unregister_module(__name__)
	bpy.types.INFO_MT_file_export.remove(menu_func)

if __name__ == "__main__":
	register()
