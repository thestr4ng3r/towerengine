
if(NOT TOWERENGINE_SWIG_PYTHON)
	message(FATAL_ERROR "SWIG Python bindings are required for the Blender Addon. Run cmake with -DTOWERENGINE_SWIG_PYTHON=ON.")
endif()

include(../cmake/Utils.cmake)

set(BLENDER_ADDON_SOURCE
		__init__.py
		export_material.py
		export_mesh.py
		mesh_converter.py
		mesh_exporter.py
		props.py
		renderer.py
		scene_exporter.py
		ui.py
		utils.py
		viewport_render.py)

set(BINDINGS_FILES
		towerengine.py
		$<TARGET_FILE_NAME:${TOWERENGINE_SWIG_TARGET}>)


prefix_entries(BLENDER_ADDON_SOURCE_ABS "${CMAKE_CURRENT_SOURCE_DIR}/" "${BLENDER_ADDON_SOURCE}")
prefix_entries(BLENDER_ADDON_SOURCE_ABS_BINARY "${CMAKE_CURRENT_BINARY_DIR}/" "${BLENDER_ADDON_SOURCE}")

prefix_entries(BINDINGS_FILES_ABS "${TOWERENGINE_SWIG_BINARY_DIR}/" "${BINDINGS_FILES}")
prefix_entries(BINDINGS_FILES_ABS_BINARY "${CMAKE_CURRENT_BINARY_DIR}/" "${BINDINGS_FILES}")

add_custom_target(blender-addon
		${CMAKE_COMMAND} -E copy ${BLENDER_ADDON_SOURCE_ABS} ${BINDINGS_FILES_ABS} ${CMAKE_CURRENT_BINARY_DIR}
		COMMAND ${CMAKE_COMMAND} -E
			tar -cf "${CMAKE_CURRENT_BINARY_DIR}/towerengine-blender.zip" --format=zip
			${BLENDER_ADDON_SOURCE_ABS_BINARY} ${BINDINGS_FILES_ABS_BINARY}
		DEPENDS ${TOWERENGINE_SWIG_TARGET}
		SOURCES ${BLENDER_ADDON_SOURCE})

