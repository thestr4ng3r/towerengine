
set(SOURCE
		${CMAKE_CURRENT_SOURCE_DIR}/vrdemo.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/vrcontroller.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/vrcontroller.h
		${CMAKE_CURRENT_SOURCE_DIR}/vrplayer.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/vrplayer.h
		${CMAKE_CURRENT_SOURCE_DIR}/common.h)

add_executable(vrdemo ${SOURCE})
target_link_libraries(vrdemo towerengine)
target_link_libraries(vrdemo ${GLFW_LIBRARIES})