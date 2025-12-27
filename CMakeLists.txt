cmake_minimum_required(VERSION 3.15...4.1)

project(
    overture 
    VERSION 0.0.0
    LANGUAGES C
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

set(INCLUDE_DIRS include src)

set(SRC_DIRS src src/core src/platform src/graphics)

foreach(src_dir ${SRC_DIRS})
    # maybe use GLOB_RECURSE and ommit the list of src dirs
    file(GLOB NEW_FILES LIST_DIRECTORIES false "${src_dir}/*.c")
    list(APPEND SRC_FILES ${NEW_FILES})
endforeach()

message(STATUS "Files: ${SRC_FILES}")

add_library(overture SHARED ${SRC_FILES})
target_link_libraries(overture PUBLIC glfw vulkan)
target_include_directories(overture PUBLIC ${INCLUDE_DIRS})
target_compile_options(overture PRIVATE "-fPIC")

set(EXAMPLE_DIR examples)

file(GLOB EXAMPLES "${EXAMPLE_DIR}/*")

message(STATUS "Examples: ${EXAMPLES}")

foreach(example_dir ${EXAMPLES})
    file(GLOB_RECURSE EXAMPLE_FILES "${example_dir}/*.c")
    cmake_path(GET example_dir FILENAME CURRENT_EXAMPLE)
    message(STATUS "${CURRENT_EXAMPLE}: ${EXAMPLE_FILES}")
    add_executable(${CURRENT_EXAMPLE} ${EXAMPLE_FILES})
    target_link_libraries(${CURRENT_EXAMPLE} PUBLIC overture)
    target_include_directories(${CURRENT_EXAMPLE} PUBLIC ${INCLUDE_DIRS})
    target_compile_options(${CURRENT_EXAMPLE} PRIVATE "-fPIC")
endforeach()
