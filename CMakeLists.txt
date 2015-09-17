cmake_minimum_required (VERSION 2.6)

project(towerengine)
set(towerengine_VERSION 1.0.0)

set(CMAKE_BUILD_TYPE Release)

OPTION(BUILD_STATIC_LIB "Build static instead of shared" OFF)


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



include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
file(GLOB towerengine_INCLUDES "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h")

set(towerengine_SRC 
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
	src/ssao_ambient_lighting_shader.cpp
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
	src/resources.cpp 
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
	src/lighting_shader.cpp)

add_definitions(-g -Wall)



if(BUILD_STATIC_LIB)
	add_library(towerengine STATIC ${towerengine_SRC} ${towerengine_INCLUDES})
else()
	add_library(towerengine SHARED ${towerengine_SRC} ${towerengine_INCLUDES})
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/rapidxml)

find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIR})
target_link_libraries(towerengine ${OPENGL_LIBRARIES})

find_package(GLEW REQUIRED)
include_directories(${GLEW_INCLUDE_DIR})
target_link_libraries(towerengine ${GLEW_LIBRARY})

find_package(DevIL REQUIRED)
include_directories(${IL_INCLUDE_DIR})
target_link_libraries(towerengine ${IL_LIBRARIES})
target_link_libraries(towerengine ${ILU_LIBRARIES})

find_package(Bullet REQUIRED)
include_directories(${BULLET_INCLUDE_DIRS})
target_link_libraries(towerengine ${BULLET_LIBRARIES})


#include(GenerateExportHeader)
#generate_export_header(towerengine)
#set_property(TARGET towerengine PROPERTY VERSION ${towerengine_VERSION})
#set_property(TARGET towerengine PROPERTY SOVERSION 3)
#set_property(TARGET towerengine PROPERTY INTERFACE_towerengine_MAJOR_VERSION 3)
#set_property(TARGET towerengine APPEND PROPERTY COMPATIBLE_INTERFACE_STRING towerengine_MAJOR_VERSION)



install(TARGETS towerengine #EXPORT towerengine_targets
	ARCHIVE DESTINATION lib
	LIBRARY DESTINATION lib
	INCLUDES DESTINATION include/towerengine)

install(FILES ${towerengine_INCLUDES} DESTINATION include/towerengine)


#config
#include(CMakePackageConfigHelpers)
#write_basic_package_version_file(
#  "${CMAKE_CURRENT_BINARY_DIR}/towerengine/towerengine-config-version.cmake"
#  VERSION ${towerengine_VERSION}
#  COMPATIBILITY AnyNewerVersion
#)
#
#
#
#export(TARGETS towerengine FILE "${CMAKE_CURRENT_BINARY_DIR}/towerengine/towerengine-targets.cmake")
#
#configure_file(towerengine-config.cmake
#  "${CMAKE_CURRENT_BINARY_DIR}/towerengine/towerengine-config.cmake"
#  COPYONLY
#)
#
#set(ConfigPackageLocation lib/cmake/towerengine)
#install(EXPORT towerengine_targets
#  FILE
#    towerengine-targets.cmake
#  NAMESPACE
#    Upstream::
#  DESTINATION
#    ${ConfigPackageLocation}
#)
#install(
#  FILES
#    towerengine-config.cmake
#    "${CMAKE_CURRENT_BINARY_DIR}/towerengine/towerengine-config-version.cmake"
#  DESTINATION
#    ${ConfigPackageLocation}
#  COMPONENT
#    Devel
#)

# uninstall target
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY)

add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)
