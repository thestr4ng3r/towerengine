cmake_minimum_required (VERSION 2.6)

project(towerengine)

set(CMAKE_BUILD_TYPE Release)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/oglft)
FILE(GLOB towerengine_INCLUDES "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h")
FILE(GLOB oglft_INCLUDES "${CMAKE_CURRENT_SOURCE_DIR}/oglft/*.h")

set(towerengine_SRC 
	src/animation.cpp 
	src/buffers.cpp 
	src/camera.cpp 
	src/engine.cpp 
	src/entity.cpp 
	src/face_shader.cpp 
	src/default_face_shader.cpp
	src/light.cpp 
	src/shadow.cpp
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
	src/world.cpp
	src/heightmap.cpp
	src/terrain.cpp
	src/terrainobject.cpp
	src/skybox.cpp
	src/skybox_shader.cpp
	src/point_shadow_shader.cpp
	src/directional_shadow_shader.cpp
	oglft/OGLFT.cpp)

add_definitions(-g -Wall)

add_library(towerengine SHARED ${towerengine_SRC})

find_package(LibXml2 REQUIRED)
include_directories(${LIBXML2_INCLUDE_DIR})
target_link_libraries(towerengine ${LIBXML2_LIBRARIES})

find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIR})
target_link_libraries(towerengine ${OPENGL_LIBRARIES})

find_package(DevIL REQUIRED)
include_directories(${IL_INCLUDE_DIR})
target_link_libraries(towerengine ${IL_LIBRARIES})
target_link_libraries(towerengine ${ILU_LIBRARIES})

find_package(Freetype REQUIRED)
include_directories(${FREETYPE_INCLUDE_DIRS})
target_link_libraries(towerengine ${FREETYPE_LIBRARIES})


install(TARGETS towerengine DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)

INSTALL(FILES ${towerengine_INCLUDES} DESTINATION ${CMAKE_INSTALL_PREFIX}/include/towerengine)
INSTALL(FILES ${oglft_INCLUDES} DESTINATION ${CMAKE_INSTALL_PREFIX}/include/towerengine)

# uninstall target
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY)

add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)
