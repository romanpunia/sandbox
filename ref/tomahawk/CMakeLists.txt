#Resolve headers and linkage
if (NOT DEFINED THAWK_DIRECTORY)
    set(THAWK_DIRECTORY ${PROJECT_SOURCE_DIR}/../tomahawk CACHE STRING "Tomahawk's directory")
endif()

add_subdirectory(${THAWK_DIRECTORY} tomahawk)
target_include_directories(sandbox PRIVATE ${THAWK_DIRECTORY})
target_link_directories(sandbox PRIVATE ${THAWK_DIRECTORY})
target_link_libraries(sandbox PRIVATE tomahawk)