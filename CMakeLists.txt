cmake_minimum_required(VERSION 3.1)
cmake_policy(SET CMP0054 NEW)

#Project's package support for non-UNIX platforms
include(opt/package/CMakeLists.txt)

#Project's metadata
project(sandbox)
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)
set(CMAKE_CONFIGURATION_TYPES "Debug;Release")

#Build directory if not defined
if (NOT DEFINED CMAKE_RUNTIME_OUTPUT_DIRECTORY)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/bin)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/bin)
endif()

#Project's source resolver
include(opt/source/CMakeLists.txt)

#Project's compile options
add_executable(sandbox ${SOURCE})
set_target_properties(sandbox PROPERTIES
		OUTPUT_NAME "sandboxd"
		CXX_STANDARD_REQUIRED 14
		VERSION ${PROJECT_VERSION}
		SOVERSION ${PROJECT_VERSION})

#Project's compiler support for non-UNIX platforms
include(opt/compiler/CMakeLists.txt)

#Project's dependency resolvers
include(ref/tomahawk/CMakeLists.txt)
include(ref/gizmo/CMakeLists.txt)