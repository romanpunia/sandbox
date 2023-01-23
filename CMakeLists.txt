cmake_minimum_required(VERSION 3.6)

#Setup toolchain file if needed
set(TH_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../tomahawk CACHE STRING "Tomahawk directory")
set(TH_USE_POSTGRESQL OFF CACHE BOOL "PostgreSQL is not used")
set(TH_USE_MONGOC OFF CACHE BOOL "MongoDB is not used")
set(TH_WITH_SIMD ON CACHE BOOL "SIMD is preferred for games")
include(${TH_DIRECTORY}/lib/toolchain.cmake)

#Project's metadata
project(sandbox)
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)
set(CMAKE_CONFIGURATION_TYPES "Debug;Release;RelWithDebInfo")

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
		CXX_STANDARD 17
		VERSION ${PROJECT_VERSION}
		SOVERSION ${PROJECT_VERSION})
target_compile_definitions(sandbox PRIVATE -DTH_EXPORT)

#Resolve needed options for different compilers and refs linkage
if (NOT MSVC)
    set(CMAKE_CXX_FLAGS_DEBUG "-g")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")
    if (NOT (WIN32 AND MINGW))
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wformat=0 -fexceptions")
		set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-implicit-function-declaration -fexceptions")
		if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
			if (${CMAKE_SYSTEM_PROCESSOR} MATCHES "arm")
				set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0")
				set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0")
			else()
				set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native")
				set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -march=native")
			endif()
		endif()
	else()
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mwindows -D_WIN32_WINNT=0x0600")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mwindows -D_WIN32_WINNT=0x0600")
    endif()
else()
    set(CMAKE_EXE_LINKER_FLAGS "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
    set(CMAKE_CXX_FLAGS_DEBUG "/MDd /Zi /Ob0 /Od /MP")
    set(CMAKE_CXX_FLAGS_RELEASE "/MD /O2 /Ob2 /DNDEBUG /MP")
    set(CMAKE_C_FLAGS_DEBUG "/MDd /Zi /Ob0 /Od /MP")
    set(CMAKE_C_FLAGS_RELEASE "/MD /O2 /Ob2 /DNDEBUG /MP")
endif()

#Add Tomahawk as dependency
add_subdirectory(${TH_DIRECTORY} tomahawk)
link_directories(${TH_DIRECTORY})
target_include_directories(sandbox PRIVATE ${TH_DIRECTORY})
target_link_libraries(sandbox PRIVATE tomahawk)

#Project's source resolvers
include(src/utils/CMakeLists.txt)
include(src/CMakeLists.txt)