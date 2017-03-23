
set(SOURCE
		${CMAKE_CURRENT_SOURCE_DIR}/simple_forward.cpp)

add_executable(simple_forward ${SOURCE})
target_link_libraries(simple_forward towerengine)
target_link_libraries(simple_forward ${GLFW_LIBRARIES})