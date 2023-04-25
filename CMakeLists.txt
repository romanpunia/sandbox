cmake_minimum_required(VERSION 3.6)

#Setup toolchain file if needed
set(ED_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../edge CACHE STRING "Edge directory")
set(ED_USE_POSTGRESQL OFF CACHE BOOL "PostgreSQL is not used")
set(ED_USE_MONGOC OFF CACHE BOOL "MongoDB is not used")
set(ED_USE_SIMD ON CACHE BOOL "SIMD is preferred for games")
set(ED_USE_FAST_MEMORY ON CACHE BOOL "Fast memory is preferred for games")
include(${ED_DIRECTORY}/lib/toolchain.cmake)

#Project's metadata
project(sandbox)
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)

#Build directory if not defined
if (NOT DEFINED CMAKE_RUNTIME_OUTPUT_DIRECTORY)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/bin)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/bin)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${CMAKE_SOURCE_DIR}/bin)
endif()

#Query all sources
file(GLOB_RECURSE SOURCE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/*.inl*
        ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h*
        ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c*
        ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cc*
        ${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp*
        ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp*
        ${CMAKE_CURRENT_SOURCE_DIR}/src/*.hxx*
        ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cxx*)

#Resolving source groups
foreach(ITEM IN ITEMS ${SOURCE})
    get_filename_component(ITEM_PATH "${ITEM}" PATH)
    string(REPLACE "${PROJECT_SOURCE_DIR}" "" ITEM_GROUP "${ITEM_PATH}")
    string(REPLACE "/" "\\" ITEM_GROUP "${ITEM_GROUP}")
    source_group("${ITEM_GROUP}" FILES "${ITEM}")
endforeach()

#Project's compile options
add_executable(sandbox ${SOURCE})
set_target_properties(sandbox PROPERTIES
		OUTPUT_NAME "sandbox"
		CXX_STANDARD ${ED_CXX}
        CXX_STANDARD_REQUIRED ON
		CXX_EXTENSIONS OFF
		VERSION ${PROJECT_VERSION}
		SOVERSION ${PROJECT_VERSION})
target_compile_definitions(sandbox PRIVATE
    -DED_EXPORT
    -DNOMINMAX)

#Inherit compiler options
include(${ED_DIRECTORY}/lib/compiler.cmake)
if (MSVC)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS")
endif()

#Add Edge as dependency
add_subdirectory(${ED_DIRECTORY} edge)
link_directories(${ED_DIRECTORY})
target_include_directories(sandbox PRIVATE ${ED_DIRECTORY})
target_link_libraries(sandbox PRIVATE edge)

#Project's source resolvers
include(src/utils/CMakeLists.txt)
include(src/CMakeLists.txt)