#Loading sources for project
file(GLOB_RECURSE SOURCE
        ${CMAKE_CURRENT_SOURCE_DIR}/ref/*.inl*
        ${CMAKE_CURRENT_SOURCE_DIR}/ref/*.h*
        ${CMAKE_CURRENT_SOURCE_DIR}/ref/*.c*
        ${CMAKE_CURRENT_SOURCE_DIR}/ref/*.cc*
        ${CMAKE_CURRENT_SOURCE_DIR}/ref/*.hpp*
        ${CMAKE_CURRENT_SOURCE_DIR}/ref/*.cpp*
        ${CMAKE_CURRENT_SOURCE_DIR}/ref/*.hxx*
        ${CMAKE_CURRENT_SOURCE_DIR}/ref/*.cxx*
        ${CMAKE_CURRENT_SOURCE_DIR}/src/*.inl*
        ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h*
        ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c*
        ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cc*
        ${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp*
        ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp*
        ${CMAKE_CURRENT_SOURCE_DIR}/src/*.hxx*
        ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cxx*)

#Resolving default source groups
foreach(ITEM IN ITEMS ${SOURCE})
    get_filename_component(ITEM_PATH "${ITEM}" PATH)
    string(REPLACE "${PROJECT_SOURCE_DIR}" "" ITEM_GROUP "${ITEM_PATH}")
    string(REPLACE "/" "\\" ITEM_GROUP "${ITEM_GROUP}")
    source_group("${ITEM_GROUP}" FILES "${ITEM}")
endforeach()