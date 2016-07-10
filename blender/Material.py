
import os
import bpy
import shutil
import base64
from xml.dom.minidom import Document

class ExportMaterial():
    
    @staticmethod
    def create_default_material_node(doc, material, path, subpath, pack_textures = False):
        diffuse_tex = specular_tex = normal_tex = bump_tex = self_illumination_tex = None
        bump_factor = 0.0
        
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
            if tex_slot.use_map_displacement:
                bump_tex = texture
                bump_factor = tex_slot.displacement_factor
            if tex_slot.use_map_emit:
                self_illumination_tex = texture
                
        if pack_textures == False and (diffuse_tex != None or specular_tex != None or normal_tex != None or bump_tex != None or self_illumination_tex != None):
            tex_path = os.path.join(path, subpath, material.name)
            try:
                os.makedirs(tex_path)
            except OSError as e:
                print(str(e))
                
        
        node = doc.createElement("material")
        node.setAttribute("name", material.name)
        node.setAttribute("type", "default")
        
        #if material.diffuse_intensity > 0.0:
        node2 = doc.createElement("diffuse")
        if diffuse_tex != None:
            file_extension = diffuse_tex.image.filepath.split(".")[-1]
            if pack_textures == False:
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



        node2 = doc.createElement("specular")
        if specular_tex != None:
            file_extension = specular_tex.image.filepath.split(".")[-1]
            if pack_textures == False:
                tex_file = os.path.join(tex_path, "specular." + file_extension)
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




        if material.emit > 0.0:
            node2 = doc.createElement("self_illumination")
            
            if self_illumination_tex != None:
                file_extension = self_illumination_tex.image.filepath.split(".")[-1]
                if pack_textures == False:
                    tex_file = os.path.join(tex_path, "self_illum." + file_extension)
                    try:
                        shutil.copy(bpy.path.abspath(self_illumination_tex.image.filepath), tex_file)
                    except OSError as e:
                        print(str(e))
                    except IOError as e:
                        print(str(e))
                    node2.setAttribute("file", os.path.relpath(tex_file, path))
                else:
                    with open(bpy.path.abspath(self_illumination_tex.image.filepath), "rb") as image_file:
                        image_data = image_file.read()
                        image_base64 = base64.b64encode(image_data)
                        image_text = doc.createTextNode(image_base64.decode("utf-8"))
                        node2.appendChild(image_text)
                    node2.setAttribute("image-extension", file_extension)
                
            node2.setAttribute("r", str(material.diffuse_color.r * material.emit))
            node2.setAttribute("g", str(material.diffuse_color.g * material.emit))
            node2.setAttribute("b", str(material.diffuse_color.b * material.emit))
            
            node.appendChild(node2)
            

        if normal_tex != None:
            file_extension = normal_tex.image.filepath.split(".")[-1]
            node2 = doc.createElement("normal")
            if pack_textures == False:
                tex_file = os.path.join(tex_path, "normal." + file_extension)
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
            
        if bump_tex != None:
            file_extension = bump_tex.image.filepath.split(".")[-1]
            node2 = doc.createElement("bump")
            if pack_textures == False:
                tex_file = os.path.join(tex_path, "bump." + file_extension)
                try:
                    shutil.copy(bpy.path.abspath(bump_tex.image.filepath), tex_file)
                except OSError as e:
                    print(str(e))
                except IOError as e:
                    print(str(e))
                node2.setAttribute("file", os.path.relpath(tex_file, path))
            else:
                with open(bpy.path.abspath(bump_tex.image.filepath), "rb") as image_file:
                    image_data = image_file.read()
                    image_base64 = base64.b64encode(image_data)
                    image_text = doc.createTextNode(image_base64.decode("utf-8"))
                    node2.appendChild(image_text)
                node2.setAttribute("image-extension", file_extension)
                
            node2.setAttribute("depth", str(bump_factor))
            node.appendChild(node2)
            
        if material.raytrace_mirror.use:
            node2 = doc.createElement("cube_map_reflection")
            node2.setAttribute("r", str(material.mirror_color.r * material.raytrace_mirror.reflect_factor))
            node2.setAttribute("g", str(material.mirror_color.g * material.raytrace_mirror.reflect_factor))
            node2.setAttribute("b", str(material.mirror_color.b * material.raytrace_mirror.reflect_factor))
            node.appendChild(node2)
            
        if material.use_cast_shadows == False:
            node2 = doc.createElement("shadow")
            node2.setAttribute("cast", "0")
            node.appendChild(node2)
            
        return node
    
    
    @staticmethod
    def create_simple_forward_material_node(doc, material, path, subpath, pack_textures = False):
        tex = None
        
        for tex_slot in material.texture_slots:
            if tex_slot == None:
                continue
            texture = tex_slot.texture
            if texture.type != 'IMAGE':
                continue
            
            if tex_slot.use_map_color_diffuse:
                tex = texture
                
        if pack_textures == False and tex != None:
            tex_path = os.path.join(path, subpath, material.name)
            try:
                os.makedirs(tex_path)
            except OSError as e:
                print(str(e))
                
        
        node = doc.createElement("material")
        node.setAttribute("name", material.name)
        node.setAttribute("type", "simple_forward")
        
        blend_mode = "alpha"
        if material.towerengine.blend_mode == "ALPHA":
            blend_mode = "alpha"
        elif material.towerengine.blend_mode == "ADD":
            blend_mode = "add"
        elif material.towerengine.blend_mode == "MULTIPLY":
            blend_mode = "multiply"
        node.setAttribute("blend_mode", blend_mode)
        
        node2 = doc.createElement("color")
        node2.setAttribute("r", str(material.diffuse_color.r * material.diffuse_intensity))
        node2.setAttribute("g", str(material.diffuse_color.g * material.diffuse_intensity))
        node2.setAttribute("b", str(material.diffuse_color.b * material.diffuse_intensity))
        
        a = 1.0
        if material.use_transparency:
            a = material.alpha
        node2.setAttribute("a", str(a))
        
        node.appendChild(node2);
        
        
        if tex != None:
            node2 = doc.createElement("tex")
            file_extension = tex.image.filepath.split(".")[-1]
            if pack_textures == False:
                tex_file = os.path.join(tex_path, "tex." + file_extension)
                try:
                    shutil.copy(bpy.path.abspath(tex.image.filepath), tex_file)
                except OSError as e:
                    print(str(e))
                except IOError as e:
                    print(str(e))
                node2.setAttribute("file", os.path.relpath(tex_file, path))
            else:
                with open(bpy.path.abspath(tex.image.filepath), "rb") as image_file:
                    image_data = image_file.read()
                    image_base64 = base64.b64encode(image_data)
                    image_text = doc.createTextNode(image_base64.decode("utf-8"))
                    node2.appendChild(image_text)
                node2.setAttribute("image-extension", file_extension)
                
            node.appendChild(node2);
            
        return node
    
    
    
    
    @staticmethod
    def create_refraction_material_node(doc, material, path, subpath, pack_textures = False):
        tex = None
        normal_tex = None
        
        for tex_slot in material.texture_slots:
            if tex_slot == None:
                continue
            texture = tex_slot.texture
            if texture.type != 'IMAGE':
                continue
            
            if tex_slot.use_map_color_diffuse:
                tex = texture
            if tex_slot.use_map_normal:
                normal_tex = texture
                
        if pack_textures == False and (tex != None or normal_tex != None):
            tex_path = os.path.join(path, subpath, material.name)
            try:
                os.makedirs(tex_path)
            except OSError as e:
                print(str(e))
                
        
        node = doc.createElement("material")
        node.setAttribute("name", material.name)
        node.setAttribute("type", "refraction")
        
        node2 = doc.createElement("color")
        node2.setAttribute("r", str(material.diffuse_color.r * material.diffuse_intensity))
        node2.setAttribute("g", str(material.diffuse_color.g * material.diffuse_intensity))
        node2.setAttribute("b", str(material.diffuse_color.b * material.diffuse_intensity))    
        
        if tex != None:
            file_extension = tex.image.filepath.split(".")[-1]
            if pack_textures == False:
                tex_file = os.path.join(tex_path, "color." + file_extension)
                try:
                    shutil.copy(bpy.path.abspath(tex.image.filepath), tex_file)
                except OSError as e:
                    print(str(e))
                except IOError as e:
                    print(str(e))
                node2.setAttribute("file", os.path.relpath(tex_file, path))
            else:
                with open(bpy.path.abspath(tex.image.filepath), "rb") as image_file:
                    image_data = image_file.read()
                    image_base64 = base64.b64encode(image_data)
                    image_text = doc.createTextNode(image_base64.decode("utf-8"))
                    node2.appendChild(image_text)
                node2.setAttribute("image-extension", file_extension)
                
            node.appendChild(node2);    
            
        node.appendChild(node2);
        
        node2 = doc.createElement("edge_color")
        node2.setAttribute("r", str(material.specular_color.r))
        node2.setAttribute("g", str(material.specular_color.g))
        node2.setAttribute("b", str(material.specular_color.b))
        node2.setAttribute("a", str(material.specular_intensity))
        node.appendChild(node2)
            
            
            
        if normal_tex != None:
            node2 = doc.createElement("normal_tex")
            file_extension = normal_tex.image.filepath.split(".")[-1]
            if pack_textures == False:
                tex_file = os.path.join(tex_path, "normal." + file_extension)
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
                
            node.appendChild(node2);
            
        return node
    
    
    
    @staticmethod
    def create_node(doc, material, path, subpath, pack_textures = False):
        if material.towerengine.mat_type == "DEFAULT":
            return ExportMaterial.create_default_material_node(doc, material, path, subpath, pack_textures)
        elif material.towerengine.mat_type == "SIMPLE_FORWARD":
            return ExportMaterial.create_simple_forward_material_node(doc, material, path, subpath, pack_textures)
        elif material.towerengine.mat_type == "REFRACTION":
            return ExportMaterial.create_refraction_material_node(doc, material, path, subpath, pack_textures)
        else:
            return ExportMaterial.create_default_material_node(doc, material, path, subpath, pack_textures)
            
            
