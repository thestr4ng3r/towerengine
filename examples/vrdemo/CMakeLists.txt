
set(SOURCE
		${CMAKE_CURRENT_SOURCE_DIR}/vrdemo.cpp)

add_executable(vrdemo ${SOURCE})
target_link_libraries(vrdemo towerengine)
target_link_libraries(vrdemo ${GLFW_LIBRARIES})