cmake_minimum_required(VERSION 3.15...4.1)

project(
    overture 
    VERSION 0.0.0
    LANGUAGES C
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

set(OVERTURE_LIBS glfw)
set(LIB_DIR lib)
file(GLOB LIB_DIRS "${LIB_DIR}/*")
message(STATUS "LIB_DIRS: ${LIB_DIRS}")

set(INCLUDE_DIRS include src)

foreach(lib ${LIB_DIRS})
    file(GLOB_RECURSE LIB_FILES "${lib}/src/*.c")
    cmake_path(GET lib FILENAME CURRENT_LIB)
    message(STATUS "${CURRENT_LIB}: ${LIB_FILES}")
    list(APPEND INCLUDE_DIRS "${lib}/include")
    add_library(${CURRENT_LIB} STATIC ${LIB_FILES})
    target_include_directories(${CURRENT_LIB} PUBLIC "${lib}/include")
    target_compile_options(${CURRENT_LIB} PRIVATE "-fPIC")
    list(APPEND OVERTURE_LIBS ${CURRENT_LIB})
endforeach()

message(STATUS "INCLUDE_DIRS: ${INCLUDE_DIRS}")

set(SRC_DIRS src src/core src/platform src/graphics)

foreach(src_dir ${SRC_DIRS})
    # maybe use GLOB_RECURSE and ommit the list of src dirs
    file(GLOB NEW_FILES LIST_DIRECTORIES false "${src_dir}/*.c")
    list(APPEND SRC_FILES ${NEW_FILES})
endforeach()

message(STATUS "Files: ${SRC_FILES}")

add_library(overture SHARED ${SRC_FILES})
target_link_libraries(overture PUBLIC ${OVERTURE_LIBS})
target_include_directories(overture PUBLIC ${INCLUDE_DIRS})
target_compile_options(overture PRIVATE "-fPIC")
#target_compile_options(overture PRIVATE "-g")

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
