cmake_minimum_required(VERSION 3.15)

# Project name and version
project(Lumo VERSION 1.0 LANGUAGES CXX C)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT DEFINED CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Compiler warnings and optimization flags
if(MSVC)
    add_compile_options(/W4 /permissive-) # For Visual Studio
else()
    add_compile_options(-Wall -Wextra)
endif()

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/include/glad)
include_directories(${CMAKE_SOURCE_DIR}/include/KHR)

# Add platform-specific include paths
if(UNIX AND NOT APPLE)
    include_directories(/usr/include/Imath)
elseif(APPLE)
    # macOS-specific includes or settings can go here
    message(WARNING "macOS support may require additional configuration.")
elseif(WIN32)
    # Windows-specific includes or settings
    message(WARNING "Windows support may require configuration of GLFW and OpenEXR paths.")
endif()

# Source files
file(GLOB_RECURSE SRC_FILES ${CMAKE_SOURCE_DIR}/src/*.cpp ${CMAKE_SOURCE_DIR}/src/*.c)

# Output binary directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Add executable
add_executable(${PROJECT_NAME} ${SRC_FILES})

# Find and link required libraries
find_package(glfw3 REQUIRED)
find_package(OpenEXR REQUIRED)

if(APPLE)
    target_link_libraries(${PROJECT_NAME} glfw OpenEXR::OpenEXR "-framework OpenGL")
elseif(UNIX AND NOT APPLE)
    target_link_libraries(${PROJECT_NAME} glfw OpenEXR::OpenEXR GL)
elseif(WIN32)
    target_link_libraries(${PROJECT_NAME} glfw OpenEXR::OpenEXR opengl32)
endif()

# Post-build instructions
add_custom_command(
    TARGET ${PROJECT_NAME}
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E echo "Build complete! Run the executable from: ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}"
)

add_custom_target(setup
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_SOURCE_DIR}/include
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_SOURCE_DIR}/src
    COMMAND ${CMAKE_COMMAND} -E touch ${CMAKE_SOURCE_DIR}/src/main.cpp
    COMMAND ${CMAKE_COMMAND} -E echo "-Iinclude/" > ${CMAKE_SOURCE_DIR}/compile_flags.txt
    COMMAND ${CMAKE_COMMAND} -E echo "IndentWidth: 4\nBasedOnStyle: LLVM\nPointerAlignment: Left\nSortIncludes: false" > ${CMAKE_SOURCE_DIR}/.clang-format
)
