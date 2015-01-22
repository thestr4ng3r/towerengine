cmake_minimum_required (VERSION 2.6)

project(towerengine)

set(CMAKE_BUILD_TYPE Release)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
FILE(GLOB towerengine_INCLUDES "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h")

set(towerengine_SRC 
	src/animation.cpp 
	src/buffers.cpp 
	src/camera.cpp 
	src/engine.cpp 
	src/entity.cpp 
	src/boundingbox.cpp
	src/transform.cpp
	src/face_shader.cpp 
	src/geometry_pass_shader.cpp
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
	src/mesh_material.cpp
	src/renderer.cpp
	src/ssao_shader.cpp
	src/utilobjects.cpp
	src/color_shader.cpp
	src/scene.cpp
	src/scene_object.cpp
	src/asset.cpp)

add_definitions(-g -Wall)

add_library(towerengine SHARED ${towerengine_SRC})

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/rapidxml)

find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIR})
target_link_libraries(towerengine ${OPENGL_LIBRARIES})

find_package(DevIL REQUIRED)
include_directories(${IL_INCLUDE_DIR})
target_link_libraries(towerengine ${IL_LIBRARIES})
target_link_libraries(towerengine ${ILU_LIBRARIES})

find_package(Bullet REQUIRED)
include_directories(${BULLET_INCLUDE_DIRS})
target_link_libraries(towerengine ${BULLET_LIBRARIES})

install(TARGETS towerengine DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)

INSTALL(FILES ${towerengine_INCLUDES} DESTINATION ${CMAKE_INSTALL_PREFIX}/include/towerengine)

# uninstall target
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY)

add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)
