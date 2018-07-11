cmake_minimum_required(VERSION 3.1)

project(fastxor)

#
# common directories
#

set(fastxor_ROOT        ${PROJECT_SOURCE_DIR})
set(fastxor_EXT_ROOT    ${fastxor_ROOT}/ext)


if (WIN32)
set(extra_INCLUDE    ${fastxor_ROOT}/ext)
else()
set(extra_INCLUDE    ${fastxor_ROOT}/ext)
endif()

#
# common compiler settings
#

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")

    #add_compile_options(-std=c++11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

    # enable all warnings and treat warnings as errors
    add_compile_options(-Wall -Werror -pedantic -Wno-unused-function)

    # always enable debugging symbols
    add_compile_options(-g)

elseif (MSVC)

    # generic preprocessor definitions
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)

    # config-specific preprocessor definitions
    set_property(DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS
        $<$<CONFIG:Debug>:_DEBUG>
        $<$<CONFIG:Debug>:_ITERATOR_DEBUG_LEVEL=2>
    )
    set_property(DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS
        $<$<CONFIG:Release>:NDEBUG>
    )
    set_property(DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS
        $<$<CONFIG:RelWithDebInfo>:NDEBUG>
        $<$<CONFIG:RelWithDebInfo>:DBGREL>
    )

    # enable all warnings and treat warnings as errors
    add_compile_options(/W4 /WX)

    # set static runtime libs in all configurations
    # enable multiprocessor build in release config
    add_compile_options(
        $<$<CONFIG:Release>:/MT>
        $<$<CONFIG:Release>:/MP>
    )
    add_compile_options(
        $<$<CONFIG:Debug>:/MTd>
    )
    add_compile_options(
        $<$<CONFIG:RelWithDebInfo>:/MT>
        $<$<CONFIG:RelWithDebInfo>:/MP>
        $<$<CONFIG:RelWithDebInfo>:/INCREMENTAL:NO>
        $<$<CONFIG:RelWithDebInfo>:/Zi>
        $<$<CONFIG:RelWithDebInfo>:/Zo>
    )
endif()

#
# sources
#

file (GLOB_RECURSE fastxor_EXTERNAL_SOURCES
    RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
)

set(Shared_SOURCES
	include/Timer.h
	include/FastXOR.h
	src/FastXOR.cpp
)


if (WIN32)
set(Platform_SOURCES
)
else()
set(Platform_SOURCES
)
endif()

set(TestFastXOR_SOURCES
	app/TestFastXOR/TestFastXOR.cpp
)

#message("External sources:\n${fastxor_EXTERNAL_SOURCES}")

# deal with subdirectories in external sources
foreach(source IN LISTS fastxor_EXTERNAL_SOURCES)
    get_filename_component(source_path "${source}" PATH)
    string(REPLACE "/" "\\" source_path_msvc "${source_path}")
    source_group("${source_path_msvc}" FILES "${source}")
endforeach()

# deal with subdirectories in sources

# deal with subdirectories in sources
foreach(source IN LISTS TestFastXOR_SOURCES)
    get_filename_component(source_path "${source}" PATH)
    string(REPLACE "/" "\\" source_path_msvc "${source_path}")
    source_group("${source_path_msvc}" FILES "${source}")
endforeach()

#
# executable target
#

add_executable(TestFastXOR
    ${fastxor_EXTERNAL_SOURCES}
    ${TestFastXOR_SOURCES}
    ${Platform_SOURCES}
    ${Shared_SOURCES}
)

target_include_directories(TestFastXOR PUBLIC
    ${fastxor_EXT_ROOT}
    ${fastxor_ROOT}/include
    ${extra_INCLUDE}
)

if (WIN32)
    target_link_libraries(TestFastXOR
    )
else()
    target_link_libraries(TestFastXOR
        -ldl
        -lGL
        -lX11
        -lpthread
    )
endif()


set(fastxor_BIN_DIR ${fastxor_ROOT}/bin)
if (fastxor_BUILD_PLATFORM)
    set(fastxor_BIN_DIR ${fastxor_BIN_DIR}/${fastxor_BUILD_PLATFORM})
endif()
if (CMAKE_BUILD_TYPE)
    set(fastxor_BIN_DIR ${fastxor_BIN_DIR}/${CMAKE_BUILD_TYPE})
endif()

set_target_properties(TestFastXOR
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${fastxor_BIN_DIR}
)
