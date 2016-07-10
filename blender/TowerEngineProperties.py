
import bpy


class TowerEngineAttribute(bpy.types.PropertyGroup):
	""" Group of properties representing an item in the list """

	name = bpy.props.StringProperty(
		name="Name",
		description="The name of the attribute",
		default="attribute")

	value = bpy.props.StringProperty(
		name="Value",
		description="The value of the attribute",
		default="")


class TowerEngineObjectPropertyGroup(bpy.types.PropertyGroup):
	tag = bpy.props.StringProperty(name="Tag")
	
	attributes = bpy.props.CollectionProperty(name="attributes", type = TowerEngineAttribute)
	attribute_index = bpy.props.IntProperty(name="attribute_index", default = 0)


class TowerEngineAttributeList(bpy.types.UIList):
	def draw_item(self, context, layout, data, item, icon, active_data, active_propname, index):

		# We could write some code to decide which icon to use here...
		custom_icon = 'OBJECT_DATAMODE'

		# Make sure your code supports all 3 layout types
		if self.layout_type in {'DEFAULT', 'COMPACT'}:
			layout.prop(item, 'name', icon_only = True);
			layout.prop(item, 'value', icon_only = True);

		elif self.layout_type in {'GRID'}:
			layout.alignment = 'CENTER'
			layout.label("", icon = custom_icon)


class TowerEngineAttributeListAddItem(bpy.types.Operator):
	""" Add a new item to the list """

	bl_idname = "towerengine.add_attr"
	bl_label = "Add attribute"

	def execute(self, context):
		context.object.towerengine.attributes.add()

		return{'FINISHED'}


class TowerEngineAddAttributeOperator(bpy.types.Operator):
	""" Delete the selected item from the list """
	
	bl_idname = "towerengine.delete_attr"
	bl_label = "Delete attribute"
	
	@classmethod
	def poll(self, context):
		""" Enable if there's something in the list """
		return len(context.object.towerengine.attributes) > 0
	
	def execute(self, context):
		alist = context.object.towerengine.attributes
		index = context.object.towerengine.attribute_index
		
		alist.remove(index)
		
		if index > 0:
			index = index - 1
		
		return{'FINISHED'}


	

class TowerEngineDeleteAttributeOperator(bpy.types.Panel):
	bl_idname = "OBJECT_PT_towerengine"
	bl_label = "TowerEngine"
	bl_space_type = "PROPERTIES"
	bl_region_type = "WINDOW"
	bl_context = "object"
	
	
	def draw(self, context):
		obj = context.object
		if not obj:
			return
		layout = self.layout
		layout.prop(obj.towerengine, 'tag')
		
		layout.template_list("TowerEngineAttributeList", "TowerEngine Attribute List", obj.towerengine, "attributes", obj.towerengine, "attribute_index")
		
		row = layout.row()
		row.operator('towerengine.add_attr')
		row.operator('towerengine.delete_attr')





MaterialTypeItems = [
    ("DEFAULT", "Default", "", 1),
    ("SIMPLE_FORWARD", "SimpleForward", "", 2),
    ("REFRACTION", "Refraction", "", 3),
    ]

MaterialBlendModeItems = [
    ("ALPHA", "Alpha", "", 1),
    ("ADD", "Add", "", 2),
    ("MULTIPLY", "Multiply", "", 3),
    ]

class TowerEngineMaterialPropertyGroup(bpy.types.PropertyGroup):
	mat_type = bpy.props.EnumProperty(name="Type", items=MaterialTypeItems, default="DEFAULT")
	blend_mode = bpy.props.EnumProperty(name="BlendMode", items=MaterialBlendModeItems, default="ALPHA")
	

class TowerEngineMaterialPanel(bpy.types.Panel):
	bl_idname = "MATERIAL_PT_towerengine"
	bl_label = "TowerEngine"
	bl_space_type = "PROPERTIES"
	bl_region_type = "WINDOW"
	bl_context = "material"
	
	
	def draw(self, context):
		mat = context.material
		if not mat:
			return
		layout = self.layout
		layout.prop(mat.towerengine, 'mat_type')
		
		row = layout.row()
		row.active = mat.towerengine.mat_type == "SIMPLE_FORWARD"
		row.prop(mat.towerengine, 'blend_mode')
		
		

class TowerEngineMeshPropertyGroup(bpy.types.PropertyGroup):
	vertices_only = bpy.props.BoolProperty(name="Vertices only", default=False)

class TowerEngineMeshPanel(bpy.types.Panel):
	bl_idname = "DATA_PT_towerengine"
	bl_label = "TowerEngine"
	bl_space_type = "PROPERTIES"
	bl_region_type = "WINDOW"
	bl_context = "data"

	def draw(self, context):
		mesh = context.mesh

		if not mesh:
			return

		layout = self.layout
		layout.prop(mesh.towerengine, 'vertices_only')
