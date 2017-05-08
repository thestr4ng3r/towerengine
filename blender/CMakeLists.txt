
if(NOT TOWERENGINE_SWIG_PYTHON)
	message(FATAL_ERROR "SWIG Python bindings are required for the Blender Addon. Run cmake with -DTOWERENGINE_SWIG_PYTHON=ON.")
endif()

include(../cmake/Utils.cmake)

set(TOWERENGINE_BLENDER_ADDON_PACK_ADDITIONAL_FILES "" CACHE FILEPATH "Additional files (e.g. DLLs) to pack into Blender Addon")

set(BLENDER_ADDON_SOURCE
		__init__.py
		export_material.py
		export_mesh.py
		material_converter.py
		mesh_converter.py
		mesh_exporter.py
		props.py
		renderer.py
		scene_exporter.py
		ui.py
		utils.py
		viewport_render.py)

set(BINDINGS_FILES_ABS
		"${TOWERENGINE_SWIG_BINARY_DIR}/towerengine.py"
		"$<TARGET_FILE:${TOWERENGINE_SWIG_TARGET}>")

prefix_entries(BLENDER_ADDON_SOURCE_ABS "${CMAKE_CURRENT_SOURCE_DIR}/" "${BLENDER_ADDON_SOURCE}")

file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/towerengine")

add_custom_target(blender-addon
		${CMAKE_COMMAND} -E copy ${BLENDER_ADDON_SOURCE_ABS} ${BINDINGS_FILES_ABS} ${TOWERENGINE_BLENDER_ADDON_PACK_ADDITIONAL_FILES} "${CMAKE_CURRENT_BINARY_DIR}/towerengine"
		COMMAND ${CMAKE_COMMAND} -E
			tar -cf "${CMAKE_CURRENT_BINARY_DIR}/towerengine-blender.zip" --format=zip
			"${CMAKE_CURRENT_BINARY_DIR}/towerengine"
		DEPENDS ${TOWERENGINE_SWIG_TARGET}
		COMMENT "Packaging Blender Addon"
		SOURCES ${BLENDER_ADDON_SOURCE})

