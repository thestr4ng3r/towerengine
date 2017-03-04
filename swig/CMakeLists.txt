find_package(SWIG REQUIRED)
include(${SWIG_USE_FILE})

find_package(PythonLibs 3 REQUIRED)
include_directories(${PYTHON_INCLUDE_DIRS})

set(SWIG_SOURCE towerengine.i)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set_property(SOURCE ${SWIG_SOURCE} PROPERTY CPLUSPLUS ON)
set_property(SOURCE ${SWIG_SOURCE} PROPERTY SWIG_FLAGS "-includeall")

swig_add_module(towerengine_python python ${SWIG_SOURCE})
swig_link_libraries(towerengine_python towerengine)
