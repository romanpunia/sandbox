#Resolve needed options for different compilers and refs linkage
if (NOT MSVC)
    set(CMAKE_CXX_FLAGS_DEBUG "-g")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")
    if (WIN32 AND MINGW)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mwindows")
    elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        if (${CMAKE_SYSTEM_PROCESSOR} MATCHES "arm")
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wformat=0 -lstdc++ -O0 -fexceptions")
            set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-implicit-function-declaration -lz -O0 -fexceptions")
        else()
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wformat=0 -lstdc++ -msse2 -fexceptions")
            set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-implicit-function-declaration -lz -msse2 -fexceptions")
        endif()
    endif()
else()
    set(CMAKE_EXE_LINKER_FLAGS "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
    set(CMAKE_CXX_FLAGS_DEBUG "/MDd /Zi /Ob0 /Od")
    set(CMAKE_CXX_FLAGS_RELEASE "/MD /O2 /Ob2 /DNDEBUG")
    target_compile_definitions(sandbox PRIVATE
            -D_CRT_SECURE_NO_WARNINGS
            -D_SCL_SECURE_NO_WARNINGS)
    target_compile_options(sandbox PRIVATE
            /wd4305
            /wd4244)
endif()