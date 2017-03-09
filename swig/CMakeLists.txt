find_package(SWIG REQUIRED)
include(${SWIG_USE_FILE})

find_package(PythonLibs 3 REQUIRED)
include_directories(${PYTHON_INCLUDE_DIRS})

set(SWIG_SOURCE towerengine.i)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_CXX_STANDARD 11)

set_source_files_properties(${SWIG_SOURCE} PROPERTIES CPLUSPLUS ON)
set_source_files_properties(${SWIG_SOURCE} PROPERTIES SWIG_FLAGS "-includeall;-Wall")

swig_add_module(towerengine python ${SWIG_SOURCE})
swig_link_libraries(towerengine towerengine)

set_property(TARGET ${SWIG_MODULE_towerengine_REAL_NAME} PROPERTY CXX_STANDARD_REQUIRED 11)
