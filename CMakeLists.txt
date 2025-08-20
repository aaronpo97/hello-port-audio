cmake_minimum_required(VERSION 3.5)
project(hello-port-audio)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include)

# Pull PortAudio from github
include(FetchContent)
FetchContent_Declare(
    portaudio
    GIT_REPOSITORY https://github.com/PortAudio/portaudio.git
    GIT_TAG        v19.7.0
)
FetchContent_MakeAvailable(portaudio)


    

# Gather all source files
file(GLOB_RECURSE SOURCES ${CMAKE_SOURCE_DIR}/src/*.cpp)


# Add executable
add_executable(${PROJECT_NAME} ${SOURCES})

# Link PortAudio library
target_link_libraries(${PROJECT_NAME} PRIVATE portaudio)