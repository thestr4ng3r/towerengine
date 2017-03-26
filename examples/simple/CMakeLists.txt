
set(SOURCE
		${CMAKE_CURRENT_SOURCE_DIR}/simple.cpp)

add_executable(simple ${SOURCE})
target_link_libraries(simple towerengine)
target_link_libraries(simple ${GLFW_LIBRARIES})