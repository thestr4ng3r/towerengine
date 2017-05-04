
set(SOURCE
		${CMAKE_CURRENT_SOURCE_DIR}/cube.cpp)

add_executable(cube ${SOURCE})
target_link_libraries(cube towerengine)
target_link_libraries(cube ${GLFW_LIBRARIES})