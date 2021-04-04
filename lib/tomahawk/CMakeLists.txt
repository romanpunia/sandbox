#Resolve headers and linkage
add_subdirectory(${TH_DIRECTORY} tomahawk)
link_directories(${TH_DIRECTORY})
target_include_directories(sandbox PRIVATE ${TH_DIRECTORY})
target_link_libraries(sandbox PRIVATE tomahawk)