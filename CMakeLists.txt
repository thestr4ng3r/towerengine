cmake_minimum_required (VERSION 2.6)

project(TowerEngine)

set(TOWERENGINE_VERSION 1.0.0)

set(INCLUDE_INSTALL_DIR include/towerengine)
set(LIB_INSTALL_DIR lib)



set(CMAKE_BUILD_TYPE Release)

option(TOWERENGINE_STATIC_LIB "Build static instead of shared" OFF)

option(TOWERENGINE_BUILD_DEVIL OFF)
option(TOWERENGINE_BUILD_LIBPNG ON)
option(TOWERENGINE_BUILD_OPENVR OFF)
option(TOWERENGINE_BUILD_OCULUS OFF)


# the following if-clause was copied from BulletPhysics' CMakeLists.txt
IF(MSVC)
	IF (NOT USE_MSVC_INCREMENTAL_LINKING)
		#MESSAGE("MSVC_INCREMENTAL_DEFAULT"+${MSVC_INCREMENTAL_DEFAULT})
		SET( MSVC_INCREMENTAL_YES_FLAG "/INCREMENTAL:NO")

		STRING(REPLACE "INCREMENTAL:YES" "INCREMENTAL:NO" replacementFlags ${CMAKE_EXE_LINKER_FLAGS_DEBUG})
		SET(CMAKE_EXE_LINKER_FLAGS_DEBUG "/INCREMENTAL:NO ${replacementFlags}" )
		MESSAGE("CMAKE_EXE_LINKER_FLAGS_DEBUG=${CMAKE_EXE_LINKER_FLAGS_DEBUG}")

		STRING(REPLACE "INCREMENTAL:YES" "INCREMENTAL:NO" replacementFlags2 ${CMAKE_EXE_LINKER_FLAGS})

		SET(CMAKE_EXE_LINKER_FLAGS ${replacementFlag2})
		STRING(REPLACE "INCREMENTAL:YES" "" replacementFlags3 "${CMAKE_EXTRA_LINK_FLAGS}")

		SET(CMAKE_EXTRA_LINK_FLAGS ${replacementFlag3})


		STRING(REPLACE "INCREMENTAL:YES" "INCREMENTAL:NO" replacementFlags3 "${CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO}")
		SET(CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO ${replacementFlags3})
		SET(CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO "/INCREMENTAL:NO ${replacementFlags3}" )

	ENDIF (NOT USE_MSVC_INCREMENTAL_LINKING)

	IF (NOT USE_MSVC_RUNTIME_LIBRARY_DLL)
		#We statically link to reduce dependancies
		FOREACH(flag_var CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE CMAKE_CXX_FLAGS_MINSIZEREL CMAKE_CXX_FLAGS_RELWITHDEBINFO CMAKE_C_FLAGS CMAKE_C_FLAGS_DEBUG CMAKE_C_FLAGS_RELEASE CMAKE_C_FLAGS_MINSIZEREL CMAKE_C_FLAGS_RELWITHDEBINFO )
			IF(${flag_var} MATCHES "/MD")
				STRING(REGEX REPLACE "/MD" "/MT" ${flag_var} "${${flag_var}}")
			ENDIF(${flag_var} MATCHES "/MD")
			IF(${flag_var} MATCHES "/MDd")
				STRING(REGEX REPLACE "/MDd" "/MTd" ${flag_var} "${${flag_var}}")
			ENDIF(${flag_var} MATCHES "/MDd")
		ENDFOREACH(flag_var)
	ENDIF (NOT USE_MSVC_RUNTIME_LIBRARY_DLL)

	IF (CMAKE_CL_64)
		ADD_DEFINITIONS(-D_WIN64)
	ELSE()
		OPTION(USE_MSVC_SSE "Use MSVC /arch:sse option"	ON)
		IF (USE_MSVC_SSE)
			ADD_DEFINITIONS(/arch:SSE)
		ENDIF()
	ENDIF()
	OPTION(USE_MSVC_FAST_FLOATINGPOINT "Use MSVC /fp:fast option"	ON)
	IF (USE_MSVC_FAST_FLOATINGPOINT)
		ADD_DEFINITIONS(/fp:fast)
	ENDIF()
ENDIF(MSVC)


set (TOWERENGINE_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
file(GLOB TOWERENGINE_INCLUDES "${TOWERENGINE_INCLUDE_DIR}/*.h")

set(TOWERENGINE_SRC 
	src/animation.cpp 
	src/buffers.cpp 
	src/camera.cpp 
	src/engine.cpp 
	src/entity.cpp 
	src/boundingbox.cpp
	src/transform.cpp
	src/geometry_pass_shader.cpp
	src/screen_shader.cpp
	src/directional_lighting_shader.cpp
	src/point_lighting_shader.cpp
	src/ambient_lighting_shader.cpp
	src/ssao_lighting_shader.cpp
	src/point_light.cpp 
	src/directional_light.cpp 
	src/point_light_shadow.cpp
	src/directional_light_shadow.cpp
	src/material.cpp 
	src/mesh.cpp 
	src/meshobject.cpp 
	src/object.cpp
	src/pose.cpp
	src/shader.cpp 
	src/triangle.cpp 
	src/utils.cpp 
	src/vector2.cpp 
	src/vector.cpp 
	src/vertex.cpp 
	src/renderspace.cpp
	src/gbuffer.cpp
	src/world.cpp
	src/heightmap.cpp
	src/terrain.cpp
	src/terrainobject.cpp
	src/skybox.cpp
	src/skybox_shader.cpp
	src/point_shadow_shader.cpp
	src/directional_shadow_shader.cpp
	src/post_process_shader.cpp
	src/material_ibo.cpp
	src/renderer.cpp
	src/ssao_shader.cpp
	src/utilobjects.cpp
	src/color_shader.cpp
	src/scene.cpp
	src/scene_object.cpp
	src/asset.cpp
	src/ssao.cpp
	src/ssao_blur_shader.cpp
	src/fog_shader.cpp
	src/cube_map_reflection.cpp
	src/material_manager.cpp
	src/matrix3.cpp
	src/matrix4.cpp
	src/2d_renderer.cpp
	src/2d_sprite_shader.cpp
	src/shader_source.cpp
	src/particle.cpp
	src/particle_shader.cpp
	src/default_renderer.cpp
	src/vr_renderer.cpp
	src/simple_forward_shader.cpp
	src/simple_forward_material.cpp
	src/refraction_shader.cpp
	src/refraction_material.cpp
	src/lighting_shader.cpp
	src/depth_pass_shader.cpp
	src/cube_map_blur_shader.cpp
	src/no_op_shader.cpp
	include/vr_context.h
	src/vr_context.cpp
	include/vr_context_openvr.h
	include/vr_context_oculus.h
	include/texture.h
	src/texture.cpp
	src/texture_devil.cpp
	src/texture_png.cpp)

if(TOWERENGINE_BUILD_OPENVR)
	list(APPEND TOWERENGINE_SRC src/vr_context_openvr.cpp)
	add_definitions(-DBUILD_OPENVR)
endif()

if(TOWERENGINE_BUILD_OCULUS)
	list(APPEND TOWERENGINE_SRC src/vr_context_oculus.cpp)
	add_definitions(-DBUILD_OCULUS)
endif()




set(RESOURCES_SRC_DIR "${CMAKE_CURRENT_BINARY_DIR}/resources")
set(RESOURCES_HEADER "${RESOURCES_SRC_DIR}/resources.h")
set(RESOURCES_SRC "${RESOURCES_SRC_DIR}/resources.cpp")
set(RESOURCES_RELATIVE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/shader")
file(GLOB RESOURCES_SHADER_FILES
		"${CMAKE_CURRENT_SOURCE_DIR}/shader/*.glsl"
		"${CMAKE_CURRENT_SOURCE_DIR}/shader/*.vert"
		"${CMAKE_CURRENT_SOURCE_DIR}/shader/*.geom"
		"${CMAKE_CURRENT_SOURCE_DIR}/shader/*.frag")



#add_definitions(-g -Wall)

if(TOWERENGINE_STATIC_LIB)
	add_library(towerengine STATIC ${TOWERENGINE_SRC} ${TOWERENGINE_INCLUDES} ${RESOURCES_HEADERS} ${RESOURCES_SRC})
else()
	add_library(towerengine SHARED ${TOWERENGINE_SRC} ${TOWERENGINE_INCLUDES} ${RESOURCES_HEADERS} ${RESOURCES_SRC})
endif()


set_target_properties(towerengine PROPERTIES
			PROJECT_LABEL "TowerEngine")



# resources

set(RESOURCE_GENERATOR_INCLUDE "${CMAKE_CURRENT_SOURCE_DIR}/cmake/ResourceGenerator.cmake")

include(${RESOURCE_GENERATOR_INCLUDE})

file(MAKE_DIRECTORY ${RESOURCES_SRC_DIR})
include_directories(${RESOURCES_SRC_DIR})

set(GENERATE_RESOURCES_ARGS
		RELATIVE_PATH ${RESOURCES_RELATIVE_PATH}
		HEADER_FILE ${RESOURCES_HEADER}
		HEADER_FILE_RELATIVE_PATH ${RESOURCES_SRC_DIR}
		SOURCE_FILE ${RESOURCES_SRC}
		FUNCTION_PREFIX "resources"
		INCLUDE_GUARD "RESOURCES"
		FILES ${RESOURCES_SHADER_FILES})

generate_resources(GENERATE_HEADER ${GENERATE_RESOURCES_ARGS})

configure_file(
		"${CMAKE_CURRENT_SOURCE_DIR}/generate_resources_src.cmake.in"
		"${CMAKE_CURRENT_BINARY_DIR}/generate_resources_src.cmake")

add_custom_command(OUTPUT ${RESOURCES_SRC}
		COMMAND ${CMAKE_COMMAND} -P "${CMAKE_CURRENT_BINARY_DIR}/generate_resources_src.cmake"
		DEPENDS ${RESOURCES_SHADER_FILES})

add_custom_target(resources DEPENDS ${RESOURCES_HEADERS} ${RESOURCES_SRC})
add_dependencies(towerengine resources)






include_directories(${CMAKE_CURRENT_SOURCE_DIR}/rapidxml)

find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIR})
target_link_libraries(towerengine ${OPENGL_LIBRARIES})

find_package(GLEW REQUIRED)
include_directories(${GLEW_INCLUDE_DIR})
target_link_libraries(towerengine ${GLEW_LIBRARY})

find_package(Bullet REQUIRED)
include_directories(${BULLET_INCLUDE_DIRS})
target_link_libraries(towerengine ${BULLET_LIBRARIES})

if(TOWERENGINE_BUILD_DEVIL)
	find_package(DevIL REQUIRED)
	include_directories(${IL_INCLUDE_DIR})
	target_link_libraries(towerengine ${IL_LIBRARIES})
	target_link_libraries(towerengine ${ILU_LIBRARIES})
	add_definitions(-DBUILD_DEVIL)
endif(TOWERENGINE_BUILD_DEVIL)

if(TOWERENGINE_BUILD_LIBPNG)
	find_package(PNG REQUIRED)
	include_directories(${PNG_INCLUDE_DIRS})
	target_link_libraries(towerengine ${PNG_LIBRARIES})
	add_definitions(-DBUILD_LIBPNG)
endif(TOWERENGINE_BUILD_LIBPNG)



install(TARGETS towerengine
		ARCHIVE DESTINATION lib
		LIBRARY DESTINATION ${LIB_INSTALL_DIR}
		INCLUDES DESTINATION ${INCLUDE_INSTALL_DIR})

install(FILES ${TOWERENGINE_INCLUDES} DESTINATION ${INCLUDE_INSTALL_DIR})




# Config
set(TOWERENGINE_INCLUDE_DIR ${CMAKE_INSTALL_PREFIX}/${INCLUDE_INSTALL_DIR})
set(TOWERENGINE_LIBRARY_DIRS ${CMAKE_INSTALL_PREFIX}/${LIB_INSTALL_DIR})
set(TOWERENGINE_LIBRARIES towerengine)

configure_file(towerengine-config.cmake.in
		"${CMAKE_CURRENT_BINARY_DIR}/towerengine-config.cmake"
		@ONLY ESCAPE_QUOTES)

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/towerengine-config.cmake"
		DESTINATION ${LIB_INSTALL_DIR}/cmake/towerengine)



# uninstall target
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY)

add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)



