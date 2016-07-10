

from xml.dom.minidom import Document

def bound_box_minmax(bound_box):
	min_v = [bound_box[0][0], bound_box[0][1], bound_box[0][2]]
	max_v = [bound_box[0][0], bound_box[0][1], bound_box[0][2]]

	for v in bound_box[:]:
		for i in range(0, 3):
			min_v[i] = min([min_v[i], v[i]])
			max_v[i] = max([max_v[i], v[i]])

	return [min_v, max_v]



def xml_rigid_body_node(xml_doc, obj):
	rigid_body = obj.rigid_body

	rigid_body_node = xml_doc.createElement("rigid_body")

	collision_shape_node = xml_doc.createElement("collision_shape")
	if rigid_body.collision_shape == "MESH":
		collision_shape_node.setAttribute("type", "mesh")
	elif rigid_body.collision_shape == "CONVEX_HULL":
		collision_shape_node.setAttribute("type", "convex")
	elif rigid_body.collision_shape == "BOX":
		collision_shape_node.setAttribute("type", "box")

		bounds = bound_box_minmax(obj.bound_box)
		half_extents_node = xml_doc.createElement("half_extents")
		half_extents_node.setAttribute("x", str((bounds[1][0] - bounds[0][0]) * 0.5))
		half_extents_node.setAttribute("y", str((bounds[1][2] - bounds[0][2]) * 0.5))
		half_extents_node.setAttribute("z", str((bounds[1][1] - bounds[0][1]) * 0.5))
		collision_shape_node.appendChild(half_extents_node)

	else:
		print("Collision Shape type " + rigid_body.collision_shape + " is currently not supported. Using mesh instead.")
		collision_shape_node.setAttribute("type", "mesh")

	rigid_body_node.appendChild(collision_shape_node)

	mass = rigid_body.mass
	if rigid_body.type == "PASSIVE":
		mass = 0.0
	rigid_body_node.setAttribute("mass", str(mass))

	return rigid_body_node