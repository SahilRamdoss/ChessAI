cmake_minimum_required(VERSION 3.30) # Minimum version of CMake required

# Project configuration
set(PROJECT_NAME "ChessAI")
project(${PROJECT_NAME} CXX) # In CMake, C++ is CXX

# Creates a json file that specifies where all the libs folders created by CMake are found
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# FetchContent setup - Enable a CMake extension which allows me to download Github repositories
include(FetchContent)

# Include the dependecies used in the project. It takes 3 parameters: The lib name, the github url for the 
# dependency and the release version
function(include_dependency libName gitURL gitTag)
    FetchContent_Declare(${libName}
        GIT_REPOSITORY ${gitURL}
        GIT_TAG        ${gitTag}
        GIT_SHALLOW    TRUE
        GIT_PROGRESS   TRUE
        GIT_CLONE_DEPTH 1
    )
    FetchContent_MakeAvailable(${libName})
endfunction()

# SDL3 setup
find_package(SDL3 QUIET) # checks if SDL3 is already installed in system
if (NOT SDL3_FOUND)
    message(STATUS "Getting SDL3 from GitHub...")
    include_dependency(SDL3 https://github.com/libsdl-org/SDL.git release-3.2.18)
else()
    message(STATUS "Using local SDL3 installation.")
endif()

# SDL3_image setup
find_package(SDL3_image QUIET)
if (NOT SDL3_image_FOUND)
    message(STATUS "Getting SDL3_image from GitHub...")
    include_dependency(SDL3_image https://github.com/libsdl-org/SDL_image.git release-3.2.0)
else()
    message(STATUS "Using local SDL3_image installation.")
endif()

# Source and include directories - Put all .cpp files in a variable called PROJECT_SOURCES
file(GLOB_RECURSE PROJECT_SOURCES CONFIGURE_DEPENDS
    ${PROJECT_SOURCE_DIR}/src/*.cpp
)

# Defines the .exe for my project, which is ChessAI.exe in our case
add_executable(${PROJECT_NAME} WIN32 ${PROJECT_SOURCES})

# Add the includes for the project and library dependencies
target_include_directories(${PROJECT_NAME} PUBLIC
    ${PROJECT_SOURCE_DIR}/include
    ${SDL3_INCLUDE_DIRS}
    ${SDL3_image_INCLUDE_DIRS}
)

# Define the macro SDL_MAIN_USE_CALLBACKS
target_compile_definitions(${PROJECT_NAME} PRIVATE SDL_MAIN_USE_CALLBACKS)

# Linking the libraries
target_link_libraries(${PROJECT_NAME} PRIVATE
    SDL3::SDL3
    SDL3_image::SDL3_image
)

# Windows: copy DLLs after build
if (WIN32)
    add_custom_command(
        TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E copy_if_different
            "$<TARGET_FILE:SDL3::SDL3>"
            "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
        COMMAND "${CMAKE_COMMAND}" -E copy_if_different
            "$<TARGET_FILE:SDL3_image::SDL3_image>"
            "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
        VERBATIM
    )
endif()
