

find_package(GLFW REQUIRED)
include_directories(${GLFW_INCLUDE_DIR})

add_subdirectory(simple)
add_subdirectory(simple_forward)
add_subdirectory(vrdemo)